#pragma once
#include <cmath>
#include <cstdint>
#include <functional>

#include "hashmap/hashes/buckethash.hpp"
#include "hashmap/hashes/hasher.hpp"
#include "hashmap/utils.hpp"
#include "hedley.h"

namespace hashmap::hashing {

template <typename KeyT, bool use_modulo>
struct MurmurHasher : public Hasher<KeyT, use_modulo> {
  MurmurHasher(uint64_t maximum_value) : Hasher<KeyT, use_modulo>(maximum_value) {}

  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const KeyT& key) {
    uint64_t x = key ^ (key >> 33U);
    x *= utils::murmur_constant1;
    x ^= x >> 33U;
    x *= utils::murmur_constant2;
    x ^= x >> 33U;

    return x;
  }

  template <typename FingerprintT, FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE BucketHash<FingerprintT> bucket_hash(const KeyT& key) {
    const uint64_t r = static_cast<uint32_t>(static_hash(key));  // murmur3 is only 32 bit, but static_hash returns uint64_t

    BucketHash<FingerprintT> hash{};
    static_assert(
        fbb != FingerprintBucketBits::LSBMSB,
        "For murmur hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if constexpr (fbb == FingerprintBucketBits::LSBMSB) {
      FAIL("For Murmur hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");
    } else if constexpr (fbb == FingerprintBucketBits::MSBLSB) {
      constexpr uint32_t fp_shift_factor = 32 - (sizeof(FingerprintT) * 8);

      hash.bucket = this->finalize(r);
      hash.fingerprint = r >> fp_shift_factor;
    } else {
      hash.bucket = this->finalize(r);
      hash.fingerprint = static_cast<FingerprintT>(r);
    }
#pragma GCC diagnostic pop

    if (HEDLEY_UNLIKELY(hash.fingerprint == invalid_fp)) {
      ++(hash.fingerprint);
    }

    return hash;
  }

  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& key) const { return this->finalize(static_hash(key)); }

  std::string get_identifier() const {
    if constexpr (use_modulo) {
      return "MurmurModHasher";
    } else {
      return "MurmurBitHasher";
    }
  }
};

}  // namespace hashmap::hashing
