#pragma once
#include <cstdint>

#include "hashmap/hashes/buckethash.hpp"
#include "hashmap/hashes/hasher.hpp"
#include "hedley.h"

namespace hashmap::hashing {

// Enforces collisions for testing
template <typename KeyT>
struct StaticHasher : public Hasher<KeyT, true> {
  StaticHasher(uint64_t maximum_value) : Hasher<KeyT, true>(maximum_value) {}
  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const KeyT& /*key*/) { return 0; }
  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& /*key*/) const { return 0; }

  template <typename FingerprintT, FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE BucketHash<FingerprintT> bucket_hash(const KeyT& key) {
    const uint64_t r = static_hash(key);
    BucketHash<FingerprintT> hash{};
    static_assert(
        fbb != FingerprintBucketBits::LSBMSB,
        "For static hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if constexpr (fbb == FingerprintBucketBits::LSBMSB) {
      FAIL("For static hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");
    } else if constexpr (fbb == FingerprintBucketBits::MSBLSB) {
      constexpr uint64_t fp_shift_factor = 64 - (sizeof(FingerprintT) * 8);

      hash.bucket = this->finalize(r);
      hash.fingerprint = r >> fp_shift_factor;
    } else {
      hash.bucket = this->finalize(r);
      hash.fingerprint = static_cast<FingerprintT>(r);
    }
#pragma GCC diagnostic pop

    if (hash.fingerprint == invalid_fp) {
      ++(hash.fingerprint);
    }

    return hash;
  }

  std::string get_identifier() const { return "StaticHasher"; }
};

}  // namespace hashmap::hashing
