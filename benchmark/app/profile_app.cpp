#ifdef IS_LINUX
#include <errno.h>
#include <fcntl.h>
#include <linux/kernel-page-flags.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

#include "benchmark/read_benchmark.hpp"
#include "fmt/format.h"
#include "gcem.hpp"
#include "hashmap/hashes/multaddshifthasher.hpp"
#include "hashmap/hashes/multshifthasher.hpp"
#include "hashmap/hashes/murmurhasher.hpp"
#include "hashmap/hashes/statichasher.hpp"
#include "hashmap/hashes/stdhasher.hpp"
#include "hashmap/hashes/xxhasher.hpp"
#include "hashmap/hashmaps/bucketing_simd.hpp"
#include "hashmap/hashmaps/linear_probing_aos.hpp"
#include "hashmap/hashmaps/quadratic_probing_aos.hpp"
#include "hashmap/hashmaps/recalc_robin_hood_aos.hpp"
#include "hashmap/hashmaps/simple_simd_soa.hpp"
#include "hashmap/hashmaps/storing_robin_hood_aos.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap::hashmaps;
using namespace hashmap::hashing;

int main(int argc, char** argv) {
  using KeyT = uint64_t;
  using ValueT = uint64_t;
  using DefaultHasher = hashing::MultShift64BHasher<KeyT, false>;

  benchmark::read::ReadBenchmarkResultCollector collector;
  std::vector<std::pair<KeyT, ValueT>> prefault_data;

  std::string benchmark_data_dir = fmt::format("{}/data", utils::getexepath().string());

  const uint8_t load_factor = 25;
  const uint64_t hashtable_size = gcem::pow<uint64_t, uint64_t>(2, 27);
  const benchmark::DataDistribution distribution = benchmark::DataDistribution::UNIFORM;
  const uint8_t successful_query_rate = 100;
  const uint32_t workload = utils::default_workload;
  const uint8_t runs_per_hashmap = 1;

  if (argc > 1 && std::string(argv[1]) == "generate") {
    benchmark::read::write_benchmark_data_to_file<KeyT, ValueT>(load_factor, hashtable_size, distribution, benchmark_data_dir, successful_query_rate,
                                                                workload);
    return EXIT_SUCCESS;
  }

  spdlog::info("This is the profiler app. Run me with ./executable generate to pre-generate data, if you want to.");

  benchmark::read::read_bench<KeyT, ValueT,
                              hashmaps::UnalignedLinearProbingAoSHashTable<KeyT, ValueT, DefaultHasher, false, utils::PrefetchingLocality::NO, true>>(
      load_factor, successful_query_rate, hashtable_size, distribution, workload, runs_per_hashmap, &collector, false, false, prefault_data,
      benchmark_data_dir);

  benchmark::read::read_bench<
      KeyT, ValueT,
      hashmaps::BucketingSIMDHashTable<KeyT, ValueT, DefaultHasher, uint8_t, hashmaps::KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ, false,
                                       true, NEONAlgo::SSE2NEON, true, false, utils::PrefetchingLocality::MEDIUM, true, true>>(
      load_factor, successful_query_rate, hashtable_size, distribution, workload, runs_per_hashmap, &collector, false, false, prefault_data,
      benchmark_data_dir);

  /*benchmark::read::read_bench<KeyT, ValueT,
                              UnchunkedSIMDSoAHashTable<KeyT, ValueT, DefaultHasher, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,
                                                        false, false, utils::PrefetchingLocality::NO, true, 1>>(
      load_factor, successful_query_rate, hashtable_size, distribution, workload, runs_per_hashmap, &collector, false, false, prefault_data,
      benchmark_data_dir); */

  return EXIT_SUCCESS;
}
#else
int main() { return -1; }
#endif