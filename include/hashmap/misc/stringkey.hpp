#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "hashmap/utils.hpp"
#include "hedley.h"

struct StringKey {
  StringKey(char* string) {
    // ASSERT(string != nullptr, "String cannot be nullptr");
    //  ASSERT(std::strlen(string) == HASHMAP_STRINGKEY_SIZE - 1, "String has invalid length");

    string_ = string;
  }

  StringKey() {}
  char* string_ = nullptr;

  void free() {
    std::free(string_);
    string_ = nullptr;
  }

  friend bool operator==(const StringKey& sk1, const StringKey& sk2);
  friend bool operator<(const StringKey& sk1, const StringKey& sk2);
} __attribute__((packed));
