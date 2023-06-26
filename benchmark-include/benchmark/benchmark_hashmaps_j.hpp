#pragma once
#include <cstdint>

/* READ BENCHMARK HASHMAPS J */
/* Fallback Comparison, assumes AVX-512 available! */

namespace benchmark {

uint64_t get_num_hashmaps_j() {
#define BENCHMARK_HASHMAPS_J                                                                                                                     \
  hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true, 1, 0>,                  \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,  \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,  \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,  \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ, \
                                       false, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,          \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,  \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>,           \
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint16_t, hashmaps::KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ, \
                                       true, false, NEONAlgo::SSE2NEON, false, false, utils::PrefetchingLocality::MEDIUM, true, true>

  uint64_t num_hashmaps = 7;

  return num_hashmaps;
}

}  // namespace benchmark
