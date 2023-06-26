#pragma once
#include <cstdint>

/* READ BENCHMARK HASHMAPS D */
/* Hash maps for hash function/dense key hash funciton benchmarks */

namespace benchmark {

/* Hash functions we consider :
    MultShift (64B) - in other benchmarks
    MultShift (64B) - mit Modulo statt Bitmask
    MultShift (128B)
    MultAddShift (64B)
    MultAddShift (128B)
    MurmurHash3
    xxHash
    xxHash - mit Modulo statt Bitmask
*/

uint64_t get_num_hashmaps_d() {
#define BASIC_READ_BENCHMARK_HASHMAPS                                                                                                          \
  hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, MultShift64ModuloHasher, false, utils::PrefetchingLocality::NO, true>,            \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, MultShift128Hasher, false, utils::PrefetchingLocality::NO, true>,             \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, MultShift128Hasher, false, utils::PrefetchingLocality::NO, true>,                   \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, MultShift128Hasher, false, utils::PrefetchingLocality::NO, true>,    \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, MultShift128Hasher, false, utils::PrefetchingLocality::NO, true>,    \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, MultShift128Hasher, true>,                                                       \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, MultAddShift64Hasher, false, utils::PrefetchingLocality::NO, true>,           \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, MultAddShift64Hasher, false, utils::PrefetchingLocality::NO, true>,                 \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, MultAddShift64Hasher, false, utils::PrefetchingLocality::NO, true>,  \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, true>,                                                     \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, MultAddShift128Hasher, false, utils::PrefetchingLocality::NO, true>,          \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, MultAddShift128Hasher, false, utils::PrefetchingLocality::NO, true>,                \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, MultAddShift128Hasher, false, utils::PrefetchingLocality::NO, true>, \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, true>,                                                    \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, MurmurHasher, false, utils::PrefetchingLocality::NO, true>,                   \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, MurmurHasher, false, utils::PrefetchingLocality::NO, true>,                         \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, MurmurHasher, false, utils::PrefetchingLocality::NO, true>,          \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, MurmurHasher, true>,                                                             \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, xxHasher, false, utils::PrefetchingLocality::NO, true>,                       \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, xxHasher, false, utils::PrefetchingLocality::NO, true>,                             \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, xxHasher, false, utils::PrefetchingLocality::NO, true>,              \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, xxHasher, true>,                                                                 \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultShift128Hasher, true, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,                 \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultShift128Hasher, true, hashmap::hashmaps::MemoryBudget::Bucket8FP, 10, true>,                \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultShift128Hasher, true, hashmap::hashmaps::MemoryBudget::Bucket16FP, 10, true>,               \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultAddShift64Hasher, true, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,               \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultAddShift64Hasher, true, hashmap::hashmaps::MemoryBudget::Bucket8FP, 10, true>,              \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultAddShift64Hasher, true, hashmap::hashmaps::MemoryBudget::Bucket16FP, 10, true>,             \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultAddShift128Hasher, true, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,              \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultAddShift128Hasher, true, hashmap::hashmaps::MemoryBudget::Bucket8FP, 10, true>,             \
      hashmaps::ChainedHashTable<KeyT, ValueT, MultAddShift128Hasher, true, hashmap::hashmaps::MemoryBudget::Bucket16FP, 10, true>,            \
      hashmaps::ChainedHashTable<KeyT, ValueT, MurmurHasher, true, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,                       \
      hashmaps::ChainedHashTable<KeyT, ValueT, MurmurHasher, true, hashmap::hashmaps::MemoryBudget::Bucket8FP, 10, true>,                      \
      hashmaps::ChainedHashTable<KeyT, ValueT, MurmurHasher, true, hashmap::hashmaps::MemoryBudget::Bucket16FP, 10, true>,                     \
      hashmaps::ChainedHashTable<KeyT, ValueT, xxHasher, true, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,                           \
      hashmaps::ChainedHashTable<KeyT, ValueT, xxHasher, true, hashmap::hashmaps::MemoryBudget::Bucket8FP, 10, true>,                          \
      hashmaps::ChainedHashTable<KeyT, ValueT, xxHasher, true, hashmap::hashmaps::MemoryBudget::Bucket16FP, 10, true>

  uint64_t num_hashmaps = 16 + 15;

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
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                           false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>,        \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,                   \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ, \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ, \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ, \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ, \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,                 \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,                 \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128,                     \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128,                     \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128,                     \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128,                     \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,                \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,                \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128,                    \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128,                    \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128,                    \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128,                    \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MurmurHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MurmurHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,       \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MurmurHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,       \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, xxHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false,  \
                                               false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,           \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, xxHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ, false,    \
                                       false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                      \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, xxHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ, false,    \
                                       false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                      \
                                       hashing::FingerprintBucketBits::MSBLSB>

  num_hashmaps += 3 * 7;

#ifdef __AVX2__
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                          \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                           false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>,        \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256,                      \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ, \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,                   \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256,                      \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ, \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,                 \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256,                    \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256,                     \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,                 \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256,                    \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256,                     \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,                \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256,                   \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256,                    \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,                \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256,                   \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256,                    \
                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,     \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MurmurHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MurmurHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MurmurHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,       \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,               \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, xxHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false,  \
                                               false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,           \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, xxHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ, false,   \
                                       false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                      \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, xxHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ, false,    \
                                       false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                      \
                                       hashing::FingerprintBucketBits::MSBLSB>

#else
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS BASIC_SIMD_READ_BENCHMARK_HASHMAPS
#endif

#ifdef __AVX512F__
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                            \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                           false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::LSBMSB>,        \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512,                      \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ, \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                \
                                       hashing::FingerprintBucketBits::LSBMSB>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,                    \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512,                      \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ, \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,                  \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512,                    \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512,                     \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,                  \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512,                    \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512,                     \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,                 \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512,                   \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512,                    \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,                 \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512,                   \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512,                    \
                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MurmurHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,      \
                                               false, false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MurmurHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MurmurHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ, true, \
                                       false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                      \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, xxHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false,   \
                                               false, utils::PrefetchingLocality::NO, true, true, hashing::FingerprintBucketBits::MSBLSB>,           \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, xxHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ, true,    \
                                       false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                      \
                                       hashing::FingerprintBucketBits::MSBLSB>,                                                                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, xxHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ, true,     \
                                       false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true,                      \
                                       hashing::FingerprintBucketBits::MSBLSB>

#else

#if defined(HASHMAP_IS_ARM) && defined(__ARM_FEATURE_SVE)
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                            \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                           NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                             \
                                           hashing::FingerprintBucketBits::MSBLSB>,                                                                  \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size, \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,  \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false,     \
                                               true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                   \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size, \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,  \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::LSBMSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false,   \
                                               true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                   \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32,                         \
                                       utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false,                \
                                       utils::PrefetchingLocality::MEDIUM, true, true, hashing::FingerprintBucketBits::MSBLSB>,                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64,                          \
                                       utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false,                \
                                       utils::PrefetchingLocality::MEDIUM, true, true, hashing::FingerprintBucketBits::MSBLSB>,                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false,   \
                                               true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                   \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32,                         \
                                       utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false,                \
                                       utils::PrefetchingLocality::MEDIUM, true, true, hashing::FingerprintBucketBits::LSBMSB>,                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift64Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64,                          \
                                       utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false,                \
                                       utils::PrefetchingLocality::MEDIUM, true, true, hashing::FingerprintBucketBits::LSBMSB>,                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false,  \
                                               true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                   \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32,                        \
                                       utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false,                \
                                       utils::PrefetchingLocality::MEDIUM, true, true, hashing::FingerprintBucketBits::MSBLSB>,                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64,                         \
                                       utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false,                \
                                       utils::PrefetchingLocality::MEDIUM, true, true, hashing::FingerprintBucketBits::MSBLSB>,                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false,  \
                                               true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                   \
                                               hashing::FingerprintBucketBits::LSBMSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32,                        \
                                       utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false,                \
                                       utils::PrefetchingLocality::MEDIUM, true, true, hashing::FingerprintBucketBits::LSBMSB>,                      \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MultAddShift128Hasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64,                         \
                                       utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false,                \
                                       utils::PrefetchingLocality::MEDIUM, true, true, hashing::FingerprintBucketBits::LSBMSB>,                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, MurmurHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,     \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MurmurHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,       \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, MurmurHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,        \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, xxHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,         \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true,                         \
                                               hashing::FingerprintBucketBits::MSBLSB>,                                                              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, xxHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,           \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, xxHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,            \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true, hashing::FingerprintBucketBits::MSBLSB>,                                                          \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 3 * 7;

#else
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS INTERM_SIMD_READ_BENCHMARK_HASHMAPS
#endif
#endif

#define BENCHMARK_HASHMAPS_D FULL_SIMD_READ_BENCHMARK_HASHMAPS, EXTERNAL_READ_BENCHMARK_HASHMAPS

  return num_hashmaps;
}

}  // namespace benchmark
