#include "benchmark/perf_monitor.hpp"

#ifdef IS_LINUX
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "hashmap/utils.hpp"
#include "spdlog/spdlog.h"

using namespace hashmap::utils;

namespace benchmark {

PerfMonitor::~PerfMonitor() {
  for (const int fd : event_fds_) {
    if (fd > 0) {
      ::close(fd);
    }
  }
}

void PerfMonitor::start() {
  // cf. https://man7.org/linux/man-pages/man2/perf_event_open.2.html
  perf_event_attr pe;
  std::memset(&pe, 0, sizeof(pe));
  pe.size = sizeof(pe);
  pe.disabled = 1;
  pe.exclude_kernel = 1;
  pe.exclude_hv = 1;
  pe.type = PERF_TYPE_HW_CACHE;

  uint8_t fd_id = 0;

  for (uint8_t i = 0; i < NUM_PERF_HW_CACHE_EVENTS; ++i) {
    pe.config = (SUPPORTED_HW_CACHE_EVENTS[i][0]) | (SUPPORTED_HW_CACHE_EVENTS[i][1] << 8) | (SUPPORTED_HW_CACHE_EVENTS[i][2] << 16);
    // File descriptors are ints
    event_fds_[fd_id] = static_cast<int>(::syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0));
    if (event_fds_[fd_id] < 2) {  // 0,1,2 is stdin, stdout, and stderr respectively
      spdlog::error(fmt::format("Error during perf event open of hw cache event {} (perf id = {}): {}", i, SUPPORTED_HW_CACHE_EVENTS[i][0],
                                std::strerror(errno)));
    }
    ++fd_id;
  }

  pe.type = PERF_TYPE_HARDWARE;
  for (uint8_t i = 0; i < NUM_PERF_HW_EVENTS; ++i) {
    pe.config = SUPPORTED_HW_EVENTS[i];
    // File descriptors are ints
    event_fds_[fd_id] = static_cast<int>(::syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0));

    if (event_fds_[fd_id] < 2) {  // 0,1,2 is stdin, stdout, and stderr respectively
      spdlog::error(fmt::format("Error during perf event open of hw event {} (perf id = {}): {}", i, SUPPORTED_HW_EVENTS[i], std::strerror(errno)));
    }
    ++fd_id;
  }

  pe.type = PERF_TYPE_SOFTWARE;
  for (uint8_t i = 0; i < NUM_PERF_SW_EVENTS; ++i) {
    pe.config = SUPPORTED_SW_EVENTS[i];
    // File descriptors are ints
    event_fds_[fd_id] = static_cast<int>(::syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0));
    if (event_fds_[fd_id] < 2) {  // 0,1,2 is stdin, stdout, and stderr respectively
      spdlog::error(fmt::format("Error during perf event open of sw event {} (perf id = {}): {}", i, SUPPORTED_SW_EVENTS[i], std::strerror(errno)));
    }
    ++fd_id;
  }

  for (uint8_t i = 0; i < NUM_PERF_RAW_EVENTS; ++i) {
    pe.config = SUPPORTED_RAW_EVENTS[i][0];
    pe.type = static_cast<uint32_t>(SUPPORTED_RAW_EVENTS[i][1]);
    pe.exclude_kernel = static_cast<typeof(pe.exclude_kernel)>(SUPPORTED_RAW_EVENTS[i][4]) & 0x00000001;
    pe.exclude_hv = static_cast<typeof(pe.exclude_hv)>(SUPPORTED_RAW_EVENTS[i][5]) & 0x00000001;

    // File descriptors are ints
    event_fds_[fd_id] = static_cast<int>(::syscall(__NR_perf_event_open, &pe, static_cast<int32_t>(SUPPORTED_RAW_EVENTS[i][3]),
                                                   static_cast<int32_t>(SUPPORTED_RAW_EVENTS[i][2]), -1, 0));
    if (event_fds_[fd_id] < 2) {  // 0,1,2 is stdin, stdout, and stderr respectively
      spdlog::error(fmt::format("Error during perf event open of raw event {} with type {} (perf id = {}): {}", i, pe.type,
                                SUPPORTED_RAW_EVENTS[i][0], std::strerror(errno)));
    }
    ++fd_id;
  }

  // Starts measuring HW counters
  for (const auto& fd : event_fds_) {
    ::ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ::ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
  }

  if (::getrusage(RUSAGE_SELF, &rusage_begin_) < 0) {
    spdlog::error(fmt::format("Could not get resource usage: {}", std::strerror(errno)));
  }

  spdlog::info("Perf measurement started.");
}

void PerfMonitor::stop() {
  if (::getrusage(RUSAGE_SELF, &rusage_end_) < 0) {
    spdlog::error(fmt::format("Could not get resource usage: {}", std::strerror(errno)));
  }

  // Stops measuring HW counters
  for (const auto& fd : event_fds_) {
    ::ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
  }

  spdlog::info("Perf measurement stopped.");
}

PerfResult PerfMonitor::get_result() const {
  PerfResult result{};
  // Seconds and milliseconds of user and system time
  result.user_time = rusage_timeval_diff(rusage_end_.ru_utime, rusage_begin_.ru_utime);
  result.system_time = rusage_timeval_diff(rusage_end_.ru_stime, rusage_begin_.ru_stime);
  result.page_faults = rusage_end_.ru_majflt - rusage_begin_.ru_majflt;
  result.nv_context_switches = rusage_end_.ru_nvcsw - rusage_begin_.ru_nvcsw;
  ssize_t bytes_read;

  bytes_read = ::read(event_fds_[0], &result.l1d_read_misses, sizeof(result.l1d_read_misses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `l1d_read_misses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[1], &result.l1d_read_accesses, sizeof(result.l1d_read_accesses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `l1d_read_accesses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[2], &result.l1d_write_accesses, sizeof(result.l1d_write_accesses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `l1d_write_accesses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[3], &result.llc_read_misses, sizeof(result.llc_read_misses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `llc_read_misses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[4], &result.llc_read_accesses, sizeof(result.llc_read_accesses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `llc_read_accesses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[5], &result.llc_write_misses, sizeof(result.llc_write_misses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `llc_write_misses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[6], &result.llc_write_accesses, sizeof(result.llc_write_accesses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `llc_write_accesses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[7], &result.data_tlb_read_misses, sizeof(result.data_tlb_read_misses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `data_tlb_read_misses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[8], &result.data_tlb_read_accesses, sizeof(result.data_tlb_read_accesses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `data_tlb_read_accesses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[9], &result.data_tlb_write_misses, sizeof(result.data_tlb_write_misses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `data_tlb_write_misses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[10], &result.data_tlb_write_accesses, sizeof(result.data_tlb_write_accesses));
  if (bytes_read != sizeof(result.data_tlb_read_misses)) {
    spdlog::error(fmt::format("Could not obtain `data_tlb_write_accesses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[11], &result.cycles, sizeof(result.cycles));
  if (bytes_read != sizeof(result.cycles)) {
    spdlog::error(fmt::format("Could not obtain `cycles` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[12], &result.cycles_noscale, sizeof(result.cycles_noscale));
  if (bytes_read != sizeof(result.cycles_noscale)) {
    spdlog::error(fmt::format("Could not obtain `cycles_noscale` HW counter: {}", std::strerror(errno)));
  }
  bytes_read = ::read(event_fds_[13], &result.instructions, sizeof(result.instructions));
  if (bytes_read != sizeof(result.instructions)) {
    spdlog::error(fmt::format("Could not obtain `instructions` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[14], &result.cache_references, sizeof(result.cache_references));
  if (bytes_read != sizeof(result.cache_references)) {
    spdlog::error(fmt::format("Could not obtain `cache_references` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[15], &result.cache_misses, sizeof(result.cache_misses));
  if (bytes_read != sizeof(result.cache_misses)) {
    spdlog::error(fmt::format("Could not obtain `cache_misses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[16], &result.branches, sizeof(result.branches));
  if (bytes_read != sizeof(result.branches)) {
    spdlog::error(fmt::format("Could not obtain `branches` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[17], &result.branch_misses, sizeof(result.branch_misses));
  if (bytes_read != sizeof(result.branch_misses)) {
    spdlog::error(fmt::format("Could not obtain `branch_misses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[18], &result.sw_page_faults, sizeof(result.sw_page_faults));
  if (bytes_read != sizeof(result.sw_page_faults)) {
    spdlog::error(fmt::format("Could not obtain `sw_page_faults` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[19], &result.minor_page_faults, sizeof(result.minor_page_faults));
  if (bytes_read != sizeof(result.minor_page_faults)) {
    spdlog::error(fmt::format("Could not obtain `minor_page_faults` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[20], &result.major_page_faults, sizeof(result.major_page_faults));
  if (bytes_read != sizeof(result.major_page_faults)) {
    spdlog::error(fmt::format("Could not obtain `major_page_faults` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[21], &result.alignment_faults, sizeof(result.alignment_faults));
  if (bytes_read != sizeof(result.alignment_faults)) {
    spdlog::error(fmt::format("Could not obtain `alignment_faults` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[22], &result.other_l3_miss_typs, sizeof(result.other_l3_miss_typs));
  if (bytes_read != sizeof(result.other_l3_miss_typs)) {
    spdlog::error(fmt::format("Could not obtain `other_l3_miss_typs` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[23], &result.request_miss, sizeof(result.request_miss));
  if (bytes_read != sizeof(result.request_miss)) {
    spdlog::error(fmt::format("Could not obtain `request_miss` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[24], &result.all_l3_req_typs, sizeof(result.all_l3_req_typs));
  if (bytes_read != sizeof(result.all_l3_req_typs)) {
    spdlog::error(fmt::format("Could not obtain `all_l3_req_typs` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[25], &result.caching_l3_cache_accesses, sizeof(result.caching_l3_cache_accesses));
  if (bytes_read != sizeof(result.all_l3_req_typs)) {
    spdlog::error(fmt::format("Could not obtain `caching_l3_cache_accesses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[26], &result.all_l3_miss_req_typs, sizeof(result.all_l3_miss_req_typs));
  if (bytes_read != sizeof(result.all_l3_req_typs)) {
    spdlog::error(fmt::format("Could not obtain `all_l3_miss_req_typs` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[27], &result.ls_dc_accesses, sizeof(result.ls_dc_accesses));
  if (bytes_read != sizeof(result.ls_dc_accesses)) {
    spdlog::error(fmt::format("Could not obtain `ls_dc_accesses` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[28], &result.ls_dispatch, sizeof(result.ls_dispatch));
  if (bytes_read != sizeof(result.ls_dispatch)) {
    spdlog::error(fmt::format("Could not obtain `ls_dispatch` HW counter: {}", std::strerror(errno)));
  }

  bytes_read = ::read(event_fds_[29], &result.ld_st_dispatch, sizeof(result.ld_st_dispatch));
  if (bytes_read != sizeof(result.ld_st_dispatch)) {
    spdlog::error(fmt::format("Could not obtain `ld_st_dispatch` HW counter: {}", std::strerror(errno)));
  }

  return result;
}

uint64_t PerfMonitor::rusage_timeval_diff(const timeval& end, const timeval& begin) {
  // Return value is scaled to milliseconds
  return ((end.tv_sec * 1'000'000) + end.tv_usec) - ((begin.tv_sec * 1'000'000) + begin.tv_usec);
}

}  // namespace benchmark
#endif
