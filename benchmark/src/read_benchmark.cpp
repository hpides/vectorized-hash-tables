#include "benchmark/read_benchmark.hpp"

#include "benchmark/benchmark_utils.hpp"

namespace benchmark::read {

void ReadBenchmarkResultCollector::add_benchmark_result(ReadBenchmarkResult result) { benchmark_results_.push_back(result); }

void ReadBenchmarkResultCollector::to_csv(std::filesystem::path path) {
  ASSERT(!std::filesystem::exists(path), "File to write to already exists!");
  std::ofstream result_file;
  result_file.open(path);
  result_file << "Timestamp,Hashmap,Compiler,SystemHostname,PageSize,HugePageSize,DataPointer,IsAlignedToHPSize,LoadFactor,SQR,Size,ThreadCount,"
                 "ThreadTableSize,Distribution,Workload,"
                 "KeySize,ValueSize,"
                 "EntrySize,EntriesProcessed,Runtime,ThreadAvgRuntime,ThreadMaxRuntime,Zipf,ZipfFactor,Successful"
              << std::endl;

  for (const ReadBenchmarkResult& result : benchmark_results_) {
    result_file << fmt::format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}", benchmark::timeSinceEpochMillisec(),
                               result.hashmap_identifier, get_compiler_identifier(), get_hostname(), hashmap::utils::page_size,
                               hashmap::utils::hugepage_size, result.data_ptr, result.is_aligned_to_hp, result.load_factor,
                               result.successful_query_rate, result.hashtable_size, result.thread_count, result.threadtable_size,
                               result.distribution_name, result.workload, result.key_size, result.value_size, result.entry_size,
                               result.entries_processed, result.runtime, result.thread_avg_runtime, result.thread_max_runtime, result.zipf,
                               result.zipf_factor, result.successful)
                << std::endl;
  }

  result_file.close();
}

}  // namespace benchmark::read
