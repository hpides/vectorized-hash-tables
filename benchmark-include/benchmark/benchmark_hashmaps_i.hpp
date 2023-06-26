#pragma once
#include <cstdint>

/* READ BENCHMARK HASHMAPS I */
/* Prefetching, anyone? */

namespace benchmark {

uint64_t get_num_hashmaps_i() {
#define BENCHMARK_HASHMAPS_I                                                                                                           \
  hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true, 1, 1>,         \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true, 1, 2>,     \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true, 1, 3>,     \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true, 1, 4>,     \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true, 1, 5>,     \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true, 1, 6>,     \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::NO, true, 1, 7>,     \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true, 1, 1>,    \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true, 1, 2>,    \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true, 1, 3>,    \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true, 1, 4>,    \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true, 1, 5>,    \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true, 1, 6>,    \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::LOW, true, 1, 7>,    \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true, 1, 1>, \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true, 1, 2>, \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true, 1, 3>, \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true, 1, 4>, \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true, 1, 5>, \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true, 1, 6>, \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::MEDIUM, true, 1, 7>, \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true, 1, 1>,   \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true, 1, 2>,   \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true, 1, 3>,   \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true, 1, 4>,   \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true, 1, 5>,   \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true, 1, 6>,   \
      hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, true, utils::PrefetchingLocality::HIGH, true, 1, 7>

  uint64_t num_hashmaps = 28;

  return num_hashmaps;
}

}  // namespace benchmark
