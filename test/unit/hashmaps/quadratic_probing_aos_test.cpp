#include "hashmap/hashmaps/quadratic_probing_aos.hpp"

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
class QuadraticProbingAoSHashMapTest : public ::testing::Test {};

template <class T>
class StringQuadraticProbingAoSHashMapTest : public ::testing::Test {};

class SpecificQuadraticProbingAoSHashMapTest : public ::testing::Test {};

typedef Types<AutoPaddedQuadraticProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              UnalignedQuadraticProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              NaturalAlignedQuadraticProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>,
              AutoPaddedQuadraticProbingAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              UnalignedQuadraticProbingAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              NaturalAlignedQuadraticProbingAoSHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>,
              AutoPaddedQuadraticProbingAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              UnalignedQuadraticProbingAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              NaturalAlignedQuadraticProbingAoSHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,
              AutoPaddedQuadraticProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              UnalignedQuadraticProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              NaturalAlignedQuadraticProbingAoSHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>>,
              AutoPaddedQuadraticProbingAoSHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>>>
    HashTableTypes;
TYPED_TEST_SUITE(QuadraticProbingAoSHashMapTest, HashTableTypes);

TYPED_TEST(QuadraticProbingAoSHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(QuadraticProbingAoSHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(QuadraticProbingAoSHashMapTest, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(QuadraticProbingAoSHashMapTest, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(QuadraticProbingAoSHashMapTest, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(QuadraticProbingAoSHashMapTest, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(QuadraticProbingAoSHashMapTest, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificQuadraticProbingAoSHashMapTest, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<AutoPaddedQuadraticProbingAoSHashTable<uint64_t, uint64_t, TwoStaticHasher<uint64_t>>>();
}

TEST_F(SpecificQuadraticProbingAoSHashMapTest, TestPointerUpdate) {
  PointerUpdateTestImpl<AutoPaddedQuadraticProbingAoSHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

TEST_F(SpecificQuadraticProbingAoSHashMapTest, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<AutoPaddedQuadraticProbingAoSHashTable<uint64_t, uint64_t*, TwoStaticHasher<uint64_t>>>();
}

typedef Types<AutoPaddedQuadraticProbingAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              UnalignedQuadraticProbingAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              NaturalAlignedQuadraticProbingAoSHashTable<StringKey, uint64_t, XXHasher<StringKey, false>>,
              AutoPaddedQuadraticProbingAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>,
              UnalignedQuadraticProbingAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>,
              NaturalAlignedQuadraticProbingAoSHashTable<StringKey, uint64_t, StaticHasher<StringKey>>>
    StringHashTableTypes;
TYPED_TEST_SUITE(StringQuadraticProbingAoSHashMapTest, StringHashTableTypes);

TYPED_TEST(StringQuadraticProbingAoSHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringQuadraticProbingAoSHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringQuadraticProbingAoSHashMapTest, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringQuadraticProbingAoSHashMapTest, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringQuadraticProbingAoSHashMapTest, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringQuadraticProbingAoSHashMapTest, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringQuadraticProbingAoSHashMapTest, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

}  // namespace hashmap
