#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <ostream>
#include <vector>

#include "fmt/format.h"
#include "gcem.hpp"
#include "hashmap/entries/aosentry.hpp"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/hashmaps/linear_probing_aos.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap;

#define DISPLACEMENT_MASK static_cast<uint64_t>(0b111111111111111111111111111111111111111111111111111111111111111)

namespace hashmap::hashmaps {

/* This RH implementation stores displacement information */
// For 64 bit keys/ 64 bit values / 64 bit displacement info, autopadded = packed, hence we only test autopadded
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class StoringRobinHoodAoSHashTable : public LinearProbingAoSHashTable<KeyT, ValueT, HasherT, entries::AutoPaddedRHAoSEntry<KeyT, ValueT>,
                                                                      use_prefetching, prefetching_locality, use_thp> {
 public:
  StoringRobinHoodAoSHashTable(uint64_t max_elements, uint8_t target_load_factor, bool print_info = true,
                               std::string base_identifier = "StoringRobinHoodAoSHashTable")
      : LinearProbingAoSHashTable<KeyT, ValueT, HasherT, entries::AutoPaddedRHAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality, use_thp>(
            max_elements, target_load_factor, false, base_identifier) {
    constexpr uint64_t maximum_allowed_elements = static_cast<uint64_t>(gcem::pow<uint64_t, uint64_t>(2, 63)) - 1;
    ASSERT(max_elements <= maximum_allowed_elements,
           fmt::format("Can only store {} elements in a StoringRobinHoodAoSHashTable due to bit packing", maximum_allowed_elements));

    if (print_info) {
      spdlog::info(
          fmt::format("Initialized {} with utils::cacheline_size = {}, "
                      "sizeof(AutoPaddedRHAoSEntry) = {}, "
                      "alignof(AutoPaddedRHAoSEntry) = {}",
                      this->get_identifier(), utils::cacheline_size, sizeof(entries::AutoPaddedRHAoSEntry<KeyT, ValueT>),
                      alignof(entries::AutoPaddedRHAoSEntry<KeyT, ValueT>)));
    }
  }

  struct FindResult {
    uint64_t idx = 0;
    entries::AutoPaddedRHAoSEntry<KeyT, ValueT>* entry;
    bool key_equal = false;
    bool entry_valid = false;
  };

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(this->size_ + 1 <= this->max_elements_, "Hashmap is full!");

    uint64_t curr_idx = this->hasher_.hash(key);
    uint64_t curr_displacement = 0;

    uint64_t curr_adjusted_idx = curr_idx;
    entries::AutoPaddedRHAoSEntry<KeyT, ValueT>* next_entry = &this->entries_[curr_adjusted_idx];

    if constexpr (use_prefetching) {
      READ_PREFETCH(next_entry, prefetching_locality);
      READ_PREFETCH(next_entry + 1, prefetching_locality);
      READ_PREFETCH(next_entry + 2, prefetching_locality);
    }

    KeyT key_to_insert = key;
    ValueT value_to_insert = value;

    uint64_t max_idx = curr_idx + this->size_;

    while (next_entry->key != key_to_insert && next_entry->is_valid && curr_idx <= max_idx) {
      if constexpr (use_prefetching) {
        // we always want to stay 3 entries ahead in cache
        READ_PREFETCH(next_entry + 3, prefetching_locality);
      }

      // Our next_entry is already occupied, check displacement
      uint64_t next_entry_displacement = next_entry->displacement;
      if (curr_displacement > next_entry_displacement || (next_entry_displacement == curr_displacement && key_to_insert < next_entry->key)) {
        std::swap(next_entry->key, key_to_insert);
        std::swap(next_entry->value, value_to_insert);

        // cannot use std::swap here due to bit-field packing
        // As curr_displacement is 64 bit integer, at least gcc warns here that next_entry->displacement could overflow
        // by making sure we only use 63 bits, the warning disappears
        // TODO(MaxiBoether): think about endianness here
        next_entry->displacement = curr_displacement & DISPLACEMENT_MASK;
        curr_displacement = next_entry_displacement;
      }

      curr_adjusted_idx = this->hasher_.finalize(++curr_idx);
      next_entry = &this->entries_[curr_adjusted_idx];
      ++curr_displacement;
    }

    if (next_entry->key != key_to_insert || !next_entry->is_valid) {
      // We are inserting a new element (no update)
      ++this->size_;
      next_entry->key = key_to_insert;
      next_entry->is_valid = 1;
    }

    next_entry->value = value_to_insert;
    next_entry->displacement = curr_displacement & DISPLACEMENT_MASK;
  };

  FindResult find(const KeyT& key) {
#ifdef HASHMAP_COLLECT_META_INFO
    if (this->minfo.measurement_started) {
      ++(this->minfo.num_finds);
      ++(this->minfo.probed_elements);
    }
    uint64_t probing_seq_len = 1;
#endif

    uint64_t curr_idx = this->hasher_.hash(key);
    uint64_t curr_displacement = 0;

    uint64_t curr_adjusted_idx = curr_idx;
    entries::AutoPaddedRHAoSEntry<KeyT, ValueT>* next_entry = &this->entries_[curr_adjusted_idx];

    if constexpr (use_prefetching) {
      READ_PREFETCH(next_entry, prefetching_locality);
      READ_PREFETCH(next_entry + 1, prefetching_locality);
      READ_PREFETCH(next_entry + 2, prefetching_locality);
    }

    bool key_not_equal = next_entry->key != key;
    bool entry_valid = next_entry->is_valid;

    while (key_not_equal && entry_valid && next_entry->displacement >= curr_displacement) {
      if constexpr (use_prefetching) {
        // we always want to stay 3 entries ahead in cache
        READ_PREFETCH(next_entry + 3, prefetching_locality);
      }

      curr_adjusted_idx = this->hasher_.finalize(++curr_idx);
      next_entry = &this->entries_[curr_adjusted_idx];
      ++curr_displacement;
      key_not_equal = next_entry->key != key;
      entry_valid = next_entry->is_valid;

#ifdef HASHMAP_COLLECT_META_INFO
      if (this->minfo.measurement_started) {
        ++(this->minfo.probed_elements);
        ++probing_seq_len;
      }
#endif
    }

#ifdef HASHMAP_COLLECT_META_INFO
    if (this->minfo.measurement_started) {
      this->minfo.min_probing_sequence = std::min(this->minfo.min_probing_sequence, probing_seq_len);
      this->minfo.max_probing_sequence = std::max(this->minfo.max_probing_sequence, probing_seq_len);
    }
#endif

    return {curr_adjusted_idx, next_entry, !key_not_equal, entry_valid};
  }

  // Convienience method for tests
  uint64_t HEDLEY_ALWAYS_INLINE calc_displacement(const KeyT& /*key*/, uint64_t index) { return this->entries_[index].displacement; };

  /* We need to redefine the following methods because they are not virtual. Otherwise, find would early-bind to the LinearHashmap find */
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
};

}  // namespace hashmap::hashmaps
