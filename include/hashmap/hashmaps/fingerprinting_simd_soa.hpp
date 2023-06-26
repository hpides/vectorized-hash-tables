#pragma once

#include <cstdint>
#include <utility>

#include "fmt/format.h"
#include "hashmap/hashes/buckethash.hpp"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/simd_utils.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

namespace hashmap::hashmaps {

template <typename KeyT, typename ValueT, typename HasherT, typename FingerprintT = uint16_t, uint16_t simd_size = 128,
          SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ, bool use_avx512_features = false, bool use_sve = false,
          NEONAlgo neon_algo = NEONAlgo::SSE2NEON, bool sve_scalar_broadcast = false, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false, bool use_likely_hints = false,
          hashing::FingerprintBucketBits fingerprint_bucket_bits = hashing::FingerprintBucketBits::LSBLSB, FingerprintT invalid_fingerprint = 0>
class FingerprintingSIMDSoAHashTable : public HashTable<KeyT, ValueT> {
 public:
  FingerprintingSIMDSoAHashTable(uint64_t max_elements, uint8_t /*target_load_factor*/, bool print_info = true,
                                 std::string base_identifier = "FingerprintingSIMDSoAHashTable")
      : size_{0}, hasher_(max_elements), base_identifier_{base_identifier} {
    // We have to over-allocate exactly so much memory that even
    // when loading the last element, we fit into the SIMD register
    simd_padding_ = fps_per_vector_;
    max_elements_ = max_elements + simd_padding_;

    // However, the hasher maps into the old range
    keys_values_.resize(max_elements_);
    fingerprints_.resize(max_elements_, invalid_fingerprint);
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

    fail_if_system_is_incompatible<FingerprintT, simd_size, simd_algo, use_avx512_features, use_sve, neon_algo, sve_scalar_broadcast>();
  }
  using SIMDH = SIMDHelper<FingerprintT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using _vector_type = typename SIMDH::_vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using MaskType = typename SIMDH::MaskType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  struct FindResult {
    std::pair<KeyT, ValueT> key_value;
    FingerprintT fingerprint;
    uint64_t index;
    bool is_valid;
  };

  bool contains(const KeyT& key) {
    const FindResult& result = find(key);
    DEBUG_ASSERT(result.index < max_elements_, "Got invalid key from find");
    return result.is_valid;
  }

  ValueT lookup(const KeyT& key) {
    const FindResult& result = find(key);
    DEBUG_ASSERT(result.index < max_elements_, "Got invalid key from find");
    if constexpr (std::is_pointer_v<ValueT>) {
      return result.is_valid ? std::get<1>(result.key_value) : nullptr;
    } else {
      return result.is_valid ? std::get<1>(result.key_value) : ValueT{};
    }
  };

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(size_ + 1 <= max_elements_, "Hashmap is full!");
    const FindResult& result = find(key);
    DEBUG_ASSERT(result.index < max_elements_, "Got invalid key from find");

    if (!result.is_valid) {
      // We are inserting a new element (no update)
      ++size_;
      fingerprints_[result.index] = result.fingerprint;
    }

    keys_values_[result.index] = {key, value};
  };

  FindResult find(const KeyT& key) {
#ifdef HASHMAP_COLLECT_META_INFO
    if (minfo.measurement_started) {
      ++(minfo.num_finds);
    }
    uint64_t probing_seq_len = 0;
#endif

    vector_type index_vector;
#if defined(__ARM_FEATURE_SVE)
    if constexpr (use_sve) {
      index_vector = SIMDH::index_creation_func_();
    }
#else
    (void)index_vector;
#endif
    const hashing::BucketHash<FingerprintT> bucket_hash =
        hasher_.template bucket_hash<FingerprintT, fingerprint_bucket_bits, invalid_fingerprint>(key);
    uint64_t curr_idx = bucket_hash.bucket;
    const FingerprintT fingerprint = bucket_hash.fingerprint;

    uint64_t next_idx = curr_idx;
    uint64_t curr_adjusted_idx = curr_idx;

    DEBUG_ASSERT(curr_idx < fingerprints_.size(), fmt::format("Got invalid curr_idx {} ({})", curr_idx, fingerprints_.size()));

    std::uintptr_t p = reinterpret_cast<std::uintptr_t>(&fingerprints_[curr_idx]);

    uint64_t remainder = p % SIMDH::_vector_alignment();

    DEBUG_ASSERT(remainder % sizeof(FingerprintT) == 0,
                 "the remainder should be a multiple sizeof(FingerprintT). Either you have a weird fingerprint type (i.e., 7 byte sized or "
                 "something) or something went "
                 "terribly wrong.");

    bool is_aligned = remainder == 0;
#ifdef HASHMAP_COLLECT_META_INFO
    if (minfo.measurement_started) {
      ++(minfo.simd_loads);
    }
#endif
    vector_type next_fingerprints =
        is_aligned ? SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(p)) : SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(p));

    next_idx += is_aligned * fps_per_vector_ + !is_aligned * ((SIMDH::_vector_alignment() - remainder) / sizeof(FingerprintT));

    // In case we cross the boundary of the hashmap (which only happens once, because we interrupt the probe if we probe for too long), we need to
    // start on index 0, to ensure alignedness
    next_idx = std::min(next_idx, max_elements_);

    DEBUG_ASSERT(next_idx - curr_idx <= fps_per_vector_, "We made a too big jump.");

    // at maximum, our linear probe will be as long as many elements we have inserted plus our excess SIMD padding
    const uint64_t max_idx = curr_adjusted_idx + size_ + simd_padding_;
    const CompareT compare_vector = SIMDH::vector_broadcast_func_(fingerprint);
    const CompareT invalid_compare_vector = SIMDH::vector_broadcast_func_(invalid_fingerprint);

    while (true) {
#ifdef HASHMAP_COLLECT_META_INFO
      if (minfo.measurement_started) {
        ++probing_seq_len;
        ++(minfo.probed_elements);
        minfo.probed_elements_total += fps_per_vector_;
      }
#endif
      MaskOrVectorInputType cmp_result;
      if constexpr (utils::is_power) {
        cmp_result = SIMDH::vector_cmp_func_(next_fingerprints, compare_vector, false);
      } else {
        cmp_result = SIMDH::vector_cmp_func_(next_fingerprints, compare_vector);
      }

      if constexpr (simd_algo == SIMDAlgorithm::TESTZ) {
        if (SIMDH::vector_any_nonzero(cmp_result)) {
          // We can use a do-while loop here because we know there was at least one match
          MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
          do {
            uint16_t next_match = 0;

            if constexpr (use_sve) {
              next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
            } else {
              next_match = CompareResultIterator::next_match(iterator);
            }

            iterator = CompareResultIterator::next_it(iterator, next_match);
            const uint64_t next_vec_id = curr_adjusted_idx + next_match;
            DEBUG_ASSERT(next_vec_id < keys_values_.size(),
                         fmt::format("Got invalid next_vec_id {} ({}, {}, {})", next_vec_id, curr_adjusted_idx, next_match, keys_values_.size()));
            const std::pair<KeyT, ValueT>& kv_pair = keys_values_[next_vec_id];

            if constexpr (use_likely_hints) {
              if (HEDLEY_LIKELY(std::get<0>(kv_pair) == key)) [[likely]] {
#ifdef HASHMAP_COLLECT_META_INFO
                if (minfo.measurement_started) {
                  minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
                  minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
                }
#endif

                return {kv_pair, fingerprint, next_vec_id, true};
              }
#ifdef HASHMAP_COLLECT_META_INFO
              if (minfo.measurement_started) {
                ++(minfo.num_collision);
              }
#endif
            } else {
              if (std::get<0>(kv_pair) == key) {
#ifdef HASHMAP_COLLECT_META_INFO
                if (minfo.measurement_started) {
                  minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
                  minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
                }
#endif

                return {kv_pair, fingerprint, next_vec_id, true};
              }
#ifdef HASHMAP_COLLECT_META_INFO
              if (minfo.measurement_started) {
                ++(minfo.num_collision);
              }
#endif
            }
          } while (CompareResultIterator::has_next(iterator));
        }
      } else if constexpr (simd_algo == SIMDAlgorithm::NO_TESTZ) {
        MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
        while (CompareResultIterator::has_next(iterator)) {
          uint16_t next_match = 0;

          if constexpr (use_sve) {
            next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
          } else {
            next_match = CompareResultIterator::next_match(iterator);
          }

          iterator = CompareResultIterator::next_it(iterator, next_match);
          const uint64_t next_vec_id = curr_adjusted_idx + next_match;
          DEBUG_ASSERT(next_vec_id < keys_values_.size(),
                       fmt::format("Got invalid next_vec_id {} ({}, {}, {})", next_vec_id, curr_adjusted_idx, next_match, keys_values_.size()));
          const std::pair<KeyT, ValueT>& kv_pair = keys_values_[next_vec_id];

          if constexpr (use_likely_hints) {
            if (HEDLEY_LIKELY(std::get<0>(kv_pair) == key)) [[likely]] {
#ifdef HASHMAP_COLLECT_META_INFO
              if (minfo.measurement_started) {
                minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
                minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
              }
#endif
              return {kv_pair, fingerprint, next_vec_id, true};
            }
#ifdef HASHMAP_COLLECT_META_INFO
            if (minfo.measurement_started) {
              ++(minfo.num_collision);
            }
#endif
          } else {
            if (std::get<0>(kv_pair) == key) {
#ifdef HASHMAP_COLLECT_META_INFO
              if (minfo.measurement_started) {
                minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
                minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
              }
#endif
              return {kv_pair, fingerprint, next_vec_id, true};
            }
#ifdef HASHMAP_COLLECT_META_INFO
            if (minfo.measurement_started) {
              ++(minfo.num_collision);
            }
#endif
          }
        }
      } else {
        FAIL("not supported");
      }

      // There was no true match, but there might have been an invalid fingerprint (even if there was a matching fingerprint)

      MaskOrVectorInputType valid_cmp_result;
      if constexpr (utils::is_power) {
        valid_cmp_result = SIMDH::vector_cmp_func_(next_fingerprints, invalid_compare_vector, false);
      } else {
        valid_cmp_result = SIMDH::vector_cmp_func_(next_fingerprints, invalid_compare_vector);
      }

      // Even though this mnight be NOTESTZ, we check this here
      // It's just more performant and NOTESTZ refers to our iterator, not to this here
      if (SIMDH::vector_any_nonzero(valid_cmp_result)) {
        /* we've reached an invalid fingerprint => abort */
        uint64_t first_invalid_index = 0;

        if constexpr (use_sve) {
          first_invalid_index = SIMDH::sve_vector_first_index(valid_cmp_result, index_vector);
        } else {
          first_invalid_index = SIMDH::vector_first_index(valid_cmp_result);
        }

        const uint64_t index_into_map = curr_adjusted_idx + first_invalid_index;
        DEBUG_ASSERT(first_invalid_index < fps_per_vector_, "First index out of range");

        if constexpr (use_likely_hints) {
          if (HEDLEY_LIKELY(index_into_map < max_elements_)) [[likely]] {
#ifdef HASHMAP_COLLECT_META_INFO
            if (minfo.measurement_started) {
              minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
              minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
            }
#endif
            return {{KeyT{}, ValueT{}}, fingerprint, hasher_.finalize(index_into_map), false};
          }
        } else {
          if (index_into_map < max_elements_) {
#ifdef HASHMAP_COLLECT_META_INFO
            if (minfo.measurement_started) {
              minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
              minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
            }
#endif
            return {{KeyT{}, ValueT{}}, fingerprint, hasher_.finalize(index_into_map), false};
          }
        }
      }

      /* If we had neither a true match nor an invalid fingerprint, we continue the probe */
      curr_idx = next_idx;

      /* However, maybe we have reached the maximum probe length that is required, then we can stop */
      if constexpr (use_likely_hints) {
        if (HEDLEY_UNLIKELY(curr_idx > max_idx)) [[unlikely]] {
#ifdef HASHMAP_COLLECT_META_INFO
          if (minfo.measurement_started) {
            minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
            minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
          }
#endif
          return {{KeyT{}, ValueT{}}, fingerprint, hasher_.finalize(max_idx), false};
        }
      } else {
        if (curr_idx > max_idx) {
#ifdef HASHMAP_COLLECT_META_INFO
          if (minfo.measurement_started) {
            minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
            minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
          }
#endif
          return {{KeyT{}, ValueT{}}, fingerprint, hasher_.finalize(max_idx), false};
        }
      }

      next_idx += fps_per_vector_;

      // In case we cross the boundary of the hashmap (which only happens once, because we interrupt the probe if we probe for too long), we need to
      // start on index 0, to ensure alignedness
      // Cannot use std::min because that would reset every time
      if constexpr (use_likely_hints) {
        if (HEDLEY_UNLIKELY(next_idx > max_elements_ && curr_idx < max_elements_)) [[unlikely]] {
          next_idx = max_elements_;
        }
      } else {
        if (next_idx > max_elements_ && curr_idx < max_elements_) {
          next_idx = max_elements_;
        }
      }

      curr_adjusted_idx = hasher_.finalize(curr_idx);

      DEBUG_ASSERT(curr_adjusted_idx < fingerprints_.size(),
                   fmt::format("Got invalid curr_adjusted_idx {} ({})", curr_adjusted_idx, fingerprints_.size()));

      DEBUG_ASSERT(curr_adjusted_idx % fps_per_vector_ == 0,
                   fmt::format("curr_adjusted_idx = {} is not a multiple of fps_per_vector {}. curr_idx={}, initial_idx={}, max_elements_={}, "
                               "fingerprints_.size()={}",
                               curr_adjusted_idx, fps_per_vector_, curr_idx, bucket_hash.bucket, max_elements_, fingerprints_.size()));

      DEBUG_ASSERT(utils::is_aligned((void*)&fingerprints_[curr_adjusted_idx], SIMDH::_vector_alignment()),
                   fmt::format("Next load of index {} would be unaligned!", curr_adjusted_idx));

#ifdef HASHMAP_COLLECT_META_INFO
      if (minfo.measurement_started) {
        ++(minfo.simd_loads);
      }
#endif
      next_fingerprints = SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&fingerprints_[curr_adjusted_idx]));
    }

    FAIL("This statement should never be reached");
    return {{KeyT{}, ValueT{}}, 0, 0, false};
  }

  void prefault() {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<uint64_t> dist{0, 0xDEADBEEF};

    for (uint64_t i = 0; i < keys_values_.size(); i++) {
      if constexpr (!std::is_same_v<KeyT, StringKey> && !std::is_pointer_v<ValueT> && std::is_arithmetic_v<ValueT>) {
        keys_values_[i] = {dist(gen), dist(gen)};
      }
      fingerprints_[i] = static_cast<FingerprintT>(dist(gen));
    }

    reset();
  }

  void prefault_pregenerated(const std::vector<std::pair<KeyT, ValueT>>& prefault_data) {
    uint64_t prefault_data_size = prefault_data.size();
    for (uint64_t i = 0; i < keys_values_.size(); i++) {
      keys_values_[i] = prefault_data[i % prefault_data_size];
      fingerprints_[i] = 42;
    }

    reset();
  }

  void reset() {
    for (uint64_t i = 0; i < keys_values_.size(); i++) {
      keys_values_[i] = {KeyT(), ValueT()};
      fingerprints_[i] = invalid_fingerprint;
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

    std::string unroll = "NoUnroll";

    std::string value_type = hashmap::utils::data_type_to_str<ValueT>();
    std::string key_type = hashmap::utils::data_type_to_str<KeyT>();
    std::string fingerprint_type = hashmap::utils::data_type_to_str<FingerprintT>();

    std::string algo = "";

    if constexpr (simd_algo == SIMDAlgorithm::TESTZ) {
      algo = "TESTZ";
    } else if constexpr (simd_algo == SIMDAlgorithm::NO_TESTZ) {
      algo = "NO_TESTZ";
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

    std::string likely = "NoLikelyHints";
    if constexpr (use_likely_hints) {
      likely = "LikelyHints";
    }

    std::string fingerprints = "SomeLSBThing";
    if constexpr (fingerprint_bucket_bits == hashing::FingerprintBucketBits::LSBLSB) {
      fingerprints = "LSBLSB";
    } else if constexpr (fingerprint_bucket_bits == hashing::FingerprintBucketBits::MSBLSB) {
      fingerprints = "MSBLSB";
    } else if constexpr (fingerprint_bucket_bits == hashing::FingerprintBucketBits::LSBMSB) {
      fingerprints = "LSBMSB";
    }

    return fmt::format("{}<{}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}>", base_identifier_, hasher_.get_identifier(), key_type,
                       value_type, fingerprint_type, prefetching, thp, unroll, key_simd_type, algo, avx512, compare_result_type, sve, neon_algo_str,
                       sve_broadcast, likely, fingerprints);
  }

  uint64_t get_entry_size() { return 0; }

  bool is_data_aligned_to(size_t alignment) { return utils::is_aligned((void*)fingerprints_.data(), alignment); }

  std::string get_data_pointer_string() { return fmt::format("{}", (void*)fingerprints_.data()); }

  double get_current_load() { return static_cast<double>(size_) / static_cast<double>(max_elements_); }
  uint64_t get_current_size() { return size_; }

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo* get_minfo() { return &minfo; }
  void start_measurement() { minfo.measurement_started = true; }
  void stop_measurement() { minfo.measurement_started = false; }
  void reset_measurement() { minfo.reset(); }
#endif

 protected:
  typedef
      typename std::conditional<use_thp, utils::TransparentHugePageAllocator<std::pair<KeyT, ValueT>>, std::allocator<std::pair<KeyT, ValueT>>>::type
          KeyValueVectorAllocator;
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<FingerprintT, SIMDH::_vector_alignment()>,
                                    utils::AlignedAllocator<FingerprintT, SIMDH::_vector_alignment()>>::type SIMDAlignedAllocator;

  alignas(utils::cacheline_size) std::vector<FingerprintT, SIMDAlignedAllocator> fingerprints_;
  alignas(utils::cacheline_size) std::vector<std::pair<KeyT, ValueT>, KeyValueVectorAllocator> keys_values_;

  alignas(utils::cacheline_size) uint64_t max_elements_;
  alignas(utils::cacheline_size) uint64_t simd_padding_;
  alignas(utils::cacheline_size) uint64_t size_;
  alignas(utils::cacheline_size) HasherT hasher_;
  alignas(utils::cacheline_size) constexpr static uint8_t fps_per_vector_ = (simd_size / 8) / sizeof(FingerprintT);
  std::string base_identifier_;

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo minfo;
#endif
};

}  // namespace hashmap::hashmaps
