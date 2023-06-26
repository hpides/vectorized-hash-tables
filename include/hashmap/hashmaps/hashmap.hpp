#pragma once

namespace hashmap::hashmaps {

// Only for the interface
template <typename KeyT, typename ValueT>
class HashTable {
 public:
  ValueT lookup(const KeyT& key);
  bool contains(const KeyT& key);
  void insert(const KeyT& key, const ValueT& value);

  void prefault();  // also resets
  void reset();

  uint64_t get_entry_size();
  std::string get_identifier();

  bool can_be_used() { return true; }
};

}  // namespace hashmap::hashmaps
