#include <cstdint>

#include "hashmap/hashes/statichasher.hpp"
#include "hashmap/hashes/stdhasher.hpp"
#include "hashmap/hashmaps/simple_simd_soa.hpp"
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
class GeneralSimpleSIMDSOAHashMapTestA : public ::testing::Test {};
class SpecificSimpleSIMDSOAHashMapTestA : public ::testing::Test {};

template <class T>
class GeneralSimpleSIMDSOAHashMapTestB : public ::testing::Test {};
class SpecificSimpleSIMDSOAHashMapTestB : public ::testing::Test {};

template <class T>
class GeneralSimpleSIMDSOAHashMapTestC : public ::testing::Test {};
class SpecificSimpleSIMDSOAHashMapTestC : public ::testing::Test {};

template <class T>
class GeneralSimpleSIMDSOAHashMapTestD : public ::testing::Test {};
class SpecificSimpleSIMDSOAHashMapTestD : public ::testing::Test {};

template <class T>
class GeneralSimpleSIMDSOAHashMapTestE : public ::testing::Test {};
class SpecificSimpleSIMDSOAHashMapTestE : public ::testing::Test {};

typedef Types<
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>>
    HashTableTypesA;

TYPED_TEST_SUITE(GeneralSimpleSIMDSOAHashMapTestA, HashTableTypesA);

TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestA, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestA, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestA, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestA, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestA, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestA, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestA, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificSimpleSIMDSOAHashMapTestA, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<
      UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestA, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(45), 1);
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestA, TestDoNotReturnPadding) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(0, 3);

  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(0), 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(0), 1);
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestA, TestPointerUpdate) {
  PointerUpdateTestImpl<
      UnchunkedSIMDSoAHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestA, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<
      UnchunkedSIMDSoAHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

#ifdef __AVX2__
typedef Types<
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 256, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::SSE2NEON>>
    HashTableTypesB;
TYPED_TEST_SUITE(GeneralSimpleSIMDSOAHashMapTestB, HashTableTypesB);

TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestB, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestB, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestB, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestB, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestB, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestB, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestB, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificSimpleSIMDSOAHashMapTestB, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<
      UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestB, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(45), 1);
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestB, TestDoNotReturnPadding) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(0, 3);

  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(0), 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(0), 1);
}
#endif

#ifdef __AVX512F__
typedef Types<
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 512, SIMDAlgorithm::MANUAL_ON_MATCH, true, false, NEONAlgo::SSE2NEON>>
    HashTableTypesC;

TYPED_TEST_SUITE(GeneralSimpleSIMDSOAHashMapTestC, HashTableTypesC);

TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestC, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestC, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestC, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestC, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestC, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestC, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestC, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificSimpleSIMDSOAHashMapTestC, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<
      UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestC, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(45), 1);
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestC, TestDoNotReturnPadding) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(0, 3);

  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(0), 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(0), 1);
}

#endif

#ifdef HASHMAP_IS_ARM
typedef Types<
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::AARCH64>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>,
    UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, 128, SIMDAlgorithm::MANUAL_ON_MATCH, false, false, NEONAlgo::UMINV>>
    HashTableTypesD;

TYPED_TEST_SUITE(GeneralSimpleSIMDSOAHashMapTestD, HashTableTypesD);

TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestD, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestD, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestD, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestD, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestD, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestD, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestD, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificSimpleSIMDSOAHashMapTestD, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<
      UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>>();
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestD, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(45), 1);
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestD, TestDoNotReturnPadding) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64> hashmap(4, 100);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(0, 3);

  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(0), 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(0), 1);
}

#ifdef __ARM_FEATURE_SVE
typedef Types<UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, hashmap::utils::sve_register_size,
                                        SIMDAlgorithm::MANUAL_ON_MATCH, false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, hashmap::utils::sve_register_size,
                                        SIMDAlgorithm::MANUAL_ON_MATCH, false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, hashmap::utils::sve_register_size,
                                        SIMDAlgorithm::MANUAL_ON_MATCH, false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, hashmap::utils::sve_register_size,
                                        SIMDAlgorithm::MANUAL_ON_MATCH, false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, hashmap::utils::sve_register_size,
                                        SIMDAlgorithm::MANUAL_ON_MATCH, false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::MANUAL_ON_MATCH,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::MANUAL_ON_MATCH,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::MANUAL_ON_MATCH,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::MANUAL_ON_MATCH,
                                        false, true, NEONAlgo::SSE2NEON, false>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ,
                                        false, true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>,
              UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::NO_TESTZ, false,
                                        true, NEONAlgo::SSE2NEON, true>>
    HashTableTypesE;

TYPED_TEST_SUITE(GeneralSimpleSIMDSOAHashMapTestE, HashTableTypesE);

TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestE, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestE, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestE, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestE, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestE, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestE, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralSimpleSIMDSOAHashMapTestE, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificSimpleSIMDSOAHashMapTestE, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, hashmap::utils::sve_register_size,
                                                    SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>>();
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestE, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,
                            NEONAlgo::SSE2NEON, false>
      hashmap(4, 100);

  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(45), 1);
}

TEST_F(SpecificSimpleSIMDSOAHashMapTestE, TestDoNotReturnPadding) {
  // TODO: Make this also parametrized...
  UnchunkedSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ, false, true,
                            NEONAlgo::SSE2NEON, false>
      hashmap(4, 100);

  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(0, 3);

  EXPECT_EQ(hashmap.lookup(42), 0);
  EXPECT_EQ(hashmap.lookup(43), 1);
  EXPECT_EQ(hashmap.lookup(44), 2);
  EXPECT_EQ(hashmap.lookup(0), 3);

  EXPECT_EQ(hashmap.find(42), 2);
  EXPECT_EQ(hashmap.find(43), 3);
  EXPECT_EQ(hashmap.find(44), 0);
  EXPECT_EQ(hashmap.find(0), 1);
}

#endif

#endif

}  // namespace hashmap
