#pragma once
#include <cstdint>

/* READ BENCHMARK HASHMAPS A */
/* Used for the initial technique benchmarks */

namespace benchmark {

uint64_t get_num_hashmaps_a() {
#define BASIC_READ_BENCHMARK_HASHMAPS                                                                                                     \
  hashmaps::AutoPaddedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                                      \
      hashmaps::NaturalAlignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                              \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                                   \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>,             \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true, 4>,          \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true, 8>,          \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true>,              \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true>,             \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true>,          \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true>,            \
      hashmaps::AutoPaddedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                               \
      hashmaps::NaturalAlignedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                           \
      hashmaps::UnalignedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                                \
      hashmaps::UnalignedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>,          \
      hashmaps::UnalignedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO>, true,           \
      hashmaps::UnalignedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true>,          \
      hashmaps::UnalignedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true>,       \
      hashmaps::UnalignedQuadraticProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true>,         \
      hashmaps::AutoPaddedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                         \
      hashmaps::NaturalAlignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                     \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                          \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>,    \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true>,     \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true>,    \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true>, \
      hashmaps::UnalignedRecalculatingRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true>,   \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false>,                                                         \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>,                   \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true>,                    \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true>,                   \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true>,                \
      hashmaps::StoringRobinHoodAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true>,                  \
      hashmaps::LinearProbingSoAHashTable<KeyT, ValueT, DefaultHasher, true>,                                                             \
      hashmaps::LinearProbingSoAHashTable<KeyT, ValueT, DefaultHasher, true, 8>,                                                          \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, DefaultHasher, true>,                                                       \
      hashmaps::LinearProbingPackedSoAHashTable<KeyT, ValueT, DefaultHasher, true, 8>

  uint64_t num_hashmaps = 35;

#ifdef HASHMAP_BUILD_EXTERNAL

#ifdef HASHMAP_ONLY_BENCH_EXTERNAL
#undef BASIC_READ_BENCHMARK_HASHMAPS
#endif

#ifdef HASHMAP_ONLY_BENCH_SIMD
#undef BASIC_READ_BENCHMARK_HASHMAPS
  num_hashmaps = 1;
#define BASIC_READ_BENCHMARK_HASHMAPS hashmaps::AutoPaddedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false>
#endif

#define BASIC_EXTERNAL_READ_BENCHMARK_HASHMAPS \
  hashmaps::AbseilFlatHashTable<KeyT, ValueT, DefaultHasher>, hashmaps::MartinusRHFlatHashTable<KeyT, ValueT, DefaultHasher>
  num_hashmaps = 2;

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
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false,        \
                                      utils::PrefetchingLocality::NO, false>,                                                                        \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false,    \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON, false, \
                                          false, utils::PrefetchingLocality::NO, true>,                                                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, false>,                                                 \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,                    \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true>,                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, false>,                                                 \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,   \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,   \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>

  num_hashmaps += 21;

#ifdef __AVX2__
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                          \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false,        \
                                      utils::PrefetchingLocality::NO, false>,                                                                        \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON, false, false,    \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON, false, false, \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON, false, \
                                          false, utils::PrefetchingLocality::NO, true>,                                                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,   \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,                    \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true>,                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,     \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,  \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,     \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,   \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,             \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,      \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,              \
      BASIC_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 18;

#else
#ifdef HASHMAP_IS_ARM

#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                         \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64, false, false,        \
                                      utils::PrefetchingLocality::NO, true>,                                                                        \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64, false, false, \
                                          utils::PrefetchingLocality::NO, true>,                                                                    \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV, false, false,      \
                                          utils::PrefetchingLocality::NO, false>,                                                                   \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV, false, false,      \
                                          utils::PrefetchingLocality::NO, true>,                                                                    \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV, false, false,   \
                                          utils::PrefetchingLocality::NO, true>,                                                                    \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,     \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                 \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV,  \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                 \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,      \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                 \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,      \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                           \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,      \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                     \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV,   \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                 \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,     \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,  \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,     \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,                \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,     \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,  \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,     \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,                \
      BASIC_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 1;

#else

#ifdef HASHMAP_IS_POWER
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                        \
  hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV, false, \
                                           false, utils::PrefetchingLocality::NO, true>,                                                           \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,     \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                          \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                    \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV,  \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ, \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ, \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,    \
                                       false, false, NEONAlgo::UMINV, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,               \
      BASIC_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 12;

#else
#define INTERM_SIMD_READ_BENCHMARK_HASHMAPS BASIC_SIMD_READ_BENCHMARK_HASHMAPS
#endif
#endif
#endif

#ifdef __AVX512F__
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                            \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,         \
                                      utils::PrefetchingLocality::NO, true>,                                                                         \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,  \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,     \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,  \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,     \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,  \
                                          utils::PrefetchingLocality::NO, false>,                                                                    \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON, false, false,  \
                                          utils::PrefetchingLocality::NO, true>,                                                                     \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON, false,  \
                                          false, utils::PrefetchingLocality::NO, true>,                                                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,    \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON, \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true>,                                            \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON,     \
                                               false, false, utils::PrefetchingLocality::NO, true, true, true>,                                      \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON,  \
                                               false, false, utils::PrefetchingLocality::NO, true>,                                                  \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,  \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::NO_TESTZ,  \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,     \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::NO_TESTZ,   \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, false>,              \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,      \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,               \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 52;

#else

#if defined(HASHMAP_IS_ARM) && defined(__ARM_FEATURE_SVE)
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS                                                                                                            \
  hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON,  \
                                      false, false, utils::PrefetchingLocality::NO, false>,                                                          \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,                  \
                                          NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true>,                                   \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true,               \
                                          NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, false>,                                  \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true,               \
                                          NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true>,                                   \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,                  \
                                          NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                                    \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true,               \
                                          NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                                    \
      hashmaps::UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, utils::sve_register_size, SIMDAlgorithm::MANUAL_ON_MATCH, false, true,        \
                                          NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                                    \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true>,                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,      \
                                               true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true>,                        \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,    \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true>,                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true>,                              \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                               \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true, true>,                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true, true, true>,                   \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true>,                        \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,   \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true, true>,                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint16_t, utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,      \
                                               true, NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,    \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                               \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,    \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true, true>,                         \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,    \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true, true, true>,                   \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,    \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true>,                        \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,    \
                                               NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::NO, true, true, true>,                  \
      hashmaps::FingerprintingSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, uint8_t, utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, \
                                               NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::NO, true>,                               \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,      \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, false>,                                                                                                 \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,      \
                                       SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,   \
                                       true, false>,                                                                                                 \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,      \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true>,                                                                                                  \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,      \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::MEDIUM, true, \
                                       false>,                                                                                                       \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, utils::sve_register_size,      \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::MEDIUM, true, \
                                       true>,                                                                                                        \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,       \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, false>,                                                                                                 \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,       \
                                       SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,   \
                                       true, false>,                                                                                                 \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,       \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM,      \
                                       true, true>,                                                                                                  \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,       \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::MEDIUM, true, \
                                       false>,                                                                                                       \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, utils::sve_register_size,       \
                                       SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::MEDIUM, true, \
                                       true>,                                                                                                        \
      INTERM_SIMD_READ_BENCHMARK_HASHMAPS

  num_hashmaps += 28;

#else
#define FULL_SIMD_READ_BENCHMARK_HASHMAPS INTERM_SIMD_READ_BENCHMARK_HASHMAPS
#endif
#endif

#ifndef HASHMAP_ONLY_BENCH_EXTERNAL
#define BENCHMARK_HASHMAPS_A FULL_SIMD_READ_BENCHMARK_HASHMAPS, EXTERNAL_READ_BENCHMARK_HASHMAPS
#else
#define BENCHMARK_HASHMAPS_A EXTERNAL_READ_BENCHMARK_HASHMAPS
#endif

  return num_hashmaps;
}

}  // namespace benchmark
