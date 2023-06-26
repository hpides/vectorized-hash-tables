#include "benchmark/benchmark_utils.hpp"

#include <limits.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <random>

namespace benchmark {

uint64_t timeSinceEpochMillisec() {
  // https://stackoverflow.com/questions/19555121/how-to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets
  using namespace std::chrono;
  return static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

uint64_t next_random_data(uint64_t max) {
  std::uniform_int_distribution<uint64_t> dis(0, max);
  return dis(reproducible_gen());
}

std::mt19937 TrulyRandomEngine() {
  std::vector<uint32_t> random_data(std::mt19937::state_size);
  std::random_device source;
  std::generate(random_data.begin(), random_data.end(), std::ref(source));
  std::seed_seq seeds(random_data.begin(), random_data.end());
  std::mt19937 engine(seeds);

  return engine;
}

std::mt19937 CustomSeededEngine(uint32_t seed) {
  std::vector<uint32_t> random_data(std::mt19937::state_size);
  std::iota(random_data.begin(), random_data.end(), seed);
  std::seed_seq seeds(random_data.begin(), random_data.end());
  std::mt19937 engine(seeds);

  return engine;
}

std::mt19937 FasterEngine() {
  std::random_device rd;
  std::mt19937 gen(rd());

  return gen;
}

std::mt19937& reproducible_gen() {
  static std::mt19937 generator = CustomSeededEngine(4711);
  return generator;
}

std::mt19937& random_gen() {
  static std::mt19937 generator = TrulyRandomEngine();
  return generator;
}

std::mt19937& fast_gen() {
  static std::mt19937 generator = FasterEngine();
  return generator;
}

void printProgress(double percentage) {
#ifndef HASHMAP_IS_PROFILE_BUILD
  int val = (int)(percentage * 100);
  int lpad = (int)(percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);
#else
  (void)percentage;
#endif
}

std::string get_compiler_identifier() {
  std::string compiler = "" TOSTRING(COMPILER_IDENTIFIER);

  if (compiler == "GNU") {
    return "gcc";
  } else if (compiler == "Clang") {
    return "clang";
  } else if (compiler == "Intel") {
    return "icc";
  }

  return compiler;
}

std::string get_hostname() {
#ifdef IS_LINUX
  char hostname[HOST_NAME_MAX];
  gethostname(hostname, HOST_NAME_MAX);
  return std::string(hostname);
#else
  return "SomeNonLinuxSystem";
#endif
}

}  // namespace benchmark
