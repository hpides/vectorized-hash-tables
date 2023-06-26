#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <utility>

#include "fmt/format.h"
#include "hashmap/hashes/buckethash.hpp"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/simd_utils.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

namespace hashmap::hashmaps {

template <typename KeyT, typename ValueT, typename FingerprintT, typename SIMDH, SIMDAlgorithm simd_algo, uint16_t number_of_fingerprints,
          uint8_t fps_per_vector_, bool use_sve, bool use_likely_hints>
class KeyValueAoSStoringBucket {
 public:
  struct BucketingFindResult {
    std::pair<KeyT, ValueT> key_value = {KeyT(), ValueT()};
    bool is_valid = false;
    KeyValueAoSStoringBucket* target_bucket = nullptr;
    uint16_t index_in_bucket = 0;
    bool can_terminate = false;
  };

  KeyValueAoSStoringBucket(FingerprintT invalid_fingerprint) {
    DEBUG_ASSERT(number_of_fingerprints <= fps_per_vector_,
                 fmt::format("Number of fingerprints {} not smaller than number of elements that fit in a SIMD register = {}", number_of_fingerprints,
                             fps_per_vector_));

    std::fill(std::begin(fingerprints_), std::end(fingerprints_), invalid_fingerprint);  // Just to avoid garbage memory in the fingerprint_ array
    std::fill(std::begin(keys_values_), std::end(keys_values_),
              std::make_pair(KeyT(), ValueT()));  // Just to avoid garbage memory in the keys_values_ array
  }

  using FindResult = BucketingFindResult;
  using vector_type = typename SIMDH::vector_type;
  using _vector_type = typename SIMDH::_vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using MaskType = typename SIMDH::MaskType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  void prefault() {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<uint64_t> dist{0, 0xDEADBEEF};

    for (uint64_t i = 0; i < fingerprints_.size(); ++i) {
      fingerprints_[i] = static_cast<FingerprintT>(dist(gen));
      if constexpr (!std::is_same_v<KeyT, StringKey> && !std::is_pointer_v<ValueT>) {
        keys_values_[i] = {static_cast<KeyT>(dist(gen)), static_cast<ValueT>(dist(gen))};
      }
    }
  }

  void prefault_pregenerated(const std::vector<std::pair<KeyT, ValueT>>& prefault_data) {
    uint64_t prefault_data_size = prefault_data.size();
    for (uint64_t i = 0; i < keys_values_.size(); i++) {
      keys_values_[i] = prefault_data[i % prefault_data_size];
      fingerprints_[i] = 42;
    }
  }

  void reset(FingerprintT invalid_fingerprint) {
    for (uint64_t i = 0; i < fingerprints_.size(); ++i) {
      fingerprints_[i] = invalid_fingerprint;
      keys_values_[i] = {KeyT(), ValueT()};
    }
  }

#ifdef HASHMAP_COLLECT_META_INFO
  HEDLEY_ALWAYS_INLINE FindResult find(const KeyT& key, const FingerprintT& fingerprint, utils::MeasurementInfo* minfo) {
    if (minfo->measurement_started) {
      ++(minfo->probed_elements);
      ++(minfo->simd_loads);
      minfo->probed_elements_total += number_of_fingerprints;
    }
#else
  HEDLEY_ALWAYS_INLINE FindResult find(const KeyT& key, const FingerprintT& fingerprint) {
#endif

    vector_type index_vector;
#if defined(__ARM_FEATURE_SVE)
    if constexpr (use_sve) {
      index_vector = SIMDH::index_creation_func_();
    }
#else
    (void)index_vector;
#endif

    const vector_type fingerprints = SIMDH::vector_load_func_(reinterpret_cast<LoadPtrT>(&fingerprints_[0]));
    const CompareT compare_vector = SIMDH::vector_broadcast_func_(fingerprint);

    MaskOrVectorInputType cmp_result;
    if constexpr (utils::is_power) {
      cmp_result = SIMDH::vector_cmp_func_(fingerprints, compare_vector, false);
    } else {
      cmp_result = SIMDH::vector_cmp_func_(fingerprints, compare_vector);
    }

    if constexpr (simd_algo == SIMDAlgorithm::TESTZ) {
      if (SIMDH::vector_any_nonzero(cmp_result)) {
        MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
        do {
          uint16_t next_match = 0;

          if constexpr (use_sve) {
            next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
          } else {
            next_match = CompareResultIterator::next_match(iterator);
          }

          iterator = CompareResultIterator::next_it(iterator, next_match);
          const std::pair<KeyT, ValueT>& kv_pair = keys_values_[next_match];

          if constexpr (use_likely_hints) {
            if (HEDLEY_LIKELY(std::get<0>(kv_pair) == key)) [[likely]] {
              return {kv_pair, true, this, next_match, true};
            }
#ifdef HASHMAP_COLLECT_META_INFO
            if (minfo->measurement_started) {
              ++(minfo->num_collision);
            }
#endif
          } else {
            if (std::get<0>(kv_pair) == key) {
              return {kv_pair, true, this, next_match, true};
            }
#ifdef HASHMAP_COLLECT_META_INFO
            if (minfo->measurement_started) {
              ++(minfo->num_collision);
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
        const std::pair<KeyT, ValueT>& kv_pair = keys_values_[next_match];

        if constexpr (use_likely_hints) {
          if (HEDLEY_LIKELY(std::get<0>(kv_pair) == key)) [[likely]] {
            return {kv_pair, true, this, next_match, true};
          }
#ifdef HASHMAP_COLLECT_META_INFO
          if (minfo->measurement_started) {
            ++(minfo->num_collision);
          }
#endif
        } else {
          if (std::get<0>(kv_pair) == key) {
            return {kv_pair, true, this, next_match, true};
          }
#ifdef HASHMAP_COLLECT_META_INFO
          if (minfo->measurement_started) {
            ++(minfo->num_collision);
          }
#endif
        }
      }

    } else {
      FAIL("not supported");
    }

    // No match within the bucket, we can check next bucket in case we overflowed
    // However, first we check if we can terminate due to reaching the maximum limit or not overflowing

    if constexpr (use_likely_hints) {
      if (HEDLEY_LIKELY(!overflowed_)) [[likely]] {
        return {{KeyT(), ValueT()}, false, this, 0, true};
      }
    } else {
      if (!overflowed_) {
        return {{KeyT(), ValueT()}, false, this, 0, true};
      }
    }
#ifdef HASHMAP_COLLECT_META_INFO
    if (minfo->measurement_started) {
      ++(minfo->num_overflows_followed);
    }
#endif
    // We overflowed, tell to not terminate search
    return {{KeyT(), ValueT()}, false, this, 0, false};
  }

#ifdef HASHMAP_COLLECT_META_INFO
  HEDLEY_ALWAYS_INLINE bool insert(const KeyT& key, const ValueT& value, const FingerprintT& fingerprint, utils::MeasurementInfo* minfo) {
#else
  HEDLEY_ALWAYS_INLINE bool insert(const KeyT& key, const ValueT& value, const FingerprintT& fingerprint) {
#endif
    if constexpr (use_likely_hints) {
      if (HEDLEY_LIKELY(next_free_entry < number_of_fingerprints)) [[likely]] {
        fingerprints_[next_free_entry] = fingerprint;
        keys_values_[next_free_entry++] = {key, value};
        return true;
      } else {
#ifdef HASHMAP_COLLECT_META_INFO
        if (!overflowed_) {
          ++(minfo->num_overflows);
        }
#endif

        overflowed_ = true;
        return false;
      }
    } else {
      if (next_free_entry < number_of_fingerprints) {
        fingerprints_[next_free_entry] = fingerprint;
        keys_values_[next_free_entry++] = {key, value};
        return true;
      } else {
#ifdef HASHMAP_COLLECT_META_INFO
        if (!overflowed_) {
          ++(minfo->num_overflows);
        }
#endif

        overflowed_ = true;
        return false;
      }
    }

    FAIL("This statement should never be reached.");

    return false;
  }

  HEDLEY_ALWAYS_INLINE void update(const KeyT& key, const ValueT& value, uint16_t index_in_bucket) {
    DEBUG_ASSERT(std::get<0>(keys_values_[index_in_bucket]) == key, "Invalid update call");
    keys_values_[index_in_bucket] = {key, value};
  }

  bool is_overflowed() { return overflowed_; }

  static std::string to_string() { return "KeyValueAoSStoringBucket"; }

 private:
  uint8_t next_free_entry = 0;
  bool overflowed_ = false;  // only if this is true, we will need to follow to next_bucket in case we don't find it within our chunk
  alignas(SIMDH::_vector_alignment()) std::array<FingerprintT, fps_per_vector_> fingerprints_;

  std::array<std::pair<KeyT, ValueT>, fps_per_vector_> keys_values_;
};  //  __attribute__((__packed__)); ?

template <
    typename KeyT, typename ValueT, typename HasherT, typename FingerprintT = uint16_t,
    template <typename, typename, typename, typename, SIMDAlgorithm, uint16_t, uint8_t, bool, bool> typename BucketTmpl = KeyValueAoSStoringBucket,
    uint16_t fingerprints_per_bucket = 8, uint16_t simd_size = 128, SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ, bool use_avx512_features = false,
    bool use_sve = false, NEONAlgo neon_algo = NEONAlgo::SSE2NEON, bool sve_scalar_broadcast = false, bool use_prefetching = false,
    utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false, bool use_likely_hints = false,
    hashing::FingerprintBucketBits fingerprint_bucket_bits = hashing::FingerprintBucketBits::MSBLSB, FingerprintT invalid_fingerprint = 0>
class BucketingSIMDHashTable : public HashTable<KeyT, ValueT> {
 public:
  BucketingSIMDHashTable(uint64_t max_elements, uint8_t /*target_load_factor*/, bool print_info = true,
                         std::string base_identifier = "BucketingSIMDHashTable")
      : num_buckets_{std::max((max_elements / fingerprints_per_bucket), static_cast<uint64_t>(1))},
        buckets_(num_buckets_, BucketT(invalid_fingerprint)),
        max_elements_{max_elements},
        size_{0},
        hasher_(num_buckets_),
        base_identifier_{base_identifier} {
    ASSERT(fingerprints_per_bucket * sizeof(FingerprintT) * 8 <= simd_size,
           fmt::format("{} fingerprints per bucket require {} bit vector registers, simd size is given as {} though!", fingerprints_per_bucket,
                       fingerprints_per_bucket * sizeof(FingerprintT) * 8, simd_size));

    if (print_info) {
      spdlog::info(
          fmt::format("Initialized {} with utils::cacheline_size = {}, sizeof(EntryT) = XX, num_buckets = {},"
                      "alignof(EntryT) = XX, simd_size = {},  vector_alignment_func={}",
                      get_identifier(), utils::cacheline_size, num_buckets_, simd_size, SIMDH::_vector_alignment()));
    }

    ASSERT(utils::is_power_of_two(max_elements) && utils::is_power_of_two(fingerprints_per_bucket),
           fmt::format("Bucketing is optimized towards hash maps that have a maximum size which is a power of two, this hash "
                       "map is of size {}.",
                       max_elements));

    fail_if_system_is_incompatible<FingerprintT, simd_size, simd_algo, use_avx512_features, use_sve, neon_algo, sve_scalar_broadcast>();

#ifdef HASHMAP_COLLECT_META_INFO
    minfo.num_buckets = num_buckets_;
#endif
  }

  using SIMDH = SIMDHelper<FingerprintT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using BucketT = BucketTmpl<KeyT, ValueT, FingerprintT, SIMDH, simd_algo, fingerprints_per_bucket, (simd_size / 8) / sizeof(FingerprintT), use_sve,
                             use_likely_hints>;
  using BucketingFindResult = typename BucketT::BucketingFindResult;

  struct FindResult {
    BucketingFindResult res;
    uint32_t bucket_idx;
    uint32_t probe_length;
  };

  bool contains(const KeyT& key) {
    const FindResult& result = find(key);
    return result.res.is_valid;
  }

  ValueT lookup(const KeyT& key) {
    const FindResult& result = find(key);
    if constexpr (std::is_pointer_v<ValueT>) {
      return result.res.is_valid ? std::get<1>(result.res.key_value) : nullptr;
    } else {
      return result.res.is_valid ? std::get<1>(result.res.key_value) : ValueT{};
    }
  };

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(size_ + 1 <= max_elements_, "Hashmap is full!");

    const hashing::BucketHash<FingerprintT> bucket_hash =
        hasher_.template bucket_hash<FingerprintT, fingerprint_bucket_bits, invalid_fingerprint>(key);
    uint32_t bucket_idx = static_cast<uint32_t>(bucket_hash.bucket);
    const FingerprintT fingerprint = bucket_hash.fingerprint;

    FindResult result;
    // Check if element already exists in hashmap; if so, update
    if constexpr (use_likely_hints) {
      if (HEDLEY_UNLIKELY((result = find_impl(key, fingerprint, bucket_idx)).res.is_valid)) [[unlikely]] {
        return result.res.target_bucket->update(key, value, result.res.index_in_bucket);
      }
    } else {
      if ((result = find_impl(key, fingerprint, bucket_idx)).res.is_valid) {
        return result.res.target_bucket->update(key, value, result.res.index_in_bucket);
      }
    }

    uint32_t bucket_number = result.probe_length;
    bucket_idx = result.bucket_idx;

    for (; bucket_number <= num_buckets_; ++bucket_number) {
#ifdef HASHMAP_COLLECT_META_INFO
      const bool insert_sucessful = buckets_[bucket_idx].insert(key, value, fingerprint, &minfo);
#else
      const bool insert_sucessful = buckets_[bucket_idx].insert(key, value, fingerprint);
#endif

      if constexpr (use_likely_hints) {
        if (HEDLEY_LIKELY(insert_sucessful)) [[likely]] {
          ++size_;
          max_bucket_chain = std::max(max_bucket_chain, static_cast<uint64_t>(bucket_number));
          return;
        }
      } else {
        if (insert_sucessful) {
          ++size_;
          max_bucket_chain = std::max(max_bucket_chain, static_cast<uint64_t>(bucket_number));
          return;
        }
      }

      if constexpr (use_likely_hints) {
        if (HEDLEY_LIKELY(bucket_idx < num_buckets_ - 1)) [[likely]] {
          ++bucket_idx;
        } else {
          bucket_idx = 0;
        }
      } else {
        bucket_idx = (bucket_idx == num_buckets_ - 1) ? 0 : bucket_idx + 1;
      }
    }
  }

  FindResult find(const KeyT& key) {
#ifdef HASHMAP_COLLECT_META_INFO
    if (minfo.measurement_started) {
      ++(minfo.num_finds);
    }
#endif

    const hashing::BucketHash<FingerprintT> bucket_hash =
        hasher_.template bucket_hash<FingerprintT, fingerprint_bucket_bits, invalid_fingerprint>(key);
    uint32_t bucket_idx = static_cast<uint32_t>(bucket_hash.bucket);
    const FingerprintT fingerprint = bucket_hash.fingerprint;

    if constexpr (std::is_same_v<KeyT, StringKey>) {
      DEBUG_ASSERT(bucket_idx < buckets_.size(), fmt::format("For key {}, I got bucket idx {} and fingerprint {}, however, we only have {} buckets!",
                                                             std::string(key.string_), bucket_idx, fingerprint, buckets_.size()));

    } else {
      DEBUG_ASSERT(bucket_idx < buckets_.size(), fmt::format("For key {}, I got bucket idx {} and fingerprint {}, however, we only have {} buckets!",
                                                             key, bucket_idx, fingerprint, buckets_.size()));
    }
    return find_impl(key, fingerprint, bucket_idx);
  }

  HEDLEY_ALWAYS_INLINE FindResult find_impl(const KeyT& key, const FingerprintT& fingerprint, uint32_t bucket_idx) {
    uint32_t bucket_number = 0;

#ifdef HASHMAP_COLLECT_META_INFO
    uint64_t probing_seq_len = 0;
#endif

    for (bucket_number = 0; bucket_number <= max_bucket_chain; ++bucket_number) {
#ifdef HASHMAP_COLLECT_META_INFO
      ++probing_seq_len;
      const BucketingFindResult res = buckets_[bucket_idx].find(key, fingerprint, &minfo);
#else
      const BucketingFindResult res = buckets_[bucket_idx].find(key, fingerprint);
#endif
      if constexpr (use_likely_hints) {
        if (HEDLEY_LIKELY(res.can_terminate)) [[likely]] {
#ifdef HASHMAP_COLLECT_META_INFO
          if (minfo.measurement_started) {
            minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
            minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
          }
#endif

          return {res, bucket_idx, bucket_number};
        }
      } else {
        if (res.can_terminate) {
#ifdef HASHMAP_COLLECT_META_INFO
          if (minfo.measurement_started) {
            minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
            minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
          }
#endif

          return {res, bucket_idx, bucket_number};
        }
      }

      if constexpr (use_likely_hints) {
        if (HEDLEY_LIKELY(bucket_idx < num_buckets_ - 1)) [[likely]] {
          ++bucket_idx;
        } else {
          bucket_idx = 0;
        }
      } else {
        bucket_idx = (bucket_idx == num_buckets_ - 1) ? 0 : bucket_idx + 1;
      }
    }
    if constexpr (std::is_pointer_v<ValueT>) {
      return {{{KeyT(), nullptr}, false, nullptr, 0, true}, bucket_idx, bucket_number};
    } else {
      return {{{KeyT(), ValueT()}, false, nullptr, 0, true}, bucket_idx, bucket_number};
    }
  }

  void prefault() {
    for (auto& bucket : buckets_) {
      bucket.prefault();
    }

    reset();
  }

  void prefault_pregenerated(const std::vector<std::pair<KeyT, ValueT>>& prefault_data) {
    for (auto& bucket : buckets_) {
      bucket.prefault_pregenerated(prefault_data);
    }

    reset();
  }

  void reset() {
    for (auto& bucket : buckets_) {
      bucket.reset(invalid_fingerprint);
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

    std::string fallback = "NoFallback";
#ifdef HASHMAP_USE_SCALAR_IMPL
    fallback = fmt::format("Fallback{}", simd_size);
#endif

    std::string fingerprints = "SomeLSBThing";
    if constexpr (fingerprint_bucket_bits == hashing::FingerprintBucketBits::LSBLSB) {
      fingerprints = "LSBLSB";
    } else if constexpr (fingerprint_bucket_bits == hashing::FingerprintBucketBits::MSBLSB) {
      fingerprints = "MSBLSB";
    } else if constexpr (fingerprint_bucket_bits == hashing::FingerprintBucketBits::LSBMSB) {
      fingerprints = "LSBMSB";
    }

    std::string bucket_type = BucketT::to_string();
    std::string fps_per_bucket = fmt::format("{}FPPB", fingerprints_per_bucket);

    return fmt::format("{}<{}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}; {}>", base_identifier_, hasher_.get_identifier(),
                       key_type, value_type, fingerprint_type, prefetching, thp, unroll, key_simd_type, algo, avx512, compare_result_type, sve,
                       neon_algo_str, sve_broadcast, likely, bucket_type, fps_per_bucket, fingerprints, fallback);
  }

  uint64_t get_entry_size() { return 0; }

  bool is_data_aligned_to(size_t alignment) { return utils::is_aligned((void*)buckets_.data(), alignment); }

  std::string get_data_pointer_string() { return fmt::format("{}", (void*)buckets_.data()); }

  double get_current_load() { return static_cast<double>(size_) / static_cast<double>(max_elements_); }

  uint64_t get_num_buckets() { return num_buckets_; }
  BucketT* get_ith_bucket(uint32_t i) { return &buckets_[i]; }
  uint64_t get_current_size() { return size_; }

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo* get_minfo() { return &minfo; }
  void start_measurement() { minfo.measurement_started = true; }
  void stop_measurement() { minfo.measurement_started = false; }
  void reset_measurement() { minfo.reset(); }
#endif

 protected:
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<BucketT>, std::allocator<BucketT>>::type BucketVectorAllocator;
  uint64_t num_buckets_;  // = 1 + (max_elements / fingerprints_per_bucket);

  alignas(utils::cacheline_size) std::vector<BucketT, BucketVectorAllocator> buckets_;

  alignas(utils::cacheline_size) uint64_t max_elements_;
  alignas(utils::cacheline_size) uint64_t size_;
  alignas(utils::cacheline_size) HasherT hasher_;
  alignas(utils::cacheline_size) constexpr static uint8_t fps_per_vector_ = (simd_size / 8) / sizeof(FingerprintT);
  std::string base_identifier_;
  uint64_t max_bucket_chain = 0;

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo minfo;
#endif
};

}  // namespace hashmap::hashmaps
