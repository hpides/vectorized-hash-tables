#pragma once

#include <cstdint>

#include "fmt/format.h"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/simd_utils.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

namespace hashmap::hashmaps {

template <typename KeyT, typename ValueT, typename HasherT, uint16_t simd_size = 128, SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ,
          bool use_avx512_features = false, bool use_sve = false, NEONAlgo neon_algo = NEONAlgo::SSE2NEON, bool sve_scalar_broadcast = false,
          bool use_prefetching = false, utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class UnchunkedSIMDSoAHashTable : public HashTable<KeyT, ValueT> {
 public:
  UnchunkedSIMDSoAHashTable(uint64_t max_elements, uint8_t /*target_load_factor*/, bool print_info = true,
                            std::string base_identifier = "UnchunkedSIMDSoAHashTable")
      : size_{0}, hasher_(max_elements), base_identifier_{base_identifier} {
    // We have to over-allocate exactly so much memory that even
    // when loading the last element, we fit into the SIMD register
    simd_padding_ = keys_per_vector_;
    max_elements_ = max_elements + simd_padding_;

    // However, the hasher maps into the old range
    keys_.resize(max_elements_);
    values_.resize(max_elements_);
    is_valids_.resize(max_elements_, false);
    max_elements_ = max_elements;

    if (print_info) {
      int32_t align_vec1 = -1;
      int32_t align_vec2 = -1;

      if constexpr (!utils::has_sve) {
        align_vec1 = alignof(_vector_type);
        align_vec2 = alignof(vector_type);
      }

      spdlog::info(
          fmt::format("Initialized {} with utils::cacheline_size = {}, sizeof(EntryT) = XX, "
                      "alignof(EntryT) = XX, simd_size = {}, alignof(_vector_type)={}, alignof(vector_type)={}, vector_alignment_func={}",
                      get_identifier(), utils::cacheline_size, simd_size, align_vec1, align_vec2, SIMDH::_vector_alignment()));
    }

    fail_if_system_is_incompatible<KeyT, simd_size, simd_algo, use_avx512_features, use_sve, neon_algo, sve_scalar_broadcast>();
  }
  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using _vector_type = typename SIMDH::_vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using MaskType = typename SIMDH::MaskType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;

  bool contains(const KeyT& key) {
    const uint64_t idx = find(key);
    DEBUG_ASSERT(idx < max_elements_, "Got invalid key from find");

    if (keys_[idx] != key || !is_valids_[idx]) {
      return false;
    }

    return true;
  }

  ValueT lookup(const KeyT& key) {
    const uint64_t idx = find(key);
    DEBUG_ASSERT(idx < max_elements_, "Got invalid key from find");

    if (keys_[idx] != key || !is_valids_[idx]) {
      if constexpr (std::is_pointer_v<ValueT>) {
        return nullptr;
      } else {
        return ValueT{};
      }
    }

    return values_[idx];
  };

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(size_ + 1 <= max_elements_, "Hashmap is full!");
    const uint64_t idx = find(key);
    DEBUG_ASSERT(idx < max_elements_, "Got invalid key from find");

    if (keys_[idx] != key || !is_valids_[idx]) {
      // We are inserting a new element (no update)
      ++size_;
      keys_[idx] = key;
      is_valids_[idx] = true;
    }

    values_[idx] = value;
  };

  uint64_t find(const KeyT& key) {
#ifdef HASHMAP_COLLECT_META_INFO
    uint64_t probing_seq_len = 0;
    if (minfo.measurement_started) {
      ++(minfo.num_finds);
    }

#define META_INCREASE_PROBED_ELEMENTS                \
  if (minfo.measurement_started) {                   \
    ++(minfo.probed_elements);                       \
    minfo.probed_elements_total += keys_per_vector_; \
    ++probing_seq_len;                               \
  }

#define META_INCREASE_SIMD_LOADS   \
  if (minfo.measurement_started) { \
    ++(minfo.simd_loads);          \
  }

#else
#define META_INCREASE_PROBED_ELEMENTS
#define META_INCREASE_SIMD_LOADS
#endif

    vector_type index_vector;
#if defined(__ARM_FEATURE_SVE)
    if constexpr (use_sve) {
      index_vector = SIMDH::index_creation_func_();
    }
#else
    (void)index_vector;
#endif

    uint64_t curr_idx = hasher_.hash(key);
    uint64_t next_idx = curr_idx;
    uint64_t curr_adjusted_idx = curr_idx;

    std::uintptr_t p = reinterpret_cast<std::uintptr_t>(&keys_[curr_idx]);

    uint64_t remainder = p % SIMDH::_vector_alignment();

    DEBUG_ASSERT(remainder % sizeof(KeyT) == 0,
                 "the remainder should be a multiple sizeof(KeyT). Either you have a weird key (i.e., 7 byte sized or something) or something went "
                 "terribly wrong.");

    bool is_aligned = remainder == 0;

    META_INCREASE_SIMD_LOADS;

    vector_type next_vector =
        is_aligned ? SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(p)) : SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(p));

    META_INCREASE_SIMD_LOADS;

    vector_type next_valid_vector = is_aligned ? SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&is_valids_[curr_adjusted_idx]))
                                               : SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&is_valids_[curr_adjusted_idx]));

    next_idx += is_aligned * keys_per_vector_ + !is_aligned * ((SIMDH::_vector_alignment() - remainder) / sizeof(KeyT));

    DEBUG_ASSERT(next_idx - curr_idx <= keys_per_vector_, "We made a too big jump.");

    // In case we cross the boundary of the hashmap (which only happens once, because we interrupt the probe if we probe for too long), we need to
    // start on index 0, to ensure alignedness
    next_idx = std::min(next_idx, max_elements_);

    // at maximum, our linear probe will be as long as many elements we have inserted plus our excess SIMD padding
    const uint64_t max_idx = curr_adjusted_idx + size_ + simd_padding_;
    const CompareT compare_vector = SIMDH::vector_broadcast_func_(key);

    // currently, each is_valid is of the same type as a key. This wastes lots of memory, but allows us
    // to keep our indices synced. Otherwise, we would be doing more comparisons in one vector register
    // for is_valids than for the keys, which would make the code very hard to read
    const CompareT invalid_compare_vector = SIMDH::vector_broadcast_func_(false);

    bool entry_found = false;
    while (!entry_found) {
      if constexpr (simd_algo == SIMDAlgorithm::TESTZ) {
        META_INCREASE_PROBED_ELEMENTS;
        MaskOrVectorInputType cmp_result;
        if constexpr (utils::is_power) {
          cmp_result = SIMDH::vector_cmp_func_(next_vector, compare_vector, false);
        } else {
          cmp_result = SIMDH::vector_cmp_func_(next_vector, compare_vector);
        }

        if (SIMDH::vector_any_nonzero(cmp_result)) {
          uint32_t first_index = 0;
          if constexpr (use_sve) {
            first_index = static_cast<uint32_t>(SIMDH::sve_vector_first_index(cmp_result, index_vector));
          } else {
            first_index = SIMDH::vector_first_index(cmp_result);
          }

          DEBUG_ASSERT(first_index < keys_per_vector_, "First index out of range");

          /* only if this is within the true hashmap range and not within padding, we return */
          const uint64_t index_into_map = curr_adjusted_idx + first_index;
          if (index_into_map < max_elements_) [[likely]] {
            /* We do not chcek for validity here, because if it is invalid, we would do the exactly same behvaior */
            entry_found = true;
            curr_adjusted_idx += first_index;
            break;
          }
        }

        MaskOrVectorInputType valid_cmp_result;
        if constexpr (utils::is_power) {
          valid_cmp_result = SIMDH::vector_cmp_func_(next_valid_vector, invalid_compare_vector, false);
        } else {
          valid_cmp_result = SIMDH::vector_cmp_func_(next_valid_vector, invalid_compare_vector);
        }

        if (SIMDH::vector_any_nonzero(valid_cmp_result)) {
          /* we've reached an invalid element => abort */
          uint64_t first_invalid_index = 0;
          if constexpr (use_sve) {
            first_invalid_index = SIMDH::sve_vector_first_index(valid_cmp_result, index_vector);
          } else {
            first_invalid_index = SIMDH::vector_first_index(valid_cmp_result);
          }

          DEBUG_ASSERT(first_invalid_index < keys_per_vector_, "First index out of range");

          /* only if this is within the true hashmap range and not within padding, we return */
          const uint64_t index_into_map = curr_adjusted_idx + first_invalid_index;
          if (index_into_map < max_elements_) [[likely]] {
            entry_found = true;
            curr_adjusted_idx = hasher_.finalize(index_into_map);
            break;
          }
        }

        /* continue the probe */
        curr_idx = next_idx;
        next_idx += keys_per_vector_;

        /* In case we cross the boundary of the hashmap (which only happens once, because we interrupt the probe if we probe for too long), we need to
        start on index 0, to ensure alignedness. We cannot use std::min because as that would reset every time. */

        if (next_idx >= max_elements_ && curr_idx < max_elements_) [[unlikely]] {
          next_idx = max_elements_;
        }

        curr_adjusted_idx = hasher_.finalize(curr_idx);

        DEBUG_ASSERT(curr_adjusted_idx < keys_.size(), fmt::format("Got invalid curr_adjusted_idx {} ({})", curr_adjusted_idx, keys_.size()));

        DEBUG_ASSERT(curr_adjusted_idx % keys_per_vector_ == 0,
                     fmt::format("curr_adjusted_idx = {} is not a multiple of keys_per_vector_ {}. curr_idx={},  max_elements_={}, keys_.size()={}",
                                 curr_adjusted_idx, keys_per_vector_, curr_idx, max_elements_, keys_.size()));

        DEBUG_ASSERT(utils::is_aligned((void*)&keys_[curr_adjusted_idx], SIMDH::_vector_alignment()),
                     fmt::format("Next load of index {} would be unaligned!", curr_adjusted_idx));

        META_INCREASE_SIMD_LOADS;
        META_INCREASE_SIMD_LOADS;

        next_vector = SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&keys_[curr_adjusted_idx]));
        next_valid_vector = SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&is_valids_[curr_adjusted_idx]));

        /* However, maybe we have reached the maximum probe length that is required, then we can stop */
        if (curr_idx > max_idx) [[unlikely]] {
          entry_found = true;
          curr_adjusted_idx = hasher_.finalize(max_idx);
        }

      } else if constexpr (simd_algo == SIMDAlgorithm::NO_TESTZ) {
        META_INCREASE_PROBED_ELEMENTS;
        MaskOrVectorInputType cmp_result;
        if constexpr (utils::is_power) {
          cmp_result = SIMDH::vector_cmp_func_(next_vector, compare_vector, false);
        } else {
          cmp_result = SIMDH::vector_cmp_func_(next_vector, compare_vector);
        }
        uint32_t first_index = 0;
        if constexpr (use_sve) {
          first_index = static_cast<uint32_t>(SIMDH::sve_vector_first_index(cmp_result, index_vector));
        } else {
          first_index = SIMDH::vector_first_index(cmp_result);
        }
        uint32_t first_invalid_index = 0;

        if (first_index < invalid_vector_index_) {
          DEBUG_ASSERT(first_index < keys_per_vector_, "First index out of range");

          /* only if this is within the true hashmap range and not within padding, we return */
          const uint64_t index_into_map = curr_adjusted_idx + first_index;
          if (index_into_map < max_elements_) [[likely]] {
            /* We do not chcek for validity here, because if it is invalid, we would do the exactly same behvaior */
            entry_found = true;
            curr_adjusted_idx += first_index;
            break;
          }
        }

        MaskOrVectorInputType valid_cmp_result;
        if constexpr (utils::is_power) {
          valid_cmp_result = SIMDH::vector_cmp_func_(next_valid_vector, invalid_compare_vector, false);
        } else {
          valid_cmp_result = SIMDH::vector_cmp_func_(next_valid_vector, invalid_compare_vector);
        }
        if constexpr (use_sve) {
          first_invalid_index = static_cast<uint32_t>(SIMDH::sve_vector_first_index(valid_cmp_result, index_vector));
        } else {
          first_invalid_index = SIMDH::vector_first_index(valid_cmp_result);
        }
        if (first_invalid_index < invalid_vector_index_) {
          DEBUG_ASSERT(first_invalid_index < keys_per_vector_, "First index out of range");

          /* only if this is within the true hashmap range and not within padding, we return */
          const uint64_t index_into_map = curr_adjusted_idx + first_invalid_index;
          if (index_into_map < max_elements_) [[likely]] {
            entry_found = true;
            curr_adjusted_idx = hasher_.finalize(index_into_map);
            break;
          }
        }
        /* continue the probe */
        curr_idx = next_idx;
        next_idx += keys_per_vector_;

        /* In case we cross the boundary of the hashmap (which only happens once, because we interrupt the probe if we probe for too long), we need to
        start on index 0, to ensure alignedness. We cannot use std::min because as that would reset every time. */

        if (next_idx > max_elements_ && curr_idx < max_elements_) [[unlikely]] {
          next_idx = max_elements_;
        }

        curr_adjusted_idx = hasher_.finalize(curr_idx);

        DEBUG_ASSERT(curr_adjusted_idx < keys_.size(), fmt::format("Got invalid curr_adjusted_idx {} ({})", curr_adjusted_idx, keys_.size()));

        DEBUG_ASSERT(curr_adjusted_idx % keys_per_vector_ == 0,
                     fmt::format("curr_adjusted_idx = {} is not a multiple of keys_per_vector_ {}. curr_idx={},  max_elements_={}, keys_.size()={}",
                                 curr_adjusted_idx, keys_per_vector_, curr_idx, max_elements_, keys_.size()));

        DEBUG_ASSERT(utils::is_aligned((void*)&keys_[curr_adjusted_idx], SIMDH::_vector_alignment()),
                     fmt::format("Next load of index {} would be unaligned!", curr_adjusted_idx));

        META_INCREASE_SIMD_LOADS;
        META_INCREASE_SIMD_LOADS;

        next_vector = SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&keys_[curr_adjusted_idx]));
        next_valid_vector = SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&is_valids_[curr_adjusted_idx]));

        /* However, maybe we have reached the maximum probe length that is required, then we can stop */
        if (curr_idx > max_idx) [[unlikely]] {
          entry_found = true;
          curr_adjusted_idx = hasher_.finalize(max_idx);
        }

      } else if constexpr (simd_algo == SIMDAlgorithm::MANUAL_ON_MATCH) {
        META_INCREASE_PROBED_ELEMENTS;
        MaskOrVectorInputType cmp_result;
        if constexpr (utils::is_power) {
          cmp_result = SIMDH::vector_cmp_func_(next_vector, compare_vector, false);
        } else {
          cmp_result = SIMDH::vector_cmp_func_(next_vector, compare_vector);
        }

        if (SIMDH::vector_any_nonzero(cmp_result)) {
          uint64_t index_into_map = curr_adjusted_idx;

          while (keys_[index_into_map] != key) {
            ++index_into_map;
          }

          if (index_into_map < max_elements_) [[likely]] {
            /* We do not chcek for validity here, because if it is invalid, we would do the exactly same behvaior */
            entry_found = true;
            curr_adjusted_idx = index_into_map;
            break;
          }
        }

        MaskOrVectorInputType valid_cmp_result;
        if constexpr (utils::is_power) {
          valid_cmp_result = SIMDH::vector_cmp_func_(next_valid_vector, invalid_compare_vector, false);
        } else {
          valid_cmp_result = SIMDH::vector_cmp_func_(next_valid_vector, invalid_compare_vector);
        }

        if (SIMDH::vector_any_nonzero(valid_cmp_result)) {
          /* we've reached an invalid element => abort */
          while (is_valids_[curr_adjusted_idx] == true) {
            ++curr_adjusted_idx;
          }

          /* only if this is within the true hashmap range and not within padding, we return */
          if (curr_adjusted_idx < max_elements_) [[likely]] {
            entry_found = true;
            curr_adjusted_idx = hasher_.finalize(curr_adjusted_idx);
            break;
          }
        }

        /* continue the probe */
        curr_idx = next_idx;
        next_idx += keys_per_vector_;

        /* In case we cross the boundary of the hashmap (which only happens once, because we interrupt the probe if we probe for too long), we need to
        start on index 0, to ensure alignedness. We cannot use std::min because as that would reset every time. */

        if (next_idx > max_elements_ && curr_idx < max_elements_) [[unlikely]] {
          next_idx = max_elements_;
        }

        curr_adjusted_idx = hasher_.finalize(curr_idx);

        DEBUG_ASSERT(curr_adjusted_idx < keys_.size(), fmt::format("Got invalid curr_adjusted_idx {} ({})", curr_adjusted_idx, keys_.size()));

        DEBUG_ASSERT(curr_adjusted_idx % keys_per_vector_ == 0,
                     fmt::format("curr_adjusted_idx = {} is not a multiple of keys_per_vector_ {}. curr_idx={},  max_elements_={}, keys_.size()={}",
                                 curr_adjusted_idx, keys_per_vector_, curr_idx, max_elements_, keys_.size()));

        DEBUG_ASSERT(utils::is_aligned((void*)&keys_[curr_adjusted_idx], SIMDH::_vector_alignment()),
                     fmt::format("Next load of index {} would be unaligned!", curr_adjusted_idx));

        META_INCREASE_SIMD_LOADS;
        META_INCREASE_SIMD_LOADS;

        next_vector = SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&keys_[curr_adjusted_idx]));
        next_valid_vector = SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&is_valids_[curr_adjusted_idx]));

        /* However, maybe we have reached the maximum probe length that is required, then we can stop */
        if (curr_idx > max_idx) [[unlikely]] {
          entry_found = true;
          curr_adjusted_idx = hasher_.finalize(max_idx);
        }

      } else {
        FAIL("not implemented");
      }
    }

#ifdef HASHMAP_COLLECT_META_INFO
    if (minfo.measurement_started) {
      minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
      minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
    }
#endif

    return curr_adjusted_idx;
  }

  void prefault() {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<uint64_t> dist{0, 0xDEADBEEF};

    for (uint64_t i = 0; i < keys_.size(); i++) {
      if constexpr (!std::is_same_v<KeyT, StringKey>) {
        keys_[i] = static_cast<KeyT>(dist(gen));
      }
      if constexpr (!std::is_pointer_v<ValueT>) {
        values_[i] = static_cast<ValueT>(dist(gen));
      }
      is_valids_[i] = true;
    }

    reset();
  }

  void prefault_pregenerated(const std::vector<std::pair<KeyT, ValueT>>& prefault_data) {
    uint64_t prefault_data_size = prefault_data.size();
    for (uint64_t i = 0; i < keys_.size(); i++) {
      keys_[i] = prefault_data[i % prefault_data_size].first;
      values_[i] = prefault_data[i % prefault_data_size].second;
      is_valids_[i] = true;
    }

    reset();
  }

  void reset() {
    for (uint64_t i = 0; i < keys_.size(); i++) {
      keys_[i] = KeyT();
      values_[i] = ValueT();
      is_valids_[i] = false;
    }

    size_ = 0;
  }

  std::string get_identifier() {
    std::string prefetching = "NoPref";
    if constexpr (use_prefetching) {
      prefetching = fmt::format("Pref{}", static_cast<int>(prefetching_locality));
    }

    std::string thp = "NoTHP";
    if constexpr (use_thp) {
      thp = "THP";
    }

    std::string value_type = hashmap::utils::data_type_to_str<ValueT>();
    std::string key_type = hashmap::utils::data_type_to_str<KeyT>();

    std::string algo = "";

    if constexpr (simd_algo == SIMDAlgorithm::TESTZ) {
      algo = "TESTZ";
    } else if constexpr (simd_algo == SIMDAlgorithm::NO_TESTZ) {
      algo = "NO_TESTZ";
    } else if constexpr (simd_algo == SIMDAlgorithm::MANUAL_ON_MATCH) {
      algo = "MANUAL_ON_MATCH";
    }

    std::string avx512 = "NoAVX512";
    if constexpr (use_avx512_features) {
      avx512 = "AVX512";
    }

    std::string key_simd_type = SIMDH::key_simd_type_to_str();
    std::string compare_result_type = SIMDH::compare_result_type_to_str();

    std::string sve = "NoSVE";
    if constexpr (use_sve) {
      sve = "SVE";
    }

    std::string neon_algo_str = "SSE2NEON";
    if constexpr (neon_algo == NEONAlgo::AARCH64) {
      neon_algo_str = "AARCH64";
    } else if constexpr (neon_algo == NEONAlgo::UMINV) {
      neon_algo_str = "UMINV";
    }

    std::string sve_broadcast = "NoSVEBroadcast";
    if constexpr (sve_scalar_broadcast) {
      sve_broadcast = "SVEBroadcast";
    }

    return fmt::format("{}<{}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}>", base_identifier_, hasher_.get_identifier(), key_type, value_type,
                       prefetching, thp, key_simd_type, algo, avx512, compare_result_type, sve, neon_algo_str, sve_broadcast);
  }

  uint64_t get_entry_size() { return 0; }

  bool is_data_aligned_to(size_t alignment) { return utils::is_aligned((void*)keys_.data(), alignment); }

  std::string get_data_pointer_string() { return fmt::format("{}", (void*)keys_.data()); }

  double get_current_load() { return static_cast<double>(size_) / static_cast<double>(max_elements_); }
  uint64_t get_current_size() { return size_; }

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo* get_minfo() { return &minfo; }
  void start_measurement() { minfo.measurement_started = true; }
  void stop_measurement() { minfo.measurement_started = false; }
  void reset_measurement() { minfo.reset(); }
#endif

 protected:
  constexpr static uint32_t invalid_vector_index_ = SIMDH::calc_invalid_vector_index();
  // Alignment allocators depending on SIMD size
  // TODO: What alignment do we need for SVE?
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<ValueT>, std::allocator<ValueT>>::type ValueVectorAllocator;
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<KeyT, SIMDH::_vector_alignment()>,
                                    utils::AlignedAllocator<KeyT, SIMDH::_vector_alignment()>>::type SIMDAlignedAllocator;

  alignas(utils::cacheline_size) std::vector<KeyT, SIMDAlignedAllocator> keys_;
  alignas(utils::cacheline_size) std::vector<ValueT, ValueVectorAllocator> values_;
  alignas(utils::cacheline_size) std::vector<KeyT, SIMDAlignedAllocator> is_valids_;

  alignas(utils::cacheline_size) uint64_t max_elements_;
  alignas(utils::cacheline_size) uint64_t simd_padding_;
  alignas(utils::cacheline_size) uint64_t size_;
  alignas(utils::cacheline_size) HasherT hasher_;
  alignas(utils::cacheline_size) constexpr static uint8_t keys_per_vector_ = (simd_size / 8) / sizeof(KeyT);
  std::string base_identifier_;

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo minfo;
#endif
};

}  // namespace hashmap::hashmaps
