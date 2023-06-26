#include "hashmap/hashmaps/chained.hpp"

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
class GeneralChainedHashMapTest : public ::testing::Test {};

template <class T>
class StringChainedHashMapTest : public ::testing::Test {};

class SpecificChainedHashMapTest : public ::testing::Test {};

// We give all these hash maps 100% extra budget so that the smaller testcases with 4 elements and 100% load factor do not fail.
typedef Types<ChainedHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, false, MemoryBudget::KeyValue, 100>,
              ChainedHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, false, MemoryBudget::KeyValue, 100>,
              ChainedHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>, false, MemoryBudget::KeyValue, 100>,
              ChainedHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, false, MemoryBudget::KeyValue, 100>,
              ChainedHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, false, MemoryBudget::KeyValue, 100>,
              ChainedHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, false, MemoryBudget::KeyValue, 100>>
    HashTableTypes;
TYPED_TEST_SUITE(GeneralChainedHashMapTest, HashTableTypes);

TYPED_TEST(GeneralChainedHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralChainedHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralChainedHashMapTest, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralChainedHashMapTest, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralChainedHashMapTest, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralChainedHashMapTest, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralChainedHashMapTest, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificChainedHashMapTest, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<ChainedHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>, false, MemoryBudget::KeyValue, 100>>();
}

TEST_F(SpecificChainedHashMapTest, TestWhiteboxCrossBoundaryInsertion) {
  ChainedHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>, false, MemoryBudget::KeyValue, 100> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).directory_idx, 2);
  EXPECT_EQ(hashmap.find(43).directory_idx, 2);
  EXPECT_EQ(hashmap.find(44).directory_idx, 2);
  EXPECT_EQ(hashmap.find(45).directory_idx, 2);

  auto PtrTo42 = hashmap.find(42).entry;
  auto PtrTo43 = hashmap.find(43).entry;
  auto PtrTo44 = hashmap.find(44).entry;

  EXPECT_EQ(hashmap.find(43).previous_entry, PtrTo42);
  EXPECT_EQ(hashmap.find(44).previous_entry, PtrTo43);
  EXPECT_EQ(hashmap.find(45).previous_entry, PtrTo44);
}

TEST_F(SpecificChainedHashMapTest, TestPointerUpdate) { PointerUpdateTestImpl<ChainedHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>(); }

TEST_F(SpecificChainedHashMapTest, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<ChainedHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

typedef Types<ChainedHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, false, MemoryBudget::KeyValue, 100>,
              ChainedHashTable<StringKey, uint64_t, StaticHasher<StringKey>, false, MemoryBudget::KeyValue, 100>>
    StringHashTableTypes;
TYPED_TEST_SUITE(StringChainedHashMapTest, StringHashTableTypes);

TYPED_TEST(StringChainedHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringChainedHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringChainedHashMapTest, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringChainedHashMapTest, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringChainedHashMapTest, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringChainedHashMapTest, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringChainedHashMapTest, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

}  // namespace hashmap
