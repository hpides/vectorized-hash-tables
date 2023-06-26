#pragma once
#include <cmath>
#include <cstdint>
#include <functional>

#include "fmt/format.h"
#include "hashmap/utils.hpp"
#include "hedley.h"

namespace hashmap::hashing {

template <typename KeyT, bool use_modulo>
struct Hasher {
 public:
  Hasher(uint64_t maximum_value) : maximum_value_{maximum_value} {
    // we allow zero here as we sometimes (chained hash map need a garbage initializer)
    ASSERT(utils::is_power_of_two(maximum_value) || maximum_value == 0 || use_modulo,
           fmt::format("When not using modulo-finalization, we need to map into the range [0; 2^w]. use_modulo = {}, "
                       "maximum_value = {}, next po2 = {} ",
                       use_modulo, maximum_value, utils::value_or_np2(maximum_value)));
  }

  HEDLEY_ALWAYS_INLINE uint64_t finalize(const uint64_t& hash) const {
    if constexpr (use_modulo) {
      return hash % maximum_value_;
    } else {
      return hash & (maximum_value_ - 1);
    }
  }

  std::string get_identifier() const {
    if constexpr (use_modulo) {
      return "UndefinedModHasher";
    } else {
      return "UndefinedBitHasher";
    }
  }

  alignas(utils::cacheline_size) const uint64_t maximum_value_;
};

}  // namespace hashmap::hashing
