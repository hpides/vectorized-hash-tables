#include "hashmap/utils.hpp"

#include <fcntl.h>
#ifdef IS_LINUX
#include <linux/kernel-page-flags.h>
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#endif

#include <sys/mman.h>
#include <unistd.h>

#include <algorithm>
#include <bit>
#include <cerrno>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace hashmap::utils {

#ifndef CLANG_COMPILER
bool is_power_of_two(uint64_t value) { return std::has_single_bit(value); }
#else
bool is_power_of_two(uint64_t value) { return std::popcount(value) == 1; }
#endif

std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

std::filesystem::path getexepath() {
  char result[PATH_MAX];  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays,-warnings-as-errors)
#ifdef IS_LINUX
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay,-warnings-as-errors)
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  std::string result_str = std::string(
      result, (count > 0) ? count : 0);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay,-warnings-as-errors,
                                         // clang-diagnostic-sign-conversion,-warnings-as-errors)
#endif

#ifdef __APPLE__
  uint32_t bufsize = PATH_MAX;
  if(!_NSGetExecutablePath(result, &bufsize)) {
    puts(result);
  }
  std::string result_str = std::string(result);
#endif

  std::filesystem::path res_path(result_str);

  return res_path.parent_path();
}

// Thanks to https://mazzo.li/posts/check-huge-page.html

#define PAGEMAP_PRESENT(ent) (((ent) & (1ull << 63)) != 0)
#define PAGEMAP_PFN(ent) ((ent) & ((1ull << 55) - 1))

void check_huge_page(void* ptr) {
  #ifdef IS_LINUX
  int pagemap_fd = open("/proc/self/pagemap", O_RDONLY | O_CLOEXEC);  // NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg,hicpp-signed-bitwise)
  if (pagemap_fd < 0) {
    FAIL("could not open /proc/self/pagemap, you need to run the benchmark as root in order to check for huge pages.");
  }

  int kpageflags_fd = open("/proc/kpageflags", O_RDONLY | O_CLOEXEC);  // NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg,hicpp-signed-bitwise)
  if (kpageflags_fd < 0) {
    FAIL("could not open /proc/kpageflags, you need to run the benchmark as root in order to check for huge pages.");
  }

  // each entry is 8 bytes long
  uint64_t ent;
  // NOLINTNEXTLINE
  if (pread(pagemap_fd, &ent, sizeof(ent), ((uintptr_t)ptr) / PAGE_SIZE * 8) != sizeof(ent)) {
    FAIL("could not read from pagemap\n");
  }

  // NOLINTNEXTLINE
  if (!PAGEMAP_PRESENT(ent)) {
    FAIL("page not present in /proc/self/pagemap, did you allocate it?\n");
  }

  // NOLINTNEXTLINE
  if (!PAGEMAP_PFN(ent)) {
    FAIL("page frame number not present, run this program as root\n");
  }

  uint64_t flags;
  // NOLINTNEXTLINE
  if (pread(kpageflags_fd, &flags, sizeof(flags), PAGEMAP_PFN(ent) << 3) != sizeof(flags)) {
    FAIL("could not read from kpageflags\n");
  }

  // NOLINTNEXTLINE
  if (!(flags & (1ull << KPF_THP))) {
    FAIL("could not allocate huge page\n");
  }

  if (close(pagemap_fd) < 0) {
    FAIL("could not close /proc/self/pagemap: %s");
  }
  if (close(kpageflags_fd) < 0) {
    FAIL("could not close /proc/kpageflags: %s");
  }
  #endif
}

}  // namespace hashmap::utils
