#include <cstdint>

#include "hashmap/hashes/statichasher.hpp"
#include "hashmap/hashes/stdhasher.hpp"
#include "hashmap/hashes/xxhasher.hpp"
#include "hashmap/hashmaps/fingerprinting_simd_soa.hpp"
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
class GeneralFingerprintingSIMDSOAHashMapTestA : public ::testing::Test {};

template <class T>
class StringFingerprintingSIMDSOAHashMapTestA : public ::testing::Test {};

class SpecificFingerprintingSIMDSOAHashMapTestA : public ::testing::Test {};

template <class T>
class GeneralFingerprintingSIMDSOAHashMapTestB : public ::testing::Test {};

template <class T>
class StringFingerprintingSIMDSOAHashMapTestB : public ::testing::Test {};

class SpecificFingerprintingSIMDSOAHashMapTestB : public ::testing::Test {};

template <class T>
class GeneralFingerprintingSIMDSOAHashMapTestC : public ::testing::Test {};

template <class T>
class StringFingerprintingSIMDSOAHashMapTestC : public ::testing::Test {};

class SpecificFingerprintingSIMDSOAHashMapTestC : public ::testing::Test {};

template <class T>
class GeneralFingerprintingSIMDSOAHashMapTestD : public ::testing::Test {};
class SpecificFingerprintingSIMDSOAHashMapTestD : public ::testing::Test {};

template <class T>
class GeneralFingerprintingSIMDSOAHashMapTestE : public ::testing::Test {};

template <class T>
class StringFingerprintingSIMDSOAHashMapTestE : public ::testing::Test {};

class SpecificFingerprintingSIMDSOAHashMapTestE : public ::testing::Test {};

template <class T>
class GeneralFingerprintingSIMDSOAHashMapTestF : public ::testing::Test {};
class SpecificFingerprintingSIMDSOAHashMapTestF : public ::testing::Test {};

typedef Types<
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true, hashing::FingerprintBucketBits::MSBLSB>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true,
                                   hashing::FingerprintBucketBits::MSBLSB>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true,
                                   hashing::FingerprintBucketBits::MSBLSB>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true,
                                   hashing::FingerprintBucketBits::MSBLSB>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true,
                                   hashing::FingerprintBucketBits::MSBLSB>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true,
                                   hashing::FingerprintBucketBits::MSBLSB>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true,
                                   hashing::FingerprintBucketBits::MSBLSB>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true, hashing::FingerprintBucketBits::MSBLSB>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON, false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true,
                                   hashing::FingerprintBucketBits::MSBLSB>>
    HashTableTypesA;

TYPED_TEST_SUITE(GeneralFingerprintingSIMDSOAHashMapTestA, HashTableTypesA);

TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestA, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestA, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestA, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestA, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestA, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestA, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestA, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestA, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false,
                                                         false, NEONAlgo::SSE2NEON>>();
}

typedef Types<FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>>
    StringHashTableTypesA;

TYPED_TEST_SUITE(StringFingerprintingSIMDSOAHashMapTestA, StringHashTableTypesA);

TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestA, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestA, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestA, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestA, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestA, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestA, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestA, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestA, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>
      hashmap(4, 0);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).index, 2);
  EXPECT_EQ(hashmap.find(43).index, 3);
  EXPECT_EQ(hashmap.find(44).index, 0);
  EXPECT_EQ(hashmap.find(45).index, 1);
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestA, TestPointerUpdate) {
  PointerUpdateTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false,
                                                       false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestA, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ,
                                                               false, false, NEONAlgo::SSE2NEON>>();
}

#ifdef __AVX2__

typedef Types<
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::SSE2NEON>>
    HashTableTypesB;
TYPED_TEST_SUITE(GeneralFingerprintingSIMDSOAHashMapTestB, HashTableTypesB);

TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestB, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestB, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestB, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestB, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestB, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestB, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestB, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestB, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, false,
                                                         false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestB, TestPointerUpdate) {
  PointerUpdateTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, false,
                                                       false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestB, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, 256, SIMDAlgorithm::TESTZ,
                                                               false, false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestB, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::SSE2NEON>
      hashmap(4, 0);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).index, 2);
  EXPECT_EQ(hashmap.find(43).index, 3);
  EXPECT_EQ(hashmap.find(44).index, 0);
  EXPECT_EQ(hashmap.find(45).index, 1);
}

typedef Types<FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, 256, SIMDAlgorithm::TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, false, false,
                                             NEONAlgo::SSE2NEON>>
    StringHashTableTypesB;

TYPED_TEST_SUITE(StringFingerprintingSIMDSOAHashMapTestB, StringHashTableTypesB);

TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestB, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestB, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestB, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestB, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestB, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestB, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestB, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

#endif

#ifdef __AVX512F__
typedef Types<
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 256, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 256, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false, NEONAlgo::SSE2NEON>>
    HashTableTypesC;

TYPED_TEST_SUITE(GeneralFingerprintingSIMDSOAHashMapTestC, HashTableTypesC);

TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestC, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestC, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestC, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestC, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestC, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestC, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestC, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestC, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 512, SIMDAlgorithm::TESTZ, true,
                                                         false, NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestC, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>
      hashmap(4, 0);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).index, 2);
  EXPECT_EQ(hashmap.find(43).index, 3);
  EXPECT_EQ(hashmap.find(44).index, 0);
  EXPECT_EQ(hashmap.find(45).index, 1);
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestC, TestPointerUpdate) {
  PointerUpdateTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                                       NEONAlgo::SSE2NEON>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestC, TestExternalPointerUpdate) {
  PointerExternalUpdateTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t*, TwoStaticHasher<uint8_t>, uint16_t, 512, SIMDAlgorithm::TESTZ,
                                                               true, false, NEONAlgo::SSE2NEON>>();
}

typedef Types<
    FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, 512, SIMDAlgorithm::TESTZ, true, false, NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>,
    FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, 512, SIMDAlgorithm::NO_TESTZ, true, false,
                                   NEONAlgo::SSE2NEON>>
    StringHashTableTypesC;

TYPED_TEST_SUITE(StringFingerprintingSIMDSOAHashMapTestC, StringHashTableTypesC);

TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestC, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestC, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestC, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestC, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestC, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestC, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestC, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

#endif

#ifdef HASHMAP_IS_ARM
typedef Types<
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::AARCH64>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>>
    HashTableTypesD;

TYPED_TEST_SUITE(GeneralFingerprintingSIMDSOAHashMapTestD, HashTableTypesD);

TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestD, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestD, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestD, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestD, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestD, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestD, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestD, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestD, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false,
                                                         false, NEONAlgo::AARCH64>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestD, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::AARCH64>
      hashmap(4, 0);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).index, 2);
  EXPECT_EQ(hashmap.find(43).index, 3);
  EXPECT_EQ(hashmap.find(44).index, 0);
  EXPECT_EQ(hashmap.find(45).index, 1);
}

#ifdef __ARM_FEATURE_SVE
typedef Types<FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, false>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>,
              FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON, true>>
    HashTableTypesE;

TYPED_TEST_SUITE(GeneralFingerprintingSIMDSOAHashMapTestE, HashTableTypesE);

TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestE, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestE, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestE, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestE, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestE, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestE, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestE, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestE, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, hashmap::utils::sve_register_size,
                                                         SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON, false>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestE, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, hashmap::utils::sve_register_size, SIMDAlgorithm::TESTZ,
                                 false, true, NEONAlgo::SSE2NEON, false>
      hashmap(4, 0);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).index, 2);
  EXPECT_EQ(hashmap.find(43).index, 3);
  EXPECT_EQ(hashmap.find(44).index, 0);
  EXPECT_EQ(hashmap.find(45).index, 1);
}

typedef Types<FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint16_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::TESTZ, false, true, NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, XXHasher<StringKey, false>, uint8_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON>,
              FingerprintingSIMDSoAHashTable<StringKey, uint64_t, StaticHasher<StringKey>, uint8_t, hashmap::utils::sve_register_size,
                                             SIMDAlgorithm::NO_TESTZ, false, true, NEONAlgo::SSE2NEON>>
    StringHashTableTypesE;

TYPED_TEST_SUITE(StringFingerprintingSIMDSOAHashMapTestE, StringHashTableTypesE);

TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestE, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestE, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestE, TestContains) { StringContainsTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestE, TestInsertAndLookup) { StringInsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestE, TestUpdate) { StringUpdateTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestE, TestMultipleInserts) { StringMultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(StringFingerprintingSIMDSOAHashMapTestE, TestContainsOnFullHashMap) { StringContainsOnFullHashMapImpl<TypeParam>(); }

#endif

#endif

#ifdef HASHMAP_IS_POWER
typedef Types<
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false,
                                   NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StaticHasher<uint64_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StaticHasher<uint32_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StaticHasher<uint16_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StaticHasher<uint8_t>, uint8_t, 128, SIMDAlgorithm::NO_TESTZ, false, false, NEONAlgo::UMINV,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint32_t, uint64_t, StdHasher<uint32_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint16_t, uint64_t, StdHasher<uint16_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, StdHasher<uint8_t, false>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>,
    FingerprintingSIMDSoAHashTable<uint64_t, uint64_t, StdHasher<uint64_t, true>, uint8_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV,
                                   false, false, hashmap::utils::PrefetchingLocality::MEDIUM, false, true>>
    HashTableTypesF;

TYPED_TEST_SUITE(GeneralFingerprintingSIMDSOAHashMapTestF, HashTableTypesF);

TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestF, TestInitialization) { InitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestF, TestLargerInitialization) { LargeInitializationTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestF, TestContains) { ContainsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestF, TestInsertAndLookup) { InsertAndLookupTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestF, TestUpdate) { UpdateTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestF, TestMultipleInserts) { MultipleInsertsTestImpl<TypeParam>(); }
TYPED_TEST(GeneralFingerprintingSIMDSOAHashMapTestF, TestContainsOnFullHashMap) { ContainsOnFullHashMapImpl<TypeParam>(); }

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestF, TestBlackboxCrossBoundaryInsertion) {
  CrossBoundariesTestImpl<FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false,
                                                         false, NEONAlgo::UMINV>>();
}

TEST_F(SpecificFingerprintingSIMDSOAHashMapTestF, TestWhiteboxCrossBoundaryInsertion) {
  // TODO: Make this also parametrized...
  FingerprintingSIMDSoAHashTable<uint8_t, uint64_t, TwoStaticHasher<uint8_t>, uint16_t, 128, SIMDAlgorithm::TESTZ, false, false, NEONAlgo::UMINV>
      hashmap(4, 0);
  hashmap.insert(42, 0);
  hashmap.insert(43, 1);
  hashmap.insert(44, 2);
  hashmap.insert(45, 3);

  EXPECT_EQ(hashmap.find(42).index, 2);
  EXPECT_EQ(hashmap.find(43).index, 3);
  EXPECT_EQ(hashmap.find(44).index, 0);
  EXPECT_EQ(hashmap.find(45).index, 1);
}

#endif

}  // namespace hashmap
