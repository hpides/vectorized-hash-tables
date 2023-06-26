#include "hashmap/simd_utils.hpp"

#include <bitset>
#include <cstdint>
#include <vector>

#include "hashmap/utils.hpp"
// Load gtest last, otherwise we get issues with the FAIL macro
// clang-format off
#include "gtest/gtest.h"
// clang-format on

using testing::Types;

namespace hashmap {

class SIMDUtilsTest : public ::testing::Test {};

TEST_F(SIMDUtilsTest, TestIterator128uint8) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint8_t> data = {1, 42, 3, 4, 5, 42, 42, 8, 9, 10, 11, 12, 13, 14, 15, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint16) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint16_t> data = {1, 42, 3, 4, 5, 42, 42, 8};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint32) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint32_t> data = {1, 42, 3, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

#ifdef __AVX2__

TEST_F(SIMDUtilsTest, TestIterator256uint8) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 256;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint8_t> data = {1,  42, 3,  4,  5,  42, 42, 8,  9,  10, 11, 12, 13, 14, 15, 42,
                               17, 18, 19, 42, 21, 22, 23, 24, 42, 26, 27, 28, 29, 30, 42, 32};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 19);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 24);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 30);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator256uint16) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 256;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint16_t> data = {1, 42, 3, 4, 5, 42, 42, 8, 9, 10, 42, 42, 42, 14, 15, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 10);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 11);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 12);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator256uint32) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 256;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint32_t> data = {1, 42, 3, 42, 5, 6, 42, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 7);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}
#endif

#ifdef __AVX512F__
TEST_F(SIMDUtilsTest, TestIterator128uint8AVX512) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint8_t> data = {1, 42, 3, 4, 5, 42, 42, 8, 9, 10, 11, 12, 13, 14, 15, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint16AVX512) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint16_t> data = {1, 42, 3, 4, 5, 42, 42, 8};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint32AVX512) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint32_t> data = {1, 42, 3, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator256uint8AVX512) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 256;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint8_t> data = {1,  42, 3,  4,  5,  42, 42, 8,  9,  10, 11, 12, 13, 14, 15, 42,
                               17, 18, 19, 42, 21, 22, 23, 24, 42, 26, 27, 28, 29, 30, 42, 32};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 19);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 24);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 30);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator256uint16AVX512) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 256;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint16_t> data = {1, 42, 3, 4, 5, 42, 42, 8, 9, 10, 42, 42, 42, 14, 15, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 10);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 11);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 12);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator256uint32AVX512) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 256;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint32_t> data = {1, 42, 3, 42, 5, 6, 42, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 7);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator512uint8AVX512) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 512;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint8_t> data = {1,  42, 3,  4,  5,  42, 42, 8,  9,  10, 11, 12, 13, 14, 15, 42, 17, 18, 19, 42, 21, 22,
                               23, 24, 42, 26, 27, 28, 29, 30, 42, 32, 33, 34, 35, 42, 37, 38, 39, 40, 42, 42, 43, 44,
                               45, 46, 47, 48, 49, 50, 51, 42, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 42};

  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 19);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 24);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 30);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 35);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 40);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 41);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 51);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 63);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator512uint16AVX512) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 512;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint16_t> data = {1,  42, 3,  4,  5,  42, 42, 8,  9,  10, 42, 42, 42, 14, 15, 42,
                                17, 18, 19, 42, 21, 22, 23, 24, 25, 26, 27, 28, 42, 30, 31, 32};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 10);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 11);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 12);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 19);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 28);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator512uint32AVX512) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 512;
  constexpr bool use_avx512_features = true;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint32_t> data = {1, 42, 3, 42, 5, 6, 42, 42, 9, 10, 11, 12, 13, 42, 42, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 7);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 13);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 14);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}
#endif

#ifdef HASHMAP_IS_ARM
TEST_F(SIMDUtilsTest, TestIterator128uint8AARCH64) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::AARCH64;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint8_t> data = {1, 42, 3, 4, 5, 42, 42, 8, 9, 10, 11, 12, 13, 14, 15, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint16AARCH64) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::AARCH64;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint16_t> data = {1, 42, 3, 4, 5, 42, 42, 8};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint32AARCH64) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::AARCH64;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint32_t> data = {1, 42, 3, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint8UMINV) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::UMINV;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint8_t> data = {1, 42, 3, 4, 5, 42, 42, 8, 9, 10, 11, 12, 13, 14, 15, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint16UMINV) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::UMINV;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint16_t> data = {1, 42, 3, 4, 5, 42, 42, 8};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint32UMINV) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::UMINV;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint32_t> data = {1, 42, 3, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

#ifdef __ARM_FEATURE_SVE
TEST_F(SIMDUtilsTest, TestIterator128uint8SVE) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = true;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;
  const vector_type index_vector = SIMDH::index_creation_func_();

  std::vector<uint8_t> data = {1, 42, 3, 4, 5, 42, 42, 8, 9, 10, 11, 12, 13, 14, 15, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint16SVE) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = true;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;
  const vector_type index_vector = SIMDH::index_creation_func_();

  std::vector<uint16_t> data = {1, 42, 3, 4, 5, 42, 42, 8};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint32SVE) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = true;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;
  const vector_type index_vector = SIMDH::index_creation_func_();

  std::vector<uint32_t> data = {1, 42, 3, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator512uint8SVE) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 512;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = true;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;
  const vector_type index_vector = SIMDH::index_creation_func_();

  if (SIMDH::calc_invalid_vector_index() < 64) {
    spdlog::error("cannot execute this test as SVE size is too small!");
    return;
  }

  std::vector<uint8_t> data = {1,  42, 3,  4,  5,  42, 42, 8,  9,  10, 11, 12, 13, 14, 15, 42, 17, 18, 19, 42, 21, 22,
                               23, 24, 42, 26, 27, 28, 29, 30, 42, 32, 33, 34, 35, 42, 37, 38, 39, 40, 42, 42, 43, 44,
                               45, 46, 47, 48, 49, 50, 51, 42, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 42};

  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 19);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 24);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 30);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 35);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 40);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 41);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 51);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 63);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator512uint16SVE) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 512;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = true;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  if (SIMDH::calc_invalid_vector_index() < 32) {
    spdlog::error("cannot execute this test as SVE size is too small!");
    return;
  }
  const vector_type index_vector = SIMDH::index_creation_func_();

  std::vector<uint16_t> data = {1,  42, 3,  4,  5,  42, 42, 8,  9,  10, 42, 42, 42, 14, 15, 42,
                                17, 18, 19, 42, 21, 22, 23, 24, 25, 26, 27, 28, 42, 30, 31, 32};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 10);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 11);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 12);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 19);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 28);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator512uint32SVE) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 512;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = true;
  constexpr NEONAlgo neon_algo = NEONAlgo::SSE2NEON;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;
  const vector_type index_vector = SIMDH::index_creation_func_();

  if (SIMDH::calc_invalid_vector_index() < 16) {
    spdlog::error("cannot execute this test as SVE size is too small!");
    return;
  }

  std::vector<uint32_t> data = {1, 42, 3, 42, 5, 6, 42, 42, 9, 10, 11, 12, 13, 42, 42, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

#ifdef HASHMAP_IS_POWER
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);
#else
  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec);
#endif

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 7);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 13);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 14);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::sve_next_match(iterator, index_vector);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}
#endif

#endif

#ifdef HASHMAP_IS_POWER
TEST_F(SIMDUtilsTest, TestIterator128uint8UMINV) {
  using KeyT = uint8_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr NEONAlgo neon_algo = NEONAlgo::UMINV;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint8_t> data = {1, 42, 3, 4, 5, 42, 42, 8, 9, 10, 11, 12, 13, 14, 15, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 15);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint16UMINV) {
  using KeyT = uint16_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::UMINV;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint16_t> data = {1, 42, 3, 4, 5, 42, 42, 8};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 5);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 6);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

TEST_F(SIMDUtilsTest, TestIterator128uint32UMINV) {
  using KeyT = uint32_t;
  constexpr uint16_t simd_size = 128;
  constexpr bool use_avx512_features = false;
  constexpr bool use_thp = false;
  constexpr bool use_sve = false;
  constexpr NEONAlgo neon_algo = NEONAlgo::UMINV;
  constexpr SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ;
  constexpr bool sve_scalar_broadcast = false;

  using SIMDH = SIMDHelper<KeyT, simd_size, simd_algo, use_avx512_features, use_thp, use_sve, neon_algo, sve_scalar_broadcast>;
  using vector_type = typename SIMDH::vector_type;
  using MaskOrVectorInputType = typename SIMDH::MaskOrVectorInputType;
  using LoadPtrT = typename SIMDH::LoadPtrT;
  using CompareT = typename SIMDH::CompareT;
  using CompareResultIterator = typename SIMDH::CompareResultIterator;
  using MaskIteratorT = typename CompareResultIterator::MaskIteratorT;

  std::vector<uint32_t> data = {1, 42, 3, 42};
  vector_type data_vec = SIMDH::vector_unaligned_load_func_(reinterpret_cast<LoadPtrT>(&data[0]));
  const CompareT compare_vec = SIMDH::vector_broadcast_func_(42);

  MaskOrVectorInputType cmp_result;

  cmp_result = SIMDH::vector_cmp_func_(data_vec, compare_vec, false);

  EXPECT_TRUE(SIMDH::vector_any_nonzero(cmp_result));

  MaskIteratorT iterator = CompareResultIterator::initialize(cmp_result);
  EXPECT_TRUE(CompareResultIterator::has_next(iterator));

  uint16_t next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 1);

  EXPECT_TRUE(CompareResultIterator::has_next(iterator));
  next_match = CompareResultIterator::next_match(iterator);
  iterator = CompareResultIterator::next_it(iterator, next_match);
  EXPECT_EQ(next_match, 3);

  EXPECT_FALSE(CompareResultIterator::has_next(iterator));
}

#endif

}  // namespace hashmap
