#include <cmath>
#include <cstdint>
#include <optional>
#include <random>
#include <tuple>

#ifdef HASHMAP_LARGE_MATRIX
#include "benchmark/benchmark_hashmaps_a.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_A
#else
#ifdef HASHMAP_HASHFUNCTIONS
#include "benchmark/benchmark_hashmaps_d.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_D
#else

#ifdef HASHMAP_STRINGKEYS
#include "benchmark/benchmark_hashmaps_f.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_F
#else
#ifdef HASHMAP_DENSEKEYS
#include "benchmark/benchmark_hashmaps_g.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_G
#else

#if defined(HASHMAP_LARGEVALUES) || defined(HASHMAP_POINTERVALUES) || defined(HASHMAP_MT)
#include "benchmark/benchmark_hashmaps_h.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_H
#else
#ifdef HASHMAP_PREFETCHING
#include "benchmark/benchmark_hashmaps_i.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_I
#else

#ifdef HASHMAP_BENCHMARK_FALLBACK
#include "benchmark/benchmark_hashmaps_j.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_J
#else
#ifdef HASHMAP_BENCHMARK_CHAINED_ONLY
#include "benchmark/benchmark_hashmaps_k.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_K
#else

#ifdef HASHMAP_REPR
#include "benchmark/benchmark_hashmaps_rep.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_REP
#else
#include "benchmark/benchmark_hashmaps_c.hpp"
#define BENCHMARK_HASHMAPS BENCHMARK_HASHMAPS_C
#endif

#endif
#endif

#endif
#endif

#endif
#endif
#endif
#endif

#include "benchmark/benchmark_shared.hpp"
#include "benchmark/write_benchmark.hpp"
#include "fmt/format.h"
#include "gcem.hpp"
#include "hashmap/hashes/multaddshifthasher.hpp"
#include "hashmap/hashes/multshifthasher.hpp"
#include "hashmap/hashes/murmurhasher.hpp"
#include "hashmap/hashes/stdhasher.hpp"
#include "hashmap/hashes/xxhasher.hpp"
#include "hashmap/hashmaps/bucketing_simd.hpp"
#include "hashmap/hashmaps/chained.hpp"
#include "hashmap/hashmaps/fingerprinting_simd_soa.hpp"
#include "hashmap/hashmaps/linear_probing_aos.hpp"
#include "hashmap/hashmaps/linear_probing_soa.hpp"
#include "hashmap/hashmaps/linear_probing_soa_packed.hpp"
#include "hashmap/hashmaps/quadratic_probing_aos.hpp"
#include "hashmap/hashmaps/recalc_robin_hood_aos.hpp"
#include "hashmap/hashmaps/simple_simd_soa.hpp"
#include "hashmap/hashmaps/storing_robin_hood_aos.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

#ifdef HASHMAP_BUILD_EXTERNAL
#include "hashmap/hashmaps/abseil.hpp"
#include "hashmap/hashmaps/martinus.hpp"

#ifndef HASHMAP_IS_POWER
#include "hashmap/hashmaps/f14.hpp"
#endif

#endif

using namespace hashmap;
using namespace benchmark;

int main(int argc, char** argv) {
  write::WriteBenchmarkResultCollector collector;

  std::optional<std::string> benchmark_data_dir = fmt::format("{}/data", utils::getexepath().string());

  std::vector<uint8_t> load_factors{25, 50, 70, 90};
  std::vector<uint64_t> hashtable_sizes{gcem::pow<uint64_t, uint64_t>(2, 27)};
  std::vector<uint8_t> thread_counts = parse_thread_counts(argc, argv);

#ifdef HASHMAP_REPR
  if (thread_counts.size() > 1) {
    load_factors = {90};
  }
  using xxHasher = hashmap::hashing::XXHasher<KeyT, false>;
#endif

#ifdef HASHMAP_DENSEKEYS
  std::vector<DataDistribution> data_distributions{DataDistribution::DENSE};
#else
  std::vector<DataDistribution> data_distributions{DataDistribution::UNIFORM};
#endif

  bool store_intermediate_results = true;
  bool warmup_run = true;
  uint8_t runs_per_hashmap = 3;

#ifdef HASHMAP_STRINGKEYS
  using KeyT = StringKey;
#else
  using KeyT = uint64_t;
#endif

#ifdef HASHMAP_POINTERVALUES
#ifdef HASHMAP_LARGEVALUES
  using ValueT = DummyTuple*;
#else
  using ValueT = uint64_t*;
#endif
#else
#ifdef HASHMAP_LARGEVALUES
  using ValueT = DummyTuple;
#else
  using ValueT = uint64_t;
#endif
#endif

#if !defined(HASHMAP_HASHFUNCTIONS) && !defined(HASHMAP_DENSEKEYS)

#ifdef HASHMAP_STRINGKEYS
  using DefaultHasher = hashmap::hashing::XXHasher<KeyT, false>;
#else
  using DefaultHasher = hashing::MultShift64BHasher<KeyT, false>;
#endif

#else
#ifndef HASHMAP_DENSEKEYS
  using MultShift64ModuloHasher = hashmap::hashing::MultShift64BHasher<KeyT, true>;
#endif

  using MultShift128Hasher = hashmap::hashing::MultShift128BHasher<KeyT, false>;
  using MultAddShift64Hasher = hashmap::hashing::MultAddShift64BHasher<KeyT, false>;
  using MultAddShift128Hasher = hashmap::hashing::MultAddShift128BHasher<KeyT, false>;
  using MurmurHasher = hashmap::hashing::MurmurHasher<KeyT, false>;
  using xxHasher = hashmap::hashing::XXHasher<KeyT, false>;

#endif

#ifdef HASHMAP_DENSEKEYS
  using MultShift64Hasher = hashmap::hashing::MultShift64BHasher<KeyT, false>;
#endif

#ifdef HASHMAP_LARGE_MATRIX
  uint64_t num_hashmaps = get_num_hashmaps_a();
  spdlog::info("Large matrix benchmark.");
#else

#ifdef HASHMAP_HASHFUNCTIONS
  uint64_t num_hashmaps = get_num_hashmaps_d();
  spdlog::info("Hashfunction benchmark.");
#else
#ifdef HASHMAP_STRINGKEYS
  uint64_t num_hashmaps = get_num_hashmaps_f();
  spdlog::info("Stringkey benchmark.");
#else
#ifdef HASHMAP_DENSEKEYS
  uint64_t num_hashmaps = get_num_hashmaps_g();
  spdlog::info("Densekey benchmark.");
#else
#if defined(HASHMAP_LARGEVALUES) || defined(HASHMAP_POINTERVALUES) || defined(HASHMAP_MT)
  uint64_t num_hashmaps = get_num_hashmaps_h();
#else
#ifdef HASHMAP_PREFETCHING
  spdlog::info("Prefetching benchmark.");
  uint64_t num_hashmaps = get_num_hashmaps_i();
#else

#ifdef HASHMAP_BENCHMARK_FALLBACK
  spdlog::info("Fallback benchmark.");
  uint64_t num_hashmaps = get_num_hashmaps_j();
#else
#ifdef HASHMAP_BENCHMARK_CHAINED_ONLY
  spdlog::info("Chained only benchmark.");
  uint64_t num_hashmaps = get_num_hashmaps_k();

#else
#ifdef HASHMAP_REPR
  spdlog::info("Reproducibliy benchmark.");
  uint64_t num_hashmaps = get_num_hashmaps_rep();
#else
  uint64_t num_hashmaps = get_num_hashmaps_c();
#endif
#endif
#endif
#endif
#endif
#endif
#endif

#endif
#endif

  uint64_t total_runs =
      thread_counts.size() * hashtable_sizes.size() * data_distributions.size() * load_factors.size() * runs_per_hashmap * num_hashmaps;

  bool is_generate_run = false;

  if (argc == 1) {
    spdlog::info("This is the write benchmark app. Run me with ./executable generate to pre-generate data, if you want to.");
    spdlog::info(fmt::format("Running {} benchmarks in total", total_runs));
  } else if (std::string(argv[1]) == "generate") {
    spdlog::info("This is the write benchmark app. This is a data-generation only run.");
    is_generate_run = true;
  } else if (std::string(argv[1]) == "disable-store") {
    spdlog::info("This is the write benchmark app. Data storing/loading is disabled.");
    benchmark_data_dir = std::nullopt;
  }

  for (const auto& hashtable_size : hashtable_sizes) {
    for (const auto& thread_count : thread_counts) {
      spdlog::info(fmt::format("Generating prefault data for hashtable size {}", hashtable_size));
      uint64_t thread_table_size = hashtable_size / static_cast<uint64_t>(thread_count);
      spdlog::info(fmt::format("Generating prefault data for hashtable size {} (thread table size = {})", hashtable_size, thread_table_size));
      std::vector<std::vector<std::pair<KeyT, ValueT>>> prefault_data = generate_prefault_data<KeyT, ValueT>(thread_table_size, thread_count);

      for (const auto& data_distribution : data_distributions) {
        for (const auto& load_factor : load_factors) {
          if (is_generate_run) {
            ASSERT(benchmark_data_dir.has_value(), "Need value for storage directory when doing generation run.");
            benchmark::write::write_benchmark_data_to_file<KeyT, ValueT>(load_factor, hashtable_size, data_distribution, benchmark_data_dir.value(),
                                                                         thread_count);
          } else {
            write::write_bench<KeyT, ValueT, BENCHMARK_HASHMAPS>(load_factor, hashtable_size, data_distribution, runs_per_hashmap, thread_count,
                                                                 &collector, store_intermediate_results, warmup_run, prefault_data,
                                                                 benchmark_data_dir);
          }
        }
      }
    }
  }
  collector.to_csv(fmt::format("finalwriteresult_{}.csv", timeSinceEpochMillisec()));

  return 0;
}
