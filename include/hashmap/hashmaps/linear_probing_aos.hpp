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
#include "hashmap/entries/aosentry.hpp"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap;

namespace hashmap::hashmaps {

// TODO(Maxiboether): do all of that stuff for all hashmaps with no is_valid bit and invalid key value instead
template <typename KeyT, typename ValueT, typename HasherT, typename EntryT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false, uint8_t unroll_factor = 1,
          uint8_t additional_prefetching = 0>
class LinearProbingAoSHashTable : public HashTable<KeyT, ValueT> {
 public:
  LinearProbingAoSHashTable(uint64_t max_elements, uint8_t /*target_load_factor*/, bool print_info = true,
                            std::string base_identifier = "LinearProbingAoSHashTable")
      : entries_(max_elements), max_elements_{max_elements}, size_{0}, hasher_(max_elements), base_identifier_{base_identifier} {
    if (print_info) {
      spdlog::info(
          fmt::format("Initialized {} with utils::cacheline_size = {}, sizeof(EntryT) = {}, "
                      "alignof(EntryT) = {}",
                      get_identifier(), utils::cacheline_size, sizeof(EntryT), alignof(EntryT)));
    }

    if constexpr (unroll_factor == 0) {
      FAIL("unroll_factor is equal to 0, needs to be at least 1!");
    }
  }

  struct FindResult {
    uint64_t idx = 0;
    EntryT* entry;
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
      return (!res.key_equal || !res.entry_valid) ? nullptr : res.entry->value;
    } else {
      return (!res.key_equal || !res.entry_valid) ? ValueT{} : res.entry->value;
    }
  };

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(size_ + 1 <= max_elements_, "Hashmap is full!");
    FindResult res = find(key);

    if (!res.key_equal || !res.entry_valid) {
      // We are inserting a new element (no update)
      ++size_;
      res.entry->key = key;
      res.entry->is_valid = 1;
    }

    res.entry->value = value;
  };

  FindResult find(const KeyT& key) {
#ifdef HASHMAP_COLLECT_META_INFO
    uint64_t probing_seq_len = 1;
    if (minfo.measurement_started) {
      ++(minfo.num_finds);
      ++(minfo.probed_elements);
    }
#endif

    uint64_t curr_idx = hasher_.hash(key);
    uint64_t curr_adjusted_idx = curr_idx;

    // at maximum, our linear probe will be as long as many elements we have inserted
    uint64_t max_idx = curr_idx + size_;

    EntryT* next_entry = &entries_[curr_adjusted_idx];

    if constexpr (use_prefetching) {
      READ_PREFETCH(next_entry, prefetching_locality);
      READ_PREFETCH(next_entry + 1, prefetching_locality);
      READ_PREFETCH(next_entry + 2, prefetching_locality);
      if constexpr (additional_prefetching > 0) {
#pragma GCC unroll 10
        for (uint8_t i = 1; i <= additional_prefetching; ++i) {
          READ_PREFETCH(next_entry + 2 + i, prefetching_locality);
        }
      }
    }
    bool key_not_equal = next_entry->key != key;
    bool entry_valid = next_entry->is_valid;

    auto loop_body = [&]() {
      if constexpr (use_prefetching) {
        // we always want to stay 3 + additional_prefetching entries ahead in cache
        READ_PREFETCH(next_entry + 3 + additional_prefetching, prefetching_locality);
      }

      curr_adjusted_idx = hasher_.finalize(++curr_idx);
      next_entry = &entries_[curr_adjusted_idx];
      key_not_equal = next_entry->key != key;
      entry_valid = next_entry->is_valid;

#ifdef HASHMAP_COLLECT_META_INFO
      if (minfo.measurement_started) {
        ++(minfo.probed_elements);
        ++probing_seq_len;
      }
#endif
    };

    // We stop using a define here because defines are hard to debug/profile
    // copy paste linear_probe_condition: (key_not_equal = (next_entry->key != key)) && (entry_valid = next_entry->is_valid) && curr_idx <= max_idx

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

#ifdef HASHMAP_COLLECT_META_INFO
    if (minfo.measurement_started) {
      minfo.min_probing_sequence = std::min(minfo.min_probing_sequence, probing_seq_len);
      minfo.max_probing_sequence = std::max(minfo.max_probing_sequence, probing_seq_len);
    }
#endif

    return {curr_adjusted_idx, next_entry, !key_not_equal, entry_valid};
  }

  void prefault() {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<uint64_t> dist{0, 0xDEADBEEF};

    for (uint64_t i = 0; i < entries_.size(); i++) {
      if constexpr (!std::is_same_v<KeyT, StringKey>) {
        entries_[i].key = static_cast<KeyT>(dist(gen));
      }
      if constexpr (!std::is_pointer_v<ValueT>) {
        entries_[i].value = static_cast<ValueT>(dist(gen));
      }
    }

    reset();
  }

  void prefault_pregenerated(const std::vector<std::pair<KeyT, ValueT>>& prefault_data) {
    uint64_t prefault_data_size = prefault_data.size();
    for (uint64_t i = 0; i < entries_.size(); i++) {
      entries_[i].key = prefault_data[i % prefault_data_size].first;
      entries_[i].value = prefault_data[i % prefault_data_size].second;
    }

    reset();
  }

  void reset() {
    for (uint64_t i = 0; i < entries_.size(); i++) {
      entries_[i].reset();
    }

    size_ = 0;
  }

  std::string get_identifier() {
    std::string prefetching = "NoPref";
    if constexpr (use_prefetching) {
      prefetching = fmt::format("Pref{}", static_cast<int>(prefetching_locality));
    }

    std::string thp = "NoTHP";
    if constexpr (use_thp) {
      thp = "THP";
    }

    std::string unroll = "NoUnroll";
    if constexpr (unroll_factor > 1) {
      unroll = fmt::format("Unroll{}", static_cast<int>(unroll_factor));
    }

    std::string additional_prf = "NoAddPrf";
    if constexpr (additional_prefetching > 0) {
      additional_prf = fmt::format("AddPrf{}", static_cast<int>(additional_prefetching));
    }

    std::string value_type = hashmap::utils::data_type_to_str<ValueT>();
    std::string key_type = hashmap::utils::data_type_to_str<KeyT>();

    return fmt::format("{}<{}; {}; {}; {}; {}; {}; {}>", base_identifier_, hasher_.get_identifier(), key_type, value_type, prefetching, thp, unroll,
                       additional_prf);
  }

  uint64_t get_entry_size() { return static_cast<uint64_t>(sizeof(EntryT)); }

  bool is_data_aligned_to(size_t alignment) { return utils::is_aligned((void*)entries_.data(), alignment); }

  std::string get_data_pointer_string() { return fmt::format("{}", (void*)entries_.data()); }

  double get_current_load() { return static_cast<double>(size_) / static_cast<double>(max_elements_); }
  uint64_t get_current_size() { return size_; }

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo* get_minfo() { return &minfo; }
  void start_measurement() { minfo.measurement_started = true; }
  void stop_measurement() { minfo.measurement_started = false; }
  void reset_measurement() { minfo.reset(); }
#endif

 protected:
  typedef typename std::conditional<use_thp, utils::TransparentHugePageAllocator<EntryT>, std::allocator<EntryT>>::type VectorAllocator;
  alignas(utils::cacheline_size) std::vector<EntryT, VectorAllocator> entries_;
  alignas(utils::cacheline_size) uint64_t max_elements_;
  alignas(utils::cacheline_size) uint64_t size_;
  alignas(utils::cacheline_size) HasherT hasher_;
  std::string base_identifier_;

#ifdef HASHMAP_COLLECT_META_INFO
  utils::MeasurementInfo minfo;
#endif
};

// Alignment of AoSEntry is done by compiler, i.e., it is aligned to the largest member
// Performance depends on KeyT and ValueT; access to key/value is aligned and thus fast
// AoSEntrys can cross cacheline boundaries
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false, uint8_t unroll_factor = 1>
class AutoPaddedLinearProbingAoSHashTable : public LinearProbingAoSHashTable<KeyT, ValueT, HasherT, entries::AutoPaddedAoSEntry<KeyT, ValueT>,
                                                                             use_prefetching, prefetching_locality, use_thp, unroll_factor> {
 public:
  AutoPaddedLinearProbingAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : LinearProbingAoSHashTable<KeyT, ValueT, HasherT, entries::AutoPaddedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality, use_thp,
                                  unroll_factor>(max_elements, target_load_factor, true, "AutoPaddedLinearProbingAoSHashTable") {}
};

// AoSEntry is densely packed and not padded at all
// Entries may cross cacheline boundaries, and access to key/value is not aligned, might be slower depending on
// architecture However, there is no memory wasted, so the increased throughput may counter these issues
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false, uint8_t unroll_factor = 1,
          uint8_t additional_prefetching = 0>
class UnalignedLinearProbingAoSHashTable
    : public LinearProbingAoSHashTable<KeyT, ValueT, HasherT, entries::PackedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality, use_thp,
                                       unroll_factor, additional_prefetching> {
 public:
  UnalignedLinearProbingAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : LinearProbingAoSHashTable<KeyT, ValueT, HasherT, entries::PackedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality, use_thp,
                                  unroll_factor, additional_prefetching>(max_elements, target_load_factor, true,
                                                                         "UnalignedLinearProbingAoSHashTable") {}
};

// Naturally aligned AoSEntrys, i.e., aligned to next_power_of_2(sizeof(AoSEntry))
// AoSEntries never cross cacheline boundaries, otherwise similar to AutoPadded, depending on KeyT/ValueT
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false, uint8_t unroll_factor = 1>
class NaturalAlignedLinearProbingAoSHashTable : public LinearProbingAoSHashTable<KeyT, ValueT, HasherT, entries::NaturalAlignedAoSEntry<KeyT, ValueT>,
                                                                                 use_prefetching, prefetching_locality, use_thp, unroll_factor> {
 public:
  NaturalAlignedLinearProbingAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : LinearProbingAoSHashTable<KeyT, ValueT, HasherT, entries::NaturalAlignedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality,
                                  use_thp, unroll_factor>(max_elements, target_load_factor, true, "NaturalAlignedLinearProbingAoSHashTable") {}
};

}  // namespace hashmap::hashmaps
