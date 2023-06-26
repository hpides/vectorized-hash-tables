#pragma once
#include <algorithm>
#include <barrier>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <random>
#include <thread>
#include <unordered_set>

#include "benchmark/benchmark_shared.hpp"
#include "benchmark/benchmark_utils.hpp"
#include "fmt/format.h"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

namespace benchmark::write {

struct WriteBenchmarkResult {
  std::string hashmap_identifier = "";
  uint8_t load_factor = 0;
  uint64_t hashtable_size = 0;
  uint8_t thread_count = 0;
  uint64_t threadtable_size = 0;
  std::string distribution_name = "";
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
};

class WriteBenchmarkResultCollector {
 public:
  WriteBenchmarkResultCollector() = default;
  void add_benchmark_result(WriteBenchmarkResult result);
  void to_csv(std::filesystem::path path);

 private:
  std::vector<WriteBenchmarkResult> benchmark_results_;
};

template <class KeyT, class ValueT>
std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>> generate_queries(uint8_t load_factor, uint64_t hashtable_size,
                                                                                          DataDistribution distribution) {
  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;

  uint64_t total_queries = (load_factor * hashtable_size) / 100 + (((load_factor * hashtable_size) % 100) != 0);

  ASSERT(total_queries < hashtable_size, fmt::format("Hash table size is {}, but target load factor {} requires {} insertions "
                                                     "which does not fit into the hashmap. Either increase hash map size or reduce workload.",
                                                     hashtable_size, load_factor, total_queries));

  std::vector<std::pair<KeyT, ValueTNoPtr>> result_queries;
  result_queries.reserve(total_queries);
  spdlog::info(fmt::format("Pre-generating {} benchmark insertion queries...", total_queries));

  spdlog::info(fmt::format("Starting with generation of keys..."));
  std::vector<KeyT> insertion_keys;

  if constexpr (std::is_same_v<KeyT, StringKey>) {
    insertion_keys = generate_fill_strings_on_heap(total_queries, std::nullopt);
  } else {
    insertion_keys = generate_n_unique_integer_keys<KeyT>(total_queries, distribution, std::nullopt);
  }

  spdlog::info(fmt::format("Generation of keys done, inserting into final query vector while generating values..."));

  ASSERT(insertion_keys.size() == total_queries,
         fmt::format("Insertion key generation did not generate the correct amount of keys ({} vs {})).", insertion_keys.size(), total_queries));

  for (const KeyT& key : insertion_keys) {
    result_queries.push_back({key, generate_random_value<ValueT>()});
  }

  // 4. Shuffle queries
  spdlog::info(fmt::format("Shuffling {} queries...", total_queries));
  std::shuffle(std::begin(result_queries), std::end(result_queries), reproducible_gen());

  ASSERT(result_queries.size() == total_queries,
         fmt::format("Query generation did not generate the correct amount of queries ({} vs {})).", result_queries.size(), total_queries));

  return result_queries;
}

template <class KeyT, class ValueT, class HashtableT>
void do_work(uint64_t hashtable_size, uint8_t thread_id, uint8_t load_factor,
             std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>* query_data_ptr,
             const std::vector<std::pair<KeyT, ValueT>>* prefault_data_ptr, std::atomic<bool>* result_success, std::atomic<uint64_t>* result_runtime,
             std::string* result_hashmap_identifier, std::atomic<uint64_t>* result_entry_size, std::atomic<uint64_t>* result_entries_processed,
             std::string* result_data_ptr, std::atomic<bool>* result_is_aligned, std::barrier<std::__empty_completion>* barrier) {
  std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>& query_data = *query_data_ptr;
  const std::vector<std::pair<KeyT, ValueT>>& prefault_data = *prefault_data_ptr;

  spdlog::info(fmt::format("[Thread {}] Allocating hashtable memory for write benchmark.", thread_id));
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

    if (barrier != nullptr) {
      barrier->arrive_and_wait();
      barrier->arrive_and_wait();
    }
    return;
  }

  // 1. Prefault table
  spdlog::info(fmt::format("[Thread {}] Prefaulting hash table of size {}...", thread_id, hashtable_size));
  hashtable.prefault_pregenerated(prefault_data);
  spdlog::info(fmt::format("[Thread {}] Hashmap load is {} (should be 0!)", thread_id, hashtable.get_current_load()));

  ClobberMemory();
  if (barrier != nullptr) {
    barrier->arrive_and_wait();
  }
  ClobberMemory();

  auto start = std::chrono::high_resolution_clock::now();

  if constexpr (std::is_pointer_v<ValueT>) {
    for (auto& entry : query_data) {
      hashtable.insert(entry.first, &entry.second);
    }
  } else {
    for (const auto& entry : query_data) {
      hashtable.insert(entry.first, entry.second);
    }
  }

  ClobberMemory();

  auto end = std::chrono::high_resolution_clock::now();

  *result_runtime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
  *result_success = true;

  if (barrier != nullptr) {
    barrier->arrive_and_wait();
  }

  spdlog::info(
      fmt::format("[Thread {}] Hashmap load is now {}, size is {}. Exiting.", thread_id, hashtable.get_current_load(), hashtable.get_current_size()));
}

template <class KeyT, class ValueT, class HashtableT>
void write_bench_impl(uint64_t hashtable_size, uint64_t thread_table_size, uint8_t load_factor, DataDistribution distribution, uint8_t thread_count,
                      std::vector<std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>>& query_data,
                      WriteBenchmarkResultCollector* collector, bool warmup_run,
                      const std::vector<std::vector<std::pair<KeyT, ValueT>>>& prefault_data) {
  spdlog::info("Running write benchmark");

  if constexpr (std::is_pointer_v<ValueT>) {
    spdlog::info("ValueType is pointer-type.");
  } else {
    spdlog::info("ValueType is not pointer-type.");
  }

  WriteBenchmarkResult result;
  result.load_factor = load_factor;
  result.hashtable_size = hashtable_size;
  result.thread_count = thread_count;
  result.threadtable_size = thread_table_size;
  result.distribution_name = get_distribution_name(distribution);
  result.key_size = sizeof(KeyT);
  result.value_size = sizeof(ValueT);
  result.successful = false;

  if (warmup_run) {
    spdlog::info("Doing warmup run...");
  }

  spdlog::info("Starting benchmark....");
  ClobberMemory();
  if (thread_count == 1) {
    std::atomic<bool> successful = false;
    std::atomic<uint64_t> runtime = 0;
    std::atomic<uint64_t> entry_size = 0;
    std::atomic<uint64_t> entries_processed = 0;
    std::atomic<bool> is_aligned_to_hp = false;

    // single-thread = do everything in main thread
    do_work<KeyT, ValueT, HashtableT>(thread_table_size, 0, load_factor, &query_data[0], &prefault_data[0], &successful, &runtime,
                                      &result.hashmap_identifier, &entry_size, &entries_processed, &result.data_ptr, &is_aligned_to_hp, nullptr);

    result.successful = successful.load();
    result.runtime = runtime.load();
    result.thread_avg_runtime = static_cast<double>(result.runtime);
    result.thread_max_runtime = result.runtime;
    result.entry_size = entry_size.load();
    result.entries_processed = entries_processed.load();
    result.is_aligned_to_hp = is_aligned_to_hp.load();

    if (!result.successful) {
      spdlog::error("Got no success!");
      result.runtime = 0;
    }

  } else {
    std::barrier sync_point(thread_count + 1);  // we wait for the main thread + worker threads

    // 1. init all threads
    std::vector<std::thread> workers;

    std::vector<std::atomic<uint8_t>> successfuls(thread_count);
    std::vector<std::atomic<uint64_t>> runtimes(thread_count);
    std::vector<std::string> identifiers(thread_count);
    std::vector<std::atomic<uint64_t>> entry_sizes(thread_count);
    std::vector<std::atomic<uint64_t>> entries_processed(thread_count);
    std::vector<std::string> data_ptrs(thread_count);
    std::vector<std::atomic<uint8_t>> is_aligneds(thread_count);

    for (uint8_t thread = 0; thread < thread_count; ++thread) {
      workers.push_back(std::thread(do_work<KeyT, ValueT, HashtableT>, thread_table_size, thread, load_factor, &query_data[thread],
                                    &prefault_data[thread], reinterpret_cast<std::atomic<bool>*>(&successfuls[thread]), &runtimes[thread],
                                    &identifiers[thread], &entry_sizes[thread], &entries_processed[thread], &data_ptrs[thread],
                                    reinterpret_cast<std::atomic<bool>*>(&is_aligneds[thread]), &sync_point));
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

    spdlog::info(fmt::format("Benchmark finished in {} milliseconds, i.e. {} Inserts/s.", result.runtime, lookup_throughput));

    std::cout << std::endl;
  }

  if (!warmup_run) {
    collector->add_benchmark_result(result);
  } else {
    spdlog::info("Not storing result, due to warmup.");
  }
}

template <class KeyT, class ValueT>
void generate_benchmark_data(std::vector<std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>>* query_data, uint8_t load_factor,
                             uint64_t thread_table_size, DataDistribution distribution, uint8_t thread_count) {
  query_data->resize(thread_count);
  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    // Generate query data
    spdlog::info(fmt::format("Generating data for thread {}/{} and load factor {}%...", thread, thread_count, load_factor));
    (*query_data)[thread] = generate_queries<KeyT, ValueT>(load_factor, thread_table_size, distribution);
  }
  spdlog::info("Query data generated.");
}

template <class KeyT, class ValueT>
void write_benchmark_data_to_file(uint8_t load_factor, uint64_t hashtable_size, DataDistribution distribution,
                                  std::string external_benchmark_data_folder, uint8_t thread_count) {
  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;
  ASSERT(hashmap::utils::is_power_of_two(thread_count), "Invalid thread count");

  uint64_t thread_table_size = hashtable_size / static_cast<uint64_t>(thread_count);

  spdlog::info("Preparing query data to write to disk.");

  std::vector<std::vector<std::pair<KeyT, ValueTNoPtr>>> query_data;
  generate_benchmark_data<KeyT, ValueT>(&query_data, load_factor, thread_table_size, distribution, thread_count);
  ASSERT(query_data.size() == thread_count, "Generation gone wrong.");

  std::string query_data_path = fmt::format("{}/writequerydata_{}_{}_{}_{}_{}_{}", external_benchmark_data_folder, hashtable_size, load_factor,
                                            static_cast<uint16_t>(distribution), thread_count, hashmap::utils::data_type_to_str<KeyT>(),
                                            hashmap::utils::data_type_to_str<ValueTNoPtr>());

  spdlog::info(fmt::format("Writing query data to file {}.", query_data_path));
  std::ofstream query_data_out(query_data_path, std::ios::out | std::ofstream::binary);
  query_data_out.exceptions(std::ofstream::badbit | std::ofstream::failbit);

  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    std::vector<std::pair<KeyT, ValueTNoPtr>>& thread_query_data = query_data[thread];

    uint64_t number_query_data_elems = thread_query_data.size();
    query_data_out.write(reinterpret_cast<const char*>(&number_query_data_elems), sizeof(uint64_t));

    for (auto [key, value] : thread_query_data) {
      if constexpr (std::is_same_v<KeyT, StringKey>) {
        query_data_out.write(key.string_, HASHMAP_STRINGKEY_SIZE);
      } else {
        query_data_out.write(reinterpret_cast<const char*>(&key), sizeof(KeyT));
      }
      query_data_out.write(reinterpret_cast<const char*>(&value), sizeof(ValueTNoPtr));
    }
    query_data_out.flush();
  }

  spdlog::info(fmt::format("Query data written to file. query_data.size={}, query_data[0].size={}", query_data.size(), query_data[0].size()));
}

template <class KeyT, class ValueT>
void load_benchmark_data_from_file(std::vector<std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>>* query_data,
                                   uint8_t load_factor, uint64_t hashtable_size, DataDistribution distribution,
                                   std::string external_benchmark_data_folder, uint8_t thread_count) {
  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;
  ASSERT(hashmap::utils::is_power_of_two(thread_count), "Invalid thread count");

  query_data->resize(thread_count);

  std::string query_data_path = fmt::format("{}/writequerydata_{}_{}_{}_{}_{}_{}", external_benchmark_data_folder, hashtable_size, load_factor,
                                            static_cast<uint16_t>(distribution), thread_count, hashmap::utils::data_type_to_str<KeyT>(),
                                            hashmap::utils::data_type_to_str<ValueTNoPtr>());

  if (!std::filesystem::exists(query_data_path)) {
    spdlog::warn(fmt::format("{} does not exist. \n Manually generating benchmark data and storing for the next run.", query_data_path));

    write_benchmark_data_to_file<KeyT, ValueTNoPtr>(load_factor, hashtable_size, distribution, external_benchmark_data_folder, thread_count);

    ASSERT(std::filesystem::exists(query_data_path), "File still doesn't exist after generation, something went wrong.");
  }

  spdlog::info("Loading query data from file.");

  std::ifstream query_data_infline;
  query_data_infline.open(query_data_path, std::ios::in | std::ofstream::binary);

  ASSERT(query_data_infline, fmt::format("Error {} while opening file {}", std::strerror(errno), query_data_path));

  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>& thread_query_data = (*query_data)[thread];

    uint64_t number_query_data_elems = 0;
    query_data_infline.read(reinterpret_cast<char*>(&number_query_data_elems), sizeof(uint64_t));
    ASSERT(number_query_data_elems > 0, "Something went wrong");

    char* string_querykey_storage = nullptr;

    if constexpr (std::is_same_v<KeyT, StringKey>) {
      string_querykey_storage = reinterpret_cast<char*>(std::calloc(number_query_data_elems * HASHMAP_STRINGKEY_SIZE, sizeof(char)));
      ASSERT(string_querykey_storage != nullptr, "Got nullptr from malloc");
    }

    spdlog::info(fmt::format("Reading {} query elements", number_query_data_elems));
    for (uint64_t i = 0; i < number_query_data_elems; ++i) {
      KeyT k;
      ValueTNoPtr v;
      if constexpr (std::is_same_v<KeyT, StringKey>) {
        char* target = string_querykey_storage + (i * HASHMAP_STRINGKEY_SIZE);
        k = StringKey(target);
        query_data_infline.read(target, HASHMAP_STRINGKEY_SIZE);
      } else {
        query_data_infline.read(reinterpret_cast<char*>(&k), sizeof(KeyT));
      }
      query_data_infline.read(reinterpret_cast<char*>(&v), sizeof(ValueTNoPtr));
      thread_query_data.push_back({k, v});
    }

    ASSERT(thread_query_data.size() == number_query_data_elems, "Something went wrong.");
  }

  spdlog::info(fmt::format("Query data loaded from file. query_data.size={}, query_data[0].size={}", query_data->size(), (*query_data)[0].size()));
}

template <class KeyT, class ValueT, class HashtableT>
void write_bench_loop_impl(uint8_t runs_per_hashmap, uint64_t hashtable_size, uint64_t thread_table_size, uint8_t load_factor,
                           DataDistribution distribution, uint8_t thread_count,
                           std::vector<std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>>>& query_data,
                           WriteBenchmarkResultCollector* collector, bool store_intermediate_results, bool warmup_run,
                           const std::vector<std::vector<std::pair<KeyT, ValueT>>>& prefault_data) {
  if (warmup_run) {
    write_bench_impl<KeyT, ValueT, HashtableT>(hashtable_size, thread_table_size, load_factor, distribution, thread_count, query_data, collector,
                                               true, prefault_data);
  }

  for (uint8_t i = 0; i < runs_per_hashmap; ++i) {
    write_bench_impl<KeyT, ValueT, HashtableT>(hashtable_size, thread_table_size, load_factor, distribution, thread_count, query_data, collector,
                                               false, prefault_data);
  }

  if (store_intermediate_results) {
    collector->to_csv(fmt::format("writeresult_{}.csv", timeSinceEpochMillisec()));
  }
}

template <class KeyT, class ValueT, class... Types>
void write_bench(uint8_t load_factor, uint64_t hashtable_size, DataDistribution distribution, uint8_t runs_per_hashmap, uint8_t thread_count,
                 WriteBenchmarkResultCollector* collector, bool store_intermediate_results, bool warmup_run,
                 const std::vector<std::vector<std::pair<KeyT, ValueT>>>& prefault_data,
                 std::optional<std::string> external_benchmark_data_folder = std::nullopt) {
  ASSERT(load_factor >= 0 && load_factor <= 100, "Invalid LF");
  ASSERT(hashmap::utils::is_power_of_two(thread_count), "Invalid thread count");
  ASSERT(prefault_data.size() == thread_count, "Invalid size of prefault array");

  constexpr std::size_t number_of_hashtables = sizeof...(Types);
  uint64_t thread_table_size = hashtable_size / static_cast<uint64_t>(thread_count);

  spdlog::info(
      fmt::format("Hello, this is the hashmap write benchmark. Running for {} hash tables, {} times per hashtable. "
                  "LF={}, Size={}, Distribution={}, thread_count={}, size_per_thread={}",
                  number_of_hashtables, static_cast<int>(runs_per_hashmap), static_cast<int>(load_factor), hashtable_size,
                  static_cast<int>(distribution), thread_count, thread_table_size));

  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;
  std::vector<std::vector<std::pair<KeyT, ValueTNoPtr>>> query_data;

  if (external_benchmark_data_folder.has_value()) {
    spdlog::info("Loading benchmarking data from disk.");
    load_benchmark_data_from_file<KeyT, ValueT>(&query_data, load_factor, hashtable_size, distribution, external_benchmark_data_folder.value(),
                                                thread_count);
  } else {
    spdlog::info("Not loading benchmarking data from disk.");
    generate_benchmark_data<KeyT, ValueT>(&query_data, load_factor, thread_table_size, distribution, thread_count);
  }
  ASSERT(query_data.size() == thread_count, "Incorrect query data size");

  spdlog::info("Query data obtained, calling benchmark loop.");

  (write_bench_loop_impl<KeyT, ValueT, Types>(runs_per_hashmap, hashtable_size, thread_table_size, load_factor, distribution, thread_count,
                                              query_data, collector, store_intermediate_results, warmup_run, prefault_data),
   ...);
}

}  // namespace benchmark::write
