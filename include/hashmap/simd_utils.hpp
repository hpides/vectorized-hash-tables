#pragma once

#include <bit>
#include <type_traits>

#include "fmt/format.h"
#include "hashmap/utils.hpp"
#include "hedley.h"
#include "spdlog/spdlog.h"

#ifdef HASHMAP_IS_X86
#include <x86intrin.h>
#endif

#ifdef HASHMAP_IS_ARM
#include <arm_neon.h>

#ifdef __ARM_FEATURE_SVE
#include <arm_sve.h>
#endif
#endif

#ifdef HASHMAP_IS_POWER
#include <altivec.h>
#endif

enum class SIMDAlgorithm { TESTZ, NO_TESTZ, MANUAL_ON_MATCH };

enum class NEONAlgo { SSE2NEON, AARCH64, UMINV };

template <auto A, typename...>
auto value = A;

template <typename KeyT, uint16_t simd_size = 128, SIMDAlgorithm simd_algo = SIMDAlgorithm::TESTZ, bool use_avx512_features = false,
          bool use_sve = false, NEONAlgo neon_algo = NEONAlgo::SSE2NEON, bool sve_scalar_broadcast = false>
void fail_if_system_is_incompatible() {
#ifndef HASHMAP_USE_SCALAR_IMPL
#ifndef HASHMAP_IS_X86
  static_assert(!use_avx512_features, "AVX-512 can only be used on x86 systems.");
#endif  // HASHMAP_IS_X86

#ifdef HASHMAP_IS_X86
  static_assert(simd_size == 128 || simd_size == 256 || simd_size == 512, "x86 only supports simd_sizes 128,256,512");
  static_assert(!(use_sve || sve_scalar_broadcast), "x86 does not support SVE");

#ifndef __AVX2__
  static_assert(simd_size != 256, "Cannot use simd_size 256 on non AVX2 system");
#endif  //__AVX2__

#ifndef __AVX512F__
  static_assert(simd_size != 512, "Cannot use simd_size 512 on non AVX2 system");
  static_assert(!use_avx512_features, "Cannot use AVX512 features on non-AVX512 system");

#else
  static_assert(!(!use_avx512_features && simd_size == 512), "Can use simd_size 512 only with use_avx512_features==truee");
#endif  // __AVX512F__
#endif  // HASHMAP_IS_X86

#ifdef HASHMAP_IS_ARM
  static_assert(!(!use_sve && sve_scalar_broadcast), "Cannot use SVE scalar broadcast without SVE");
  static_assert(!(use_sve && neon_algo == NEONAlgo::AARCH64), "Cannot use SVE with aarch64 movemask");

  static_assert(!(!use_sve && simd_size != 128), "When not using SVE, we only support SIMD size 128 (NEON)");

  static_assert(!(use_sve && !hashmap::utils::has_sve), "Cannot use SVE on non SVE system or system where we don't know the SVE vector size.");
  static_assert(!use_sve || hashmap::utils::sve_register_size == simd_size,
                "SIMD size does not match sve register size, this application only supports using the entire SVE register currently.");

#endif

#ifdef HASHMAP_IS_POWER
  static_assert(sizeof(unsigned long) == 8, "Unsigned long needs to be 64 bit to have correct mapping between cstdint types and vector types.");
  static_assert(sizeof(unsigned int) == 4, "Unsigned long needs to be 32 bit to have correct mapping between cstdint types and vector types.");
  static_assert(sizeof(unsigned short) == 2, "Unsigned long needs to be 16 bit to have correct mapping between cstdint types and vector types.");
  static_assert(sizeof(unsigned char) == 1, "Unsigned long needs to be 16 bit to have correct mapping between cstdint types and vector types.");

  static_assert(simd_size == 128, "Power only supports 128-bit SIMD");

  static_assert(!(use_sve || neon_algo == NEONAlgo::AARCH64 || use_avx512_features || sve_scalar_broadcast),
                "Using an incompatible option with Power.");

#ifndef _ARCH_PWR9
  static_assert(value<false, KeyT>, "Not on a Power9 compatible system. We need Power9 for vec_first_match_index().");
#endif
#endif
#endif

  static_assert(simd_size % sizeof(KeyT) == 0, "Key size does not align with simd size");
}

template <typename KeyT, uint16_t simd_size, SIMDAlgorithm simd_algo, bool use_avx512_features, bool use_thp, bool use_sve, NEONAlgo neon_algo,
          bool sve_scalar_broadcast>
class SIMDHelper {
 public:
#ifdef HASHMAP_USE_SCALAR_IMPL

  constexpr static uint16_t _vector_alignment() {
    if constexpr (simd_size == 128) {
      return 16;
    }

    if constexpr (simd_size == 256) {
      return 32;
    }

    return 64;
  }
  using vector_type = KeyT*;  // pointer to aligned data
  using LoadPtrT = KeyT*;     // pointer to aligned data

  using _vector_type = vector_type;

  HEDLEY_ALWAYS_INLINE static KeyT* load_func_impl(KeyT* data) { return data; }
  constexpr static auto vector_load_func() { return &load_func_impl; }
  using VectorLoadFuncT = decltype(vector_load_func());
  constexpr static VectorLoadFuncT vector_load_func_ = vector_load_func();
  constexpr static VectorLoadFuncT vector_unaligned_load_func_ = vector_load_func();

  constexpr static uint8_t elements_per_vector = (simd_size / 8) / sizeof(KeyT);

  using MaskType = typename std::conditional<
      elements_per_vector <= 8, uint8_t,
      typename std::conditional<elements_per_vector <= 16, uint16_t,
                                typename std::conditional<elements_per_vector <= 32, uint32_t, uint64_t>::type>::type>::type;

  using MaskOrVectorInputType = MaskType;
  using CompareT = KeyT;  // broadacst is noop

  HEDLEY_ALWAYS_INLINE static KeyT broadcast_func_impl(KeyT needle) { return needle; }
  constexpr static auto vector_broadcast_func() { return &broadcast_func_impl; }
  using VectorBroadcastFuncT = decltype(vector_broadcast_func());
  constexpr static VectorBroadcastFuncT vector_broadcast_func_ = vector_broadcast_func();

#ifdef HASHMAP_IS_POWER
  HEDLEY_ALWAYS_INLINE static MaskType compare_func_impl(KeyT* data, CompareT val, bool /*ignored*/) {
#else
  HEDLEY_ALWAYS_INLINE static MaskType compare_func_impl(KeyT* data, CompareT val) {
#endif
    alignas(_vector_alignment()) MaskType mask = 0;
    for (uint8_t i = 0; i < elements_per_vector; ++i) {
      mask |= static_cast<MaskType>((data[i] == val) << i);
    }

    /*
    Variant 2:
        alignas(_vector_alignment) std::array<bool, 32> mask{0};
        for (uint8_t i = 0; i < 32; ++i) {
            mask[i] = (val == data[i]);
        }

        alignas(_vector_alignment) uint32_t mask_int = 0;
        for (uint8_t i = 0; i < 32; ++i) {
            mask_int |= mask[i] << i;
        }

        return mask_int;
    */

    return mask;
  }

  constexpr static auto vector_compare_func() { return &compare_func_impl; }

  using VectorCompFuncT = decltype(vector_compare_func());
  constexpr static VectorCompFuncT vector_cmp_func_ = vector_compare_func();

  HEDLEY_ALWAYS_INLINE static bool vector_any_nonzero(MaskType input) { return input > 0; }
  HEDLEY_ALWAYS_INLINE static uint32_t vector_first_index(MaskOrVectorInputType input) { return static_cast<uint32_t>(std::countr_zero(input)); }
  HEDLEY_ALWAYS_INLINE constexpr static uint32_t calc_invalid_vector_index() {
    return elements_per_vector;  // all bits are 0 if there are no matches
  }

  class CompareResultIterator {
   public:
    using MaskIteratorT = MaskType;

    HEDLEY_ALWAYS_INLINE static MaskIteratorT initialize(const MaskType& compare_result) { return compare_result; }
    HEDLEY_ALWAYS_INLINE static bool has_next(MaskIteratorT it) { return it > 0; }
    HEDLEY_ALWAYS_INLINE static uint16_t next_match(MaskIteratorT it) { return static_cast<uint16_t>(std::countr_zero(it)); }
    HEDLEY_ALWAYS_INLINE static MaskIteratorT next_it(MaskIteratorT it, uint32_t /*last_match*/) { return it & static_cast<MaskIteratorT>((it - 1)); }
  };

#else

#ifdef HASHMAP_IS_X86
#pragma GCC diagnostic push
// We need to disable this warning to make it compile_tzcnt_u32
// However, this is a valid warning: The typedef vector_type loses the alignment information
// of the SIMD data types, but the most important vector attribute is kept
// This is a bug in GCC/Clang, which is complex to solve
// See https://www.mail-archive.com/gcc-bugs@gcc.gnu.org/msg664254.html
// Hence, we first define the unalgined vector_type, and then align it using alignas
#pragma GCC diagnostic ignored "-Wignored-attributes"
  // clang-format off
  using _vector_type = typename std::conditional<simd_size == 128,
                            __m128i,
                            typename std::conditional<simd_size == 256,
                                                      __m256i,
                                                      __m512i>::type
                          >::type;
  // clang-format on

  constexpr static uint16_t _vector_alignment() {
    // For some reason, alignof(__mXXXi) is constant 16, despite the header
    // defining larger alignments. Hence, we have to hardcode these values here.

    if constexpr (std::is_same_v<_vector_type, __m128i>) {
      return 16;
    }

    if constexpr (std::is_same_v<_vector_type, __m256i>) {
      return 32;
    }

    if constexpr (std::is_same_v<_vector_type, __m512i>) {
      return 64;
    }

    return alignof(_vector_type);
  }
#pragma GCC diagnostic pop

#ifdef HEDLEY_GNUC_VERSION
// clang/gcc
#ifndef HEDLEY_GCC_VERSION
  // clang
  using vector_type __attribute__((aligned(_vector_alignment()))) = _vector_type;
#else
  // gcc
  // The following is not well-defined in the standard and hence clang complains, but gcc doesn't
  // See update to https://stackoverflow.com/a/15912208 for a discussion
  using vector_type alignas(_vector_alignment()) = _vector_type;

#endif  // HEDLEY_GCC_VERSION
#else
  // any other compiler - not sure what works here, using gcc version for now
  using vector_type alignas(_vector_alignment()) = _vector_type;
#endif
  constexpr static auto vector_load_func() {
    if constexpr (simd_size == 128) {
      return &_mm_load_si128;
    } else if constexpr (simd_size == 256) {
      return &_mm256_load_si256;
    } else if constexpr (simd_size == 512) {
      return &_mm512_load_si512;
    } else {
      FAIL("Not supported");
    }
  }

  using VectorLoadFuncT = decltype(vector_load_func());
  constexpr static VectorLoadFuncT vector_load_func_ = vector_load_func();

  constexpr static auto vector_unaligned_load_func() {
    if constexpr (simd_size == 128) {
      return &_mm_loadu_si128;
    } else if constexpr (simd_size == 256) {
      return &_mm256_loadu_si256;
    } else if constexpr (simd_size == 512) {
      return &_mm512_loadu_si512;
    } else {
      FAIL("Not supported");
    }
  }

  using VectorUnalignedLoadFuncT = decltype(vector_unaligned_load_func());
  constexpr static VectorLoadFuncT vector_unaligned_load_func_ = vector_unaligned_load_func();

  constexpr static auto vector_store_func() {
    if constexpr (simd_size == 128) {
      return &_mm_store_si128;
    } else if constexpr (simd_size == 256) {
      return &_mm256_store_si256;
    } else if constexpr (simd_size == 512) {
      return &_mm512_store_si512;
    } else {
      FAIL("Not supported");
    }
  }

  using VectorStoreFuncT = decltype(vector_store_func());
  constexpr static VectorStoreFuncT vector_store_func_ = vector_store_func();

  using LoadPtrT = vector_type*;

  constexpr static auto non_avx512_vector_compare_func() {
    if constexpr (simd_size == 128) {
      if constexpr (std::is_same_v<KeyT, uint8_t> || std::is_same_v<KeyT, int8_t>) {
        // unsigned equality can use the same function as signed equality
        return &_mm_cmpeq_epi8;
      }

      if constexpr (std::is_same_v<KeyT, uint16_t> || std::is_same_v<KeyT, int16_t>) {
        return &_mm_cmpeq_epi16;
      }

      if constexpr (std::is_same_v<KeyT, uint32_t> || std::is_same_v<KeyT, int32_t>) {
        return &_mm_cmpeq_epi32;
      }

      if constexpr (std::is_same_v<KeyT, uint64_t> || std::is_same_v<KeyT, int64_t>) {
        return &_mm_cmpeq_epi64;
      }

      FAIL("Unsupported key");

    } else if constexpr (simd_size == 256) {
      if constexpr (std::is_same_v<KeyT, uint8_t> || std::is_same_v<KeyT, int8_t>) {
        // unsigned equality can use the same function as signed equality
        return &_mm256_cmpeq_epi8;
      }

      if constexpr (std::is_same_v<KeyT, uint16_t> || std::is_same_v<KeyT, int16_t>) {
        return &_mm256_cmpeq_epi16;
      }

      if constexpr (std::is_same_v<KeyT, uint32_t> || std::is_same_v<KeyT, int32_t>) {
        return &_mm256_cmpeq_epi32;
      }

      if constexpr (std::is_same_v<KeyT, uint64_t> || std::is_same_v<KeyT, int64_t>) {
        return &_mm256_cmpeq_epi64;
      }

      FAIL("Unsupported key");
    }
    FAIL("Unsupported SIMD size");
  }

#ifdef CLANG_COMPILER
#ifdef __AVX512F__
// On clang, the AVX512 compare intrinsics are actually macros - hence we have to write wrappers around all of them
#define GENERATE_AVX512_FUNC(NAM) clang##NAM
  HEDLEY_ALWAYS_INLINE static __mmask16 clang_mm_cmpeq_epi8_mask(__m128i a, __m128i b) { return _mm_cmpeq_epi8_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm_cmpeq_epi16_mask(__m128i a, __m128i b) { return _mm_cmpeq_epi16_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm_cmpeq_epi32_mask(__m128i a, __m128i b) { return _mm_cmpeq_epi32_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm_cmpeq_epi64_mask(__m128i a, __m128i b) { return _mm_cmpeq_epi64_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask16 clang_mm_cmpeq_epu8_mask(__m128i a, __m128i b) { return _mm_cmpeq_epu8_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm_cmpeq_epu16_mask(__m128i a, __m128i b) { return _mm_cmpeq_epu16_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm_cmpeq_epu32_mask(__m128i a, __m128i b) { return _mm_cmpeq_epu32_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm_cmpeq_epu64_mask(__m128i a, __m128i b) { return _mm_cmpeq_epu64_mask(a, b); }

  HEDLEY_ALWAYS_INLINE static __mmask32 clang_mm256_cmpeq_epi8_mask(__m256i a, __m256i b) { return _mm256_cmpeq_epi8_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask16 clang_mm256_cmpeq_epi16_mask(__m256i a, __m256i b) { return _mm256_cmpeq_epi16_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm256_cmpeq_epi32_mask(__m256i a, __m256i b) { return _mm256_cmpeq_epi32_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm256_cmpeq_epi64_mask(__m256i a, __m256i b) { return _mm256_cmpeq_epi64_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask32 clang_mm256_cmpeq_epu8_mask(__m256i a, __m256i b) { return _mm256_cmpeq_epu8_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask16 clang_mm256_cmpeq_epu16_mask(__m256i a, __m256i b) { return _mm256_cmpeq_epu16_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm256_cmpeq_epu32_mask(__m256i a, __m256i b) { return _mm256_cmpeq_epu32_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm256_cmpeq_epu64_mask(__m256i a, __m256i b) { return _mm256_cmpeq_epu64_mask(a, b); }

  HEDLEY_ALWAYS_INLINE static __mmask64 clang_mm512_cmpeq_epi8_mask(__m512i a, __m512i b) { return _mm512_cmpeq_epi8_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask32 clang_mm512_cmpeq_epi16_mask(__m512i a, __m512i b) { return _mm512_cmpeq_epi16_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask16 clang_mm512_cmpeq_epi32_mask(__m512i a, __m512i b) { return _mm512_cmpeq_epi32_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm512_cmpeq_epi64_mask(__m512i a, __m512i b) { return _mm512_cmpeq_epi64_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask64 clang_mm512_cmpeq_epu8_mask(__m512i a, __m512i b) { return _mm512_cmpeq_epu8_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask32 clang_mm512_cmpeq_epu16_mask(__m512i a, __m512i b) { return _mm512_cmpeq_epu16_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask16 clang_mm512_cmpeq_epu32_mask(__m512i a, __m512i b) { return _mm512_cmpeq_epu32_mask(a, b); }
  HEDLEY_ALWAYS_INLINE static __mmask8 clang_mm512_cmpeq_epu64_mask(__m512i a, __m512i b) { return _mm512_cmpeq_epu64_mask(a, b); }

#else
#define GENERATE_AVX512_FUNC(NAM) NAM
#endif
#else
#define GENERATE_AVX512_FUNC(NAM) NAM
#endif

  constexpr static auto avx512_vector_compare_func() {
#ifdef __AVX512F__
    // Only for gcc, these are functions. For clang, these are macros, hence we have to use our wrappers
    // For some reason, unlike, e.g., the load intrinsics, these intrinsics are undefined on non-AVX512 systems
    if constexpr (simd_size == 128) {
      if constexpr (std::is_same_v<KeyT, int8_t>) {
        // NOLINTNEXTLINE(clang-diagnostic-error)
        return &GENERATE_AVX512_FUNC(_mm_cmpeq_epi8_mask);
      }

      if constexpr (std::is_same_v<KeyT, int16_t>) {
        return &GENERATE_AVX512_FUNC(_mm_cmpeq_epi16_mask);
      }

      if constexpr (std::is_same_v<KeyT, int32_t>) {
        return &GENERATE_AVX512_FUNC(_mm_cmpeq_epi32_mask);
      }

      if constexpr (std::is_same_v<KeyT, int64_t>) {
        return &GENERATE_AVX512_FUNC(_mm_cmpeq_epi64_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint8_t>) {
        return &GENERATE_AVX512_FUNC(_mm_cmpeq_epu8_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint16_t>) {
        return &GENERATE_AVX512_FUNC(_mm_cmpeq_epu16_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint32_t>) {
        return &GENERATE_AVX512_FUNC(_mm_cmpeq_epu32_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint64_t>) {
        return &GENERATE_AVX512_FUNC(_mm_cmpeq_epu64_mask);
      }

      FAIL("Unsupported key");

    } else if constexpr (simd_size == 256) {
      if constexpr (std::is_same_v<KeyT, int8_t>) {
        return &GENERATE_AVX512_FUNC(_mm256_cmpeq_epi8_mask);
      }

      if constexpr (std::is_same_v<KeyT, int16_t>) {
        return &GENERATE_AVX512_FUNC(_mm256_cmpeq_epi16_mask);
      }

      if constexpr (std::is_same_v<KeyT, int32_t>) {
        return &GENERATE_AVX512_FUNC(_mm256_cmpeq_epi32_mask);
      }

      if constexpr (std::is_same_v<KeyT, int64_t>) {
        return &GENERATE_AVX512_FUNC(_mm256_cmpeq_epi64_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint8_t>) {
        return &GENERATE_AVX512_FUNC(_mm256_cmpeq_epu8_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint16_t>) {
        return &GENERATE_AVX512_FUNC(_mm256_cmpeq_epu16_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint32_t>) {
        return &GENERATE_AVX512_FUNC(_mm256_cmpeq_epu32_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint64_t>) {
        return &GENERATE_AVX512_FUNC(_mm256_cmpeq_epu64_mask);
      }

      FAIL("Unsupported key");

    } else if constexpr (simd_size == 512) {
      if constexpr (std::is_same_v<KeyT, int8_t>) {
        return &GENERATE_AVX512_FUNC(_mm512_cmpeq_epi8_mask);
      }

      if constexpr (std::is_same_v<KeyT, int16_t>) {
        return &GENERATE_AVX512_FUNC(_mm512_cmpeq_epi16_mask);
      }

      if constexpr (std::is_same_v<KeyT, int32_t>) {
        return &GENERATE_AVX512_FUNC(_mm512_cmpeq_epi32_mask);
      }

      if constexpr (std::is_same_v<KeyT, int64_t>) {
        return &GENERATE_AVX512_FUNC(_mm512_cmpeq_epi64_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint8_t>) {
        return &GENERATE_AVX512_FUNC(_mm512_cmpeq_epu8_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint16_t>) {
        return &GENERATE_AVX512_FUNC(_mm512_cmpeq_epu16_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint32_t>) {
        return &GENERATE_AVX512_FUNC(_mm512_cmpeq_epu32_mask);
      }

      if constexpr (std::is_same_v<KeyT, uint64_t>) {
        return &GENERATE_AVX512_FUNC(_mm512_cmpeq_epu64_mask);
      }

      FAIL("Unsupported key");
    }

    FAIL("Unsupported SIMD size");
#else
    FAIL("AVX512 is not supported on this system");
#endif
  }

  constexpr static auto vector_compare_func() {
    if constexpr (use_avx512_features) {
      return avx512_vector_compare_func();
    } else {
      return non_avx512_vector_compare_func();
    }
  }

  using VectorCompFuncT = decltype(vector_compare_func());
  constexpr static VectorCompFuncT vector_cmp_func_ = vector_compare_func();

  constexpr static auto vector_broadcast_func() {
    if constexpr (simd_size == 128) {
      if constexpr (std::is_same_v<KeyT, uint8_t> || std::is_same_v<KeyT, int8_t>) {
        // unsigned equality can use the same function as signed equality
        return &_mm_set1_epi8;
      }

      if constexpr (std::is_same_v<KeyT, uint16_t> || std::is_same_v<KeyT, int16_t>) {
        return &_mm_set1_epi16;
      }

      if constexpr (std::is_same_v<KeyT, uint32_t> || std::is_same_v<KeyT, int32_t>) {
        return &_mm_set1_epi32;
      }

      if constexpr (std::is_same_v<KeyT, uint64_t> || std::is_same_v<KeyT, int64_t>) {
        return &_mm_set1_epi64x;
      }

      FAIL("unsupported key for 128 bit SIMD");

    } else if constexpr (simd_size == 256) {
      if constexpr (std::is_same_v<KeyT, uint8_t> || std::is_same_v<KeyT, int8_t>) {
        // unsigned equality can use the same function as signed equality
        return &_mm256_set1_epi8;
      }

      if constexpr (std::is_same_v<KeyT, uint16_t> || std::is_same_v<KeyT, int16_t>) {
        return &_mm256_set1_epi16;
      }

      if constexpr (std::is_same_v<KeyT, uint32_t> || std::is_same_v<KeyT, int32_t>) {
        return &_mm256_set1_epi32;
      }

      if constexpr (std::is_same_v<KeyT, uint64_t> || std::is_same_v<KeyT, int64_t>) {
        return &_mm256_set1_epi64x;
      }

      FAIL("unsupported key for 256 bit SIMD");
    } else {
      if constexpr (std::is_same_v<KeyT, uint8_t> || std::is_same_v<KeyT, int8_t>) {
        // unsigned equality can use the same function as signed equality
        return &_mm512_set1_epi8;
      }

      if constexpr (std::is_same_v<KeyT, uint16_t> || std::is_same_v<KeyT, int16_t>) {
        return &_mm512_set1_epi16;
      }

      if constexpr (std::is_same_v<KeyT, uint32_t> || std::is_same_v<KeyT, int32_t>) {
        return &_mm512_set1_epi32;
      }

      if constexpr (std::is_same_v<KeyT, uint64_t> || std::is_same_v<KeyT, int64_t>) {
        return &_mm512_set1_epi64;
      }

      FAIL("unsupported key for 512 bit SIMD");
    }
  }

  using VectorBroadcastFuncT = decltype(vector_broadcast_func());
  constexpr static VectorBroadcastFuncT vector_broadcast_func_ = vector_broadcast_func();

  using CompareT = vector_type;

  /* Variante 1: Check if any nonzero, falls ja get first index */
  using MaskType = decltype(vector_cmp_func_(std::declval<vector_type>(), std::declval<vector_type>()));
  typedef typename std::conditional<use_avx512_features, MaskType, vector_type>::type MaskOrVectorInputType;

  HEDLEY_ALWAYS_INLINE static bool non_avx512_vector_any_nonzero(vector_type vector) {
    if constexpr (simd_size == 128) {
      return !_mm_testz_si128(vector, vector);
    } else if constexpr (simd_size == 256) {
      return !_mm256_testz_si256(vector, vector);
    } else {
      FAIL("Unsupported SIMD size");
      return false;
    }
  }

  HEDLEY_ALWAYS_INLINE static bool avx512_vector_any_nonzero(MaskType mask) {
    return mask > 0;
    /* This could probably just be > 0?
    if constexpr (std::is_same_v<MaskType, __mmask8>) {
      return !_ktestz_mask8_u8(mask, mask);
    } else if constexpr (std::is_same_v<MaskType, __mmask16>) {
      return !_ktestz_mask16_u8(mask, mask);
    } else if constexpr (std::is_same_v<MaskType, __mmask32>) {
      return !_ktestz_mask32_u8(mask, mask);
    } else if constexpr (std::is_same_v<MaskType, __mmask64>) {
      return !_ktestz_mask64_u8(mask, mask);
    } else {
      FAIL("Error while parsing AVX-512 mask type");
      return false;
    } */
  }

  HEDLEY_ALWAYS_INLINE static bool vector_any_nonzero(MaskOrVectorInputType input) {
    if constexpr (use_avx512_features) {
      return avx512_vector_any_nonzero(input);
    } else {
      return non_avx512_vector_any_nonzero(input);
    }
  }

  // vector should be the result of a compare function, i.e., it is either fully 1 or fully 0 bits per element
  HEDLEY_ALWAYS_INLINE static uint32_t non_avx512_vector_first_index(vector_type vector) {
    uint32_t mask = 0;
    if constexpr (simd_size == 128) {
      mask = _mm_movemask_epi8(vector);
      // creates a mask where the LSB corresponds to the zeroth element in the vector
    } else if constexpr (simd_size == 256) {
      mask = _mm256_movemask_epi8(vector);
    } else {
      FAIL("simd size not supported");
    }

    // tzcnt counts the number tof trailing zeros, which is what we need as LSB corresponds to index 01
    return _tzcnt_u32(mask) / static_cast<uint32_t>(sizeof(KeyT));
  }

  HEDLEY_ALWAYS_INLINE static uint32_t avx512_vector_first_index(MaskType mask) {
    // We need to decide which tzcnt to use
    if constexpr (sizeof(MaskType) * 8 > 32) {
      return static_cast<uint32_t>(_tzcnt_u64(mask));
    } else {
      return _tzcnt_u32(mask);
    }
  }

  HEDLEY_ALWAYS_INLINE static uint32_t vector_first_index(MaskOrVectorInputType input) {
    if constexpr (!use_avx512_features) {
      return non_avx512_vector_first_index(input);
    } else {
      return avx512_vector_first_index(input);
    }
  }

  constexpr static uint32_t calc_invalid_vector_index() {
    if constexpr (!use_avx512_features) {
      // for non avx-512, the mask inputted into tzcnt is always for uint8_ts, hence we need to re-scale.
      return 32 / static_cast<uint32_t>(sizeof(KeyT));
    } else {
      // for avx-512, u32 returns 32 if nothing is found, and u64 64. we don't need to scale thta because the ops that generate the mask hide that
      if constexpr (sizeof(MaskType) * 8 > 32) {
        return 64;
      } else {
        return 32;
      }
    }
  }

  HEDLEY_ALWAYS_INLINE static uint32_t vector_manual_first_index(
      vector_type vector, KeyT needle, std::vector<KeyT, hashmap::utils::AlignedAllocator<KeyT, _vector_alignment()> >* tmp_vector) {
    DEBUG_ASSERT(simd_algo == SIMDAlgorithm::MANUAL_ON_MATCH, "Using manual first index with wrong simd_algo!");
    constexpr static uint8_t keys_per_vector = (simd_size / 8) / sizeof(KeyT);

    vector_store_func_(reinterpret_cast<LoadPtrT>(tmp_vector->data()), vector);

    for (uint8_t i = 0; i < keys_per_vector; ++i) {
      if ((*tmp_vector)[i] == needle) {
        return i;
      }
    }

    return keys_per_vector;
  }

  class CompareResultIterator {
   public:
    typedef typename std::conditional<use_avx512_features && (sizeof(MaskType) * 8 > 32), uint64_t, uint32_t>::type AVX512MaskIntType;
    typedef typename std::conditional<use_avx512_features, AVX512MaskIntType, uint32_t>::type MaskIteratorT;

    HEDLEY_ALWAYS_INLINE static MaskIteratorT initialize(const MaskOrVectorInputType& compare_result) {
      if constexpr (use_avx512_features) {
        return init_avx512_mask(compare_result);
      } else {
        return init_nonavx512_mask(compare_result);
      }
    }

    HEDLEY_ALWAYS_INLINE static bool has_next(MaskIteratorT it) {
      if constexpr (use_avx512_features) {
        return avx512_has_next(it);
      } else {
        return nonavx512_has_next(it);
      }
    }

    HEDLEY_ALWAYS_INLINE static uint16_t next_match(MaskIteratorT it) {
      if constexpr (use_avx512_features) {
        return avx512_next_match(it);
      } else {
        return nonavx512_next_match(it);
      }
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT next_it(MaskIteratorT it, uint32_t last_match) {
      if constexpr (use_avx512_features) {
        return avx512_next_it(it, last_match);
      } else {
        return nonavx512_next_it(it, last_match);
      }
    }

   private:
    HEDLEY_ALWAYS_INLINE static MaskIteratorT init_nonavx512_mask(const vector_type& compare_result) {
      if constexpr (simd_size == 128) {
        return _mm_movemask_epi8(compare_result);
      } else if constexpr (simd_size == 256) {
        return _mm256_movemask_epi8(compare_result);
      } else {
        FAIL("simd size not supported");
      }
    }

    HEDLEY_ALWAYS_INLINE static bool nonavx512_has_next(MaskIteratorT nonavx512_mask) { return nonavx512_mask != 0; }

    HEDLEY_ALWAYS_INLINE static uint16_t nonavx512_next_match(MaskIteratorT nonavx512_mask) {
      return static_cast<uint16_t>(_tzcnt_u32(nonavx512_mask)) / static_cast<uint16_t>(sizeof(KeyT));
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT nonavx512_next_it(MaskIteratorT nonavx512_mask, uint32_t /*last_match*/) {
// If the right most bit is 1, this sets it to 0
// If it is not, we "ripple-carry" through out bitmask to ensure that we set the last 1 to 0
#pragma GCC unroll 20
      for (uint16_t i = 0; i < key_size_; ++i) {
        nonavx512_mask &= nonavx512_mask - 1;
      }
      return nonavx512_mask;
    }

    /* AVX-512 STUFF */

    HEDLEY_ALWAYS_INLINE static MaskIteratorT init_avx512_mask(const MaskType& compare_result) { return compare_result; }

    HEDLEY_ALWAYS_INLINE static bool avx512_has_next(MaskIteratorT avx512_mask) { return avx512_mask != 0; }
    HEDLEY_ALWAYS_INLINE static uint16_t avx512_next_match(MaskIteratorT avx512_mask) {
      uint16_t next_index = 0;
      if constexpr (sizeof(MaskType) * 8 > 32) {
        next_index = static_cast<uint16_t>(_tzcnt_u64(avx512_mask));
      } else {
        next_index = static_cast<uint16_t>(_tzcnt_u32(avx512_mask));
      }

      return next_index;
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT avx512_next_it(MaskIteratorT avx512_mask, uint32_t /*last_match*/) {
      avx512_mask &= avx512_mask - 1;
      return avx512_mask;
    }

   private:
    constexpr static uint16_t key_size_ = static_cast<uint16_t>(sizeof(KeyT));
  };

#endif  // HASHMAP_IS_x86

#ifdef HASHMAP_IS_ARM

  // clang-format off
  // TODO(MaxiBoether): Signed Keys

#ifdef __ARM_FEATURE_SVE
  using _sve_vector_type = typename std::conditional<std::is_same_v<KeyT, uint8_t>,
                          svuint8_t,
                          typename std::conditional<std::is_same_v<KeyT, uint16_t>,
                                                    svuint16_t,
                                                    typename std::conditional<
                                                                              std::is_same_v<KeyT, uint32_t>,
                                                                              svuint32_t,
                                                                              typename std::conditional<
                                                                                                        std::is_same_v<KeyT, uint64_t>,
                                                                                                        svuint64_t,
                                                                                                        void
                                                                                                        >::type
                                                                              >::type
                                                    >::type
                      >::type;
#else
  using _sve_vector_type = void;
#endif

  using _vector_type = typename std::conditional<use_sve, _sve_vector_type, typename std::conditional<std::is_same_v<KeyT, uint8_t>,
                            uint8x16_t,
                            typename std::conditional<std::is_same_v<KeyT, uint16_t>,
                                                      uint16x8_t,
                                                      typename std::conditional<
                                                                                std::is_same_v<KeyT, uint32_t>,
                                                                                uint32x4_t,
                                                                                typename std::conditional<
                                                                                                          std::is_same_v<KeyT, uint64_t>,
                                                                                                          uint64x2_t,
                                                                                                          void
                                                                                                         >::type
                                                                               >::type
                                                     >::type
                        >::type>::type;
  // clang-format on

  constexpr static uint16_t _vector_alignment() {
    if (use_sve) {
      if constexpr (hashmap::utils::sve_register_size == 128) {
        return 16;
      } else if constexpr (hashmap::utils::sve_register_size == 256) {
        return 32;
      } else if constexpr (hashmap::utils::sve_register_size == 512) {
        return 64;
      } else if constexpr (hashmap::utils::sve_register_size == 1024) {
        return 128;
      } else if constexpr (hashmap::utils::sve_register_size == 2048) {
        return 256;
      }
      return 16;
    } else {
      return 16;
    }
  }

#ifdef HEDLEY_GNUC_VERSION
// clang/gcc
#ifndef HEDLEY_GCC_VERSION
  // clang
  using _aligned_vector_type __attribute__((aligned(_vector_alignment()))) = _vector_type;
#else
  // gcc
  // The following is not well-defined in the standard and hence clang complains, but gcc doesn't
  // See update to https://stackoverflow.com/a/15912208 for a discussion
  using _aligned_vector_type alignas(_vector_alignment()) = _vector_type;

#endif  // HEDLEY_GCC_VERSION
#else
  // any other compiler - not sure what works here, using gcc version for now
  using _aligned_vector_type alignas(_vector_alignment()) = _vector_type;
#endif

  // calling alignment on sve vectors leads to stack overflows and weird behavior, it is not intended
  using vector_type = typename std::conditional<use_sve, _vector_type, _aligned_vector_type>::type;

#ifdef CLANG_COMPILER
// On clang, several NEON/SVE intrinsics are actually macros - hence we have to write wrappers around all of them
#define GENERATE_ARM_FUNC(NAM) clang_##NAM
  HEDLEY_ALWAYS_INLINE static uint8x16_t clang_vld1q_u8(const uint8_t* source) { return vld1q_u8(source); }
  HEDLEY_ALWAYS_INLINE static uint16x8_t clang_vld1q_u16(const uint16_t* source) { return vld1q_u16(source); }
  HEDLEY_ALWAYS_INLINE static uint32x4_t clang_vld1q_u32(const uint32_t* source) { return vld1q_u32(source); }
  HEDLEY_ALWAYS_INLINE static uint64x2_t clang_vld1q_u64(const uint64_t* source) { return vld1q_u64(source); }

  HEDLEY_ALWAYS_INLINE static void clang_vst1q_u8(uint8_t* dest, uint8x16_t data) { return vst1q_u8(dest, data); }
  HEDLEY_ALWAYS_INLINE static void clang_vst1q_u16(uint16_t* dest, uint16x8_t data) { return vst1q_u16(dest, data); }
  HEDLEY_ALWAYS_INLINE static void clang_vst1q_u32(uint32_t* dest, uint32x4_t data) { return vst1q_u32(dest, data); }
  HEDLEY_ALWAYS_INLINE static void clang_vst1q_u64(uint64_t* dest, uint64x2_t data) { return vst1q_u64(dest, data); }

#else
#define GENERATE_ARM_FUNC(NAM) NAM
#endif

  constexpr static auto non_sve_vector_load_func() {
    // todo signed

    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      // unsigned equality can use the same function as signed equality
      return &GENERATE_ARM_FUNC(vld1q_u8);
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &GENERATE_ARM_FUNC(vld1q_u16);
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &GENERATE_ARM_FUNC(vld1q_u32);
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &GENERATE_ARM_FUNC(vld1q_u64);
    }

    FAIL("Unsupported key");
  }

  constexpr static auto non_sve_vector_store_func() {
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      // unsigned equality can use the same function as signed equality
      return &GENERATE_ARM_FUNC(vst1q_u8);
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &GENERATE_ARM_FUNC(vst1q_u16);
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &GENERATE_ARM_FUNC(vst1q_u32);
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &GENERATE_ARM_FUNC(vst1q_u64);
    }

    FAIL("Unsupported key");
  }

#ifdef __ARM_FEATURE_SVE

  HEDLEY_ALWAYS_INLINE static svuint8_t sve_load_uint8_t(const uint8_t* source) { return svld1_u8(svptrue_b8(), source); }
  HEDLEY_ALWAYS_INLINE static svuint16_t sve_load_uint16_t(const uint16_t* source) { return svld1_u16(svptrue_b16(), source); }
  HEDLEY_ALWAYS_INLINE static svuint32_t sve_load_uint32_t(const uint32_t* source) { return svld1_u32(svptrue_b32(), source); }
  HEDLEY_ALWAYS_INLINE static svuint64_t sve_load_uint64_t(const uint64_t* source) { return svld1_u64(svptrue_b64(), source); }

  constexpr static auto sve_vector_load_func() {
    // todo signed

    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return &sve_load_uint8_t;
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &sve_load_uint16_t;
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &sve_load_uint32_t;
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &sve_load_uint64_t;
    }

    FAIL("Unsupported key");
  }

  HEDLEY_ALWAYS_INLINE static void sve_store_uint8_t(uint8_t* dest, svuint8_t data) {
    DEBUG_ASSERT(dest != nullptr, "Cannot handle nullptr");
    return svst1_u8(svptrue_b8(), dest, data);
  }
  HEDLEY_ALWAYS_INLINE static void sve_store_uint16_t(uint16_t* dest, svuint16_t data) {
    DEBUG_ASSERT(dest != nullptr, "Cannot handle nullptr");
    return svst1_u16(svptrue_b16(), dest, data);
  }
  HEDLEY_ALWAYS_INLINE static void sve_store_uint32_t(uint32_t* dest, svuint32_t data) {
    DEBUG_ASSERT(dest != nullptr, "Cannot handle nullptr");
    return svst1_u32(svptrue_b32(), dest, data);
  }
  HEDLEY_ALWAYS_INLINE static void sve_store_uint64_t(uint64_t* dest, svuint64_t data) {
    DEBUG_ASSERT(dest != nullptr, "Cannot handle nullptr");
    return svst1_u64(svptrue_b64(), dest, data);
  }

  constexpr static auto sve_vector_store_func() {
    // todo signed

    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return &sve_store_uint8_t;
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &sve_store_uint16_t;
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &sve_store_uint32_t;
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &sve_store_uint64_t;
    }

    FAIL("Unsupported key");
  }

#else
  constexpr static auto sve_vector_load_func() { return non_sve_vector_load_func(); }

#endif

  constexpr static auto vector_load_func() {
    if constexpr (use_sve) {
      return sve_vector_load_func();
    } else {
      return non_sve_vector_load_func();
    }
  }

  using VectorLoadFuncT = decltype(vector_load_func());
  constexpr static VectorLoadFuncT vector_load_func_ = vector_load_func();

  using LoadPtrT = KeyT*;

  constexpr static auto vector_unaligned_load_func() { return vector_load_func(); }

  using VectorUnalignedLoadFuncT = decltype(vector_unaligned_load_func());
  constexpr static VectorLoadFuncT vector_unaligned_load_func_ = vector_unaligned_load_func();

  constexpr static auto vector_store_func() {
    if constexpr (use_sve) {
      return sve_vector_store_func();
    } else {
      return non_sve_vector_store_func();
    }
  }

  using VectorStoreFuncT = decltype(vector_store_func());
  constexpr static VectorStoreFuncT vector_store_func_ = vector_store_func();

  constexpr static auto non_sve_vector_compare_func() {
    // todo signed
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      // unsigned equality can use the same function as signed equality
      return &vceqq_u8;
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &vceqq_u16;
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &vceqq_u32;
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &vceqq_u64;
    }

    FAIL("Unsupported key");
  }

  constexpr static auto non_sve_vector_broadcast_func() {
    // TODO: signed keys
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return &vdupq_n_u8;
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &vdupq_n_u16;
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &vdupq_n_u32;
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &vdupq_n_u64;
    }
  }

  HEDLEY_ALWAYS_INLINE static uint8_t noop_u8(uint8_t val) {
    uint8_t copy = val;
    return copy;
  }
  HEDLEY_ALWAYS_INLINE static uint16_t noop_u16(uint16_t val) {
    uint16_t copy = val;
    return copy;
  }
  HEDLEY_ALWAYS_INLINE static uint32_t noop_u32(uint32_t val) {
    uint32_t copy = val;
    return copy;
  }
  HEDLEY_ALWAYS_INLINE static uint64_t noop_u64(uint64_t val) {
    uint64_t copy = val;
    return copy;
  }

#ifdef CLANG_COMPILER
  // On clang, several NEON/SVE intrinsics are actually macros - hence we have to write wrappers around all of them
  HEDLEY_ALWAYS_INLINE static svuint8_t clang_svdup_u8(uint8_t op) { return svdup_u8(op); }
  HEDLEY_ALWAYS_INLINE static svuint16_t clang_svdup_u16(uint16_t op) { return svdup_u16(op); }
  HEDLEY_ALWAYS_INLINE static svuint32_t clang_svdup_u32(uint32_t op) { return svdup_u32(op); }
  HEDLEY_ALWAYS_INLINE static svuint64_t clang_svdup_u64(uint64_t op) { return svdup_u64(op); }
#endif

  constexpr static auto sve_vector_broadcast_func() {
    // TODO: signed keys

    if constexpr (sve_scalar_broadcast) {
      if constexpr (std::is_same_v<KeyT, uint8_t>) {
        return &GENERATE_ARM_FUNC(svdup_u8);
      }

      if constexpr (std::is_same_v<KeyT, uint16_t>) {
        return &GENERATE_ARM_FUNC(svdup_u16);
      }

      if constexpr (std::is_same_v<KeyT, uint32_t>) {
        return &GENERATE_ARM_FUNC(svdup_u32);
      }

      if constexpr (std::is_same_v<KeyT, uint64_t>) {
        return &GENERATE_ARM_FUNC(svdup_u64);
      }
    } else {
      if constexpr (std::is_same_v<KeyT, uint8_t>) {
        return &noop_u8;
      }

      if constexpr (std::is_same_v<KeyT, uint16_t>) {
        return &noop_u16;
      }

      if constexpr (std::is_same_v<KeyT, uint32_t>) {
        return &noop_u32;
      }

      if constexpr (std::is_same_v<KeyT, uint64_t>) {
        return &noop_u64;
      }
    }
  }

  constexpr static auto vector_broadcast_func() {
    if constexpr (use_sve) {
      return sve_vector_broadcast_func();
    } else {
      return non_sve_vector_broadcast_func();
    }
  }

  using VectorBroadcastFuncT = decltype(vector_broadcast_func());
  constexpr static VectorBroadcastFuncT vector_broadcast_func_ = vector_broadcast_func();

  using CompareT = decltype(vector_broadcast_func_(std::declval<KeyT>()));

#ifdef __ARM_FEATURE_SVE

  HEDLEY_ALWAYS_INLINE static svbool_t sve_cmp_uint8_t(svuint8_t vector, uint8_t scalar) { return svcmpeq_n_u8(svptrue_b8(), vector, scalar); }
  HEDLEY_ALWAYS_INLINE static svbool_t sve_cmp_uint16_t(svuint16_t vector, uint16_t scalar) { return svcmpeq_n_u16(svptrue_b16(), vector, scalar); }
  HEDLEY_ALWAYS_INLINE static svbool_t sve_cmp_uint32_t(svuint32_t vector, uint32_t scalar) { return svcmpeq_n_u32(svptrue_b32(), vector, scalar); }
  HEDLEY_ALWAYS_INLINE static svbool_t sve_cmp_uint64_t(svuint64_t vector, uint64_t scalar) { return svcmpeq_n_u64(svptrue_b64(), vector, scalar); }

  HEDLEY_ALWAYS_INLINE static svbool_t sve_veccmp_uint8_t(svuint8_t vector, svuint8_t cmp_vec) { return svcmpeq_u8(svptrue_b8(), vector, cmp_vec); }
  HEDLEY_ALWAYS_INLINE static svbool_t sve_veccmp_uint16_t(svuint16_t vector, svuint16_t cmp_vec) {
    return svcmpeq_u16(svptrue_b16(), vector, cmp_vec);
  }
  HEDLEY_ALWAYS_INLINE static svbool_t sve_veccmp_uint32_t(svuint32_t vector, svuint32_t cmp_vec) {
    return svcmpeq_u32(svptrue_b32(), vector, cmp_vec);
  }
  HEDLEY_ALWAYS_INLINE static svbool_t sve_veccmp_uint64_t(svuint64_t vector, svuint64_t cmp_vec) {
    return svcmpeq_u64(svptrue_b64(), vector, cmp_vec);
  }

  constexpr static auto sve_vector_compare_func() {
    // todo signed
    if constexpr (sve_scalar_broadcast) {
      if constexpr (std::is_same_v<KeyT, uint8_t>) {
        return &sve_veccmp_uint8_t;
      }

      if constexpr (std::is_same_v<KeyT, uint16_t>) {
        return &sve_veccmp_uint16_t;
      }

      if constexpr (std::is_same_v<KeyT, uint32_t>) {
        return &sve_veccmp_uint32_t;
      }

      if constexpr (std::is_same_v<KeyT, uint64_t>) {
        return &sve_veccmp_uint64_t;
      }

      FAIL("Unsupported key");
    } else {
      if constexpr (std::is_same_v<KeyT, uint8_t>) {
        return &sve_cmp_uint8_t;
      }

      if constexpr (std::is_same_v<KeyT, uint16_t>) {
        return &sve_cmp_uint16_t;
      }

      if constexpr (std::is_same_v<KeyT, uint32_t>) {
        return &sve_cmp_uint32_t;
      }

      if constexpr (std::is_same_v<KeyT, uint64_t>) {
        return &sve_cmp_uint64_t;
      }

      FAIL("Unsupported key");
    }
  }
#else
  constexpr static auto sve_vector_compare_func() { return non_sve_vector_compare_func(); }
#endif

  constexpr static auto vector_compare_func() {
    if constexpr (use_sve) {
      return sve_vector_compare_func();
    } else {
      return non_sve_vector_compare_func();
    }
  }

  using VectorCompFuncT = decltype(vector_compare_func());
  constexpr static VectorCompFuncT vector_cmp_func_ = vector_compare_func();

  using MaskType = decltype(vector_cmp_func_(std::declval<vector_type>(), std::declval<CompareT>()));
  typedef typename std::conditional<use_sve, svbool_t, vector_type>::type MaskOrVectorInputType;

  // adapted from https://github.com/DLTcollab/sse2neon/blob/master/sse2neon.h
  HEDLEY_ALWAYS_INLINE static uint32_t movemask_sse2neon(uint8x16_t input) {
    uint16x8_t high_bits = vreinterpretq_u16_u8(vshrq_n_u8(input, 7));
    uint32x4_t paired16 = vreinterpretq_u32_u16(vsraq_n_u16(high_bits, high_bits, 7));
    uint64x2_t paired32 = vreinterpretq_u64_u32(vsraq_n_u32(paired16, paired16, 14));
    uint8x16_t paired64 = vreinterpretq_u8_u64(vsraq_n_u64(paired32, paired32, 28));

    return vgetq_lane_u8(paired64, 0) | ((int)vgetq_lane_u8(paired64, 8) << 8);
  }

  // adapted from https://stackoverflow.com/a/68694558
  HEDLEY_ALWAYS_INLINE static uint32_t movemask_aarch64(uint8x16_t input) {
    const int8_t __attribute__((aligned(16))) ucShift[] = {-7, -6, -5, -4, -3, -2, -1, 0, -7, -6, -5, -4, -3, -2, -1, 0};
    uint8x16_t vshift = vreinterpretq_u8_s8(vld1q_s8(ucShift));
    uint8x16_t vmask = vandq_u8(input, vdupq_n_u8(0x80));
    uint32_t out;

    vmask = vshlq_u8(vmask, vreinterpretq_s8_u8(vshift));
    out = vaddv_u8(vget_low_u8(vmask));
    out += (vaddv_u8(vget_high_u8(vmask)) << 8);

    return out;
  }

  constexpr static auto non_sve_vector_and_func() {
    // TODO: signed keys
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return &vandq_u8;
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &vandq_u16;
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &vandq_u32;
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &vandq_u64;
    }
  }

  using VectorAndFuncT = decltype(non_sve_vector_and_func());
  constexpr static VectorAndFuncT vector_and_func_ = non_sve_vector_and_func();

  HEDLEY_ALWAYS_INLINE static uint64_t vminvq_u64(uint64x2_t vec) {
    uint64_t x1 = vgetq_lane_u64(vec, 0);
    uint64_t x2 = vgetq_lane_u64(vec, 1);

    if (x1 > x2) {
      return x2;
    }

    return x1;
  }

  constexpr static auto non_sve_vector_min_func() {
    // TODO: signed keys
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return &vminvq_u8;
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &vminvq_u16;
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &vminvq_u32;
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &vminvq_u64;
    }
  }

  using VectorMinFuncT = decltype(non_sve_vector_min_func());
  constexpr static VectorMinFuncT vector_min_func_ = non_sve_vector_min_func();

  constexpr static auto non_sve_vector_bsl_func() {
    // TODO: signed keys
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return &vbslq_u8;
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &vbslq_u16;
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &vbslq_u32;
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &vbslq_u64;
    }
  }

  using VectorBslFuncT = decltype(non_sve_vector_bsl_func());
  constexpr static VectorBslFuncT vector_bsl_func_ = non_sve_vector_bsl_func();

  constexpr static vector_type non_sve_create_index_vector() {
    // TODO: signed keys
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return vector_type{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return vector_type{0, 1, 2, 3, 4, 5, 6, 7};
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return vector_type{0, 1, 2, 3};
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return vector_type{0, 1};
    }
  }

  // vector should be the result of a compare function, i.e., it is either fully 1 or fully 0 bits per element
  HEDLEY_ALWAYS_INLINE static uint32_t non_sve_vector_first_index(vector_type vector) {
    if constexpr (neon_algo == NEONAlgo::UMINV) {
      static const vector_type index_vec = non_sve_create_index_vector();
      // vector_type and_result = vector_and_func_(index_vec, vector);
      vector_type bsl_result = vector_bsl_func_(vector, index_vec, vector_broadcast_func_(calc_invalid_vector_index()));

      uint32_t min_index = static_cast<uint32_t>(vector_min_func_(bsl_result));
      return min_index;

    } else {
      uint32_t mask = 0;

      if constexpr (neon_algo == NEONAlgo::AARCH64) {
        mask = movemask_aarch64(to_uint8_vec(vector));
      } else {
        mask = movemask_sse2neon(to_uint8_vec(vector));
      }

      // __builtin_ctz does not work with all zero mask
      if (mask == 0) {
        return calc_invalid_vector_index();
      }

      return __builtin_ctz(mask) / static_cast<uint32_t>(sizeof(KeyT));
    }
  }

  constexpr static uint32_t non_sve_calc_invalid_vector_index() { return 32 / static_cast<uint32_t>(sizeof(KeyT)); }

#ifdef __ARM_FEATURE_SVE

  HEDLEY_ALWAYS_INLINE static svuint8_t sve_index_uint8() { return svindex_u8(0, 1); }
  HEDLEY_ALWAYS_INLINE static svuint16_t sve_index_uint16() { return svindex_u16(0, 1); }
  HEDLEY_ALWAYS_INLINE static svuint32_t sve_index_uint32() { return svindex_u32(0, 1); }
  HEDLEY_ALWAYS_INLINE static svuint64_t sve_index_uint64() { return svindex_u64(0, 1); }

  constexpr static auto sve_index_creation_func() {
    // TODO: signed keys
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return &sve_index_uint8;
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return &sve_index_uint16;
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return &sve_index_uint32;
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return &sve_index_uint64;
    }
  }

  using IndexCreationFuncT = decltype(sve_index_creation_func());
  constexpr static IndexCreationFuncT index_creation_func_ = sve_index_creation_func();

  constexpr static uint32_t sve_calc_invalid_vector_index() { return (hashmap::utils::sve_register_size / 8) / sizeof(KeyT); }

  HEDLEY_ALWAYS_INLINE static uint64_t sve_vector_first_index(svbool_t mask, vector_type index_vector) {
    mask = svbrka_b_z(mask, mask);  // makes sure that we only keep the first match in case there are multiple

    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return svclastb_n_u8(mask, static_cast<uint8_t>(sve_calc_invalid_vector_index()), index_vector);
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return svclastb_n_u16(mask, static_cast<uint16_t>(sve_calc_invalid_vector_index()), index_vector);
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return svclastb_n_u32(mask, static_cast<uint32_t>(sve_calc_invalid_vector_index()), index_vector);
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return svclastb_n_u64(mask, static_cast<uint64_t>(sve_calc_invalid_vector_index()), index_vector);
    }

    FAIL("Unsupported key");
  }

#else
  HEDLEY_ALWAYS_INLINE static uint32_t sve_vector_first_index(vector_type vector, vector_type index_vector) { return calc_invalid_vector_index(); }
  constexpr static uint32_t sve_calc_invalid_vector_index() { return non_sve_calc_invalid_vector_index(); }
#endif

  constexpr static uint32_t calc_invalid_vector_index() {
    if constexpr (use_sve) {
      return sve_calc_invalid_vector_index();
    } else {
      return non_sve_calc_invalid_vector_index();
    }
  }

  HEDLEY_ALWAYS_INLINE static uint32_t vector_first_index(MaskOrVectorInputType input) {
    if constexpr (use_sve) {
      static_assert(value<false, KeyT>, "On SVE, you need to call sve_vector_first_index(input, index_vector) directly.");
      FAIL("On SVE, you need to call sve_vector_first_index(input, index_vector) directly.");
    } else {
      return non_sve_vector_first_index(input);
    }
  }

  HEDLEY_ALWAYS_INLINE static bool non_sve_vector_any_nonzero(vector_type input) {
    // Idea: 1. Reinterpret vector_type as uint64x2_t (if necessary)
    // 2. Our 128-bit register now contains two 64-bit uints. Only if both of them are zero, there is no zero bit
    uint64x2_t reinterpreted;

    if constexpr (std::is_same_v<_vector_type, uint64x2_t>) {
      reinterpreted = input;
    } else {
      if constexpr (std::is_same_v<_vector_type, uint8x16_t>) {
        reinterpreted = vreinterpretq_u64_u8(input);
      } else if constexpr (std::is_same_v<_vector_type, uint16x8_t>) {
        reinterpreted = vreinterpretq_u64_u16(input);
      } else if constexpr (std::is_same_v<_vector_type, uint32x4_t>) {
        reinterpreted = vreinterpretq_u64_u32(input);
      } else {
        FAIL("Unsupported vector type in conversion");
      }
    }

    return vgetq_lane_u64(reinterpreted, 0) | vgetq_lane_u64(reinterpreted, 1);
  }

#ifdef __ARM_FEATURE_SVE
  HEDLEY_ALWAYS_INLINE static bool sve_vector_any_nonzero(svbool_t mask) {
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return svptest_any(svptrue_b8(), mask);
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return svptest_any(svptrue_b16(), mask);
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return svptest_any(svptrue_b32(), mask);
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return svptest_any(svptrue_b64(), mask);
    }

    FAIL("Unsupported key");
  }

#else
  HEDLEY_ALWAYS_INLINE static bool sve_vector_any_nonzero(vector_type vector) { return false; }
#endif

  HEDLEY_ALWAYS_INLINE static bool vector_any_nonzero(MaskOrVectorInputType input) {
    if constexpr (use_sve) {
      return sve_vector_any_nonzero(input);
    } else {
      return non_sve_vector_any_nonzero(input);
    }
  }

  HEDLEY_ALWAYS_INLINE static uint8x16_t to_uint8_vec(vector_type vec) {
    if constexpr (use_sve) {
      FAIL("This should not be called if using SVE.");
    }

    if constexpr (std::is_same_v<_vector_type, uint8x16_t>) {
      return vec;
    } else if constexpr (std::is_same_v<_vector_type, uint16x8_t>) {
      return vreinterpretq_u8_u16(vec);
    } else if constexpr (std::is_same_v<_vector_type, uint32x4_t>) {
      return vreinterpretq_u8_u32(vec);
    } else if constexpr (std::is_same_v<_vector_type, uint64x2_t>) {
      return vreinterpretq_u8_u64(vec);
    } else {
      // todo: signed int types
    }
  }

  HEDLEY_ALWAYS_INLINE static KeyT non_sve_get_index(vector_type vector, const uint8_t index) {
    // This looks absurd, but vgetq_lane requires a compile time constant index
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      switch (index) {
        case 0:
          return vgetq_lane_u8(vector, 0);
          break;
        case 1:
          return vgetq_lane_u8(vector, 1);
          break;
        case 2:
          return vgetq_lane_u8(vector, 2);
          break;
        case 3:
          return vgetq_lane_u8(vector, 3);
          break;
        case 4:
          return vgetq_lane_u8(vector, 4);
          break;
        case 5:
          return vgetq_lane_u8(vector, 5);
          break;
        case 6:
          return vgetq_lane_u8(vector, 6);
          break;
        case 7:
          return vgetq_lane_u8(vector, 7);
          break;
        case 8:
          return vgetq_lane_u8(vector, 8);
          break;
        case 9:
          return vgetq_lane_u8(vector, 9);
          break;
        case 10:
          return vgetq_lane_u8(vector, 10);
          break;
        case 11:
          return vgetq_lane_u8(vector, 11);
          break;
        case 12:
          return vgetq_lane_u8(vector, 12);
          break;
        case 13:
          return vgetq_lane_u8(vector, 13);
          break;
        case 14:
          return vgetq_lane_u8(vector, 14);
          break;
        case 15:
          return vgetq_lane_u8(vector, 15);
          break;
        default:
          FAIL(fmt::format("Invalid index", index));
      }
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      switch (index) {
        case 0:
          return vgetq_lane_u16(vector, 0);
          break;
        case 1:
          return vgetq_lane_u16(vector, 1);
          break;
        case 2:
          return vgetq_lane_u16(vector, 2);
          break;
        case 3:
          return vgetq_lane_u16(vector, 3);
          break;
        case 4:
          return vgetq_lane_u16(vector, 4);
          break;
        case 5:
          return vgetq_lane_u16(vector, 5);
          break;
        case 6:
          return vgetq_lane_u16(vector, 6);
          break;
        case 7:
          return vgetq_lane_u16(vector, 7);
          break;
        default:
          FAIL(fmt::format("Invalid index", index));
      }
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      switch (index) {
        case 0:
          return vgetq_lane_u32(vector, 0);
          break;
        case 1:
          return vgetq_lane_u32(vector, 1);
          break;
        case 2:
          return vgetq_lane_u32(vector, 2);
          break;
        case 3:
          return vgetq_lane_u32(vector, 3);
          break;
        default:
          FAIL(fmt::format("Invalid index", index));
      }
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      switch (index) {
        case 0:
          return vgetq_lane_u64(vector, 0);
          break;
        case 1:
          return vgetq_lane_u64(vector, 1);
          break;
        default:
          FAIL(fmt::format("Invalid index", index));
      }
    }

    FAIL("Unsupported key");
  }

  HEDLEY_ALWAYS_INLINE static vector_type non_sve_set_index(vector_type vector, const uint8_t index, KeyT key) {
    // This looks absurd, but vsetq_lane requires a compile time constant index
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      switch (index) {
        case 0:
          return vsetq_lane_u8(key, vector, 0);
          break;
        case 1:
          return vsetq_lane_u8(key, vector, 1);
          break;
        case 2:
          return vsetq_lane_u8(key, vector, 2);
          break;
        case 3:
          return vsetq_lane_u8(key, vector, 3);
          break;
        case 4:
          return vsetq_lane_u8(key, vector, 4);
          break;
        case 5:
          return vsetq_lane_u8(key, vector, 5);
          break;
        case 6:
          return vsetq_lane_u8(key, vector, 6);
          break;
        case 7:
          return vsetq_lane_u8(key, vector, 7);
          break;
        case 8:
          return vsetq_lane_u8(key, vector, 8);
          break;
        case 9:
          return vsetq_lane_u8(key, vector, 9);
          break;
        case 10:
          return vsetq_lane_u8(key, vector, 10);
          break;
        case 11:
          return vsetq_lane_u8(key, vector, 11);
          break;
        case 12:
          return vsetq_lane_u8(key, vector, 12);
          break;
        case 13:
          return vsetq_lane_u8(key, vector, 13);
          break;
        case 14:
          return vsetq_lane_u8(key, vector, 14);
          break;
        case 15:
          return vsetq_lane_u8(key, vector, 15);
          break;
        default:
          FAIL(fmt::format("Invalid index", index));
      }
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      switch (index) {
        case 0:
          return vsetq_lane_u16(key, vector, 0);
          break;
        case 1:
          return vsetq_lane_u16(key, vector, 1);
          break;
        case 2:
          return vsetq_lane_u16(key, vector, 2);
          break;
        case 3:
          return vsetq_lane_u16(key, vector, 3);
          break;
        case 4:
          return vsetq_lane_u16(key, vector, 4);
          break;
        case 5:
          return vsetq_lane_u16(key, vector, 5);
          break;
        case 6:
          return vsetq_lane_u16(key, vector, 6);
          break;
        case 7:
          return vsetq_lane_u16(key, vector, 7);
          break;
        default:
          FAIL(fmt::format("Invalid index", index));
      }
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      switch (index) {
        case 0:
          return vsetq_lane_u32(key, vector, 0);
          break;
        case 1:
          return vsetq_lane_u32(key, vector, 1);
          break;
        case 2:
          return vsetq_lane_u32(key, vector, 2);
          break;
        case 3:
          return vsetq_lane_u32(key, vector, 3);
          break;
        default:
          FAIL(fmt::format("Invalid index", index));
      }
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      switch (index) {
        case 0:
          return vsetq_lane_u64(key, vector, 0);
          break;
        case 1:
          return vsetq_lane_u64(key, vector, 1);
          break;
        default:
          FAIL(fmt::format("Invalid index", index));
      }
    }

    FAIL("Unsupported key");
  }

  HEDLEY_ALWAYS_INLINE static uint32_t non_sve_vector_manual_first_index(
      vector_type vector, KeyT needle, std::vector<KeyT, hashmap::utils::AlignedAllocator<KeyT, _vector_alignment()> >* /*tmp_vector*/) {
    DEBUG_ASSERT(simd_algo == SIMDAlgorithm::MANUAL_ON_MATCH, "Using manual first index with wrong simd_algo!");
    constexpr static uint8_t keys_per_vector = (simd_size / 8) / sizeof(KeyT);

    for (uint8_t i = 0; i < keys_per_vector; ++i) {
      if (non_sve_get_index(vector, i) == needle) {
        return i;
      }
    }

    return keys_per_vector;
  }

#ifdef __ARM_FEATURE_SVE
  HEDLEY_ALWAYS_INLINE static uint16_t keys_per_vector() {
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return static_cast<uint16_t>(svlen_u8(svdup_u8(0)));
    }

    if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return static_cast<uint16_t>(svlen_u16(svdup_u16(0)));
    }

    if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return static_cast<uint16_t>(svlen_u32(svdup_u32(0)));
    }

    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return static_cast<uint16_t>(svlen_u64(svdup_u64(0)));
    }

    FAIL("Unsupported key");
  }

  HEDLEY_ALWAYS_INLINE static uint32_t sve_vector_manual_first_index(
      vector_type vector, KeyT needle, std::vector<KeyT, hashmap::utils::AlignedAllocator<KeyT, _vector_alignment()> >* tmp_vector) {
    DEBUG_ASSERT(simd_algo == SIMDAlgorithm::MANUAL_ON_MATCH, "Using manual first index with wrong simd_algo!");
    static uint16_t keys_per_vector = sve_calc_invalid_vector_index();

    DEBUG_ASSERT(tmp_vector != nullptr, "Cannot handle nullptr");
    DEBUG_ASSERT(tmp_vector->data() != nullptr, "Cannot handle nullptr");
    DEBUG_ASSERT(
        tmp_vector->size() == keys_per_vector,
        fmt::format(
            "Invalid vector *size* {} (should be {}). Capacity is not enough, because the SIMD instructions will not update the size information",
            tmp_vector->size(), keys_per_vector));

    vector_store_func_(tmp_vector->data(), vector);

    for (uint16_t i = 0; i < keys_per_vector; ++i) {
      if ((*tmp_vector)[i] == needle) {
        return i;
      }
    }
    return keys_per_vector;
  }
#endif

  HEDLEY_ALWAYS_INLINE static uint32_t vector_manual_first_index(
      vector_type vector, KeyT needle, std::vector<KeyT, hashmap::utils::AlignedAllocator<KeyT, _vector_alignment()> >* tmp_vector) {
    if constexpr (use_sve) {
      return sve_vector_manual_first_index(vector, needle, tmp_vector);
    } else {
      return non_sve_vector_manual_first_index(vector, needle, tmp_vector);
    }
  }

  class CompareResultIterator {
   public:
    typedef typename std::conditional<neon_algo == NEONAlgo::UMINV, vector_type, uint32_t>::type NONSVEMaskIteratorT;

#ifdef __ARM_FEATURE_SVE
    typedef typename std::conditional<use_sve, svbool_t, NONSVEMaskIteratorT>::type MaskIteratorT;
#else
    using MaskIteratorT = NONSVEMaskIteratorT;
#endif

    HEDLEY_ALWAYS_INLINE static MaskIteratorT initialize(MaskOrVectorInputType compare_result) {
      if constexpr (use_sve) {
        return init_sve_mask(compare_result);
      } else {
        return init_non_sve_mask(compare_result);
      }
    }

    HEDLEY_ALWAYS_INLINE static bool has_next(MaskIteratorT it) {
      if constexpr (use_sve) {
        return sve_has_next(it);
      } else {
        return non_sve_has_next(it);
      }
    }
    HEDLEY_ALWAYS_INLINE static uint16_t next_match(MaskIteratorT it) {
      if constexpr (use_sve) {
        FAIL("On SVE, you need to call sve_next_match(it, index_vector) directly.");
        static_assert(value<false, KeyT>, "On SVE, you need to call sve_next_match(input, index_vector) directly.");
      } else {
        return non_sve_next_match(it);
      }
    }
    HEDLEY_ALWAYS_INLINE static MaskIteratorT next_it(MaskIteratorT it, uint32_t last_match) {
      if constexpr (use_sve) {
        return sve_next_it(it, last_match);
      } else {
        return non_sve_next_it(it, last_match);
      }
    }

    HEDLEY_ALWAYS_INLINE static vector_type init_non_sve_uminv_mask(vector_type compare_result) { return compare_result; }

    HEDLEY_ALWAYS_INLINE static uint32_t init_non_sve_mm_mask(vector_type compare_result) {
      uint32_t mask = 0;

      if constexpr (neon_algo == NEONAlgo::AARCH64) {
        mask = movemask_aarch64(to_uint8_vec(compare_result));
      } else {
        mask = movemask_sse2neon(to_uint8_vec(compare_result));
      }

      return mask;
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT init_non_sve_mask(vector_type compare_result) {
      if constexpr (neon_algo == NEONAlgo::UMINV) {
        return init_non_sve_uminv_mask(compare_result);
      } else {
        return init_non_sve_mm_mask(compare_result);
      }
    }

    HEDLEY_ALWAYS_INLINE static bool non_sve_uminv_has_next(vector_type it) { return non_sve_vector_any_nonzero(it); }

    HEDLEY_ALWAYS_INLINE static bool non_sve_mm_has_next(uint32_t it) { return it != 0; }

    HEDLEY_ALWAYS_INLINE static bool non_sve_has_next(MaskIteratorT it) {
      if constexpr (neon_algo == NEONAlgo::UMINV) {
        return non_sve_uminv_has_next(it);
      } else {
        return non_sve_mm_has_next(it);
      }
    }

    HEDLEY_ALWAYS_INLINE static uint16_t non_sve_uminv_next_match(vector_type it) { return static_cast<uint16_t>(non_sve_vector_first_index(it)); }

    HEDLEY_ALWAYS_INLINE static uint16_t non_sve_mm_next_match(uint32_t it) {
      // we assume it != 0, because we have called next_match. everything else is undefined behavior (__builtin_ctz(0))
      return static_cast<uint16_t>(__builtin_ctz(it)) / static_cast<uint16_t>(sizeof(KeyT));
    }

    HEDLEY_ALWAYS_INLINE static uint16_t non_sve_next_match(MaskIteratorT it) {
      if constexpr (neon_algo == NEONAlgo::UMINV) {
        return non_sve_uminv_next_match(it);
      } else {
        return non_sve_mm_next_match(it);
      }
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT non_sve_uminv_next_it(vector_type it, uint32_t last_match) {
      return non_sve_set_index(it, static_cast<uint8_t>(last_match), 0);
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT non_sve_mm_next_it(uint32_t it, uint32_t /*last_match*/) {
#pragma GCC unroll 20
      for (uint16_t i = 0; i < key_size_; ++i) {
        it &= it - 1;
      }
      return it;
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT non_sve_next_it(MaskIteratorT it, uint32_t last_match) {
      if constexpr (neon_algo == NEONAlgo::UMINV) {
        return non_sve_uminv_next_it(it, last_match);
      } else {
        return non_sve_mm_next_it(it, last_match);
      }
    }

#ifdef __ARM_FEATURE_SVE
    HEDLEY_ALWAYS_INLINE static MaskIteratorT init_sve_mask(svbool_t mask) { return mask; }

    HEDLEY_ALWAYS_INLINE static bool sve_has_next(MaskIteratorT it) { return sve_vector_any_nonzero(it); }
    HEDLEY_ALWAYS_INLINE static uint16_t sve_next_match(MaskIteratorT it, vector_type index_vector) {
      return static_cast<uint16_t>(sve_vector_first_index(it, index_vector));
    }
    HEDLEY_ALWAYS_INLINE static MaskIteratorT sve_next_it(MaskIteratorT it, uint32_t /*last_match*/) {
      // The following instruction inverts the mask where only the first match is true and ANDs it with it
      return svbic_b_z(it, it, svbrka_b_z(it, it));
    }
#else
    HEDLEY_ALWAYS_INLINE static MaskIteratorT init_sve_mask(MaskOrVectorInputType /*cmp_result*/) { FAIL("using sve on non sve system"); }
    HEDLEY_ALWAYS_INLINE static bool sve_has_next(MaskIteratorT /*cmp_result*/) { FAIL("using sve on non sve system"); }
    HEDLEY_ALWAYS_INLINE static uint16_t sve_next_match(MaskIteratorT /*cmp_result*/) { FAIL("using sve on non sve system"); }
    HEDLEY_ALWAYS_INLINE static MaskIteratorT sve_next_it(MaskIteratorT /*cmp_result*/, uint32_t /*last_match*/) {
      FAIL("using sve on non sve system");
    }
#endif

   private:
    constexpr static uint16_t key_size_ = static_cast<uint16_t>(sizeof(KeyT));
  };

#endif  // HASHMAP_IS_ARM

#if HASHMAP_IS_POWER
  // clang-format off
  using _vector_type = typename std::conditional<std::is_same_v<KeyT, uint8_t>,
                          vector unsigned char,
                          typename std::conditional<std::is_same_v<KeyT, uint16_t>,
                                                    vector unsigned short,
                                                    typename std::conditional<
                                                                              std::is_same_v<KeyT, uint32_t>,
                                                                              vector unsigned int,
                                                                              typename std::conditional<
                                                                                                        std::is_same_v<KeyT, uint64_t>,
                                                                                                        vector unsigned long,
                                                                                                        void
                                                                                                        >::type
                                                                              >::type
                                                    >::type
                      >::type;
  // clang-format on

  // clang-format off
  using _power_scalar_t = typename std::conditional<std::is_same_v<KeyT, uint8_t>,
                          unsigned char,
                          typename std::conditional<std::is_same_v<KeyT, uint16_t>,
                                                    unsigned short,
                                                    typename std::conditional<
                                                                              std::is_same_v<KeyT, uint32_t>,
                                                                              unsigned int,
                                                                              typename std::conditional<
                                                                                                        std::is_same_v<KeyT, uint64_t>,
                                                                                                        unsigned long,
                                                                                                        void
                                                                                                        >::type
                                                                              >::type
                                                    >::type
                      >::type;
  // clang-format on

  constexpr static uint16_t _vector_alignment() { return 16; }

#ifdef HEDLEY_GNUC_VERSION
// clang/gcc
#ifndef HEDLEY_GCC_VERSION
  // clang
  using vector_type __attribute__((aligned(_vector_alignment()))) = _vector_type;
#else
  // gcc
  // The following is not well-defined in the standard and hence clang complains, but gcc doesn't
  // See update to https://stackoverflow.com/a/15912208 for a discussion
  using vector_type alignas(_vector_alignment()) = _vector_type;

#endif  // HEDLEY_GCC_VERSION
#else
  // any other compiler - not sure what works here, using gcc version for now
  using vector_type alignas(_vector_alignment()) = _vector_type;
#endif

  HEDLEY_ALWAYS_INLINE static vector_type vec_load(const KeyT* data) {
    return reinterpret_cast<vector_type>(vec_vsx_ld(0, static_cast<const _power_scalar_t*>(data)));
  }

  constexpr static auto vector_load_func() { return &vec_load; }

  using VectorLoadFuncT = decltype(vector_load_func());
  constexpr static VectorLoadFuncT vector_load_func_ = vector_load_func();

  // Since Power7, we can use vec_vsx_ld for unaligned data as well
  constexpr static auto vector_unaligned_load_func() { return vector_load_func(); }

  using VectorUnalignedLoadFuncT = decltype(vector_unaligned_load_func());
  constexpr static VectorLoadFuncT vector_unaligned_load_func_ = vector_unaligned_load_func();

  using LoadPtrT = KeyT*;

  HEDLEY_ALWAYS_INLINE static vector_type vec_broadcast(KeyT scalar) {
    if constexpr (std::is_same_v<KeyT, uint8_t>) {
      return vector_type{scalar, scalar, scalar, scalar, scalar, scalar, scalar, scalar,
                         scalar, scalar, scalar, scalar, scalar, scalar, scalar, scalar};
    } else if constexpr (std::is_same_v<KeyT, uint16_t>) {
      return vector_type{scalar, scalar, scalar, scalar, scalar, scalar, scalar, scalar};
    } else if constexpr (std::is_same_v<KeyT, uint32_t>) {
      return vector_type{scalar, scalar, scalar, scalar};
    } else if constexpr (std::is_same_v<KeyT, uint64_t>) {
      return vector_type{scalar, scalar};
    }
  }

  constexpr static auto vector_broadcast_func() { return &vec_broadcast; }

  using VectorBroadcastFuncT = decltype(vector_broadcast_func());
  constexpr static VectorBroadcastFuncT vector_broadcast_func_ = vector_broadcast_func();

  using CompareT = vector_type;

  struct CompareResult {
    vector_type vec;
    vector_type cmp;
    bool match_found = false;
  };

  HEDLEY_ALWAYS_INLINE static CompareResult vec_compare(vector_type vec, vector_type cmp, bool no_test_later = false) {
    if (no_test_later) {
      return {vec, cmp, false};  // de facto noop to implement NO_TESTZ on power
    } else {
      return {vec, cmp, static_cast<bool>(vec_any_eq(vec, cmp))};
    }
  }

  constexpr static auto vector_compare_func() { return &vec_compare; }
  using VectorCompFuncT = decltype(vector_compare_func());
  constexpr static VectorCompFuncT vector_cmp_func_ = vector_compare_func();

  using MaskType = CompareResult;
  using MaskOrVectorInputType = MaskType;

  HEDLEY_ALWAYS_INLINE static uint32_t vector_first_index(const CompareResult& cmp_result) {
    if constexpr (std::is_same_v<KeyT, uint64_t>) {
      // I was not able to find any Power intrinsic that supports index calculation for 64 bit keys
      uint32_t result = calc_invalid_vector_index();

      if (cmp_result.vec[0] == cmp_result.cmp[0]) {
        result = 0;
      } else if (cmp_result.vec[1] == cmp_result.cmp[1]) {
        result = 1;
      }

      return result;
    } else {
      return static_cast<uint32_t>(vec_first_match_index(cmp_result.vec, cmp_result.cmp));
    }
  }

  HEDLEY_ALWAYS_INLINE static bool vector_any_nonzero(const CompareResult& cmp_result) { return cmp_result.match_found; }

  constexpr static uint32_t calc_invalid_vector_index() { return 16 / static_cast<uint32_t>(sizeof(KeyT)); }

  HEDLEY_ALWAYS_INLINE static uint32_t vector_manual_first_index(
      vector_type vector, KeyT needle, std::vector<KeyT, hashmap::utils::AlignedAllocator<KeyT, _vector_alignment()> >* /*tmp_vector*/) {
    DEBUG_ASSERT(simd_algo == SIMDAlgorithm::MANUAL_ON_MATCH, "Using manual first index with wrong simd_algo!");
    constexpr static uint8_t keys_per_vector = (simd_size / 8) / sizeof(KeyT);

    for (uint8_t i = 0; i < keys_per_vector; ++i) {
      if (vector[i] == needle) {
        return i;
      }
    }
    return keys_per_vector;
  }

  class CompareResultIterator {
   public:
    typedef typename std::conditional<neon_algo == NEONAlgo::UMINV, vector_type, uint32_t>::type MaskIteratorT;

    HEDLEY_ALWAYS_INLINE static uint32_t initialize_movemask(const CompareResult& compare_result) {
      vector unsigned char mask;
#if defined(__clang__)
      mask = __builtin_altivec_lvsl(0, (const unsigned char*)0);
#elif defined(__GNUC__)
      // The const unsigned char* overload of __builtin_altivec_lvsl is not defined
      // on GCC and other overloads have different behavior from clang (may be a
      // bug).
      asm volatile("lvsl %0, %1, %1" : "=v"(mask) : "r"(0));
#else
#error Unsupported compiler. Maybe try inline assembly from above
#endif

      vector unsigned long long mask_vector =
          vec_vbpermq(reinterpret_cast<vector unsigned char>(vec_cmpeq(compare_result.vec, compare_result.cmp)), mask << 3);

      return static_cast<uint32_t>(mask_vector[1]);
    }

    HEDLEY_ALWAYS_INLINE static vector_type initialize_vectorized(const CompareResult& compare_result) {
      vector_type vec = reinterpret_cast<vector_type>(vec_cmpeq(compare_result.vec, compare_result.cmp));
      return vec;
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT initialize(CompareResult compare_result) {
      // We create a movemask similar to intel. However, the movemask always has sizeof(KeyT) bits set per vector element
      // The code is mainly taken from https://github.com/google/dimsum/blob/master/dimsum_x86.h

      // TODO: we abuse the NEONAlgo here. We might want to reconsider the interface, because now it is the general fingerprinting algo this also
      // defines
      if constexpr (neon_algo == NEONAlgo::UMINV) {
        return initialize_vectorized(compare_result);
      } else {
        return initialize_movemask(compare_result);
      }
    }

    HEDLEY_ALWAYS_INLINE static bool has_next_movemask(uint32_t mask) { return mask != 0; }
    HEDLEY_ALWAYS_INLINE static bool has_next_vectorized(vector_type iterator_vec) { return !vec_all_eq(vector_broadcast_func_(0), iterator_vec); }

    HEDLEY_ALWAYS_INLINE static bool has_next(MaskIteratorT mask) {
      if constexpr (neon_algo == NEONAlgo::UMINV) {
        return has_next_vectorized(mask);
      } else {
        return has_next_movemask(mask);
      }
    }

    HEDLEY_ALWAYS_INLINE static uint16_t next_match_movemask(uint32_t mask) {
      // natively supported by cnttzw instruction on power
      return static_cast<uint16_t>(__builtin_ctz(mask)) / key_size_;
    }

    HEDLEY_ALWAYS_INLINE static uint16_t next_match_vectorized(vector_type iterator_vec) {
      uint16_t next_index = 0;

      if constexpr (std::is_same_v<KeyT, uint64_t>) {
        // I was not able to find any Power intrinsic that supports index calculation for 64 bit keys
        next_index = calc_invalid_vector_index();

        if (iterator_vec[0] == std::numeric_limits<KeyT>::max()) {
          next_index = 0;
        } else if (iterator_vec[1] == std::numeric_limits<KeyT>::max()) {
          next_index = 1;
        }

      } else {
        next_index = static_cast<uint16_t>(vec_first_match_index(iterator_vec, vector_broadcast_func_(std::numeric_limits<KeyT>::max())));
      }

      return next_index;
    }

    HEDLEY_ALWAYS_INLINE static uint16_t next_match(MaskIteratorT mask) {
      if constexpr (neon_algo == NEONAlgo::UMINV) {
        return next_match_vectorized(mask);
      } else {
        return next_match_movemask(mask);
      }
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT next_it_movemask(uint32_t mask, uint32_t /*last_match*/) {
#pragma GCC unroll 20
      for (uint16_t i = 0; i < key_size_; ++i) {
        mask &= mask - 1;
      }

      return mask;
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT next_it_vectorized(vector_type mask, uint32_t last_match) {
      mask[last_match] = 0;
      return mask;
    }

    HEDLEY_ALWAYS_INLINE static MaskIteratorT next_it(MaskIteratorT mask, uint32_t last_match) {
      if constexpr (neon_algo == NEONAlgo::UMINV) {
        return next_it_vectorized(mask, last_match);
      } else {
        return next_it_movemask(mask, last_match);
      }
    }

   private:
    constexpr static uint16_t key_size_ = static_cast<uint16_t>(sizeof(KeyT));
  };

#endif
#endif

  static std::string key_simd_type_to_str() { return hashmap::utils::data_type_to_str<_vector_type>(); }

  static std::string compare_result_type_to_str() { return hashmap::utils::data_type_to_str<MaskType>(); }
};
