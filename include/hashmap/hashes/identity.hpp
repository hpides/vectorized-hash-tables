#pragma once
#include <cstdint>
#include <functional>

#include "hashmap/hashes/buckethash.hpp"
#include "hashmap/hashes/hasher.hpp"
#include "hedley.h"

namespace hashmap::hashing {

template <typename KeyT, bool use_modulo>
struct IdentityHasher : public Hasher<KeyT, use_modulo> {
  IdentityHasher(uint64_t maximum_value) : Hasher<KeyT, use_modulo>(maximum_value) {}
  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const KeyT& key) { return key; }
  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& key) const { return this->finalize(static_hash(key)); }

  template <typename FingerprintT, FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE BucketHash<FingerprintT> bucket_hash(const KeyT& key) {
    const uint64_t r = static_hash(key);
    BucketHash hash{};
    static_assert(
        fbb != FingerprintBucketBits::LSBMSB,
        "For identity hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if constexpr (fbb == FingerprintBucketBits::LSBMSB) {
      FAIL("For identity hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");
    } else if constexpr (fbb == FingerprintBucketBits::MSBLSB) {
      constexpr uint64_t fp_shift_factor = 64 - (sizeof(FingerprintT) * 8);
      hash.bucket = this->finalize(r);
      hash.fingerprint = r >> fp_shift_factor;
    } else {
      hash.bucket = this->finalize(r);
      hash.fingerprint = static_cast<FingerprintT>(r);
    }

    if (HEDLEY_UNLIKELY(hash.fingerprint == invalid_fp)) {
      ++(hash.fingerprint);
    }
#pragma GCC diagnostic pop

    return hash;
  }

  std::string get_identifier() const {
    if constexpr (use_modulo) {
      return "IdentityModHasher";
    } else {
      return "IdentityBitHasher";
    }
  }
};

}  // namespace hashmap::hashing
