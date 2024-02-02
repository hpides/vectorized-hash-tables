#pragma once
#include <cstdint>
#include <iostream>
#include <limits>
#include <ostream>
#include <vector>

#include "fmt/format.h"
#include "hashmap/entries/aosentry.hpp"
#include "hashmap/hashmaps/hashmap.hpp"
#include "hashmap/hashmaps/linear_probing_aos.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap;

namespace hashmap::hashmaps {

/* This RH implementation does not store displacement information, similar to Richter et al
However, while Richter et al. state how they implement hash recalcuation for lookups, they do not state how they do it
for inserts. As they do not store information, they _have to_ recalcuate it on each iteration, I assume, but I am not
sure, as this makes insertions probably expensive. Hence, I implement a recalculating variant, and a variant that stores
displacement information.
*/
template <typename KeyT, typename ValueT, typename HasherT, typename EntryT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class RecalculatingRobinHoodAoSHashTable
    : public LinearProbingAoSHashTable<KeyT, ValueT, HasherT, EntryT, use_prefetching, prefetching_locality, use_thp> {
 public:
  RecalculatingRobinHoodAoSHashTable(uint64_t max_elements, uint8_t target_load_factor, bool print_info = true,
                                     std::string base_identifier = "RecalculatingRobinHoodAoSHashTable")
      : LinearProbingAoSHashTable<KeyT, ValueT, HasherT, EntryT, use_prefetching, prefetching_locality, use_thp>(max_elements, target_load_factor,
                                                                                                                 false, base_identifier) {
    if (print_info) {
      spdlog::info(
          fmt::format("Initialized {} with utils::cacheline_size = {}, sizeof(EntryT) = {}, "
                      "alignof(EntryT) = {}, reprobe_interval = {}",
                      this->get_identifier(), utils::cacheline_size, sizeof(EntryT), alignof(EntryT), this->reprobe_interval));
    }
  }

  struct FindResult {
    uint64_t idx = 0;
    EntryT* entry;
    bool key_equal = false;
    bool entry_valid = false;
  };

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(this->size_ + 1 <= this->max_elements_, "Hashmap is full!");

    uint64_t curr_idx = this->hasher_.hash(key);
    uint64_t curr_displacement = 0;

    uint64_t curr_adjusted_idx = curr_idx;
    EntryT* next_entry = &this->entries_[curr_adjusted_idx];

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
      uint64_t next_entry_displacement = calc_displacement(next_entry->key, curr_adjusted_idx);

      if (curr_displacement > next_entry_displacement || (next_entry_displacement == curr_displacement && key_to_insert < next_entry->key)) {
        if constexpr (std::is_same_v<entries::PackedAoSEntry<KeyT, ValueT>, EntryT> ||
                      std::is_same_v<entries::NaturalAlignedAoSEntry<KeyT, ValueT>, EntryT>) {
          // cannot use std::swap here due to struct packing (compiler complains)
          KeyT next_entry_key = next_entry->key;
          ValueT next_entry_value = next_entry->value;
          next_entry->key = key_to_insert;
          next_entry->value = value_to_insert;

          key_to_insert = next_entry_key;
          value_to_insert = next_entry_value;
        } else {
          std::swap(next_entry->key, key_to_insert);
          std::swap(next_entry->value, value_to_insert);
        }

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

    EntryT* next_entry = &this->entries_[curr_adjusted_idx];

    if constexpr (use_prefetching) {
      READ_PREFETCH(next_entry, prefetching_locality);
      READ_PREFETCH(next_entry + 1, prefetching_locality);
      READ_PREFETCH(next_entry + 2, prefetching_locality);
    }

    bool key_not_equal = next_entry->key != key;
    bool entry_valid = next_entry->is_valid;

    while (key_not_equal && entry_valid) {
      if constexpr (use_prefetching) {
        // we always want to stay 3 entries ahead in cache
        READ_PREFETCH(next_entry + 3, prefetching_locality);
      }

      // every m-th iteration (m = reprobe_interval), we check if we can terminate
      if (curr_displacement % reprobe_interval == 0 && calc_displacement(next_entry->key, curr_adjusted_idx) < curr_displacement) {
        break;
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

  // key is not a reference here because we might pass a key that is not aligned correctly due to struct packing. references
  // to misaligned addresses are undefined behavior, and as we always inline this, we shouldn't suffer a performance penalty
  uint64_t HEDLEY_ALWAYS_INLINE calc_displacement(const KeyT key, uint64_t index) {
    uint64_t original_index = this->hasher_.hash(key);

    return index - original_index + (this->max_elements_ * (index < original_index));
    /* EQUIV TO
    if (index >= original_index) {
      return index - original_index;
    } else {
      return max_elements_ - original_index + index;
    } */
  };

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

 protected:
  alignas(utils::cacheline_size) static constexpr uint64_t reprobe_interval = std::max(static_cast<uint64_t>(1), static_cast<uint64_t>(utils::cacheline_size / sizeof(EntryT)));
};

// Alignment of AoSEntry is done by compiler, i.e., it is aligned to the largest member
// Performance depends on KeyT and ValueT; access to key/value is aligned and thus fast
// AoSEntrys can cross cacheline boundaries
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class AutoPaddedRecalculatingRobinHoodAoSHashTable
    : public RecalculatingRobinHoodAoSHashTable<KeyT, ValueT, HasherT, entries::AutoPaddedAoSEntry<KeyT, ValueT>, use_prefetching,
                                                prefetching_locality, use_thp> {
 public:
  AutoPaddedRecalculatingRobinHoodAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : RecalculatingRobinHoodAoSHashTable<KeyT, ValueT, HasherT, entries::AutoPaddedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality,
                                           use_thp>(max_elements, target_load_factor, true, "AutoPaddedRecalculatingRobinHoodAoSHashTable") {}
};

// AoSEntry is densely packed and not padded at all
// Entries may cross cacheline boundaries, and access to key/value is not aligned, might be slower depending on
// architecture However, there is no memory wasted, so the increased throughput may counter these issues
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class UnalignedRecalculatingRobinHoodAoSHashTable
    : public RecalculatingRobinHoodAoSHashTable<KeyT, ValueT, HasherT, entries::PackedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality,
                                                use_thp> {
 public:
  UnalignedRecalculatingRobinHoodAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : RecalculatingRobinHoodAoSHashTable<KeyT, ValueT, HasherT, entries::PackedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality,
                                           use_thp>(max_elements, target_load_factor, true, "UnalignedRecalculatingRobinHoodAoSHashTable") {}
};

// Naturally aligned AoSEntrys, i.e., aligned to next_power_of_2(sizeof(AoSEntry))
// AoSEntries never cross cacheline boundaries, otherwise similar to AutoPadded, depending on KeyT/ValueT
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class NaturalAlignedRecalculatingRobinHoodAoSHashTable
    : public RecalculatingRobinHoodAoSHashTable<KeyT, ValueT, HasherT, entries::NaturalAlignedAoSEntry<KeyT, ValueT>, use_prefetching,
                                                prefetching_locality, use_thp> {
 public:
  NaturalAlignedRecalculatingRobinHoodAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : RecalculatingRobinHoodAoSHashTable<KeyT, ValueT, HasherT, entries::NaturalAlignedAoSEntry<KeyT, ValueT>, use_prefetching,
                                           prefetching_locality, use_thp>(max_elements, target_load_factor, true,
                                                                          "NaturalAlignedRecalculatingRobinHoodAoSHashTable") {}
};

}  // namespace hashmap::hashmaps
