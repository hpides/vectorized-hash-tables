#pragma once
#include <algorithm>
#include <cstdint>

#include "hashmap/hashes/buckethash.hpp"
#include "hashmap/hashes/hasher.hpp"

namespace hashmap {

// Enforces collisions for testing
template <typename KeyT>
struct OneStaticHasher : public hashing::Hasher<KeyT, true> {
  OneStaticHasher(uint64_t maximum_value) : hashing::Hasher<KeyT, true>(maximum_value) {}
  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& /*key*/) { return 1; }
  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const KeyT& /*key*/) { return 1; }

  template <typename FingerprintT, hashing::FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE hashing::BucketHash<FingerprintT> bucket_hash(const KeyT& /*key*/) {
    hashing::BucketHash<FingerprintT> hash{};
    hash.fingerprint = 1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

    hash.bucket = this->finalize(1);
#pragma GCC diagnostic pop

    if (hash.fingerprint == invalid_fp) {
      ++(hash.fingerprint);
    }

    return hash;
  }
};

// Enforces collisions for testing
template <typename KeyT>
struct TwoStaticHasher : public hashing::Hasher<KeyT, true> {
  TwoStaticHasher(uint64_t maximum_value) : hashing::Hasher<KeyT, true>(maximum_value) {}
  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& /*key*/) { return 2; }
  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const KeyT& /*key*/) { return 2; }

  template <typename FingerprintT, hashing::FingerprintBucketBits fbb, FingerprintT invalid_fp = 0>
  HEDLEY_ALWAYS_INLINE hashing::BucketHash<FingerprintT> bucket_hash(const KeyT& /*key*/) {
    hashing::BucketHash<FingerprintT> hash{};
    hash.fingerprint = 2;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

    hash.bucket = this->finalize(2);
#pragma GCC diagnostic pop

    if (hash.fingerprint == invalid_fp) {
      ++(hash.fingerprint);
    }

    return hash;
  }
};

template <typename KeyT>
struct RobinHoodTestHasher : public hashing::Hasher<KeyT, true> {
  RobinHoodTestHasher(uint64_t maximum_value) : hashing::Hasher<KeyT, true>(maximum_value) {}
  HEDLEY_ALWAYS_INLINE uint64_t hash(const KeyT& key) {
    if (key == 42 || key == 43 || key == 44) {
      return 1;
    }

    if (key == 45 || key == 46 || key == 10) {
      return 0;
    }

    return 2;
  }
  HEDLEY_ALWAYS_INLINE static uint64_t static_hash(const KeyT& key) {
    if (key == 42 || key == 43 || key == 44) {
      return 1;
    }

    if (key == 45 || key == 46 || key == 10) {
      return 0;
    }

    return 2;
  }
};

}  // namespace hashmap
