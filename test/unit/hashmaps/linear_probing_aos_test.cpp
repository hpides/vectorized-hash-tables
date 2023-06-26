#include "hashmap/hashmaps/linear_probing_aos.hpp"

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
class GeneralLinearProbingAoSHashMapTest : public ::testing::Test {};

template <class T>
class StringLinearProbingAoSHashMapTest : public ::testing::Test {};

class SpecificLinearProbingAoSHashMapTest : public ::testing::Test {};

typedef Types<AutoPaddedLinearProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              UnalignedLinearProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              NaturalAlignedLinearProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              AutoPaddedLinearProbingAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              UnalignedLinearProbingAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              NaturalAlignedLinearProbingAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              AutoPaddedLinearProbingAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              UnalignedLinearProbingAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              NaturalAlignedLinearProbingAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              AutoPaddedLinearProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              UnalignedLinearProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              NaturalAlignedLinearProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              AutoPaddedLinearProbingAoSHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>>,
              AutoPaddedLinearProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, false, utils::PrefetchingLocality::NO, false, 10>>
    HashTableTypes;
TYPED_TEST_SUITE(GeneralLinearProbingAoSHashMapTest, HashTableTypes);

TYPED_TEST(GeneralLinearProbingAoSHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingAoSHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingAoSHashMapTest, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingAoSHashMapTest, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingAoSHashMapTest, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingAoSHashMapTest, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingAoSHashMapTest, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificLinearProbingAoSHashMapTest, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<AutoPaddedLinearProbingAoSHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>>>();
}

TEST_F(SpecificLinearProbingAoSHashMapTest, TestWhiteboxCrossBoundaryInsertion) {
  AutoPaddedLinearProbingAoSHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).idx, 2);
  EXPECT_EQ(hashmap.find(43).idx, 3);
  EXPECT_EQ(hashmap.find(44).idx, 0);
  EXPECT_EQ(hashmap.find(45).idx, 1);
}

TEST_F(SpecificLinearProbingAoSHashMapTest, TestPointerUpdate) {
  PointerUpdateTestImpl<AutoPaddedLinearProbingAoSHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

TEST_F(SpecificLinearProbingAoSHashMapTest, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<AutoPaddedLinearProbingAoSHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

typedef Types<AutoPaddedLinearProbingAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              UnalignedLinearProbingAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              NaturalAlignedLinearProbingAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              AutoPaddedLinearProbingAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>,
              UnalignedLinearProbingAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>,
              NaturalAlignedLinearProbingAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>>
    StringHashTableTypes;
TYPED_TEST_SUITE(StringLinearProbingAoSHashMapTest, StringHashTableTypes);

TYPED_TEST(StringLinearProbingAoSHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingAoSHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingAoSHashMapTest, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingAoSHashMapTest, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingAoSHashMapTest, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingAoSHashMapTest, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingAoSHashMapTest, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

}  // namespace hashmap
