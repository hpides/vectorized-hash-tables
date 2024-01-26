#pragma once
#include <cstdint>

/* Reproducibility Hashmaps */

namespace benchmark {

uint64_t get_num_hashmaps_rep() {
#define BASIC_READ_BENCHMARK_HASHMAPS                                                                                                  \
  hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::MEDIUM, true>,          \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>, \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, DefaultHasher, true>,                                                    \
      hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, true, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,              \
      hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, true, hashmap::hashmaps::MemoryBudget::Bucket16FP, 10, true>

  uint64_t num_hashmaps = 5;

#define EXTERNAL_READ_BENCHMARK_HASHMAPS BASIC_READ_BENCHMARK_HASHMAPS

#define BASIC_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                           \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false,        \
                                      utils::PrefetchingLocality::NO, true>,                                                                         \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,    \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>,    \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,    \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>,    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::LSBMSB>

  num_hashmaps += 12;

#ifdef __AVX2__
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                        \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false,      \
                                      utils::PrefetchingLocality::NO, true>,                                                                       \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>,  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>,  \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,           \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,             \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false,            \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,           \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false,            \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,             \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                    \
      BASIC_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 15;

#else
#if defined(HASHMAP_IS_ARM) || defined(HASHMAP_IS_POWER)

#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                       \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV, false, false,        \
                                      utils::PrefetchingLocality::NO, true>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>, \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                   \
      BASIC_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 8;

#else
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS BASIC_SIMD_READ_BENCHMARK_HASHMAPS
#endif
#endif

#ifdef __AVX512F__
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                         \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,      \
                                      utils::PrefetchingLocality::NO, true>,                                                                      \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,  \
                                          utils::PrefetchingLocality::NO, true>,                                                                  \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,  \
                                          utils::PrefetchingLocality::NO, true>,                                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>, \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>, \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,             \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,             \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,  \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,  \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,             \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,             \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,  \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,  \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,             \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,             \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                   \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 26;

#else

#if defined(HASHMAP_IS_ARM) && defined(__ARM_FEATURE_SVE)
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                           \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, \
                                      false, false, utils::PrefetchingLocality::NO, true>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,  \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                        \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                             \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,  \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                        \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                             \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true>,                       \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                        \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                             \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                        \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,     \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true>,                                                                                                 \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,      \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true>,                                                                                                 \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,     \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                         \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,      \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                         \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 10;

#else
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS INTERM_SIMD_READ_BENCHMARK_HASHMAPS
#endif
#endif

#ifndef HASHMAP_ONLY_BENCH_EXTERNAL
#define BENCHMARK_HASHMAPS_REP FULL_SIMD_READ_BENCHMARK_HASHMAPS, EXTERNAL_READ_BENCHMARK_HASHMAPS
#else
#define BENCHMARK_HASHMAPS_REP EXTERNAL_READ_BENCHMARK_HASHMAPS
#endif
  return num_hashmaps;
}

}  // namespace benchmark
