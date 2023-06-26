#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

#include "fmt/format.h"
#include "hashmap/hashes/multshifthasher.hpp"
#include "hashmap/hashmaps/bucketing_simd.hpp"
#include "hashmap/hashmaps/chained.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap::hashmaps;
using namespace hashmap::hashing;

int main() {
  ChainedHashTable<uint64_t, uint64_t, MultShift64BHasher<uint64_t, false>, true, MemoryBudget::KeyValue, 255> chained(64, 50);

  chained.insert(42, 1337);
  chained.insert(43, 1339);
  chained.insert(44, 1338);
  chained.insert(45, 1338);
  chained.insert(46, 1338);

  spdlog::info(chained.lookup(42));
  spdlog::info(chained.lookup(43));
  spdlog::info(chained.lookup(44));
  spdlog::info(chained.lookup(45));
  spdlog::info(chained.lookup(46));

  BucketingSIMDHashTable<uint64_t, uint64_t, MultShift64BHasher<uint64_t, true>, uint8_t> hashmap(64, 50);

  hashmap.insert(42, 1337);
  hashmap.insert(43, 1339);
  hashmap.insert(44, 1338);
  hashmap.insert(45, 1338);
  hashmap.insert(46, 1338);

  spdlog::info(hashmap.lookup(42));
  spdlog::info(hashmap.lookup(43));
  spdlog::info(hashmap.lookup(44));
  spdlog::info(hashmap.lookup(45));
  spdlog::info(hashmap.lookup(46));

  return 0;
}
