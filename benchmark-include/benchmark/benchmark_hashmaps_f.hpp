#pragma once
#include <cstdint>

// Hashmaps for stringkeys

namespace benchmark {

uint64_t get_num_hashmaps_f() {
#define BASIC_READ_BENCHMARK_HASHMAPS                                                                                                  \
  hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>,              \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>,                \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>, \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, DefaultHasher, true>,                                                    \
      hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, true, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,              \
      hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, true, hashmap::hashmaps::MemoryBudget::Bucket8FP, 10, true>,             \
      hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, true, hashmap::hashmaps::MemoryBudget::Bucket16FP, 10, true>

  uint64_t num_hashmaps = 7;

#ifdef HASHMAP_BUILD_EXTERNAL

#ifdef HASHMAP_ONLY_BENCH_EXTERNAL
#undef BASIC_READ_BENCHMARK_HASHMAPS
  num_hashmaps = 1;
#define BASIC_READ_BENCHMARK_HASHMAPS hashmaps::AutoPaddedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>
#endif

#ifdef HASHMAP_ONLY_BENCH_SIMD
#undef BASIC_READ_BENCHMARK_HASHMAPS
  num_hashmaps = 1;
#define BASIC_READ_BENCHMARK_HASHMAPS hashmaps::AutoPaddedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>
#endif

#define BASIC_EXTERNAL_READ_BENCHMARK_HASHMAPS                                                                                \
  hashmaps::AbseilFlatHashTable<KeyT, ValueT, DefaultHasher>, hashmaps::MartinusRHFlatHashTable<KeyT, ValueT, DefaultHasher>, \
      BASIC_READ_BENCHMARK_HASHMAPS
  num_hashmaps += 2;

#ifdef HASHMAP_IS_POWER
#define EXTERNAL_READ_BENCHMARK_HASHMAPS BASIC_EXTERNAL_READ_BENCHMARK_HASHMAPS
#else
#define EXTERNAL_READ_BENCHMARK_HASHMAPS hashmaps::F14ValueHashTable<KeyT, ValueT, DefaultHasher>, BASIC_EXTERNAL_READ_BENCHMARK_HASHMAPS
  num_hashmaps++;
#endif

#else
#define EXTERNAL_READ_BENCHMARK_HASHMAPS BASIC_READ_BENCHMARK_HASHMAPS
#endif

#define BASIC_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                           \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, \
                                           false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>

  num_hashmaps += 3;

#ifdef __AVX2__
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                          \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, \
                                           false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,     \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      BASIC_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 3;

#else
#if defined(HASHMAP_IS_ARM) || defined(HASHMAP_IS_POWER)

#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                       \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV, false, \
                                           false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                   \
      BASIC_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 3;

#else
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS BASIC_SIMD_READ_BENCHMARK_HASHMAPS
#endif
#endif

#ifdef __AVX512F__
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                           \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, \
                                           false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                     \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                     \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                     \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                     \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,    \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                     \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                     \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 3;

#else

#if defined(HASHMAP_IS_ARM) && defined(__ARM_FEATURE_SVE)
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                       \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                           NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                        \
                                           hashing::FingerprintBucketBits::MSBLSB>,                                                             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size, \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                     \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,  \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                     \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 3;

#else
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS INTERM_SIMD_READ_BENCHMARK_HASHMAPS
#endif
#endif

#define BENCHMARK_HASHMAPS_F FULL_SIMD_READ_BENCHMARK_HASHMAPS, EXTERNAL_READ_BENCHMARK_HASHMAPS

  return num_hashmaps;
}

}  // namespace benchmark
