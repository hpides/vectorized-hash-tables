#pragma once
#include <cstdint>

/* READ BENCHMARK HASHMAPS K */
/* Chaining hashmaps */

namespace benchmark {

uint64_t get_num_hashmaps_k() {
#define BENCHMARK_HASHMAPS_K                                                                                               \
  hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, false, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,     \
      hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, true, hashmap::hashmaps::MemoryBudget::KeyValue, 10, true>,  \
      hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, true, hashmap::hashmaps::MemoryBudget::Bucket8FP, 10, true>, \
      hashmaps::ChainedHashTable<KeyT, ValueT, DefaultHasher, true, hashmap::hashmaps::MemoryBudget::Bucket16FP, 10, true>

  uint64_t num_hashmaps = 4;

  return num_hashmaps;
}

}  // namespace benchmark
