#pragma once

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <type_traits>
#include <vector>

#include "benchmark/benchmark_utils.hpp"
#include "fmt/format.h"
#include "hashmap/misc/stringkey.hpp"
#include "hashmap/utils.hpp"
#include "hedley.h"
#include "spdlog/spdlog.h"

namespace benchmark {

#ifdef HASHMAP_COLLECT_META_INFO
struct MetadataBenchmarkResult {
  std::string hashmap_identifier = "";
  uint8_t load_factor = 0;
  uint8_t successful_query_rate = 0;
  uint64_t hashtable_size = 0;
  std::string distribution_name = "";
  uint32_t workload = 0;
  double key_size = 0;
  double value_size = 0;
  uint64_t entry_size = 0;
  uint64_t entries_processed = 0;
  std::string data_ptr = "";
  bool is_aligned_to_hp = false;
  hashmap::utils::MeasurementInfo minfo;
};

class MetadataBenchmarkResultCollector {
 public:
  MetadataBenchmarkResultCollector() = default;
  void add_benchmark_result(MetadataBenchmarkResult result);
  void to_csv(std::filesystem::path path);

 private:
  std::vector<MetadataBenchmarkResult> benchmark_results_;
};
#endif

struct DummyTuple {
  uint64_t value1 = 0;
  uint64_t value2 = 0;
  uint64_t value3 = 0;
  uint64_t value4 = 0;

  operator uint64_t() const;  // NOLINT: we want this to be implicit!
};

// https://stackoverflow.com/a/62883066/1625689
template <class IntType = unsigned long, class RealType = double>
class zipf_distribution {
 public:
  typedef IntType result_type;

  static_assert(std::numeric_limits<IntType>::is_integer, "");
  static_assert(!std::numeric_limits<RealType>::is_integer, "");

  /// zipf_distribution(N, s, q)
  /// Zipf distribution for `N` items, in the range `[1,N]` inclusive.
  /// The distribution follows the power-law 1/(n+q)^s with exponent
  /// `s` and Hurwicz q-deformation `q`.
  zipf_distribution(const IntType n = std::numeric_limits<IntType>::max(), const RealType s = 1.0, const RealType q = 0.0)
      : n(n),
        _s(s),
        _q(q),
        oms(1.0 - s),
        spole(std::abs(oms) < epsilon),
        rvs(spole ? 0.0 : 1.0 / oms),
        H_x1(H(1.5) - h(1.0)),
        H_n(H(static_cast<double>(n) + 0.5)),
        cut(1.0 - H_inv(H(1.5) - h(1.0))),
        dist(H_x1, H_n) {
    if (-0.5 >= q) throw std::runtime_error("Range error: Parameter q must be greater than -0.5!");
  }
  void reset() {}

  IntType operator()(std::mt19937& rng) {
    while (true) {
      const RealType u = dist(rng);
      const RealType x = H_inv(u);
      const IntType k = static_cast<IntType>(std::round(x));
      if (static_cast<double>(k) - x <= cut) return k;
      if (u >= H(static_cast<double>(k) + 0.5) - h(static_cast<double>(k))) return k;
    }
  }

  /// Returns the parameter the distribution was constructed with.
  RealType s() const { return _s; }
  /// Returns the Hurwicz q-deformation parameter.
  RealType q() const { return _q; }
  /// Returns the minimum value potentially generated by the distribution.
  result_type min() const { return 1; }
  /// Returns the maximum value potentially generated by the distribution.
  result_type max() const { return n; }

 private:
  IntType n;                                      ///< Number of elements
  RealType _s;                                    ///< Exponent
  RealType _q;                                    ///< Deformation
  RealType oms;                                   ///< 1-s
  bool spole;                                     ///< true if s near 1.0
  RealType rvs;                                   ///< 1/(1-s)
  RealType H_x1;                                  ///< H(x_1)
  RealType H_n;                                   ///< H(n)
  RealType cut;                                   ///< rejection cut
  std::uniform_real_distribution<RealType> dist;  ///< [H(x_1), H(n)]

  // This provides 16 decimal places of precision,
  // i.e. good to (epsilon)^4 / 24 per expanions log, exp below.
  static constexpr RealType epsilon = 2e-5;

  /** (exp(x) - 1) / x */
  static double expxm1bx(const double x) {
    if (std::abs(x) > epsilon) return std::expm1(x) / x;
    return (1.0 + x / 2.0 * (1.0 + x / 3.0 * (1.0 + x / 4.0)));
  }

  /** log(1 + x) / x */
  static RealType log1pxbx(const RealType x) {
    if (std::abs(x) > epsilon) return std::log1p(x) / x;
    return 1.0 - x * ((1 / 2.0) - x * ((1 / 3.0) - x * (1 / 4.0)));
  }
  /**
   * The hat function h(x) = 1/(x+q)^s
   */
  const RealType h(const RealType x) { return std::pow(x + _q, -_s); }

  /**
   * H(x) is an integral of h(x).
   *     H(x) = [(x+q)^(1-s) - (1+q)^(1-s)] / (1-s)
   * and if s==1 then
   *     H(x) = log(x+q) - log(1+q)
   *
   * Note that the numerator is one less than in the paper
   * order to work with all s. Unfortunately, the naive
   * implementation of the above hits numerical underflow
   * when q is larger than 10 or so, so we split into
   * different regimes.
   *
   * When q != 0, we shift back to what the paper defined:

   *    H(x) = (x+q)^{1-s} / (1-s)
   * and for q != 0 and also s==1, use
   *    H(x) = [exp{(1-s) log(x+q)} - 1] / (1-s)
   */
  const RealType H(const RealType x) {
    if (not spole) return std::pow(x + _q, oms) / oms;

    const RealType log_xpq = std::log(x + _q);
    return log_xpq * expxm1bx(oms * log_xpq);
  }

  /**
   * The inverse function of H(x).
   *    H^{-1}(y) = [(1-s)y + (1+q)^{1-s}]^{1/(1-s)} - q
   * Same convergence issues as above; two regimes.
   *
   * For s far away from 1.0 use the paper version
   *    H^{-1}(y) = -q + (y(1-s))^{1/(1-s)}
   */
  const RealType H_inv(const RealType y) {
    if (not spole) return std::pow(y * oms, rvs) - _q;

    return std::exp(y * log1pxbx(oms * y)) - _q;
  }
};

enum class DataDistribution { UNIFORM, DENSE, ZIPF };
std::string get_distribution_name(DataDistribution distribution);

std::vector<uint8_t> parse_thread_counts(int argc, char** argv);

template <class KeyT>
std::vector<KeyT> generate_n_unique_integer_keys(uint64_t n, DataDistribution distribution, std::optional<std::vector<KeyT>*> keys_to_exclude) {
  std::vector<KeyT> keys;

  spdlog::info(fmt::format("Generating {} unique integer keys...", n));

  if (distribution == DataDistribution::UNIFORM || distribution == DataDistribution::ZIPF) {
    std::uniform_int_distribution<KeyT> uniform_dis(0, std::numeric_limits<KeyT>::max());

    while (keys.size() < n) {
      uint64_t missing_keys = n - keys.size();
      std::vector<KeyT> temporary_vector;
      temporary_vector.reserve(missing_keys);

      for (uint64_t i = 0; i < missing_keys; ++i) {
        temporary_vector.push_back(uniform_dis(reproducible_gen()));
        if (i == 0 || i % 1000 == 0) {
          printProgress(static_cast<double>(i) / static_cast<double>(missing_keys));
        }
      }
      printProgress(1);

#ifndef HASHMAP_IS_PROFILE_BUILD
      std::cout << std::endl;
#endif
      if (keys_to_exclude.has_value()) {
        std::vector<KeyT>* keys_to_excl = keys_to_exclude.value();
        spdlog::info("Sorting keys_to_exclude...");
        std::sort(keys_to_excl->begin(), keys_to_excl->end());

        std::vector<KeyT> adapted_vector;
        adapted_vector.reserve(missing_keys);
        spdlog::info("Building new vector with keys_to_exclude...");

        for (uint64_t i = 0; i < temporary_vector.size(); i++) {
          if (std::binary_search(keys_to_excl->begin(), keys_to_excl->end(), temporary_vector[i])) {
            continue;
          }
          if (i == 0 || i % 1000 == 0) {
            printProgress(static_cast<double>(i) / static_cast<double>(missing_keys));
          }
          adapted_vector.push_back(temporary_vector[i]);
        }
        printProgress(1);

#ifndef HASHMAP_IS_PROFILE_BUILD
        std::cout << std::endl;
#endif
        temporary_vector.clear();
        temporary_vector = adapted_vector;
      }

      spdlog::info("Making keys unique...");
      spdlog::info("Sorting temporary vector...");
      std::sort(temporary_vector.begin(), temporary_vector.end());

      spdlog::info("Erasing duplicate elements");
      temporary_vector.erase(std::unique(temporary_vector.begin(), temporary_vector.end()), temporary_vector.end());

      spdlog::info("Appending to final vector...");
      keys.insert(keys.end(), temporary_vector.begin(), temporary_vector.end());

      if (keys.size() < n) {
        spdlog::info(fmt::format("First iteration only created {}/{} unique keys, doing another iteration.", keys.size(), n));
      } else {
        spdlog::info(fmt::format("Successfully created {} unique keys, done.", keys.size()));
      }
    }
  } else if (distribution == DataDistribution::DENSE) {
    ASSERT(keys_to_exclude == std::nullopt, "Dense distribution currently does not support keys_to_exclude_");
    keys.reserve(n);
    for (int i = 0; i < n; ++i) {
      keys.push_back(i);
    }
  } else {
    FAIL("unsupported distribution");
  }

  return keys;
}

std::vector<StringKey> generate_fill_strings_on_heap(uint64_t n, std::optional<std::vector<StringKey>*> keys_to_exclude);

template <class ValueT>
typename std::remove_pointer<ValueT>::type generate_random_value() {
  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;

  if constexpr (std::is_same_v<ValueTNoPtr, DummyTuple>) {
    static std::uniform_int_distribution<uint64_t> value_dis(0, std::numeric_limits<uint64_t>::max());
    DummyTuple t;
    t.value1 = value_dis(reproducible_gen());
    t.value2 = value_dis(reproducible_gen());
    t.value3 = value_dis(reproducible_gen());
    t.value4 = value_dis(reproducible_gen());

    return t;

  } else {
    // we assume either dummytuple or integer value because this is how we roll!
    static std::uniform_int_distribution<ValueTNoPtr> value_dis(0, std::numeric_limits<ValueTNoPtr>::max());
    return value_dis(reproducible_gen());
  }
}

template <class KeyT, class ValueT>
std::vector<std::pair<KeyT, typename std::remove_pointer<ValueT>::type>> generate_fill_data(uint8_t load_factor, uint64_t hashtable_size,
                                                                                            DataDistribution distribution) {
  typedef typename std::remove_pointer<ValueT>::type ValueTNoPtr;

  uint64_t entries_to_insert = (hashtable_size * static_cast<uint64_t>(load_factor));

  entries_to_insert = (entries_to_insert / 100) + (entries_to_insert % 100 > 0);
  std::vector<KeyT> keys;
  if constexpr (std::is_arithmetic<KeyT>::value) {
    keys = generate_n_unique_integer_keys<KeyT>(entries_to_insert, distribution, std::nullopt);
  } else if constexpr (std::is_same_v<KeyT, StringKey>) {
    keys = generate_fill_strings_on_heap(entries_to_insert, std::nullopt);
  } else {
    FAIL("unsupported key type, cannot generate fill data!");
  }

  std::vector<std::pair<KeyT, ValueTNoPtr>> result;
  result.reserve(keys.size());

  for (const KeyT& key : keys) {
    result.push_back({key, generate_random_value<ValueT>()});
  }

  return result;
}

template <class KeyT, class ValueT>
std::vector<std::vector<std::pair<KeyT, ValueT>>> generate_prefault_data(uint64_t hashtable_size, uint8_t thread_count) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<uint64_t> dist{0, 0xDEADBEEF};
  std::vector<std::vector<std::pair<KeyT, ValueT>>> result;
  result.resize(thread_count);

  for (uint8_t thread = 0; thread < thread_count; ++thread) {
    spdlog::info(fmt::format("Generating prefault data for thread {}/{}", thread, thread_count));
    std::vector<std::pair<KeyT, ValueT>>& thread_result = result[thread];
    for (uint64_t i = 0; i < hashtable_size; i++) {
      if (i == 0 || i % 1000 == 0) {
        printProgress(static_cast<double>(i) / static_cast<double>(hashtable_size));
      }

      KeyT k;
      ValueT v;
      if constexpr (!std::is_same_v<KeyT, StringKey>) {
        k = static_cast<KeyT>(dist(gen));
      } else {
        k = StringKey(reinterpret_cast<char*>(dist(gen)));
      }

      if constexpr (!std::is_pointer_v<ValueT>) {
        v = static_cast<ValueT>(dist(gen));
      } else {
        v = reinterpret_cast<ValueT>(dist(gen));
      }
      thread_result.push_back({k, v});
    }
    printProgress(1);

#ifndef HASHMAP_IS_PROFILE_BUILD
    std::cout << std::endl;
#endif
  }

  return result;
}

}  // namespace benchmark
