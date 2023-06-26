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
struct MultAddShift128BHasher : public Hasher<KeyT, use_modulo> {
  MultAddShift128BHasher(uint64_t maximum_value)
      : Hasher<KeyT, use_modulo>(maximum_value), shiftfactor_{128 - static_cast<uint64_t>(std::log2(maximum_value))} {}

  HEDLEY_ALWAYS_INLINE static uint128_t static_hash(const KeyT& key) {
    return utils::multiply_constant_128b * static_cast<uint128_t>(key) + utils::add_constant_128b;
  }

  template <typename FingerprintT, FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE BucketHash<FingerprintT> bucket_hash(const KeyT& key) {
    // In 128-B multiply shift hashing, we are interested in the upper 64 bit of the result.
    // In case of LSB/LSB, both the bucket and fingerprint come from the LSBs of these 64 bit (LSB fingerprinting in MA thesis)
    // In case of MSB/LSB, the MSBs of the upper 64 bit define the fingerprint, and the LSBs the bucket (MSB fingerprinting in thesis)
    // In case of LSB/MSB fingerprinting, the MSBs of the upper 64 bit define the bucket, and the LSBs the fingerprint

    const uint128_t r = static_hash(key);
    BucketHash<FingerprintT> hash{};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if constexpr (fbb == FingerprintBucketBits::LSBMSB) {
      hash.bucket = static_cast<uint64_t>(r >> shiftfactor_);
      hash.fingerprint = static_cast<FingerprintT>(r >> 64);
    } else if constexpr (fbb == FingerprintBucketBits::MSBLSB) {
      constexpr uint64_t fp_shift_factor = 128 - (sizeof(FingerprintT) * 8);

      hash.bucket = this->finalize(static_cast<uint64_t>(r >> 64));
      hash.fingerprint = static_cast<uint64_t>(r >> fp_shift_factor);
    } else {
      const uint64_t shifted = static_cast<uint64_t>(r >> 64);
      hash.bucket = this->finalize(shifted);
      hash.fingerprint = static_cast<FingerprintT>(shifted);
    }
#pragma GCC diagnostic pop

    if (HEDLEY_UNLIKELY(hash.fingerprint == invalid_fp)) {
      ++(hash.fingerprint);
    }

    return hash;
  }

  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& key) const {
    uint128_t r = static_hash(key);
    // We don't do finalize here because we have to shift anyway, so we can directly incorporate it

    if constexpr (use_modulo) {
      // Make r a 64 bit integer first, to make modulo faster
      return static_cast<uint64_t>(r >> 64) % this->maximum_value_;
    } else {
      return static_cast<uint64_t>(r >> shiftfactor_);
    }
  }

  std::string get_identifier() const {
    if constexpr (use_modulo) {
      return "MultAddShift128BModHasher";
    } else {
      return "MultAddShift128BBitHasher";
    }
  }

 private:
  alignas(utils::cacheline_size) const uint64_t shiftfactor_;
};

template <typename KeyT, bool use_modulo>
struct MultAddShift64BHasher : public Hasher<KeyT, use_modulo> {
  MultAddShift64BHasher(uint64_t maximum_value)
      : Hasher<KeyT, use_modulo>(maximum_value), shiftfactor_{64 - static_cast<uint64_t>(std::log2(maximum_value))} {}

  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const KeyT& key) { return utils::multiply_constant_64b * key + utils::add_constant_64b; }

  template <typename FingerprintT, FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE BucketHash<FingerprintT> bucket_hash(const KeyT& key) {
    // In 64-B multiply shift hashing, we have 64 bit after the multiplication, but we only know that the MSBs have certain entropy, the LSBs might be
    // zero In case of LSB/LSB, both the bucket and fingerprint come from the LSBs of the multiplication result (LSB fingerprinting in MA thesis) In
    // case of MSB/LSB, the MSBs of the multiplication result define the fingerprint, and the LSBs the bucket (MSB fingerprinting in thesis) In case
    // of LSB/MSB fingerprinting, the MSBs of the multiplication result define the bucket (which is the true intention of 64B MS hashing), and the LSB
    // the fingerprint

    const uint64_t r = static_hash(key);
    BucketHash<FingerprintT> hash{};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if constexpr (fbb == FingerprintBucketBits::LSBMSB) {
      hash.bucket = r >> shiftfactor_;
      hash.fingerprint = static_cast<FingerprintT>(r);
    } else if constexpr (fbb == FingerprintBucketBits::MSBLSB) {
      constexpr uint64_t fp_shift_factor = 64 - (sizeof(FingerprintT) * 8);

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

  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& key) const {
    uint64_t r = static_hash(key);
    // We don't do finalize here because we have to shift anyway, so we can directly incorporate it

    if constexpr (use_modulo) {
      return r % this->maximum_value_;
    } else {
      return static_cast<uint64_t>(r >> shiftfactor_);
    }
  }

  std::string get_identifier() const {
    if constexpr (use_modulo) {
      return "MultAddShift64BModHasher";
    } else {
      return "MultAddShift64BBitHasher";
    }
  }

 private:
  alignas(utils::cacheline_size) const uint64_t shiftfactor_;
};

}  // namespace hashmap::hashing
