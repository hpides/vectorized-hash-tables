#include "benchmark/write_benchmark.hpp"

#include "benchmark/benchmark_utils.hpp"

namespace benchmark::write {

void WriteBenchmarkResultCollector::add_benchmark_result(WriteBenchmarkResult result) { benchmark_results_.push_back(result); }

void WriteBenchmarkResultCollector::to_csv(std::filesystem::path path) {
  ASSERT(!std::filesystem::exists(path), "File to write to already exists!");
  std::ofstream result_file;
  result_file.open(path);
  result_file << "Hashmap,Compiler,SystemHostname,PageSize,HugePageSize,DataPointer,IsAlignedToHPSize,LoadFactor,Size,ThreadCount,ThreadTableSize,"
                 "Distribution,"
                 "KeySize,ValueSize,EntrySize,EntriesProcessed,Runtime,ThreadAvgRuntime,ThreadMaxRuntime,Successful"
              << std::endl;

  for (const WriteBenchmarkResult& result : benchmark_results_) {
    result_file << fmt::format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}", result.hashmap_identifier, get_compiler_identifier(),
                               get_hostname(), hashmap::utils::page_size, hashmap::utils::hugepage_size, result.data_ptr, result.is_aligned_to_hp,
                               result.load_factor, result.hashtable_size, result.thread_count, result.threadtable_size, result.distribution_name,
                               result.key_size, result.value_size, result.entry_size, result.entries_processed, result.runtime,
                               result.thread_avg_runtime, result.thread_max_runtime, result.successful)
                << std::endl;
  }

  result_file.close();
}

}  // namespace benchmark::write
