#pragma once
#include <cstdint>
#include <iostream>
#include <limits>
#include <ostream>
#include <type_traits>
#include <typeinfo>

#include "absl/container/flat_hash_map.h"
#include "fmt/format.h"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap;

namespace hashmap::hashmaps {

template <typename KeyT, typename ValueT, typename HasherT>
class AbseilFlatHashTable : public HashTable<KeyT, ValueT> {
 public:
  AbseilFlatHashTable(uint64_t max_elements, uint8_t /*target_load_factor*/, bool print_info = true,
                      std::string base_identifier = "AbseilFlatHashTable")
      : max_elements_(max_elements), hasher_(max_elements), base_identifier_{base_identifier} {
    if (print_info) {
      spdlog::info(fmt::format("Initialized {} with utils::cacheline_size = {}", get_identifier(), utils::cacheline_size));
    }

    map_.reserve(max_elements);
  }

  bool contains(const KeyT& key) { return map_.contains(key); }

  ValueT lookup(const KeyT& key) {
    auto it = map_.find(key);
    if (it == map_.end()) {
      return ValueT{};
    } else {
      return it->second;
    }
  };

  void insert(const KeyT& key, const ValueT& value) { map_[key] = value; };

  uint64_t find(const KeyT& /*key*/) {
    spdlog::error("find is not implemented for abseil");
    return std::numeric_limits<uint64_t>::max();  // not valid for abseil
  }

  void prefault() {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<uint64_t> dist{0, 0xDEADBEEF};

    // we cannot really target all buckets here but oh well
    for (uint64_t i = 0; i < max_elements_; i++) {
      map_[dist(gen)] = dist(gen);
    }

    reset();
  }

  void prefault_pregenerated(const std::vector<std::pair<KeyT, ValueT>>& prefault_data) {
    uint64_t prefault_data_size = prefault_data.size();
    for (uint64_t i = 0; i < max_elements_; i++) {
      map_[prefault_data[i % prefault_data_size].first] = prefault_data[i % prefault_data_size].second;
    }

    reset();
  }

  void reset() { map_.erase(map_.begin(), map_.end()); }

  std::string get_identifier() {
    std::string value_type = hashmap::utils::data_type_to_str<ValueT>();
    std::string key_type = hashmap::utils::data_type_to_str<KeyT>();

    return fmt::format("{}<{}; {}; {}>", base_identifier_, hasher_.get_identifier(), key_type, value_type);
  }
  uint64_t get_entry_size() { return std::numeric_limits<uint64_t>::max(); }

  bool is_data_aligned_to(size_t /* alignment */) { return false; }

  std::string get_data_pointer_string() { return "0x0"; }

  double get_current_load() { return static_cast<double>(map_.size()) / static_cast<double>(max_elements_); }
  uint64_t get_current_size() { return static_cast<uint64_t>(map_.size()); }

 protected:
  alignas(utils::cacheline_size) absl::flat_hash_map<KeyT, ValueT, utils::ExternalHashWrapper<KeyT, HasherT>> map_;
  alignas(utils::cacheline_size) uint64_t max_elements_;
  HasherT hasher_;
  std::string base_identifier_;
};

}  // namespace hashmap::hashmaps
