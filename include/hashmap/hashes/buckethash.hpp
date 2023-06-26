#pragma once
#include <cmath>
#include <cstdint>
#include <functional>
#include <numeric>

#include "fmt/format.h"
#include "hashmap/utils.hpp"
#include "hedley.h"

namespace hashmap::hashing {

// LSBLSB: Fingerprint and Bucket both come from the LSBs of the hash
// MSBLSB: Fingerprint MSB, Bucket LSB (can lead to issues with certain hash functions, e.g., multiply-shift, where the LSBs might be 0)
// LSBMSB: Fingerprint LSB, Bucket MSB (under the assumption that most entropy lies within the upper bits, this is probably the best option)
enum class FingerprintBucketBits { LSBLSB, MSBLSB, LSBMSB };

template <typename FingerprintT>
struct BucketHash {
  static constexpr uint64_t FINGERPRINT_BITS = sizeof(FingerprintT) * 8;
  static constexpr uint64_t BUCKET_BITS = 64 - FINGERPRINT_BITS;

  FingerprintT fingerprint : FINGERPRINT_BITS;
  uint64_t bucket : BUCKET_BITS;

  bool operator==(const BucketHash& other) const = default;
};
static_assert(sizeof(BucketHash<uint8_t>) == 8, "BucketHash should be 64 Bits.");
static_assert(BucketHash<uint8_t>::FINGERPRINT_BITS + BucketHash<uint8_t>::BUCKET_BITS == 64, "BucketHash should be 64 Bits.");
static_assert(sizeof(BucketHash<uint16_t>) == 8, "BucketHash should be 64 Bits.");
static_assert(BucketHash<uint16_t>::FINGERPRINT_BITS + BucketHash<uint16_t>::BUCKET_BITS == 64, "BucketHash should be 64 Bits.");

}  // namespace hashmap::hashing
