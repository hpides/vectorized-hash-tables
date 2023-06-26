#include <cstdint>

#include "hashmap/hashes/statichasher.hpp"
#include "hashmap/hashes/stdhasher.hpp"
#include "hashmap/hashes/xxhasher.hpp"
#include "hashmap/hashmaps/recalc_robin_hood_aos.hpp"
#include "hashmap/hashmaps/storing_robin_hood_aos.hpp"
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
class RobinHoodAoSHashMapTest : public ::testing::Test {};

template <class T>
class StringRobinHoodAoSHashMapTest : public ::testing::Test {};

template <class T>
class SpecificRobinHoodAoSHashMapTest : public ::testing::Test {};

typedef Types<AutoPaddedRecalculatingRobinHoodAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              UnalignedRecalculatingRobinHoodAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              NaturalAlignedRecalculatingRobinHoodAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              AutoPaddedRecalculatingRobinHoodAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              UnalignedRecalculatingRobinHoodAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              NaturalAlignedRecalculatingRobinHoodAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              AutoPaddedRecalculatingRobinHoodAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              UnalignedRecalculatingRobinHoodAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              NaturalAlignedRecalculatingRobinHoodAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              AutoPaddedRecalculatingRobinHoodAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              UnalignedRecalculatingRobinHoodAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              NaturalAlignedRecalculatingRobinHoodAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              AutoPaddedRecalculatingRobinHoodAoSHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>>,
              StoringRobinHoodAoSHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>>,
              StoringRobinHoodAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              StoringRobinHoodAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              StoringRobinHoodAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              StoringRobinHoodAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>>
    GeneralHashTableTypes;
TYPED_TEST_SUITE(RobinHoodAoSHashMapTest, GeneralHashTableTypes);

typedef Types<AutoPaddedRecalculatingRobinHoodAoSHashTable<uint64_t, uint64_t, RobinHoodTestHasher<uint64_t>>,
              StoringRobinHoodAoSHashTable<uint64_t, uint64_t, RobinHoodTestHasher<uint64_t>>>
    SpecificHashTableTypes;
TYPED_TEST_SUITE(SpecificRobinHoodAoSHashMapTest, SpecificHashTableTypes);

TYPED_TEST(RobinHoodAoSHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(RobinHoodAoSHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(RobinHoodAoSHashMapTest, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(RobinHoodAoSHashMapTest, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(RobinHoodAoSHashMapTest, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(RobinHoodAoSHashMapTest, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(RobinHoodAoSHashMapTest, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TYPED_TEST(SpecificRobinHoodAoSHashMapTest, TestBlackboxCrossBoundaryInsertion) { CrossBoundariesTestImpl<TypeParam>(); }

TYPED_TEST(SpecificRobinHoodAoSHashMapTest, TestWhiteboxRobinHoodAlgorithm) {
  TypeParam hashmap(8, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  // [] [42, disp = 0] [43, disp = 1] [44, disp = 2] [] [] [] []

  EXPECT_EQ(hashmap.find(42).idx, 1);
  EXPECT_EQ(hashmap.find(43).idx, 2);
  EXPECT_EQ(hashmap.find(44).idx, 3);

  EXPECT_EQ(hashmap.calc_displacement(42, hashmap.find(42).idx), 0);
  EXPECT_EQ(hashmap.calc_displacement(43, hashmap.find(43).idx), 1);
  EXPECT_EQ(hashmap.calc_displacement(44, hashmap.find(44).idx), 2);

  hashmap.insert(45, 4);
  // [45, disp = 0] [42, disp = 0] [43, disp = 1] [44, disp = 2] [] [] [] []

  EXPECT_EQ(hashmap.find(45).idx, 0);
  EXPECT_EQ(hashmap.calc_displacement(45, hashmap.find(45).idx), 0);

  hashmap.insert(46, 5);
  // [45, disp = 0] [46, disp = 1] [42, disp = 1] [43, disp = 2] [44, disp = 3] [] [] []

  EXPECT_EQ(hashmap.find(45).idx, 0);
  EXPECT_EQ(hashmap.find(46).idx, 1);
  EXPECT_EQ(hashmap.find(42).idx, 2);
  EXPECT_EQ(hashmap.find(43).idx, 3);
  EXPECT_EQ(hashmap.find(44).idx, 4);

  EXPECT_EQ(hashmap.calc_displacement(45, hashmap.find(45).idx), 0);
  EXPECT_EQ(hashmap.calc_displacement(46, hashmap.find(46).idx), 1);
  EXPECT_EQ(hashmap.calc_displacement(42, hashmap.find(42).idx), 1);
  EXPECT_EQ(hashmap.calc_displacement(43, hashmap.find(43).idx), 2);
  EXPECT_EQ(hashmap.calc_displacement(44, hashmap.find(44).idx), 3);

  hashmap.insert(10, 1337);
  // [10, disp = 0] [45, disp = 1] [46, disp = 2] [42, disp = 2] [43, disp = 3] [44, disp = 4] [] []

  EXPECT_EQ(hashmap.find(10).idx, 0);
  EXPECT_EQ(hashmap.find(45).idx, 1);
  EXPECT_EQ(hashmap.find(46).idx, 2);
  EXPECT_EQ(hashmap.find(42).idx, 3);
  EXPECT_EQ(hashmap.find(43).idx, 4);
  EXPECT_EQ(hashmap.find(44).idx, 5);

  EXPECT_EQ(hashmap.calc_displacement(10, hashmap.find(10).idx), 0);
  EXPECT_EQ(hashmap.calc_displacement(45, hashmap.find(45).idx), 1);
  EXPECT_EQ(hashmap.calc_displacement(46, hashmap.find(46).idx), 2);
  EXPECT_EQ(hashmap.calc_displacement(42, hashmap.find(42).idx), 2);
  EXPECT_EQ(hashmap.calc_displacement(43, hashmap.find(43).idx), 3);
  EXPECT_EQ(hashmap.calc_displacement(44, hashmap.find(44).idx), 4);

  // Test if values have been perserved during movement

  EXPECT_EQ(hashmap.lookup(10), 1337);
  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(45), 4);
  EXPECT_EQ(hashmap.lookup(46), 5);
}

typedef Types<AutoPaddedRecalculatingRobinHoodAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              UnalignedRecalculatingRobinHoodAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              NaturalAlignedRecalculatingRobinHoodAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              AutoPaddedRecalculatingRobinHoodAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>,
              UnalignedRecalculatingRobinHoodAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>,
              NaturalAlignedRecalculatingRobinHoodAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>,
              StoringRobinHoodAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>,
              StoringRobinHoodAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>>
    StringGeneralHashTableTypes;
TYPED_TEST_SUITE(StringRobinHoodAoSHashMapTest, StringGeneralHashTableTypes);

TYPED_TEST(StringRobinHoodAoSHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringRobinHoodAoSHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringRobinHoodAoSHashMapTest, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringRobinHoodAoSHashMapTest, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringRobinHoodAoSHashMapTest, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringRobinHoodAoSHashMapTest, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringRobinHoodAoSHashMapTest, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

}  // namespace hashmap
