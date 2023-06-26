#ifdef HASHMAP_BUILD_EXTERNAL
#include <cstdint>

#include "hashmap/hashes/statichasher.hpp"
#include "hashmap/hashes/stdhasher.hpp"
#include "hashmap/hashmaps/abseil.hpp"
#include "hashmap/hashmaps/martinus.hpp"

#ifndef HASHMAP_IS_POWER
#include "hashmap/hashmaps/f14.hpp"
#endif

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
class GeneralExternalHashMapTest : public ::testing::Test {};

#define BASIC_EXTERNAL_HASHMAPS                                                                                                                     \
  AbseilFlatHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>, AbseilFlatHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,         \
      AbseilFlatHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>, MartinusRHFlatHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>, \
      MartinusRHFlatHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>,                                                                      \
      MartinusRHFlatHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>

#ifdef HASHMAP_IS_POWER
#define EXTERNAL_HASHMAPS BASIC_EXTERNAL_HASHMAPS
#else
#define F14_HASHMAPS                                                                                                                    \
  F14ValueHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>>, F14ValueHashTable<uint64_t, uint32_t, StdHasher<uint64_t, false>>, \
      F14ValueHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>>
#define EXTERNAL_HASHMAPS BASIC_EXTERNAL_HASHMAPS, F14_HASHMAPS
#endif

typedef Types<EXTERNAL_HASHMAPS> HashTableTypes;
TYPED_TEST_SUITE(GeneralExternalHashMapTest, HashTableTypes);

TYPED_TEST(GeneralExternalHashMapTest, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralExternalHashMapTest, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralExternalHashMapTest, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralExternalHashMapTest, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralExternalHashMapTest, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralExternalHashMapTest, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralExternalHashMapTest, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

}  // namespace hashmap
#endif
