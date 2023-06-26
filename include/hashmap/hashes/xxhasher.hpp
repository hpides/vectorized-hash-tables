#pragma once
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ostream>

#define XXH_INLINE_ALL 1

#include "hashmap/hashes/buckethash.hpp"
#include "hashmap/hashes/hasher.hpp"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/utils.hpp"
#include "hedley.h"
#include "xxhash.h"

namespace hashmap::hashing {

template <typename KeyT, bool use_modulo>
struct XXHasherBase : public Hasher<KeyT, use_modulo> {
  XXHasherBase(uint64_t maximum_value) : Hasher<KeyT, use_modulo>(maximum_value) {}

  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const KeyT& key) { return static_cast<uint64_t>(XXH3_64bits(&key, key_len)); }

  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& key) const { return this->finalize(static_hash(key)); }

  template <typename FingerprintT, FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE BucketHash<FingerprintT> bucket_hash(const KeyT& key) {
    const uint64_t r = static_hash(key);
    BucketHash<FingerprintT> hash{};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if constexpr (fbb == FingerprintBucketBits::LSBMSB) {
      FAIL("For XX hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");
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

  std::string get_identifier() const {
    if constexpr (use_modulo) {
      return "XXModHasher";
    } else {
      return "XXBitHasher";
    }
  }

 private:
  alignas(utils::cacheline_size) constexpr static size_t key_len = sizeof(KeyT);
};

template <typename KeyT, bool use_modulo>
struct XXHasher : public XXHasherBase<KeyT, use_modulo> {};

template <bool use_modulo>
struct XXHasher<StringKey, use_modulo> : public XXHasherBase<StringKey, use_modulo> {
  XXHasher(uint64_t maximum_value) : XXHasherBase<StringKey, use_modulo>(maximum_value) {}

  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const StringKey& key) {
    DEBUG_ASSERT(key.string_ != nullptr, "String of key cannot be nullptr.");
    return static_cast<uint64_t>(XXH3_64bits(key.string_, HASHMAP_STRINGKEY_SIZE));
  }

  template <typename FingerprintT, FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE BucketHash<FingerprintT> bucket_hash(const StringKey& key) {
    const uint64_t r = static_hash(key);
    BucketHash<FingerprintT> hash{};

    static_assert(
        fbb != FingerprintBucketBits::LSBMSB,
        "For XX hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if constexpr (fbb == FingerprintBucketBits::LSBMSB) {
      FAIL("For XX hashing, please do not use LSB/MSB, and use MSB/LSB instead. LSB/MSB is only relevant for multiply-shift types of hashing.");
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

  HEDLEY_ALWAYS_INLINE uint64_t hash(const StringKey& key) const { return this->finalize(static_hash(key)); }
};

}  // namespace hashmap::hashing
