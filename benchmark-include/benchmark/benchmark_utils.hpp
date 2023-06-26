#pragma once

#include <atomic>
#include <cstdint>
#include <random>

#include "hedley.h"

namespace benchmark {

uint64_t next_random_data(uint64_t max);
std::mt19937 TrulyRandomEngine();
std::mt19937 CustomSeededEngine(uint32_t seed);
std::mt19937& reproducible_gen();
std::mt19937& random_gen();
std::mt19937& fast_gen();

uint64_t timeSinceEpochMillisec();

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

std::string get_compiler_identifier();
std::string get_hostname();

template <typename T>
HEDLEY_ALWAYS_INLINE void doNotOptimize(T const& val) {
  // NOLINTNEXTLINE: hicpp-no-assembler
  asm volatile("" : : "r,m"(val) : "memory");
}

HEDLEY_ALWAYS_INLINE void ClobberMemory() { std::atomic_signal_fence(std::memory_order_acq_rel); }

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

}  // namespace benchmark
