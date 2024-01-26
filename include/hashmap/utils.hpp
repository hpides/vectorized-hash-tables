#pragma once

#include <sys/mman.h>  // madvise

#include <bit>
#include <cstdlib>  // posix_memalign
#include <exception>
#include <filesystem>
#include <new>
#include <random>
#include <string>
#include <type_traits>
#include <utility>

#include "hedley.h"

#ifdef HASHMAP_IS_VTUNEPROFILE_BUILD
#include "ittnotify.h"
#endif

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

__extension__ typedef unsigned __int128 uint128_t;

#define FAIL(msg) throw hashmap::utils::HashmapException("ERROR at " __FILE__ ":" + std::to_string(__LINE__) + " " + (msg) + "\nExecution failed.")

#define ASSERT(expr, msg)         \
  if (!static_cast<bool>(expr)) { \
    FAIL((msg));                  \
  }                               \
  static_assert(true, "End call of macro with a semicolon")

#ifdef NDEBUG
#define DEBUG_ASSERT(expr, msg) \
  do {                          \
  } while (0)
#else
#define DEBUG_ASSERT(expr, msg) ASSERT((expr), (msg))
#endif

#define FRIEND_TEST(test_case_name, test_name) friend class test_case_name##_##test_name##_Test

struct StringKey;

namespace {
template <typename, template <typename...> typename>
struct is_instance_impl : public std::false_type {};

template <template <typename...> typename U, typename... Ts>
struct is_instance_impl<U<Ts...>, U> : public std::true_type {};
}  // namespace

template <typename T, template <typename...> typename U>
using is_instance = is_instance_impl<std::decay_t<T>, U>;

namespace benchmark {
struct DummyTuple;
}  // namespace benchmark

namespace hashmap::utils {

#ifdef HASHMAP_COLLECT_META_INFO
struct MeasurementInfo {
  bool measurement_started = false;
  uint64_t num_finds = 0;
  uint64_t probed_elements = 0;         // probed keys/key vectors/fingerprint vectors/buckets
  uint64_t probed_elements_total = 0;   // probed keys/fingerprints (vectors * entries per vec) - does not make sense for classical HMS
  uint64_t simd_loads = 0;              // number of simd loads, for unchunked should be probled_elemets * 2, otherwise equal
  uint64_t num_collision = 0;           // number of hash collisions (hash equals, key doesnt)
  uint64_t num_overflows_followed = 0;  // how often we followed an overflow
  uint64_t num_overflows = 0;           // how many buckets have overflowed (constant for a given load)
  uint64_t num_buckets = 0;             // how many buckets do we have overall
  uint64_t min_probing_sequence =
      std::numeric_limits<uint64_t>::max();  // min number of keys/key vectors/fingerprint vectors/buckets we had to touch in this setting
  uint64_t max_probing_sequence = 0;         // max number of keys/key vectors/fingerprint vectors/buckets we had to touch in this setting

  void reset() {
    measurement_started = false;
    num_finds = 0;
    probed_elements = 0;
    probed_elements_total = 0;
    simd_loads = 0;
    num_collision = 0;
    num_overflows_followed = 0;
    num_overflows = 0;
  }
};
#endif

#ifdef HASHMAP_IS_VTUNEPROFILE_BUILD
static __itt_domain* vtune_domain = __itt_domain_create("HashmapDomain");
static __itt_string_handle* vtune_task_ll = __itt_string_handle_create("LookupLoop");
#endif

// From https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2?page=1&tab=modifieddesc#tab-top

// Closest least power of 2 minus 1. Returns 0 if n = 0.
template <typename UInt, std::enable_if_t<std::is_unsigned<UInt>::value, int> = 0>
constexpr UInt clp2m1(UInt n, unsigned i = 1) noexcept {
  return i < sizeof(UInt) * 8 ? clp2m1(UInt(n | (n >> i)), i << 1) : n;
}

/// Closest least power of 2 minus 1. Returns 0 if n <= 0.
template <typename Int, std::enable_if_t<std::is_integral<Int>::value && std::is_signed<Int>::value, int> = 0>
constexpr auto clp2m1(Int n) noexcept {
  return clp2m1(std::make_unsigned_t<Int>(n <= 0 ? 0 : n));
}

/// Closest least power of 2. Returns 2^N: 2^(N-1) < n <= 2^N. Returns 0 if n <= 0.
template <typename Int, std::enable_if_t<std::is_integral<Int>::value, int> = 0>
constexpr auto clp2(Int n) noexcept {
  return clp2m1(std::make_unsigned_t<Int>(n - 1)) + 1;
}

/// Next power of 2. Returns 2^N: 2^(N-1) <= n < 2^N. Returns 1 if n = 0. Returns 0 if n < 0.
template <typename Int, std::enable_if_t<std::is_integral<Int>::value, int> = 0>
constexpr auto np2(Int n) noexcept {
  return clp2m1(std::make_unsigned_t<Int>(n)) + 1;
}

/// Returns value if it is power of two, else next power of 2.
template <typename Int, std::enable_if_t<std::is_integral<Int>::value, int> = 0>
constexpr auto value_or_np2(Int n) noexcept {
  if (n > 1) {
    return np2(n - 1);
  } else {
    return np2(n);
  }
}

bool is_power_of_two(uint64_t value);

std::filesystem::path getexepath();

class HashmapException : public std::exception {
 public:
  explicit HashmapException(std::string msg) : msg_{std::move(msg)} {}
  const char* what() const noexcept override { return msg_.c_str(); }

 private:
  const std::string msg_;
};

template <typename KeyT, typename HasherT>
struct ExternalHashWrapper {
  size_t operator()(KeyT const& key) const { return static_cast<size_t>(HasherT::static_hash(key)); }
};

// taken from https://github.com/rurban/smhasher/blob/28de33a868763a00439a5fc408b56b20f2d86f7c/Hashes.cpp

constexpr static uint128_t multiply_constant_128b = ((uint128_t)0x75f17d6b3588f843 << 64) | 0xb13dea7c9c324e51;
constexpr static uint64_t multiply_constant_64b = 0x75f17d6b3588f843;

constexpr static uint128_t add_constant_128b = ((uint128_t)0x736f6d6570736575 << 64) | 0x646f72616e646f6d;
constexpr static uint64_t add_constant_64b = 0x736f6d6570736575;

constexpr static uint64_t murmur_constant1 = 0xff51afd7ed558ccd;
constexpr static uint64_t murmur_constant2 = 0xc4ceb9fe1a85ec53;

enum class PrefetchingLocality { HIGH = 3, MEDIUM = 2, LOW = 1, NO = 0 };

#ifdef __GNUC__
#ifdef __clang__
#define CLANG_COMPILER 1
#else
#define GCC_COMPILER 1
#endif
#endif

// TODO(Maxiboether): This currently probably only works with gcc/clang/icc
#ifdef CLANG_COMPILER
#define READ_PREFETCH(address, locality) __builtin_prefetch(address, 0, static_cast<int>(locality))
#else
#define READ_PREFETCH(address, locality) __builtin_prefetch(address, 0, locality)
#endif

#ifdef __cpp_lib_hardware_interference_size
#pragma message("Using std::hardware_destructive_interference_size for cacheline size")
constexpr static size_t cacheline_size = std::hardware_destructive_interference_size;
#elif defined(CACHELINE_SIZE)  // this will probably happen most of the time, show no info here
constexpr static size_t cacheline_size = CACHELINE_SIZE;
#else
constexpr static size_t cacheline_size = 64;
#pragma message("Using hardcoded cacheline size of 64 B. Please check.")
#endif

#if defined(HUGEPAGE_SIZE)
constexpr static size_t hugepage_size = HUGEPAGE_SIZE;
#else
constexpr static size_t hugepage_size = 2097152;
#pragma message("Using hardcoded huge page size of 2097152 (2 MiB). Please check.")
#endif

#if defined(PAGE_SIZE)
constexpr static size_t page_size = PAGE_SIZE;
#else
constexpr static size_t page_size = 4096;
#pragma message("Using hardcoded huge page size of 4096. Please check.")
#endif

#if defined(DEFAULT_WORKLOAD)
constexpr static size_t default_workload = DEFAULT_WORKLOAD;
#else
constexpr static size_t default_workload = 1024;
#pragma message("Using hardcoded default workload of 1024 MiB. Please check.")
#endif

#if defined(HASHMAP_IS_ARM)
constexpr static bool is_arm = true;
#else
constexpr static bool is_arm = false;
#endif

#if defined(HASHMAP_IS_POWER)
constexpr static bool is_power = true;
#else
constexpr static bool is_power = false;
#endif

#if defined(SVE_REGISTER_SIZE) && defined(__ARM_FEATURE_SVE)
constexpr static size_t sve_register_size = SVE_REGISTER_SIZE;
constexpr static bool has_sve = true;
#else
constexpr static bool has_sve = false;
#endif

template <typename T, uint32_t alignment = 1, bool hugepage_hint = false>
struct AlignedAllocator {
  using value_type = T;

  AlignedAllocator() = default;
  template <class U>
  constexpr AlignedAllocator(const AlignedAllocator<U, alignment, hugepage_hint>&) noexcept {}

  template <class U>
  struct rebind {
    using other = AlignedAllocator<U, alignment, hugepage_hint>;
  };

  T* allocate(std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      throw std::bad_alloc();
    }
    void* p = nullptr;

    posix_memalign(&p, alignment, n * sizeof(T));

#ifdef IS_LINUX
    if constexpr (hugepage_hint) {
      madvise(p, n * sizeof(T), MADV_HUGEPAGE);
    }
#endif

    if (p == nullptr) {
      throw std::bad_alloc();
    }

    return static_cast<T*>(p);
  }

  void deallocate(T* p, std::size_t /*n*/) { std::free(p); }
};

// Thanks to https://rigtorp.se/hugepages/
template <typename T, uint32_t extra_alignment = 1>
struct TransparentHugePageAllocator : public AlignedAllocator<T, std::lcm(hugepage_size, extra_alignment), true> {
  using value_type = T;

  TransparentHugePageAllocator() = default;

  template <class U>
  struct rebind {
    using other = TransparentHugePageAllocator<U, extra_alignment>;
  };

  template <class U>
  constexpr TransparentHugePageAllocator(const TransparentHugePageAllocator<U, extra_alignment>&) noexcept {}
};

// Thanks to https://mazzo.li/posts/check-huge-page.html
void check_huge_page(void* ptr);

static inline bool is_aligned(const void* pointer, size_t byte_count) { return (uintptr_t)pointer % byte_count == 0; }

// https://stackoverflow.com/questions/46871005/how-to-unroll-a-for-loop-using-template-metaprogramming
namespace detail {

template <class T, T... inds, class F>
constexpr void loop(std::integer_sequence<T, inds...>, F&& f) {
  (f(std::integral_constant<T, inds>{}), ...);
}

}  // namespace detail

template <class T, T count, class F>
constexpr void loop(F&& f) {
  detail::loop(std::make_integer_sequence<T, count>{}, std::forward<F>(f));
}

std::string random_string(size_t length);

#if defined(HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES)
#undef HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#endif
#if HEDLEY_HAS_WARNING("-Wattributes")
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES _Pragma("clang diagnostic ignored \"-Wattributes\"")
#elif HEDLEY_GCC_VERSION_CHECK(4, 6, 0)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES _Pragma("GCC diagnostic ignored \"-Wattributes\"")
#elif HEDLEY_INTEL_VERSION_CHECK(17, 0, 0)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#elif HEDLEY_INTEL_CL_VERSION_CHECK(2021, 1, 0)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#elif HEDLEY_MSVC_VERSION_CHECK(19, 0, 0)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#elif HEDLEY_PGI_VERSION_CHECK(20, 7, 0)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#elif HEDLEY_PGI_VERSION_CHECK(17, 10, 0)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#elif HEDLEY_SUNPRO_VERSION_CHECK(5, 14, 0) && defined(__cplusplus)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#elif HEDLEY_TI_VERSION_CHECK(18, 1, 0) || HEDLEY_TI_CL6X_VERSION_CHECK(8, 3, 0) || HEDLEY_TI_CL7X_VERSION_CHECK(1, 2, 0)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#elif HEDLEY_IAR_VERSION_CHECK(8, 0, 0)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#elif HEDLEY_MCST_LCC_VERSION_CHECK(1, 25, 10)
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#else
#define HEDLEY_DIAGNOSTIC_DISABLE_ATTRIBUTES
#endif

template <class Type>
std::string data_type_to_str() {
  if constexpr (std::is_same_v<Type, uint64_t*>) {
    return "ptr_uint64_t";
  }

  if constexpr (std::is_same_v<Type, uint64_t>) {
    return "uint64_t";
  }
  if constexpr (std::is_same_v<Type, uint32_t>) {
    return "uint32_t";
  }
  if constexpr (std::is_same_v<Type, uint16_t>) {
    return "uint16_t";
  }
  if constexpr (std::is_same_v<Type, uint8_t>) {
    return "uint8_t";
  }

  if constexpr (std::is_same_v<Type, int64_t>) {
    return "int64_t";
  }
  if constexpr (std::is_same_v<Type, int32_t>) {
    return "int32_t";
  }
  if constexpr (std::is_same_v<Type, int16_t>) {
    return "int16_t";
  }
  if constexpr (std::is_same_v<Type, int8_t>) {
    return "int8_t";
  }

  if constexpr (std::is_same_v<Type, std::string>) {
    return "std::string";
  }

  if constexpr (std::is_same_v<Type, StringKey>) {
    return "stringkey";
  }

  if constexpr (std::is_same_v<Type, benchmark::DummyTuple*>) {
    return "ptr_DummyTuple";
  }

  if constexpr (std::is_same_v<Type, benchmark::DummyTuple>) {
    return "DummyTuple";
  }

#ifdef HASHMAP_IS_X86
#pragma GCC diagnostic push
// The implications of this are discussed below, as here it is not important (because the checks works despite losing attributess)
#pragma GCC diagnostic ignored "-Wignored-attributes"

  if constexpr (std::is_same_v<Type, __m128i>) {
    return "__m128i";
  }
  if constexpr (std::is_same_v<Type, __m256i>) {
    return "__m256i";
  }
  if constexpr (std::is_same_v<Type, __m512i>) {
    return "__m512i";
  }

  if constexpr (std::is_same_v<Type, __mmask8>) {
    return "__mmask8";
  }
  if constexpr (std::is_same_v<Type, __mmask16>) {
    return "__mmask16";
  }
  if constexpr (std::is_same_v<Type, __mmask32>) {
    return "__mmask32";
  }
  if constexpr (std::is_same_v<Type, __mmask64>) {
    return "__mmask64";
  }

#pragma GCC diagnostic pop
#endif

#ifdef HASHMAP_IS_ARM
  if constexpr (std::is_same_v<Type, uint8x16_t>) {
    return "uint8x16_t";
  } else if constexpr (std::is_same_v<Type, uint16x8_t>) {
    return "uint16x8_t";
  } else if constexpr (std::is_same_v<Type, uint32x4_t>) {
    return "uint32x4_t";
  } else if constexpr (std::is_same_v<Type, uint64x2_t>) {
    return "uint64x2_t";
  }

#ifdef __ARM_FEATURE_SVE
  if constexpr (std::is_same_v<Type, svuint8_t>) {
    return "svuint8_t";
  } else if constexpr (std::is_same_v<Type, svuint16_t>) {
    return "svuint16_t";
  } else if constexpr (std::is_same_v<Type, svuint32_t>) {
    return "svuint32_t";
  } else if constexpr (std::is_same_v<Type, svuint64_t>) {
    return "svuint64_t";
  }
  if constexpr (std::is_same_v<Type, svbool_t>) {
    return "svbool_t";
  }
#endif
#endif

#ifdef HASHMAP_IS_POWER
  if constexpr (std::is_same_v<Type, vector unsigned char>) {
    return "vector unsigned char";
  } else if constexpr (std::is_same_v<Type, vector unsigned short>) {
    return "vector unsigned short";
  } else if constexpr (std::is_same_v<Type, vector unsigned int>) {
    return "vector unsigned int";
  } else if constexpr (std::is_same_v<Type, vector unsigned long>) {
    return "vector unsigned long";
  }
#endif

  return typeid(Type).name();
}

};  // namespace hashmap::utils
