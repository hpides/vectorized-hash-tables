#include "benchmark/benchmark_shared.hpp"

#include "hashmap/utils.hpp"

namespace benchmark {

DummyTuple::operator uint64_t() const { return value1 + value2 + value4 + value7 + value8 + value10 + value12 + value14 + value16; }

// The configuration of the benchmark suite is all really messy and the entire configuration via DEFINES/command line/recompiling should be
// refactored.
std::vector<uint8_t> parse_thread_counts(int argc, char** argv) {
  if (argc == 1) {
    spdlog::info("No thread count supplied, defaulting to single threaded run.");
    return {1};
  } else {
    uint8_t i = 1;
    if (std::string(argv[1]) == "generate" || std::string(argv[1]) == "disable-store") {
      ++i;
      if (argc == 2) {
        spdlog::info("No thread count supplied, defaulting to single threaded run.");
        return {1};
      }
    }

    std::vector<uint8_t> thread_counts;
    thread_counts.reserve(argc - i);
    for (; i < argc; ++i) {
      uint8_t thread_count = static_cast<uint8_t>(std::stoi(std::string(argv[i])));
      ASSERT(hashmap::utils::is_power_of_two(thread_count), "Thread Count must be power of 2!");
      thread_counts.push_back(thread_count);
    }

    spdlog::info(fmt::format("Parsed thread counts: {}", fmt::join(thread_counts, " ")));

    return thread_counts;
  }
}

std::string get_distribution_name(DataDistribution distribution) {
  switch (distribution) {
    case DataDistribution::UNIFORM:
      return "Uniform";
    case DataDistribution::DENSE:
      return "Dense";
    case DataDistribution::ZIPF:
      return "Zipf";
  }

  return "Invalid";
}

std::vector<StringKey> generate_fill_strings_on_heap(uint64_t n, std::optional<std::vector<StringKey>*> keys_to_exclude) {
  std::vector<StringKey> keys;
  spdlog::info(fmt::format("Generating {} unique string keys...", n));

  while (keys.size() < n) {
    uint64_t missing_keys = n - keys.size();
    std::vector<StringKey> temporary_vector;
    temporary_vector.reserve(missing_keys);

    for (uint64_t i = 0; i < missing_keys; ++i) {
      std::string curr_str = hashmap::utils::random_string(HASHMAP_STRINGKEY_SIZE - 1);  // -1 for 0 termination
      char* target_cstr = reinterpret_cast<char*>(std::calloc(HASHMAP_STRINGKEY_SIZE, sizeof(char)));
      std::strcpy(target_cstr, curr_str.c_str());
      temporary_vector.push_back(StringKey(target_cstr));

      if (i == 0 || i % 1000 == 0) {
        printProgress(static_cast<double>(i) / static_cast<double>(missing_keys));
      }
    }
    printProgress(1);

#ifndef HASHMAP_IS_PROFILE_BUILD
    std::cout << std::endl;
#endif
    if (keys_to_exclude.has_value()) {
      std::vector<StringKey>* keys_to_excl = keys_to_exclude.value();
      spdlog::info("Sorting keys_to_exclude...");
      std::sort(keys_to_excl->begin(), keys_to_excl->end());

      std::vector<StringKey> adapted_vector;
      adapted_vector.reserve(missing_keys);
      spdlog::info("Building new vector with keys_to_exclude...");

      for (uint64_t i = 0; i < temporary_vector.size(); i++) {
        if (std::binary_search(keys_to_excl->begin(), keys_to_excl->end(), temporary_vector[i])) {
          continue;
        }
        if (i == 0 || i % 1000 == 0) {
          printProgress(static_cast<double>(i) / static_cast<double>(missing_keys));
        }
        adapted_vector.push_back(temporary_vector[i]);
      }
      printProgress(1);

#ifndef HASHMAP_IS_PROFILE_BUILD
      std::cout << std::endl;
#endif
      temporary_vector.clear();
      temporary_vector = adapted_vector;
    }

    spdlog::info("Making keys unique...");
    spdlog::info("Sorting temporary vector...");
    std::sort(temporary_vector.begin(), temporary_vector.end());

    spdlog::info("Erasing duplicate elements");
    temporary_vector.erase(std::unique(temporary_vector.begin(), temporary_vector.end()), temporary_vector.end());

    spdlog::info("Appending to final vector...");
    keys.insert(keys.end(), temporary_vector.begin(), temporary_vector.end());

    if (keys.size() < n) {
      spdlog::info(fmt::format("First iteration only created {}/{} unique keys, doing another iteration.", keys.size(), n));
    } else {
      spdlog::info(fmt::format("Successfully created {} unique keys, done.", keys.size()));
    }
  }

  return keys;
}

#ifdef HASHMAP_COLLECT_META_INFO
void MetadataBenchmarkResultCollector::add_benchmark_result(MetadataBenchmarkResult result) { benchmark_results_.push_back(result); }

void MetadataBenchmarkResultCollector::to_csv(std::filesystem::path path) {
  ASSERT(!std::filesystem::exists(path), "File to write to already exists!");
  std::ofstream result_file;
  result_file.open(path);
  result_file
      << "Timestamp,Hashmap,Compiler,SystemHostname,PageSize,HugePageSize,DataPointer,IsAlignedToHPSize,LoadFactor,SQR,Size,Distribution,Workload,"
         "KeySize,ValueSize,"
         "EntrySize,EntriesProcessed,NumFinds,ProbedElements,TotalProbedElements,SIMDLoads,NumCollisions,NumOverflowsFollowed,NumOverflows,"
         "NumBuckets,MinProbingSequence,MaxProbingSequence"
      << std::endl;

  for (const MetadataBenchmarkResult& result : benchmark_results_) {
    result_file << fmt::format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}",
                               benchmark::timeSinceEpochMillisec(), result.hashmap_identifier, get_compiler_identifier(), get_hostname(),
                               hashmap::utils::page_size, hashmap::utils::hugepage_size, result.data_ptr, result.is_aligned_to_hp, result.load_factor,
                               result.successful_query_rate, result.hashtable_size, result.distribution_name, result.workload, result.key_size,
                               result.value_size, result.entry_size, result.entries_processed, result.minfo.num_finds, result.minfo.probed_elements,
                               result.minfo.probed_elements_total, result.minfo.simd_loads, result.minfo.num_collision,
                               result.minfo.num_overflows_followed, result.minfo.num_overflows, result.minfo.num_buckets,
                               result.minfo.min_probing_sequence, result.minfo.max_probing_sequence)
                << std::endl;
  }

  result_file.close();
}
#endif

}  // namespace benchmark
