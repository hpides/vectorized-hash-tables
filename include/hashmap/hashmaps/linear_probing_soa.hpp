#pragma once
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <ostream>
#include <random>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "fmt/format.h"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap;

namespace hashmap::hashmaps {

template <typename KeyT, typename ValueT, typename HasherT, bool use_thp = false, uint8_t unroll_factor = 1>
class LinearProbingSoAHashTable : public HashTable<KeyT, ValueT> {
 public:
  LinearProbingSoAHashTable(uint64_t max_elements, uint8_t /*target_load_factor*/, bool print_info = true,
                            std::string base_identifier = "LinearProbingSoAHashTable")
      : keys_(max_elements),
        values_(max_elements),
        is_valids_(max_elements),
        max_elements_{max_elements},
        size_{0},
        hasher_(max_elements),
        base_identifier_{base_identifier} {
    if (print_info) {
      spdlog::info(fmt::format("Initialized {} with utils::cacheline_size = {}", get_identifier(), utils::cacheline_size));
    }

    if constexpr (unroll_factor == 0) {
      FAIL("unroll_factor is equal to 0, needs to be at least 1!");
    }
  }

  struct FindResult {
    uint64_t idx = 0;
    KeyT key;
    bool key_equal = false;
    bool entry_valid = false;
  };

  bool contains(const KeyT& key) {
    const FindResult res = find(key);
    return (!res.key_equal || !res.entry_valid) ? false : true;
  }

  ValueT lookup(const KeyT& key) {
    const FindResult res = find(key);
    if constexpr (std::is_pointer_v<ValueT>) {
      return (!res.key_equal || !res.entry_valid) ? nullptr : values_[res.idx];
    } else {
      return (!res.key_equal || !res.entry_valid) ? ValueT{} : values_[res.idx];
    }
  };

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(size_ + 1 <= max_elements_, "Hashmap is full!");
    FindResult res = find(key);

    if (!res.key_equal || !res.entry_valid) {
      // We are inserting a new element (no update)
      ++size_;
      keys_[res.idx] = key;
      is_valids_[res.idx] = true;
    }

    values_[res.idx] = value;
  };

  FindResult find(const KeyT& key) {
    uint64_t curr_idx = hasher_.hash(key);
    uint64_t curr_adjusted_idx = curr_idx;

    // at maximum, our linear probe will be as long as many elements we have inserted
    uint64_t max_idx = curr_idx + size_;

    KeyT next_key = keys_[curr_adjusted_idx];

    bool key_not_equal = next_key != key;
    bool entry_valid = is_valids_[curr_adjusted_idx];

    auto loop_body = [&]() {
      curr_adjusted_idx = hasher_.finalize(++curr_idx);
      next_key = keys_[curr_adjusted_idx];
      key_not_equal = next_key != key;
      entry_valid = is_valids_[curr_adjusted_idx];
    };

    while (key_not_equal && entry_valid && curr_idx <= max_idx) {
      if constexpr (unroll_factor == 1) {
        // probably after compiler optimizations this should be equivalent to utils:loop<0,1> but we make sure to not have any overhead here by making
        // it explicit
        loop_body();
      } else {
        utils::loop<uint8_t, unroll_factor>([&](auto i) {
          // if i == 0, this is the first unrolled element, so we just did the check in the while loop and do not need to do it again
          if constexpr (i == 0) {
            loop_body();
          } else {
            if (key_not_equal && entry_valid && curr_idx <= max_idx) {
              loop_body();
            }
          }
        });
      }
    }

    return {curr_adjusted_idx, next_key, !key_not_equal, entry_valid};
  }

  void prefault() {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<uint64_t> dist{0, 0xDEADBEEF};

    for (uint64_t i = 0; i < keys_.size(); i++) {
      if constexpr (!std::is_same_v<KeyT, StringKey>) {
        keys_[i] = static_cast<KeyT>(dist(gen));
      }
      if constexpr (!std::is_pointer_v<ValueT>) {
        values_[i] = static_cast<ValueT>(dist(gen));
      }
      is_valids_[i] = true;
    }

    reset();
  }

  void prefault_pregenerated(const std::vector<std::pair<KeyT, ValueT>>& prefault_data) {
    uint64_t prefault_data_size = prefault_data.size();
    for (uint64_t i = 0; i < keys_.size(); i++) {
      keys_[i] = prefault_data[i % prefault_data_size].first;
      values_[i] = prefault_data[i % prefault_data_size].second;
      is_valids_[i] = true;
    }

    reset();
  }

  void reset() {
    for (uint64_t i = 0; i < keys_.size(); i++) {
      keys_[i] = KeyT{};
      values_[i] = ValueT{};
      is_valids_[i] = false;
    }

    size_ = 0;
  }

  std::string get_identifier() {
    std::string thp = "NoTHP";
    if constexpr (use_thp) {
      thp = "THP";
    }
    std::string unroll = "NoUnroll";
    if constexpr (unroll_factor > 1) {
      unroll = fmt::format("Unroll{}", static_cast<int>(unroll_factor));
    }

    std::string value_type = hashmap::utils::data_type_to_str<ValueT>();
    std::string key_type = hashmap::utils::data_type_to_str<KeyT>();

    return fmt::format("{}<{}; {}; {}; {}; {}>", base_identifier_, hasher_.get_identifier(), key_type, value_type, thp, unroll);
  }

  uint64_t get_entry_size() { return static_cast<uint64_t>(sizeof(KeyT)); }

  bool is_data_aligned_to(size_t alignment) { return utils::is_aligned((void*)keys_.data(), alignment); }

  std::string get_data_pointer_string() { return fmt::format("{}", (void*)keys_.data()); }

  double get_current_load() { return static_cast<double>(size_) / static_cast<double>(max_elements_); }
  uint64_t get_current_size() { return size_; }

 protected:
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<KeyT>, std::allocator<KeyT>>::type KeyTVectorAllocator;
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<ValueT>, std::allocator<ValueT>>::type ValueTVectorAllocator;
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<bool>, std::allocator<bool>>::type BoolVectorAllocator;

  alignas(utils::cacheline_size) std::vector<KeyT, KeyTVectorAllocator> keys_;
  alignas(utils::cacheline_size) std::vector<ValueT, ValueTVectorAllocator> values_;
  alignas(utils::cacheline_size) std::vector<bool, BoolVectorAllocator> is_valids_;

  alignas(utils::cacheline_size) uint64_t max_elements_;
  alignas(utils::cacheline_size) uint64_t size_;
  alignas(utils::cacheline_size) HasherT hasher_;
  std::string base_identifier_;
};

}  // namespace hashmap::hashmaps
