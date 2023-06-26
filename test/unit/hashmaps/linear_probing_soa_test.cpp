#include "hashmap/hashmaps/linear_probing_soa.hpp"

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
class GeneralLinearProbingSoAHashMapTest : public ::testing::Test {};

template <class T>
class StringLinearProbingSoAHashMapTest : public ::testing::Test {};

class SpecificLinearProbingSoAHashMapTest : public ::testing::Test {};

typedef Types<LinearProbingSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              LinearProbingSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              LinearProbingSoAHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              LinearProbingSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              LinearProbingSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>>,
              LinearProbingSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, false, 10>>
    HashTableTypes;
TYPED_TEST_SUITE(GeneralLinearProbingSoAHashMapTest, HashTableTypes);

TYPED_TEST(GeneralLinearProbingSoAHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingSoAHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingSoAHashMapTest, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingSoAHashMapTest, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingSoAHashMapTest, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingSoAHashMapTest, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralLinearProbingSoAHashMapTest, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificLinearProbingSoAHashMapTest, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<LinearProbingSoAHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>>>();
}

TEST_F(SpecificLinearProbingSoAHashMapTest, TestWhiteboxCrossBoundaryInsertion) {
  LinearProbingSoAHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).idx, 2);
  EXPECT_EQ(hashmap.find(43).idx, 3);
  EXPECT_EQ(hashmap.find(44).idx, 0);
  EXPECT_EQ(hashmap.find(45).idx, 1);
}

TEST_F(SpecificLinearProbingSoAHashMapTest, TestPointerUpdate) {
  PointerUpdateTestImpl<LinearProbingSoAHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

TEST_F(SpecificLinearProbingSoAHashMapTest, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<LinearProbingSoAHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

typedef Types<LinearProbingSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              LinearProbingSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>>>
    StringHashTableTypes;
TYPED_TEST_SUITE(StringLinearProbingSoAHashMapTest, StringHashTableTypes);

TYPED_TEST(StringLinearProbingSoAHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingSoAHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingSoAHashMapTest, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingSoAHashMapTest, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingSoAHashMapTest, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingSoAHashMapTest, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringLinearProbingSoAHashMapTest, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

}  // namespace hashmap
