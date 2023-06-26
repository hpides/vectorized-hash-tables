#pragma once
#include <cstdint>

#include "hashmap/utils.hpp"
#include "hedley.h"

namespace hashmap::entries {

// This entry gets padded by the compiler to be aligned to its largest member
// Potentially, this wastes more space, but leads to naturally aligned access on the entry
template <typename KeyT, typename ValueT>
struct AutoPaddedAoSEntry {
  KeyT key;
  ValueT value;
  bool is_valid;

  AutoPaddedAoSEntry() { reset(); }

  AutoPaddedAoSEntry(KeyT entry_key, ValueT entry_value) {
    key = entry_key;
    value = entry_value;
    is_valid = 1;
  }

  void reset() {
    key = KeyT{};
    value = ValueT{};
    is_valid = 0;
  }
};

HEDLEY_DIAGNOSTIC_PUSH
// alignment = 0 is ignored according to standard, gcc warns here annoyingly
HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
template <typename KeyT, typename ValueT, size_t alignment = 0>
struct alignas(alignment) PackedAoSEntry {
  KeyT key;
  ValueT value;
  uint8_t is_valid;

  PackedAoSEntry() { reset(); }

  PackedAoSEntry(KeyT entry_key, ValueT entry_value) {
    key = entry_key;
    value = entry_value;
    is_valid = 1;
  }

  void reset() {
    key = KeyT{};
    value = ValueT{};
    is_valid = 0;
  }
} __attribute__((__packed__));

HEDLEY_DIAGNOSTIC_POP

template <typename KeyT, typename ValueT>
using NaturalAlignedAoSEntry = PackedAoSEntry<KeyT, ValueT, hashmap::utils::value_or_np2(sizeof(PackedAoSEntry<KeyT, ValueT>))>;

/* Special Types for Robin Hood */
template <typename KeyT, typename ValueT>
struct AutoPaddedRHAoSEntry {
  KeyT key;
  ValueT value;
  bool is_valid : 1;
  uint64_t displacement : 63;

  AutoPaddedRHAoSEntry() { reset(); }

  AutoPaddedRHAoSEntry(KeyT entry_key, ValueT entry_value) {
    key = entry_key;
    value = entry_value;
    is_valid = 1;
    displacement = 0;
  }

  void reset() {
    key = KeyT{};
    value = ValueT{};
    is_valid = 0;
    displacement = 0;
  }
};

}  // namespace hashmap::entries
