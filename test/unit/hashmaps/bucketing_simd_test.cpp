#include "hashmap/hashmaps/bucketing_simd.hpp"

#include <cstdint>

#include "hashmap/hashes/statichasher.hpp"
#include "hashmap/hashes/stdhasher.hpp"
#include "hashmap/hashes/xxhasher.hpp"
#include "hashmap/misc/stringkey.hpp"
#include "unit/hashmaps/hashers.hpp"
#include "unit/hashmaps/hashmap_test_impl.hpp"
// Load gtest last, otherwise we get issues with the FAIL macro
// clang-format off
#include "gtest/gtest.h"
// clang-format on

using namespace hashmap::hashmaps;
using namespace hashmap::hashing;
using testing::Types;

namespace hashmap {

template <class T>
class GeneralBucketingSIMDHashTableHashMapTestA : public ::testing::Test {};

template <class T>
class StringBucketingSIMDHashTableHashMapTestA : public ::testing::Test {};

class SpecificBucketingSIMDHashTableHashMapTestA : public ::testing::Test {};

template <class T>
class GeneralBucketingSIMDHashTableHashMapTestB : public ::testing::Test {};

template <class T>
class StringBucketingSIMDHashTableHashMapTestB : public ::testing::Test {};

class SpecificBucketingSIMDHashTableHashMapTestB : public ::testing::Test {};

template <class T>
class GeneralBucketingSIMDHashTableHashMapTestC : public ::testing::Test {};

template <class T>
class StringBucketingSIMDHashTableHashMapTestC : public ::testing::Test {};

class SpecificBucketingSIMDHashTableHashMapTestC : public ::testing::Test {};

template <class T>
class GeneralBucketingSIMDHashTableHashMapTestD : public ::testing::Test {};
class SpecificBucketingSIMDHashTableHashMapTestD : public ::testing::Test {};

template <class T>
class GeneralBucketingSIMDHashTableHashMapTestE : public ::testing::Test {};

template <class T>
class StringBucketingSIMDHashTableHashMapTestE : public ::testing::Test {};

class SpecificBucketingSIMDHashTableHashMapTestE : public ::testing::Test {};

template <class T>
class GeneralBucketingSIMDHashTableHashMapTestF : public ::testing::Test {};
class SpecificBucketingSIMDHashTableHashMapTestF : public ::testing::Test {};

typedef Types<BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>>
    HashTableTypesA;

TYPED_TEST_SUITE(GeneralBucketingSIMDHashTableHashMapTestA, HashTableTypesA);

TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestA, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestA, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestA, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestA, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestA, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestA, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestA, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificBucketingSIMDHashTableHashMapTestA, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                                 SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestA, TestBucketOverflow) {
  // force overflow by setting fingerprints per bucket to 1
  BucketingSIMDHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 1, 128, SIMDAlgorithm::TESTZ, false, false,
                         NEONAlgo::SSE2NEON>
      hashmap(64, 0);

  EXPECT_EQ(hashmap.get_num_buckets(), 64);
  EXPECT_FALSE(hashmap.get_ith_bucket(2)->is_overflowed());

  hashmap.insert(42, 0);
  EXPECT_FALSE(hashmap.get_ith_bucket(2)->is_overflowed());
  EXPECT_FALSE(hashmap.get_ith_bucket(3)->is_overflowed());
  hashmap.insert(43, 1);
  EXPECT_TRUE(hashmap.get_ith_bucket(2)->is_overflowed());
  EXPECT_FALSE(hashmap.get_ith_bucket(3)->is_overflowed());
  EXPECT_FALSE(hashmap.get_ith_bucket(4)->is_overflowed());

  hashmap.insert(44, 2);
  EXPECT_TRUE(hashmap.get_ith_bucket(2)->is_overflowed());
  EXPECT_TRUE(hashmap.get_ith_bucket(3)->is_overflowed());
  EXPECT_FALSE(hashmap.get_ith_bucket(4)->is_overflowed());

  hashmap.insert(45, 3);
  EXPECT_TRUE(hashmap.get_ith_bucket(2)->is_overflowed());
  EXPECT_TRUE(hashmap.get_ith_bucket(3)->is_overflowed());
  EXPECT_TRUE(hashmap.get_ith_bucket(4)->is_overflowed());
  EXPECT_FALSE(hashmap.get_ith_bucket(5)->is_overflowed());

  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(45), 3);

  // Test update on overflowed buckets
  hashmap.insert(44, 1338);
  EXPECT_EQ(hashmap.lookup(44), 1338);
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestA, TestPointerUpdate) {
  PointerUpdateTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                               SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestA, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

typedef Types<BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>>
    StringHashTableTypesA;

TYPED_TEST_SUITE(StringBucketingSIMDHashTableHashMapTestA, StringHashTableTypesA);

TYPED_TEST(StringBucketingSIMDHashTableHashMapTestA, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestA, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestA, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestA, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestA, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestA, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestA, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

#ifdef __AVX2__

typedef Types<BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>>
    HashTableTypesB;
TYPED_TEST_SUITE(GeneralBucketingSIMDHashTableHashMapTestB, HashTableTypesB);

TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestB, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestB, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestB, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestB, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestB, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestB, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestB, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificBucketingSIMDHashTableHashMapTestB, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                                 SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestB, TestPointerUpdate) {
  PointerUpdateTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                               SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestB, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                                       SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

typedef Types<BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>>
    StringHashTableTypesB;

TYPED_TEST_SUITE(StringBucketingSIMDHashTableHashMapTestB, StringHashTableTypesB);

TYPED_TEST(StringBucketingSIMDHashTableHashMapTestB, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestB, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestB, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestB, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestB, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestB, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestB, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

#endif

#ifdef __AVX512F__
typedef Types<BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ, true,
                                     false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 16, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 16, 256, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ, true,
                                     false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::TESTZ, true,
                                     false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 32, 256,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 32, 256, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ, true,
                                     false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 64, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::NO_TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>>
    HashTableTypesC;

TYPED_TEST_SUITE(GeneralBucketingSIMDHashTableHashMapTestC, HashTableTypesC);

TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestC, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestC, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestC, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestC, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestC, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestC, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestC, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificBucketingSIMDHashTableHashMapTestC, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                                 SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestC, TestPointerUpdate) {
  PointerUpdateTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                               SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestC, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                                       SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>>();
}

typedef Types<BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, KeyValueAoSStoringBucket, 32, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, KeyValueAoSStoringBucket, 32, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512,
                                     SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, KeyValueAoSStoringBucket, 64, 512, SIMDAlgorithm::TESTZ,
                                     true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, KeyValueAoSStoringBucket, 64, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, KeyValueAoSStoringBucket, 64, 512,
                                     SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>>
    StringHashTableTypesC;

TYPED_TEST_SUITE(StringBucketingSIMDHashTableHashMapTestC, StringHashTableTypesC);

TYPED_TEST(StringBucketingSIMDHashTableHashMapTestC, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestC, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestC, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestC, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestC, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestC, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestC, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

#endif

#ifdef HASHMAP_IS_ARM
typedef Types<BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::AARCH64>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>>
    HashTableTypesD;

TYPED_TEST_SUITE(GeneralBucketingSIMDHashTableHashMapTestD, HashTableTypesD);

TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestD, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestD, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestD, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestD, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestD, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestD, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestD, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificBucketingSIMDHashTableHashMapTestD, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                                 SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>>();
}

#ifdef __ARM_FEATURE_SVE
typedef Types<BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 64,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 64,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 64,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 64,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>>
    HashTableTypesE;

TYPED_TEST_SUITE(GeneralBucketingSIMDHashTableHashMapTestE, HashTableTypesE);

TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestE, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestE, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestE, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestE, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestE, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestE, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestE, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificBucketingSIMDHashTableHashMapTestE, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                                 hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>>();
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestE, TestPointerUpdate) {
  PointerUpdateTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                               hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificBucketingSIMDHashTableHashMapTestE, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 32,
                                                       hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>>();
}

typedef Types<BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, KeyValueAoSStoringBucket, 32,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, KeyValueAoSStoringBucket, 64,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, KeyValueAoSStoringBucket, 64,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, KeyValueAoSStoringBucket, 64,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON>,
              BucketingSIMDHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, KeyValueAoSStoringBucket, 64,
                                     hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON>>
    StringHashTableTypesE;

TYPED_TEST_SUITE(StringBucketingSIMDHashTableHashMapTestE, StringHashTableTypesE);

TYPED_TEST(StringBucketingSIMDHashTableHashMapTestE, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestE, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestE, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestE, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestE, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestE, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringBucketingSIMDHashTableHashMapTestE, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

#endif

#endif

#ifdef HASHMAP_IS_POWER
typedef Types<BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 16, 128,
                                     SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::NO_TESTZ,
                                     false, false, NEONAlgo::UMINV, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
              BucketingSIMDHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, KeyValueAoSStoringBucket, 16, 128, SIMDAlgorithm::TESTZ,
                                     false, false, NEONAlgo::UMINV, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>>
    HashTableTypesF;

TYPED_TEST_SUITE(GeneralBucketingSIMDHashTableHashMapTestF, HashTableTypesF);

TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestF, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestF, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestF, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestF, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestF, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestF, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralBucketingSIMDHashTableHashMapTestF, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificBucketingSIMDHashTableHashMapTestF, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<BucketingSIMDHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, KeyValueAoSStoringBucket, 8, 128,
                                                 SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>>();
}
#endif

}  // namespace hashmap
