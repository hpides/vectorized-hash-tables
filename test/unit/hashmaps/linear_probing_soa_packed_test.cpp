#include "hashmap/hashmaps/linear_probing_soa_packed.hpp"

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
class GeneralLinearProbingPackedSoAHashMapTest : public ::testing::Test {};

template <class T>
class StringLinearProbingPackedSoAHashMapTest : public ::testing::Test {};

class SpecificLinearProbingPackedSoAHashMapTest : public ::testing::Test {};

typedef Types<LinearProbingPackedSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              LinearProbingPackedSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              LinearProbingPackedSoAHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              LinearProbingPackedSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              LinearProbingPackedSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>>,
              LinearProbingPackedSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, false, 10>>
    HashTableTypes;
TYPED_TEST_SUITE(GeneralLinearProbingPackedSoAHashMapTest, HashTableTypes);

TYPED_TEST(GeneralLinearProbingPackedSoAHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingPackedSoAHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingPackedSoAHashMapTest, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingPackedSoAHashMapTest, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingPackedSoAHashMapTest, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingPackedSoAHashMapTest, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingPackedSoAHashMapTest, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificLinearProbingPackedSoAHashMapTest, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<LinearProbingPackedSoAHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>>>();
}

TEST_F(SpecificLinearProbingPackedSoAHashMapTest, TestWhiteboxCrossBoundaryInsertion) {
  LinearProbingPackedSoAHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).idx, 2);
  EXPECT_EQ(hashmap.find(43).idx, 3);
  EXPECT_EQ(hashmap.find(44).idx, 0);
  EXPECT_EQ(hashmap.find(45).idx, 1);
}

TEST_F(SpecificLinearProbingPackedSoAHashMapTest, TestPointerUpdate) {
  PointerUpdateTestImpl<LinearProbingPackedSoAHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

TEST_F(SpecificLinearProbingPackedSoAHashMapTest, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<LinearProbingPackedSoAHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

typedef Types<LinearProbingPackedSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              LinearProbingPackedSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>>>
    StringHashTableTypes;
TYPED_TEST_SUITE(StringLinearProbingPackedSoAHashMapTest, StringHashTableTypes);

TYPED_TEST(StringLinearProbingPackedSoAHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingPackedSoAHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingPackedSoAHashMapTest, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingPackedSoAHashMapTest, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingPackedSoAHashMapTest, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingPackedSoAHashMapTest, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingPackedSoAHashMapTest, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

}  // namespace hashmap
