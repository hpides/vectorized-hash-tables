#pragma once
#include <cstdint>

/* READ BENCHMARK HASHMAPS B */
/* Used to collect meta information */

namespace benchmark {

uint64_t get_num_hashmaps_b() {
#define BASIC_READ_BENCHMARK_HASHMAPS                                                                                                  \
  hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>,              \
      hashmaps::UnalignedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>,       \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>, \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>

  uint64_t num_hashmaps = 4;

#define EXTERNAL_READ_BENCHMARK_HASHMAPS BASIC_READ_BENCHMARK_HASHMAPS

#define BASIC_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                         \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false,      \
                                      utils::PrefetchingLocality::NO, true>,                                                                       \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                    \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>

  num_hashmaps += 7;

#ifdef __AVX2__
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                        \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false,      \
                                      utils::PrefetchingLocality::NO, true>,                                                                       \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                    \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                    \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,   \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      BASIC_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 7;

#else
// Other variants (UMINV, AARCH64, ...) do not change probing length, just performance
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS BASIC_SIMD_READ_BENCHMARK_HASHMAPS
#endif

#ifdef __AVX512F__
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                         \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,      \
                                      utils::PrefetchingLocality::NO, true>,                                                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                               \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                   \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                               \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,  \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,            \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 7;

#else

#if defined(HASHMAP_IS_ARM) && defined(__ARM_FEATURE_SVE)
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                            \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON,  \
                                      true, false, utils::PrefetchingLocality::NO, true>,                                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                               \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,    \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                               \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true, true, true>,                   \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,    \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true, true, true>,                   \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,      \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::MEDIUM, true, \
                                       true>,                                                                                                        \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,       \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::MEDIUM, true, \
                                       true>,                                                                                                        \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 7;

#else
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS INTERM_SIMD_READ_BENCHMARK_HASHMAPS
#endif
#endif

#define BENCHMARK_HASHMAPS_B FULL_SIMD_READ_BENCHMARK_HASHMAPS, EXTERNAL_READ_BENCHMARK_HASHMAPS

  return num_hashmaps;
}

}  // namespace benchmark
