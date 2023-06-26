#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include "fmt/format.h"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap;

namespace hashmap::hashmaps {

// Question: if we have PointerValues, do store the values itself in the ChainedHashTable (would invalidate the assumption that we can upadte
// externally) or store pointer (even more random memory access)

// TODO: add load factor (ignored) for all other hash maps (they do not utilize the information)

enum class MemoryBudget { KeyValue, Bucket8FP, Bucket16FP };

template <typename KeyT, typename ValueT>
struct ChainedHashTableEntry {
  KeyT key = KeyT{};
  ValueT value = ValueT{};
  ChainedHashTableEntry* next = nullptr;
} __attribute__((__packed__));

// additional budget is in percent, e.g. 10 = 10% additional memory
template <typename KeyT, typename ValueT, typename HasherT, bool use_thp = false, MemoryBudget budget = MemoryBudget::KeyValue,
          uint8_t additional_budget = 10, bool set_bool_on_invalid_budget = false>
class ChainedHashTable : public HashTable<KeyT, ValueT> {
  using HTE = ChainedHashTableEntry<KeyT, ValueT>;

 public:
  struct FindResult {
    uint64_t directory_idx = 0;
    HTE* entry;
    HTE* previous_entry;
    bool is_valid = false;
  };

  static std::pair<uint64_t, uint64_t> calculate_directory_buffer_size(const uint64_t& max_elements, const uint8_t& target_load_factor) {
    const double additional_factor = 1.0 + (static_cast<double>(additional_budget) / 100.0);
    const uint64_t load_factor_elements =
        static_cast<uint64_t>(std::ceil((static_cast<double>(target_load_factor) / 100.0) * static_cast<double>(max_elements)));

    // KeyValueBudget (is_valid bit is disregarded, this is why we have the additional budget)
    uint64_t entry_size = sizeof(KeyT) + sizeof(ValueT);

    if constexpr (budget == MemoryBudget::Bucket8FP) {
      entry_size += 1;  // 1 byte fingerprint
    } else if constexpr (budget == MemoryBudget::Bucket16FP) {
      entry_size += 2;  // 2 byte fingerprint
    }

    const uint64_t memory_budget =
        static_cast<uint64_t>(std::ceil(additional_factor * static_cast<double>(entry_size) * static_cast<double>(max_elements)));
    const uint64_t buffer_memory_usage = load_factor_elements * sizeof(HTE);

    spdlog::debug(fmt::format("Got a memory budget of {}, will insert {} elements, buffer memory usage is {}", memory_budget, load_factor_elements,
                              buffer_memory_usage));

    const int64_t pot = static_cast<int64_t>(std::log2(max_elements));
    ASSERT(static_cast<uint64_t>(std::pow(2, pot)) == max_elements,
           fmt::format("Could not obtain power of two of max_elements, pot = {}, max_elements = {}, {} =! {}", pot, max_elements,
                       static_cast<uint64_t>(std::pow(2, pot)), max_elements));
    spdlog::debug(fmt::format("Got pot = {}", pot));

    uint64_t num_elements_in_directory = max_elements;

    bool first_iteration = true;

    for (int64_t i = pot; i > 0; --i) {
      uint64_t directory_memory_usage = num_elements_in_directory * sizeof(HTE*);

      spdlog::debug(
          fmt::format("Checking i = {}, num elements in dir = {}, directory mem usage = {}", i, num_elements_in_directory, directory_memory_usage));

      if (directory_memory_usage + buffer_memory_usage <= memory_budget) {
        if (first_iteration) {
          bool still_fits = true;
          // in case pot fits in first iteration, we increase until it does not fit anymore
          int64_t j = i;
          for (; still_fits; ++j) {
            directory_memory_usage = num_elements_in_directory * sizeof(HTE*);
            still_fits = directory_memory_usage + buffer_memory_usage <= memory_budget;
            if (!still_fits) {
              // reset to previous it and exit loop
              num_elements_in_directory >>= 1;
              directory_memory_usage = num_elements_in_directory * sizeof(HTE*);
              break;
            } else {
              num_elements_in_directory <<= 1;
            }
          }

          if (j - 1 > i) {
            spdlog::info(fmt::format("Chained hash table was able to increase directory POT from {} to {}", pot, j - 1));
          }
        }

        // If we found a suitable configuration, we allocate the directory according to i, and give the rest of the budget to the buffer
        const uint64_t remaining_budget = memory_budget - directory_memory_usage;
        const uint64_t num_elements_in_buffer =
            static_cast<uint64_t>(std::floor(static_cast<double>(remaining_budget) / static_cast<double>(sizeof(HTE))));

        ASSERT(num_elements_in_buffer >= load_factor_elements,
               fmt::format("Something went wrong calculating buffer size. We should be able to fit {} elements, but can only fit {} elements.",
                           load_factor_elements, num_elements_in_buffer));

        return std::make_pair(num_elements_in_directory, num_elements_in_buffer);
      }

      num_elements_in_directory >>= 1;
      first_iteration = false;
    }

    if constexpr (!set_bool_on_invalid_budget) {
      FAIL("Memory budget too small.");
    } else {
      spdlog::warn("Warning. Setting bool on invalid budget. Make sure to know what you are doing.");
    }

    return std::make_pair(0, 0);
  }

  ChainedHashTable(uint64_t max_elements, uint8_t target_load_factor, bool print_info = true)
      : directory_buffer_size_{calculate_directory_buffer_size(max_elements, target_load_factor)},
        directory_(directory_buffer_size_.first, nullptr),
        buffer_(directory_buffer_size_.second, HTE{}),
        max_elements_{max_elements},
        target_load_factor_{target_load_factor},
        size_{0},
        hasher_(directory_buffer_size_.first),
        next_free_element_{0},
        invalid_budget_{false},
        directory_pot_{static_cast<uint64_t>(std::log2(directory_.size()))} {
    ASSERT(utils::is_power_of_two(max_elements),
           fmt::format("ChainedHashMap currently only works for sizes that are a power of two, got {} instead.", max_elements));

    if (directory_buffer_size_.first == 0 && directory_buffer_size_.second == 0) {
      spdlog::error("Got invalid budget, but setting bool instead. Make sure to check this bool and not use the hashmap.");
      invalid_budget_ = true;
    }

    if (print_info) {
      const uint64_t load_factor_elements =
          static_cast<uint64_t>(std::ceil((static_cast<double>(target_load_factor) / 100.0) * static_cast<double>(max_elements)));
      const double additional_factor = 1.0 + (static_cast<double>(additional_budget) / 100.0);

      uint64_t entry_size = sizeof(KeyT) + sizeof(ValueT);
      if constexpr (budget == MemoryBudget::Bucket8FP) {
        entry_size += 1;  // 1 byte fingerprint
      } else if constexpr (budget == MemoryBudget::Bucket16FP) {
        entry_size += 2;  // 2 byte fingerprint
      }

      const uint64_t memory_budget =
          static_cast<uint64_t>(std::ceil(additional_factor * static_cast<double>(entry_size) * static_cast<double>(max_elements)));

      additional_elements_ = buffer_.size() - load_factor_elements;

      spdlog::info(
          fmt::format("Initialized ChainedHashTable with utils::cacheline_size = {},  sizeof(HTE) = {}, sizeof(HTEPtr) = {}, directory size = {}, "
                      "directory_pot = {}, "
                      "buffer size = {}, load_factor_elements = {}, additional buffer elements = {}, additional_factor = {}, memory_budget = {}",
                      utils::cacheline_size, sizeof(HTE), sizeof(HTE*), directory_.size(), directory_pot_, buffer_.size(), load_factor_elements,
                      additional_elements_, additional_factor, memory_budget));
    }
  }

  bool contains(const KeyT& key) { return find(key).is_valid; }

  ValueT lookup(const KeyT& key) {
    const FindResult res = find(key);

    DEBUG_ASSERT(!(res.is_valid && res.entry == nullptr), "Invalid state reached.");

    if constexpr (std::is_pointer_v<ValueT>) {
      return !res.is_valid ? nullptr : res.entry->value;
    } else {
      return !res.is_valid ? ValueT{} : res.entry->value;
    }
  };

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(size_ + 1 <= max_elements_, "Hashmap is full!");
    FindResult res = find(key);

    // In chained, we could support more than foreign key joins, but keep update semantics to be consistent with the other implementations

    if (!res.is_valid) {
      // We are inserting a new element (no update)
      ASSERT(next_free_element_ < buffer_.size(), "Buffer is full");
      DEBUG_ASSERT(res.entry == nullptr, "We are inserting a new element but entry is not nullptr");

      HTE* target = &buffer_[next_free_element_++];

      // In case that previous element is nullptr, we need to update the dictionary, otherwise the next pointer of the previous element
      if (res.previous_entry == nullptr) {
        directory_[res.directory_idx] = target;
      } else {
        res.previous_entry->next = target;
      }

      ++size_;
      target->key = key;
      target->value = value;
    } else {
      // just update the value of existing entry
      DEBUG_ASSERT(res.entry != nullptr, "Got valid nullptr");
      DEBUG_ASSERT(res.entry->key == key, "Got valid result with non-matching key.");
      res.entry->value = value;
    }
  };

  FindResult find(const KeyT& key) {
#ifdef HASHMAP_COLLECT_META_INFO
    uint64_t probing_seq_len = 1;
    if (minfo.measurement_started) {
      ++(minfo.num_finds);
      ++(minfo.probed_elements);
    }
#endif

    const uint64_t directory_idx = hasher_.hash(key);
    HTE* current_entry = directory_[directory_idx];
    HTE* previous_entry = nullptr;

    bool entry_invalid = current_entry == nullptr;

    if (entry_invalid) {
      return {directory_idx, current_entry, previous_entry, false};
    }

    while (current_entry->key != key) {
#ifdef HASHMAP_COLLECT_META_INFO
      if (minfo.measurement_started) {
        ++(minfo.probed_elements);
        ++probing_seq_len;
      }
#endif
      previous_entry = current_entry;
      current_entry = current_entry->next;
      entry_invalid = current_entry == nullptr;

      if (entry_invalid) {
        return {directory_idx, current_entry, previous_entry, false};
      }
    }

#ifdef HASHMAP_COLLECT_META_INFO
    if (minfo.measurement_started) {
      minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
      minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
    }
#endif

    return {directory_idx, current_entry, previous_entry, true};
  }

  void prefault() {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<uint64_t> dist{0, 0xDEADBEEF};

    for (uint64_t i = 0; i < directory_.size(); i++) {
      directory_[i] = reinterpret_cast<HTE*>(dist(gen));
    }

    for (uint64_t i = 0; i < buffer_.size(); i++) {
      if constexpr (!std::is_same_v<KeyT, StringKey>) {
        buffer_[i].key = static_cast<KeyT>(dist(gen));
      }
      if constexpr (!std::is_pointer_v<ValueT>) {
        buffer_[i].value = static_cast<ValueT>(dist(gen));
      }
      buffer_[i].next = reinterpret_cast<HTE*>(dist(gen));
    }

    reset();
  }

  void prefault_pregenerated(const std::vector<std::pair<KeyT, ValueT>>& prefault_data) {
    uint64_t prefault_data_size = prefault_data.size();
    for (uint64_t i = 0; i < directory_.size(); i++) {
      if constexpr (!std::is_same_v<KeyT, StringKey>) {
        directory_[i] = reinterpret_cast<HTE*>(prefault_data[i % prefault_data_size].first);
      } else {
        directory_[i] = reinterpret_cast<HTE*>(prefault_data[i % prefault_data_size].second);
      }
    }

    for (uint64_t i = 0; i < buffer_.size(); i++) {
      buffer_[i].key = prefault_data[i % prefault_data_size].first;
      buffer_[i].value = prefault_data[i % prefault_data_size].second;
      buffer_[i].next = reinterpret_cast<HTE*>(42);
    }

    reset();
  }

  void reset() {
    for (uint64_t i = 0; i < directory_.size(); i++) {
      directory_[i] = nullptr;
    }

    for (uint64_t i = 0; i < buffer_.size(); i++) {
      buffer_[i] = HTE{};
    }

    next_free_element_ = 0;
    size_ = 0;
  }

  std::string get_identifier() {
    std::string thp = "NoTHP";
    if constexpr (use_thp) {
      thp = "THP";
    }

    std::string value_type = hashmap::utils::data_type_to_str<ValueT>();
    std::string key_type = hashmap::utils::data_type_to_str<KeyT>();
    std::string memory_budget = "SomeUnknownMemBudget";
    if constexpr (budget == MemoryBudget::KeyValue) {
      memory_budget = "BudgetKV";
    } else if constexpr (budget == MemoryBudget::Bucket8FP) {
      memory_budget = "BudgetBucket8FP";
    } else if constexpr (budget == MemoryBudget::Bucket16FP) {
      memory_budget = "BudgetBucket16FP";
    }

    std::string addbudget_str = fmt::format("AdditionalBudget{}", additional_budget);
    std::string pot_str = fmt::format("DirectoryPot{}", directory_pot_);
    std::string size_str = fmt::format("DirectorySize{}", directory_.size());
    std::string bufsize_str = fmt::format("BufferSize{}", buffer_.size());
    std::string addelem_str = fmt::format("AdditionalElements{}", additional_elements_);

    return fmt::format("ChainedHashTable<{}; {}; {}; {}; {}; {}; {}; {}; {}; {}>", hasher_.get_identifier(), key_type, value_type, thp, memory_budget,
                       addbudget_str, pot_str, size_str, bufsize_str, addelem_str);
  }

  uint64_t get_entry_size() { return 0; }

  bool is_data_aligned_to(size_t alignment) { return utils::is_aligned((void*)buffer_.data(), alignment); }

  std::string get_data_pointer_string() { return fmt::format("{}", (void*)buffer_.data()); }

  double get_current_load() { return static_cast<double>(size_) / static_cast<double>(max_elements_); }

  uint64_t get_current_size() { return size_; }

  bool can_be_used() { return !invalid_budget_; }

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo* get_minfo() { return &minfo; }
  void start_measurement() { minfo.measurement_started = true; }
  void stop_measurement() { minfo.measurement_started = false; }
  void reset_measurement() { minfo.reset(); }
#endif

 protected:
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<HTE*>, std::allocator<HTE*>>::type PtrVectorAllocator;
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<HTE>, std::allocator<HTE>>::type VectorAllocator;
  std::pair<uint64_t, uint64_t> directory_buffer_size_;

  alignas(utils::cacheline_size) std::vector<HTE*, PtrVectorAllocator> directory_;
  alignas(utils::cacheline_size) std::vector<HTE, VectorAllocator> buffer_;
  alignas(utils::cacheline_size) uint64_t max_elements_;
  alignas(utils::cacheline_size) uint8_t target_load_factor_;
  alignas(utils::cacheline_size) uint64_t size_;
  alignas(utils::cacheline_size) HasherT hasher_;

  alignas(utils::cacheline_size) uint64_t next_free_element_;

  bool invalid_budget_;
  uint64_t directory_pot_;
  uint64_t additional_elements_;

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo minfo;
#endif
};

}  // namespace hashmap::hashmaps
