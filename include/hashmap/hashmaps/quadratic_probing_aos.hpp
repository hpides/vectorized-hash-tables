#pragma once

#include "fmt/format.h"
#include "hashmap/entries/aosentry.hpp"
#include "hashmap/hashmaps/linear_probing_aos.hpp"
#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap;

namespace hashmap::hashmaps {

template <typename KeyT, typename ValueT, typename HasherT, typename EntryT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class QuadraticProbingAoSHashTable : public LinearProbingAoSHashTable<KeyT, ValueT, HasherT, EntryT, use_prefetching, prefetching_locality, use_thp> {
 public:
  QuadraticProbingAoSHashTable(uint64_t max_elements, uint8_t target_load_factor, std::string base_identifier = "QuadraticProbingAoSHashTable")
      : LinearProbingAoSHashTable<KeyT, ValueT, HasherT, EntryT, use_prefetching, prefetching_locality, use_thp>(max_elements, target_load_factor,
                                                                                                                 false, base_identifier) {
    spdlog::info(
        fmt::format("Initialized {} with utils::cacheline_size = {}, sizeof(EntryT) = {}, "
                    "alignof(EntryT) = {}",
                    this->get_identifier(), utils::cacheline_size, sizeof(EntryT), alignof(EntryT)));

    ASSERT(utils::is_power_of_two(max_elements),
           fmt::format("QP is optimized towards hash maps that have a maximum size which is a power of two, this hash "
                       "map is of size {}.",
                       max_elements));
  }

  struct FindResult {
    uint64_t idx = 0;
    EntryT* entry;
    bool key_equal = false;
    bool entry_valid = false;
  };

  FindResult find(const KeyT& key) {
#ifdef HASHMAP_COLLECT_META_INFO
    uint64_t probing_seq_len = 1;
    if (this->minfo.measurement_started) {
      ++(this->minfo.num_finds);
      ++(this->minfo.probed_elements);
    }
#endif

    uint64_t start_idx = this->hasher_.hash(key);
    uint64_t curr_adjusted_idx = start_idx;
    EntryT* next_entry = &this->entries_[curr_adjusted_idx];
    // because we assume power of two, it is sufficient to probe size_ many times
    // it can be shown that QP considers every slot in that case
    uint64_t probe_index = 0;

    bool key_not_equal = next_entry->key != key;
    bool entry_valid = next_entry->is_valid;

    if constexpr (use_prefetching) {
      READ_PREFETCH(&this->entries_[curr_adjusted_idx], prefetching_locality);

      // If prefetching, we need to calculate the next 2 indices in advance
      uint64_t next_adjusted_idx =
          this->hasher_.finalize(start_idx + static_cast<uint64_t>((probe_index + 1 + ((probe_index + 1) * (probe_index + 1)))) / 2);
      uint64_t next_next_adjusted_idx =
          this->hasher_.finalize(start_idx + static_cast<uint64_t>((probe_index + 2 + ((probe_index + 2) * (probe_index + 2)))) / 2);

      READ_PREFETCH(&this->entries_[next_adjusted_idx], prefetching_locality);
      READ_PREFETCH(&this->entries_[next_next_adjusted_idx], prefetching_locality);
      key_not_equal = next_entry->key != key;
      entry_valid = next_entry->is_valid;

      while (key_not_equal && entry_valid && probe_index <= this->size_) {
        ++probe_index;
        curr_adjusted_idx = next_adjusted_idx;
        next_adjusted_idx = next_next_adjusted_idx;

        next_next_adjusted_idx =
            this->hasher_.finalize(start_idx + static_cast<uint64_t>((probe_index + 2 + ((probe_index + 2) * (probe_index + 2)))) / 2);

        READ_PREFETCH(&this->entries_[next_next_adjusted_idx], prefetching_locality);

        next_entry = &this->entries_[curr_adjusted_idx];
        key_not_equal = next_entry->key != key;
        entry_valid = next_entry->is_valid;
#ifdef HASHMAP_COLLECT_META_INFO
        if (this->minfo.measurement_started) {
          ++(this->minfo.probed_elements);
          ++probing_seq_len;
        }
#endif
      }

    } else {
      // when not prefetching, do regular QP and calculate only next index as required
      while (key_not_equal && entry_valid && probe_index <= this->size_) {
        ++probe_index;
        curr_adjusted_idx = this->hasher_.finalize(start_idx + static_cast<uint64_t>((probe_index + (probe_index * probe_index))) / 2);

        next_entry = &this->entries_[curr_adjusted_idx];
        key_not_equal = next_entry->key != key;
        entry_valid = next_entry->is_valid;

#ifdef HASHMAP_COLLECT_META_INFO
        if (this->minfo.measurement_started) {
          ++(this->minfo.probed_elements);
          ++probing_seq_len;
        }
#endif
      }
    }

#ifdef HASHMAP_COLLECT_META_INFO
    if (this->minfo.measurement_started) {
      this->minfo.min_probing_sequence = std::min(this->minfo.min_probing_sequence, probing_seq_len);
      this->minfo.max_probing_sequence = std::max(this->minfo.max_probing_sequence, probing_seq_len);
    }
#endif

    return {curr_adjusted_idx, next_entry, !key_not_equal, entry_valid};
  }

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

  void insert(const KeyT& key, const ValueT& value) {
    DEBUG_ASSERT(this->size_ + 1 <= this->max_elements_, "Hashmap is full!");
    FindResult res = find(key);

    if (!res.key_equal || !res.entry_valid) {
      // We are inserting a new element (no update)
      ++(this->size_);
      res.entry->key = key;
      res.entry->is_valid = 1;
    }

    res.entry->value = value;
  };
};

// Alignment of AoSEntry is done by compiler, i.e., it is aligned to the largest member
// Performance depends on KeyT and ValueT; access to key/value is aligned and thus fast
// AoSEntrys can cross cacheline boundaries
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class AutoPaddedQuadraticProbingAoSHashTable : public QuadraticProbingAoSHashTable<KeyT, ValueT, HasherT, entries::AutoPaddedAoSEntry<KeyT, ValueT>,
                                                                                   use_prefetching, prefetching_locality, use_thp> {
 public:
  AutoPaddedQuadraticProbingAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : QuadraticProbingAoSHashTable<KeyT, ValueT, HasherT, entries::AutoPaddedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality,
                                     use_thp>(max_elements, target_load_factor, "AutoPaddedQuadraticProbingAoSHashTable") {}
};

// AoSEntry is densely packed and not padded at all
// Entries may cross cacheline boundaries, and access to key/value is not aligned, might be slower depending on
// architecture However, there is no memory wasted, so the increased throughput may counter these issues
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class UnalignedQuadraticProbingAoSHashTable : public QuadraticProbingAoSHashTable<KeyT, ValueT, HasherT, entries::PackedAoSEntry<KeyT, ValueT>,
                                                                                  use_prefetching, prefetching_locality, use_thp> {
 public:
  UnalignedQuadraticProbingAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : QuadraticProbingAoSHashTable<KeyT, ValueT, HasherT, entries::PackedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality, use_thp>(
            max_elements, target_load_factor, "UnalignedQuadraticProbingAoSHashTable") {}
};

// Naturally aligned AoSEntrys, i.e., aligned to next_power_of_2(sizeof(AoSEntry))
// AoSEntries never cross cacheline boundaries, otherwise similar to AutoPadded, depending on KeyT/ValueT
template <typename KeyT, typename ValueT, typename HasherT, bool use_prefetching = false,
          utils::PrefetchingLocality prefetching_locality = utils::PrefetchingLocality::MEDIUM, bool use_thp = false>
class NaturalAlignedQuadraticProbingAoSHashTable
    : public QuadraticProbingAoSHashTable<KeyT, ValueT, HasherT, entries::NaturalAlignedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality,
                                          use_thp> {
 public:
  NaturalAlignedQuadraticProbingAoSHashTable(uint64_t max_elements, uint8_t target_load_factor)
      : QuadraticProbingAoSHashTable<KeyT, ValueT, HasherT, entries::NaturalAlignedAoSEntry<KeyT, ValueT>, use_prefetching, prefetching_locality,
                                     use_thp>(max_elements, target_load_factor, "NaturalAlignedQuadraticProbingAoSHashTable") {}
};

}  // namespace hashmap::hashmaps
