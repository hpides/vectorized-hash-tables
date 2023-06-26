#include "hashmap/misc/stringkey.hpp"

#include "hashmap/utils.hpp"

bool operator==(const StringKey& sk1, const StringKey& sk2) {
  if (sk1.string_ != nullptr && sk2.string_ != nullptr) [[likely]] {
    return std::strcmp(sk1.string_, sk2.string_) == 0;
  }

  if (sk1.string_ == nullptr && sk2.string_ == nullptr) {
    return true;
  }

  return false;
}

bool operator<(const StringKey& sk1, const StringKey& sk2) {
  if (sk1.string_ != nullptr && sk2.string_ != nullptr) [[likely]] {
    return std::strcmp(sk1.string_, sk2.string_) < 0;
  }

  if (sk1.string_ == nullptr && sk2.string_ == nullptr) {
    return false;
  }

  if (sk1.string_ == nullptr) {
    return false;
  }

  return true;
}
