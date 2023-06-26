#pragma once
#include <algorithm>
#include <barrier>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "benchmark/benchmark_shared.hpp"
#include "benchmark/benchmark_utils.hpp"
#include "fmt/format.h"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

#ifdef HASHMAP_IS_VTUNEPROFILE_BUILD
#include "ittnotify.h"
#endif

#ifdef HASHMAP_IS_VALGRINDPROFILE_BUILD
#include <valgrind/callgrind.h>
#endif

#ifdef HASHMAP_IS_UPROF_BUILD
#include <AMDProfileController.h>
#endif

#ifdef HASHMAP_IS_PERFPROFILE_BUILD
#include "benchmark/perf_monitor.hpp"
#endif

namespace benchmark::read {

struct IncrementMap {
  IncrementMap(std::map<uint64_t, uint64_t>& m) : m_map(m) {}
  void operator()(uint64_t val) const { ++m_map[val]; }
  std::map<uint64_t, uint64_t>& m_map;
};

struct SpecialPrint {
  SpecialPrint(std::ostream& s) : m_str(s) {}
  void operator()(std::map<uint64_t, uint64_t>::value_type const& value) const {
    if (value.second > 10) {
      m_str << value.first;
      m_str << "\t" << value.second;
      m_str << "\n";
    }
  }
  std::ostream& m_str;
};

struct ReadBenchmarkResult {
  std::string hashmap_identifier = "";
  uint8_t load_factor = 0;
  uint8_t successful_query_rate = 0;
  uint64_t hashtable_size = 0;
  uint8_t thread_count = 0;
  uint64_t threadtable_size = 0;
  std::string distribution_name = "";
  uint32_t workload = 0;
  double key_size = 0;
  double value_size = 0;
  uint64_t entry_size = 0;
  uint64_t entries_processed = 0;
  uint64_t runtime = 0;  // in milliseconds
  double thread_avg_runtime = 0;
  uint64_t thread_max_runtime = 0;
  std::string data_ptr = "";
  bool is_aligned_to_hp = false;
  bool successful = true;
  bool zipf = false;
  uint64_t zipf_factor = 0;
};

class ReadBenchmarkResultCollector {
 public:
  ReadBenchmarkResultCollector() = default;
  void add_benchmark_result(ReadBenchmarkResult result);
  void to_csv(std::filesystem::path path);

 private:
  std::vector<ReadBenchmarkResult> benchmark_results_;
};

template <class KeyT>
std::vector<KeyT> generate_queries(uint8_t successful_query_rate, uint32_t workload /* in MB w.r.t. sizeof(KeyT) */, std::vector<KeyT>& used_keys,
                                   bool zipf_requests, uint64_t zipf_factor) {
  ASSERT(used_keys.size() > 0, "Used Keys cannot be 0, as we use fill degrees larger than 0...");
  // 1. Setup
  uint64_t workload_in_bytes = static_cast<uint64_t>(workload) * 1024 * 1024;
  uint64_t key_size = sizeof(KeyT);
  double zipf = static_cast<double>(zipf_factor) / 100.0;

  if constexpr (std::is_same_v<KeyT, StringKey>) {
    key_size = HASHMAP_STRINGKEY_SIZE;
  }

  uint64_t total_queries = workload_in_bytes / key_size + (workload_in_bytes % key_size != 0);

  uint64_t successful_queries = (successful_query_rate * total_queries) / 100;
  uint64_t unsuccessful_queries = total_queries - successful_queries;
  std::vector<KeyT> result_queries;
  result_queries.reserve(total_queries);
  spdlog::info(fmt::format("Pre-generating {} benchmark queries (sucessful_query_rate = {}%, used_keys.size() = {})...", total_queries,
                           successful_query_rate, used_keys.size()));

  // 2. Generate successful queries
  spdlog::info(fmt::format("Generating {} successful queries...", successful_queries));

  if (zipf_requests) {
    spdlog::info(fmt::format("Zipf with {}", zipf_factor));
    ASSERT(zipf_factor > 100, "Invalid zipf factor (needs to be > 100)");

    zipf_distribution<uint64_t, double> distribution(used_keys.size() - 1, zipf);
    for (uint64_t i = 0; i < successful_queries; ++i) {
      uint64_t key_idx = distribution(reproducible_gen());
      ASSERT(key_idx < used_keys.size(), "Invalid key_idx");
      result_queries.push_back(used_keys[key_idx]);
    }

    // DEBUG: print out counter
    /*spdlog::info("DEBUG START");
    std::map<uint64_t, uint64_t> data;
    std::for_each(result_queries.begin(), result_queries.end(), IncrementMap(data));
    std::for_each(data.begin(), data.end(), SpecialPrint(std::cout));
    spdlog::info("DEBUG END");
    */

  } else {
    spdlog::info("Uniform distribution");
    // We want a uniform distribution. We first fill up the queries with all used keys as often as we can, and then fill
    // up the rest uniform randomly

    uint64_t copy_amount = successful_queries / used_keys.size();
    for (uint64_t i = 0; i < copy_amount; ++i) {
      result_queries.insert(result_queries.end(), used_keys.begin(), used_keys.end());
    }

    ASSERT(result_queries.size() == copy_amount * used_keys.size(),
           fmt::format("Copy of used keys did not generate the correct amount of queries ({} vs {})).", result_queries.size(), copy_amount));

    uint64_t remainder_amount = successful_queries - result_queries.size();
    uint64_t max_key_idx = used_keys.size() - 1;

    ASSERT(max_key_idx < std::numeric_limits<int64_t>::max(), "max_key_idx is too high!");

    auto start_it = used_keys.begin();
    for (uint64_t i = 0; i < remainder_amount; ++i) {
      int64_t key_idx = static_cast<int64_t>(next_random_data(max_key_idx));
      result_queries.push_back(*std::next(start_it, key_idx));
    }
  }
  ASSERT(result_queries.size() == successful_queries,
         fmt::format("Remainder did not generate the correct amount of queries ({} vs {})).", result_queries.size(), successful_queries));

  // 3. Generate unsucessful queries
  std::vector<KeyT> unsuccessful_keys;
  spdlog::info(fmt::format("Generating {} unsucessful queries...", unsuccessful_queries));
  if constexpr (std::is_same_v<KeyT, StringKey>) {
    if (zipf_requests) {
      FAIL("zipf string keys not supported");
    } else {
      unsuccessful_keys = generate_fill_strings_on_heap(unsuccessful_queries, &used_keys);
    }
  } else {
    if (zipf_requests) {
      spdlog::info("Unsucessful zipf");
      std::vector<KeyT> uniform_unsuccessful_keys;
      uniform_unsuccessful_keys = generate_n_unique_integer_keys<KeyT>(unsuccessful_queries, DataDistribution::UNIFORM, &used_keys);
      unsuccessful_keys.reserve(unsuccessful_queries);

      spdlog::info("Creating zipfian sample of unsucessful keys");

      zipf_distribution<uint64_t, double> distribution(uniform_unsuccessful_keys.size() - 1, zipf);
      for (uint64_t i = 0; i < unsuccessful_queries; ++i) {
        uint64_t key_idx = distribution(reproducible_gen());
        ASSERT(key_idx < uniform_unsuccessful_keys.size(), "Invalid key_idx");
        unsuccessful_keys.push_back(uniform_unsuccessful_keys[key_idx]);
      }

      spdlog::info("Zipf sample created");
      uniform_unsuccessful_keys.clear();
      uniform_unsuccessful_keys.shrink_to_fit();

      /*spdlog::info("DEBUG START");
      std::map<uint64_t, uint64_t> data;
      std::for_each(unsuccessful_keys.begin(), unsuccessful_keys.end(), IncrementMap(data));
      std::for_each(data.begin(), data.end(), SpecialPrint(std::cout));
      spdlog::info("DEBUG END");*/

    } else {
      spdlog::info("Unsuccesful uniform");
      unsuccessful_keys = generate_n_unique_integer_keys<KeyT>(unsuccessful_queries, DataDistribution::UNIFORM, &used_keys);
    }
  }

  spdlog::info(fmt::format("Generation of unsucessful queries done, inserting into final query vector...", unsuccessful_queries));

  ASSERT(unsuccessful_keys.size() == unsuccessful_queries,
         fmt::format("Unsucessful query generation did not generate the correct amount of queries ({} vs {})).", unsuccessful_keys.size(),
                     unsuccessful_queries));

  result_queries.insert(result_queries.end(), unsuccessful_keys.begin(), unsuccessful_keys.end());

  // 4. Shuffle queries
  spdlog::info(fmt::format("Shuffling {} queries...", total_queries));
  std::shuffle(std::begin(result_queries), std::end(result_queries), reproducible_gen());

  ASSERT(result_queries.size() == total_queries,
         fmt::format("Query generation did not generate the correct amount of queries ({} vs {})).", result_queries.size(), total_queries));

  return result_queries;
}

template <class KeyT, class ValueT, class HashtableT>
void do_work(uint64_t hashtable_size, uint8_t thread_id, uint8_t load_factor,
             std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>* fill_data_ptr, const std::vector<KeyT>* query_data_ptr,
             const std::vector<std::pair<KeyT, ValueT>>* prefault_data_ptr, bool warmup_run, std::atomic<bool>* result_success,
             std::atomic<uint64_t>* result_runtime, std::atomic<uint64_t>* sum_ptr, std::string* result_hashmap_identifier,
             std::atomic<uint64_t>* result_entry_size, std::atomic<uint64_t>* result_entries_processed, std::string* result_data_ptr,
             std::atomic<bool>* result_is_aligned, std::barrier<std::__empty_completion>* barrier, [[maybe_unused]] void* meta_collector_ptr) {
#ifdef HASHMAP_COLLECT_META_INFO
  MetadataBenchmarkResult meta_result;
  meta_result.hashmap_identifier = hashtable.get_identifier();
  meta_result.load_factor = load_factor;
  meta_result.successful_query_rate = successful_query_rate;
  meta_result.hashtable_size = hashtable_size;
  meta_result.distribution_name = get_distribution_name(distribution);
  meta_result.workload = workload;
  meta_result.key_size = sizeof(KeyT);
  meta_result.value_size = sizeof(ValueT);
  meta_result.entry_size = hashtable.get_entry_size();
  meta_result.entries_processed = query_data.size();
  meta_result.data_ptr = hashtable.get_data_pointer_string();
  meta_result.is_aligned_to_hp = hashtable.is_data_aligned_to(hashmap::utils::hugepage_size);
#endif

  std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>& fill_data = *fill_data_ptr;
  const std::vector<KeyT>& query_data = *query_data_ptr;
  const std::vector<std::pair<KeyT, ValueT>>& prefault_data = *prefault_data_ptr;

  spdlog::info(fmt::format("[Thread {}] Allocating hashtable memory for read benchmark.", thread_id));
  HashtableT hashtable(hashtable_size, load_factor);
  spdlog::info(fmt::format("[Thread {}] Hash table identifier is {}", thread_id, hashtable.get_identifier()));

  *result_hashmap_identifier = hashtable.get_identifier();
  *result_entry_size = hashtable.get_entry_size();
  *result_entries_processed = query_data.size();
  *result_data_ptr = hashtable.get_data_pointer_string();
  *result_is_aligned = hashtable.is_data_aligned_to(hashmap::utils::hugepage_size);

  if (!hashtable.can_be_used()) {
    spdlog::error(
        fmt::format("[Thread {}] Hashtable reported it cannot be used (should only happen for chained hash tables. Skipping the benchmark (sleeping "
                    "for a second to avoid "
                    "file naming problems).",
                    thread_id));
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    *result_success = false;
    *result_runtime = 0;
    *sum_ptr = 0;

    if (barrier != nullptr) {
      barrier->arrive_and_wait();
      barrier->arrive_and_wait();
    }
    return;
  }

  // 1. Prefault table
  spdlog::info(fmt::format("[Thread {}] Prefaulting hash table of size {}...", thread_id, hashtable_size));
  if (prefault_data.empty()) {
    ASSERT(std::is_arithmetic<KeyT>::value, "Cannot use hashmap.prefault for non-numeric keys. Please provide custom prefault data.");
    hashtable.prefault();
  } else {
    hashtable.prefault_pregenerated(prefault_data);
  }

  // 2. Fill the table up to desired load factor
  spdlog::info(fmt::format("[Thread {}] Filling hash table...", thread_id));

  if constexpr (std::is_pointer_v<ValueT>) {
    for (auto& entry : fill_data) {
      hashtable.insert(entry.first, &entry.second);
    }
  } else {
    for (const auto& entry : fill_data) {
      hashtable.insert(entry.first, entry.second);
    }
  }

  spdlog::info(fmt::format("[Thread {}] Hashmap load is now {}", thread_id, hashtable.get_current_load()));
  ClobberMemory();
  if (barrier != nullptr) {
    barrier->arrive_and_wait();
  }
  ClobberMemory();
  // 4. Do all the requests (simple sum aggregation, I guess)

#ifdef HASHMAP_IS_VTUNEPROFILE_BUILD
  __itt_task_begin(hashmap::utils::vtune_domain, __itt_null, __itt_null, hashmap::utils::vtune_task_ll);
#endif

#ifdef HASHMAP_IS_VALGRINDPROFILE_BUILD
  CALLGRIND_START_INSTRUMENTATION;
  CALLGRIND_TOGGLE_COLLECT;
#endif

#ifdef HASHMAP_IS_UPROF_BUILD
  amdProfileResume(AMD_PROFILE_CPU);
#endif

#ifdef HASHMAP_IS_PERFPROFILE_BUILD
  PerfMonitor monitor;
  monitor.start();
#endif

#ifdef HASHMAP_COLLECT_META_INFO
  hashtable.start_measurement();
#endif

  auto start = std::chrono::high_resolution_clock::now();
  uint64_t sum = 0;

  if constexpr (std::is_pointer_v<ValueT>) {
    const uint64_t constant_addition = 42;

    for (const KeyT& key : query_data) {
      ValueT result = hashtable.lookup(key);
      sum += (result != nullptr) ? *result : constant_addition;
      doNotOptimize(sum);
    }
  } else {
    for (const KeyT& key : query_data) {
      sum += hashtable.lookup(key);
      doNotOptimize(sum);
    }
  }

  ClobberMemory();

  auto end = std::chrono::high_resolution_clock::now();

#ifdef HASHMAP_IS_VTUNEPROFILE_BUILD
  __itt_task_end(hashmap::utils::vtune_domain);
#endif

#ifdef HASHMAP_IS_VALGRINDPROFILE_BUILD
  CALLGRIND_TOGGLE_COLLECT;
  CALLGRIND_STOP_INSTRUMENTATION;
#endif

#ifdef HASHMAP_IS_UPROF_BUILD
  amdProfilePause(AMD_PROFILE_CPU);
#endif

#ifdef HASHMAP_IS_PERFPROFILE_BUILD
  monitor.stop();
  PerfResult perfresult = monitor.get_result();
  spdlog::info(fmt::format("Perf Result: {}", perfresult.to_string()));

  // Also print values to collect
  if (!warmup_run) {
    perfresult.benchmark_print();
  }
#endif

#ifdef HASHMAP_COLLECT_META_INFO
  hashtable.stop_measurement();
  meta_result.minfo = *(hashtable.get_minfo());
#endif

  *result_runtime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
  *sum_ptr = sum;
  *result_success = true;

  if (!warmup_run) {
#ifdef HASHMAP_COLLECT_META_INFO
    static_cast<MetadataBenchmarkResultCollector*>(meta_collector_ptr)->add_benchmark_result(meta_result);
#endif
  }
  if (barrier != nullptr) {
    barrier->arrive_and_wait();
  }
}

template <class KeyT, class ValueT, class HashtableT>
void read_bench_impl(uint64_t hashtable_size, uint64_t thread_table_size, uint8_t load_factor, uint8_t successful_query_rate,
                     DataDistribution distribution, uint32_t workload, uint8_t thread_count,
                     std::vector<std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>>& fill_data,
                     const std::vector<std::vector<KeyT>>& query_data, ReadBenchmarkResultCollector* collector, bool warmup_run, bool zipf_requests,
                     uint64_t zipf_factor, const std::vector<std::vector<std::pair<KeyT, ValueT>>>& prefault_data,
                     [[maybe_unused]] void* meta_collector_ptr) {
  spdlog::info("Running read benchmark");

  if constexpr (std::is_pointer_v<ValueT>) {
    spdlog::info("ValueType is pointer-type.");
  } else {
    spdlog::info("ValueType is not pointer-type.");
  }

  ReadBenchmarkResult result;
  result.load_factor = load_factor;
  result.successful_query_rate = successful_query_rate;
  result.hashtable_size = hashtable_size;
  result.thread_count = thread_count;
  result.threadtable_size = thread_table_size;
  result.distribution_name = get_distribution_name(distribution);
  result.workload = workload;
  result.key_size = sizeof(KeyT);
  result.value_size = sizeof(ValueT);
  result.successful = false;
  result.zipf = zipf_requests;
  result.zipf_factor = zipf_factor;

  if (warmup_run) {
    spdlog::info("Doing warmup run...");
  }

  spdlog::info("Starting benchmark....");
  ClobberMemory();

  uint64_t sum = 0;

  if (thread_count == 1) {
    std::atomic<bool> successful = false;
    std::atomic<uint64_t> runtime = 0;
    std::atomic<uint64_t> atomic_sum = 0;
    std::atomic<uint64_t> entry_size = 0;
    std::atomic<uint64_t> entries_processed = 0;
    std::atomic<bool> is_aligned_to_hp = false;

    // single-thread = do everything in main thread
    do_work<KeyT, ValueT, HashtableT>(thread_table_size, 0, load_factor, &fill_data[0], &query_data[0], &prefault_data[0], warmup_run, &successful,
                                      &runtime, &atomic_sum, &result.hashmap_identifier, &entry_size, &entries_processed, &result.data_ptr,
                                      &is_aligned_to_hp, nullptr, meta_collector_ptr);
    result.successful = successful.load();
    result.runtime = runtime.load();
    result.thread_avg_runtime = static_cast<double>(result.runtime);
    result.thread_max_runtime = result.runtime;
    sum = atomic_sum.load();
    result.entry_size = entry_size.load();
    result.entries_processed = entries_processed.load();
    result.is_aligned_to_hp = is_aligned_to_hp.load();

    if (!result.successful) {
      spdlog::error("Got no success!");
      result.runtime = 0;
    }

  } else {
#ifdef HASHMAP_COLLECT_META_INFO
    FAIL("meta collection only supports single-threaded execution.");
#endif
    std::barrier sync_point(thread_count + 1);  // we wait for the main thread + worker threads

    // 1. init all threads
    std::vector<std::thread> workers;

    std::vector<std::atomic<uint8_t>> successfuls(thread_count);
    std::vector<std::atomic<uint64_t>> runtimes(thread_count);
    std::vector<std::atomic<uint64_t>> sums(thread_count);
    std::vector<std::string> identifiers(thread_count);
    std::vector<std::atomic<uint64_t>> entry_sizes(thread_count);
    std::vector<std::atomic<uint64_t>> entries_processed(thread_count);
    std::vector<std::string> data_ptrs(thread_count);
    std::vector<std::atomic<uint8_t>> is_aligneds(thread_count);

    for (uint8_t thread = 0; thread < thread_count; ++thread) {
      workers.push_back(std::thread(do_work<KeyT, ValueT, HashtableT>, thread_table_size, thread, load_factor, &fill_data[thread],
                                    &query_data[thread], &prefault_data[thread], warmup_run,
                                    reinterpret_cast<std::atomic<bool>*>(&successfuls[thread]), &runtimes[thread], &sums[thread],
                                    &identifiers[thread], &entry_sizes[thread], &entries_processed[thread], &data_ptrs[thread],
                                    reinterpret_cast<std::atomic<bool>*>(&is_aligneds[thread]), &sync_point, meta_collector_ptr));
    }

    // 2. wait until all threads have signaled they are ready, start clock (or if any thread is not successful, then skip)
    sync_point.arrive_and_wait();
    auto start = std::chrono::high_resolution_clock::now();
    spdlog::info("All threads are working.");

    // 3. wait until all of them have signaled they are finished, end clock
    sync_point.arrive_and_wait();
    auto end = std::chrono::high_resolution_clock::now();
    result.runtime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    spdlog::info("All workers have reached the barrier, joining.");
    for (auto& worker : workers) {
      worker.join();
    }
    spdlog::info("All threads exited.");

    // 4. aggregate sum, validate vectors, set result struct.
    for (const auto& asum : sums) {
      sum += asum.load();
    }

    bool all_successful = true;
    for (const auto& succ : successfuls) {
      all_successful &= succ.load() > 0;  // cannot use std::all_of due to deleted copy constructor of atomics
    }

    if (!all_successful) {
      spdlog::error("Some worker was unsucessful!");
      result.runtime = 0;
      result.successful = false;
    } else {
      result.successful = true;
    }

    result.hashmap_identifier = identifiers[0];
    bool consistent_ids = std::all_of(identifiers.begin(), identifiers.end(), [&](std::string id) { return id == result.hashmap_identifier; });
    if (!consistent_ids) {
      result.hashmap_identifier = "INCONSISTENT";
      spdlog::error("inconsistent identifier");
    }

    result.entry_size = entry_sizes[0].load();
    bool consistent_es = true;
    for (const auto& entry_size : entry_sizes) {
      consistent_es &= entry_size.load() == result.entry_size;
    }
    if (!consistent_es) {
      result.entry_size = 0;
      result.successful = false;
      spdlog::error("inconsistent entry sizes");
    }

    result.entries_processed = entries_processed[0].load();
    bool consistent_ep = true;
    for (const auto& entries_proc : entries_processed) {
      consistent_ep &= entries_proc.load() == result.entries_processed;
    }

    if (!consistent_ep) {
      result.entries_processed = 0;
      result.successful = false;
      spdlog::error("inconsistent entries processed");
    }

    bool all_aligned = true;
    for (const auto& aligned : is_aligneds) {
      all_aligned &= aligned.load() > 0;  // cannot use std::all_of due to deleted copy constructor of atomics
    }
    result.is_aligned_to_hp = all_aligned;  // can be false, no need to set successful
    result.data_ptr = data_ptrs[0];

    uint64_t total_runtime = 0;
    uint64_t max_runtime = 0;
    for (const auto& thread_runtime : runtimes) {
      total_runtime += thread_runtime.load();
      max_runtime = std::max(max_runtime, thread_runtime.load());
    }

    double thread_avg_runtime = static_cast<double>(total_runtime) / static_cast<double>(thread_count);
    result.thread_avg_runtime = thread_avg_runtime;
    result.thread_max_runtime = max_runtime;
  }

  if (result.successful) {
    double runtime_in_seconds = static_cast<double>(result.runtime) / 1000.0;
    uint64_t total_data = 0;
    for (const auto& vec : query_data) {
      total_data += vec.size();
    }

    double lookup_throughput = static_cast<double>(total_data) / runtime_in_seconds;

    spdlog::info(fmt::format("Benchmark finished in {} milliseconds, i.e. {} Lookups/s. (temp value={})", result.runtime, lookup_throughput, sum));
    std::cout << std::endl;
  }

  if (!warmup_run) {
    collector->add_benchmark_result(result);
  } else {
    spdlog::info("Not storing result, due to warmup.");
  }
}

template <class KeyT, class ValueT, class HashtableT>
void read_bench_loop_impl(uint8_t runs_per_hashmap, uint64_t hashtable_size, uint64_t thread_table_size, uint8_t load_factor,
                          uint8_t successful_query_rate, DataDistribution distribution, uint32_t workload, uint8_t thread_count,
                          std::vector<std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>>& fill_data,
                          const std::vector<std::vector<KeyT>>& query_data, ReadBenchmarkResultCollector* collector, bool store_intermediate_results,
                          bool warmup_run, bool zipf_requests, uint64_t zipf_factor,
                          const std::vector<std::vector<std::pair<KeyT, ValueT>>>& prefault_data, [[maybe_unused]] void* meta_collector_ptr) {
  if (warmup_run) {
    read_bench_impl<KeyT, ValueT, HashtableT>(hashtable_size, thread_table_size, load_factor, successful_query_rate, distribution, workload,
                                              thread_count, fill_data, query_data, collector, true, zipf_requests, zipf_factor, prefault_data,
                                              meta_collector_ptr);
  }

  for (uint8_t i = 0; i < runs_per_hashmap; ++i) {
    read_bench_impl<KeyT, ValueT, HashtableT>(hashtable_size, thread_table_size, load_factor, successful_query_rate, distribution, workload,
                                              thread_count, fill_data, query_data, collector, false, zipf_requests, zipf_factor, prefault_data,
                                              meta_collector_ptr);
  }

  if (store_intermediate_results) {
    collector->to_csv(fmt::format("readresult_{}.csv", timeSinceEpochMillisec()));

#ifdef HASHMAP_COLLECT_META_INFO
    ASSERT(meta_collector_ptr != nullptr, "meta_collector_ptr is nullptr");
    static_cast<MetadataBenchmarkResultCollector*>(meta_collector_ptr)->to_csv(fmt::format("metaresult_{}.csv", timeSinceEpochMillisec()));
#endif
  }
}

template <class KeyT, class ValueT>
void generate_benchmark_data(std::vector<std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>>* fill_data,
                             std::vector<std::vector<KeyT>>* query_data, uint8_t load_factor, uint64_t thread_table_size,
                             DataDistribution distribution, uint8_t successful_query_rate, uint32_t thread_workload_size, uint8_t thread_count,
                             bool zipf_requests, uint64_t zipf_factor) {
  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;

  fill_data->resize(thread_count);
  query_data->resize(thread_count);

  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    // 1. Generate fill data
    spdlog::info(fmt::format("Generating fill data for thread {}/{} and load factor {}%...", thread, thread_count, load_factor));
    (*fill_data)[thread] = generate_fill_data<KeyT, ValueTNoPtr>(load_factor, thread_table_size, distribution);

    uint64_t fill_data_size = (*fill_data)[thread].size();

    // 2. Extract used keys
    spdlog::info(fmt::format("Extracting used keys, fill_data.size={}...", fill_data_size));

    std::vector<KeyT> used_keys;
    used_keys.reserve(fill_data_size);

    for (const auto& entry : (*fill_data)[thread]) {
      used_keys.push_back(entry.first);
    }

    // 3. Generate query data
    (*query_data)[thread] = generate_queries<KeyT>(successful_query_rate, thread_workload_size, used_keys, zipf_requests, zipf_factor);

    // clear used_keys, we don't need it anymore, save some memory
    used_keys.clear();
    used_keys.shrink_to_fit();
  }

  spdlog::info("Query data generated.");
}

template <class KeyT, class ValueT>
void write_benchmark_data_to_file(uint8_t load_factor, uint64_t hashtable_size, DataDistribution distribution,
                                  std::string external_benchmark_data_folder, uint8_t successful_query_rate, uint32_t workload, uint8_t thread_count,
                                  bool zipf_requests, uint64_t zipf_factor) {
  spdlog::info("Preparing query data to write to disk.");
  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;
  ASSERT(hashmap::utils::is_power_of_two(thread_count), "Invalid thread count");

  uint64_t thread_table_size = hashtable_size / static_cast<uint64_t>(thread_count);
  uint32_t thread_workload_size = workload / static_cast<uint32_t>(thread_count);

  std::vector<std::vector<std::pair<KeyT, ValueTNoPtr>>> fill_data;
  std::vector<std::vector<KeyT>> query_data;
  generate_benchmark_data<KeyT, ValueT>(&fill_data, &query_data, load_factor, thread_table_size, distribution, successful_query_rate,
                                        thread_workload_size, thread_count, zipf_requests, zipf_factor);

  ASSERT(fill_data.size() == thread_count && query_data.size() == thread_count, "INVALID SIZES");

  std::string fill_data_path = fmt::format("{}/filldata_{}_{}_{}_{}_{}_{}_{}_{}", external_benchmark_data_folder, hashtable_size, load_factor,
                                           static_cast<uint16_t>(distribution), successful_query_rate, workload, thread_count,
                                           hashmap::utils::data_type_to_str<KeyT>(), hashmap::utils::data_type_to_str<ValueTNoPtr>());

  std::string zipf = "nozipf";
  if (zipf_requests) {
    zipf = fmt::format("zipf{}", zipf_factor);
  }

  std::string query_data_path =
      fmt::format("{}/querydata_{}_{}_{}_{}_{}_{}_{}_{}", external_benchmark_data_folder, hashtable_size, load_factor,
                  static_cast<uint16_t>(distribution), successful_query_rate, workload, thread_count, hashmap::utils::data_type_to_str<KeyT>(), zipf);

  spdlog::info(fmt::format("Writing fill data to file {}.", fill_data_path));
  spdlog::info(fmt::format("Writing query data to file {}.", query_data_path));

  std::ofstream fill_data_out(fill_data_path, std::ios::out | std::ofstream::binary);
  fill_data_out.exceptions(std::ofstream::badbit | std::ofstream::failbit);

  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    std::vector<std::pair<KeyT, ValueTNoPtr>>& thread_fill_data = fill_data[thread];

    uint64_t number_fill_data_elems = thread_fill_data.size();
    fill_data_out.write(reinterpret_cast<const char*>(&number_fill_data_elems), sizeof(uint64_t));
    fill_data_out.flush();

    for (auto [key, value] : thread_fill_data) {
      if constexpr (std::is_same_v<KeyT, StringKey>) {
        fill_data_out.write(key.string_, HASHMAP_STRINGKEY_SIZE);
      } else {
        fill_data_out.write(reinterpret_cast<const char*>(&key), sizeof(KeyT));
      }
      fill_data_out.write(reinterpret_cast<const char*>(&value), sizeof(ValueTNoPtr));
    }
    fill_data_out.flush();
  }

  std::ofstream query_data_out(query_data_path, std::ios::out | std::ofstream::binary);
  query_data_out.exceptions(std::ofstream::badbit | std::ofstream::failbit);

  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    std::vector<KeyT>& thread_query_data = query_data[thread];

    uint64_t number_query_data_elems = thread_query_data.size();
    query_data_out.write(reinterpret_cast<const char*>(&number_query_data_elems), sizeof(uint64_t));
    query_data_out.flush();

    for (const KeyT& key : thread_query_data) {
      if constexpr (std::is_same_v<KeyT, StringKey>) {
        ASSERT(key.string_ != nullptr, "string is nullptr");
        query_data_out.write(key.string_, HASHMAP_STRINGKEY_SIZE);
      } else {
        query_data_out.write(reinterpret_cast<const char*>(&key), sizeof(KeyT));
      }
    }
    query_data_out.flush();
    spdlog::info(fmt::format("Wrote {} query elements for thread {}/{}", number_query_data_elems, thread, thread_count));
  }

  spdlog::info(fmt::format("Query data written to file. query_data.size={}, fill_data.size={}, query_data[0].size={}, fill_data[0].size={},",
                           query_data.size(), fill_data.size(), query_data[0].size(), fill_data[0].size()));
}

template <class KeyT, class ValueT>
void load_benchmark_data_from_file(std::vector<std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>>* fill_data,
                                   std::vector<std::vector<KeyT>>* query_data, uint8_t load_factor, uint64_t hashtable_size,
                                   DataDistribution distribution, std::string external_benchmark_data_folder, uint8_t successful_query_rate,
                                   uint32_t workload, uint8_t thread_count, bool zipf_requests, uint64_t zipf_factor) {
  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;
  ASSERT(hashmap::utils::is_power_of_two(thread_count), "Invalid thread count");

  fill_data->resize(thread_count);
  query_data->resize(thread_count);

  std::string fill_data_path = fmt::format("{}/filldata_{}_{}_{}_{}_{}_{}_{}_{}", external_benchmark_data_folder, hashtable_size, load_factor,
                                           static_cast<uint16_t>(distribution), successful_query_rate, workload, thread_count,
                                           hashmap::utils::data_type_to_str<KeyT>(), hashmap::utils::data_type_to_str<ValueTNoPtr>());

  std::string zipf = "nozipf";
  if (zipf_requests) {
    zipf = fmt::format("zipf{}", zipf_factor);
  }

  std::string query_data_path =
      fmt::format("{}/querydata_{}_{}_{}_{}_{}_{}_{}_{}", external_benchmark_data_folder, hashtable_size, load_factor,
                  static_cast<uint16_t>(distribution), successful_query_rate, workload, thread_count, hashmap::utils::data_type_to_str<KeyT>(), zipf);

  if (!std::filesystem::exists(fill_data_path) || !std::filesystem::exists(query_data_path)) {
    spdlog::warn(
        fmt::format("One of the following two files does not exist: {} and {}. \n Manually generating benchmark data and storing for the next run.",
                    fill_data_path, query_data_path));

    write_benchmark_data_to_file<KeyT, ValueTNoPtr>(load_factor, hashtable_size, distribution, external_benchmark_data_folder, successful_query_rate,
                                                    workload, thread_count, zipf_requests, zipf_factor);

    ASSERT(std::filesystem::exists(fill_data_path) && std::filesystem::exists(query_data_path),
           "Files still don't exist after generation, something went wrong.");
  }

  spdlog::info("Loading query data from file.");

  std::ifstream fill_data_infline;
  fill_data_infline.open(fill_data_path, std::ios::in | std::ofstream::binary);

  ASSERT(fill_data_infline, fmt::format("Error {} while opening file {}", std::strerror(errno), fill_data_path));

  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>& thread_fill_data = (*fill_data)[thread];
    uint64_t number_fill_data_elems = 0;
    fill_data_infline.read(reinterpret_cast<char*>(&number_fill_data_elems), sizeof(uint64_t));
    ASSERT(number_fill_data_elems > 0, "Something went wrong");
    spdlog::info(fmt::format("Reading {} fill elements for thread {}/{}", number_fill_data_elems, thread, thread_count));

    char* string_key_storage = nullptr;

    if constexpr (std::is_same_v<KeyT, StringKey>) {
      string_key_storage = reinterpret_cast<char*>(std::calloc(number_fill_data_elems * HASHMAP_STRINGKEY_SIZE, sizeof(char)));
      ASSERT(string_key_storage != nullptr, "Got nullptr from malloc");
    }

    for (uint64_t i = 0; i < number_fill_data_elems; ++i) {
      KeyT k;
      ValueTNoPtr v;
      if constexpr (std::is_same_v<KeyT, StringKey>) {
        char* target = string_key_storage + (i * HASHMAP_STRINGKEY_SIZE);
        k = StringKey(target);
        fill_data_infline.read(target, HASHMAP_STRINGKEY_SIZE);
      } else {
        fill_data_infline.read(reinterpret_cast<char*>(&k), sizeof(KeyT));
      }
      fill_data_infline.read(reinterpret_cast<char*>(&v), sizeof(ValueTNoPtr));
      thread_fill_data.push_back({k, v});
    }

    ASSERT(thread_fill_data.size() == number_fill_data_elems,
           fmt::format("Something went wrong. Was expecting to read {} elements, read {} instead.", number_fill_data_elems, thread_fill_data.size()));
  }

  std::ifstream query_data_infline;
  query_data_infline.open(query_data_path, std::ios::in | std::ofstream::binary);

  ASSERT(query_data_infline, fmt::format("Error {} while opening file {}", std::strerror(errno), fill_data_path));
  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    std::vector<KeyT>& thread_query_data = (*query_data)[thread];

    uint64_t number_query_data_elems = 0;
    query_data_infline.read(reinterpret_cast<char*>(&number_query_data_elems), sizeof(uint64_t));
    ASSERT(number_query_data_elems > 0, "Something went wrong, there are 0 query elements in file.");

    char* string_querykey_storage = nullptr;

    if constexpr (std::is_same_v<KeyT, StringKey>) {
      string_querykey_storage = reinterpret_cast<char*>(std::calloc(number_query_data_elems * HASHMAP_STRINGKEY_SIZE, sizeof(char)));
      ASSERT(string_querykey_storage != nullptr, "Got nullptr from malloc");
    }

    spdlog::info(fmt::format("Reading {} query elements for thread {}/{}", number_query_data_elems, thread, thread_count));
    for (uint64_t i = 0; i < number_query_data_elems; ++i) {
      KeyT k;
      if constexpr (std::is_same_v<KeyT, StringKey>) {
        char* target = string_querykey_storage + (i * HASHMAP_STRINGKEY_SIZE);
        k = StringKey(target);
        query_data_infline.read(target, HASHMAP_STRINGKEY_SIZE);
      } else {
        query_data_infline.read(reinterpret_cast<char*>(&k), sizeof(KeyT));
      }
      thread_query_data.push_back(k);
    }
    ASSERT(
        thread_query_data.size() == number_query_data_elems,
        fmt::format("Something went wrong. Was expecting to read {} elements, read {} instead.", number_query_data_elems, thread_query_data.size()));
  }

  spdlog::info(fmt::format("Query data loaded from file. fill_data.size={}, query_data.size={}, fill_data[0].size={}, query_data[0].size={}",
                           fill_data->size(), query_data->size(), (*query_data)[0].size(), (*fill_data)[0].size()));
}

template <class KeyT, class ValueT, class... Types>
void read_bench(uint8_t load_factor, uint8_t successful_query_rate, uint64_t hashtable_size, DataDistribution distribution,
                uint32_t workload /* in MB w.r.t. sizeof(KeyT) */, uint8_t runs_per_hashmap, uint8_t thread_count,
                ReadBenchmarkResultCollector* collector, bool store_intermediate_results, bool warmup_run,
                const std::vector<std::vector<std::pair<KeyT, ValueT>>>& prefault_data, bool zipf_requests, uint64_t zipf_factor,
                std::optional<std::string> external_benchmark_data_folder = std::nullopt, [[maybe_unused]] void* meta_collector_ptr = nullptr) {
  ASSERT(successful_query_rate >= 0 && successful_query_rate <= 100, "Invalid SQR");
  ASSERT(load_factor >= 0 && load_factor <= 100, "Invalid LF");
  ASSERT(hashmap::utils::is_power_of_two(thread_count), "Invalid thread count");
  ASSERT(prefault_data.size() == thread_count, "Invalid size of prefault array");

  constexpr std::size_t number_of_hashtables = sizeof...(Types);
  uint64_t thread_table_size = hashtable_size / static_cast<uint64_t>(thread_count);
  uint32_t thread_workload_size = workload / static_cast<uint32_t>(thread_count);

  spdlog::info(fmt::format(
      "Hello, this is the hashmap read benchmark. Running for {} hash tables, {} times per hashtable. "
      "LF={}, SQR={}, Size={}, Distribution={}, Workload={}, Workload/Thread={}, thread_count={}, size_per_thread={}, use_zipf={}, zipf_factor={}",
      number_of_hashtables, static_cast<int>(runs_per_hashmap), static_cast<int>(load_factor), static_cast<int>(successful_query_rate),
      hashtable_size, static_cast<int>(distribution), workload, thread_workload_size, thread_count, thread_table_size, zipf_requests, zipf_factor));

  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;

  std::vector<std::vector<std::pair<KeyT, ValueTNoPtr>>> fill_data;
  std::vector<std::vector<KeyT>> query_data;

  if (external_benchmark_data_folder.has_value()) {
    load_benchmark_data_from_file<KeyT, ValueT>(&fill_data, &query_data, load_factor, hashtable_size, distribution,
                                                external_benchmark_data_folder.value(), successful_query_rate, workload, thread_count, zipf_requests,
                                                zipf_factor);
  } else {
    generate_benchmark_data<KeyT, ValueT>(&fill_data, &query_data, load_factor, thread_table_size, distribution, successful_query_rate, workload,
                                          thread_count, zipf_requests, zipf_factor);
  }

  ASSERT(fill_data.size() == thread_count, "Incorrect fill data size");
  ASSERT(query_data.size() == thread_count, "Incorrect query data size");

  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    ASSERT(fill_data[thread].size() == fill_data[0].size(), "Inconsistent fill data size");  // doesn't check for correctness but at least equality
    ASSERT(query_data[thread].size() == query_data[0].size(), "Inconsistent query data size");
  }

  spdlog::info("Query data obtained, calling benchmark loop.");

  (read_bench_loop_impl<KeyT, ValueT, Types>(runs_per_hashmap, hashtable_size, thread_table_size, load_factor, successful_query_rate, distribution,
                                             workload, thread_count, fill_data, query_data, collector, store_intermediate_results, warmup_run,
                                             zipf_requests, zipf_factor, prefault_data, meta_collector_ptr),
   ...);

  if constexpr (std::is_same_v<KeyT, StringKey>) {
    spdlog::info("Cleaning up string data.");
    for (uint8_t thread = 0; thread < thread_count; ++thread) {
      query_data[thread][0].free();
      fill_data[thread][0].first.free();
    }
  }
}

}  // namespace benchmark::read
