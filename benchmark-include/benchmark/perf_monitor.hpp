#pragma once

#ifdef IS_LINUX
#include <linux/perf_event.h>
#endif
#include <sys/resource.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>

#include "fmt/format.h"
#include "spdlog/spdlog.h"

namespace benchmark {

/** Number of gathered hardware events from perf. */
static const uint8_t NUM_PERF_HW_CACHE_EVENTS = 11;
static const uint8_t NUM_PERF_HW_EVENTS = 7;
static const uint8_t NUM_PERF_SW_EVENTS = 4;
static const uint8_t NUM_PERF_RAW_EVENTS = 8;

static const uint8_t NUM_PERF_TOTAL_EVENTS = NUM_PERF_HW_CACHE_EVENTS + NUM_PERF_HW_EVENTS + NUM_PERF_SW_EVENTS + NUM_PERF_RAW_EVENTS;

#ifdef IS_LINUX
/** Number of options per perf type. */
static const uint8_t NUM_PERF_TYPE_OPTIONS = 3;
static constexpr std::array<std::array<uint8_t, NUM_PERF_TYPE_OPTIONS>, NUM_PERF_HW_CACHE_EVENTS> SUPPORTED_HW_CACHE_EVENTS{
    {{PERF_COUNT_HW_CACHE_L1D, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_MISS},
     {PERF_COUNT_HW_CACHE_L1D, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_ACCESS},
     {PERF_COUNT_HW_CACHE_L1D, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_RESULT_ACCESS},
     {PERF_COUNT_HW_CACHE_LL, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_MISS},
     {PERF_COUNT_HW_CACHE_LL, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_ACCESS},
     {PERF_COUNT_HW_CACHE_LL, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_RESULT_MISS},
     {PERF_COUNT_HW_CACHE_LL, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_RESULT_ACCESS},
     {PERF_COUNT_HW_CACHE_DTLB, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_MISS},
     {PERF_COUNT_HW_CACHE_DTLB, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_ACCESS},
     {PERF_COUNT_HW_CACHE_DTLB, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_RESULT_MISS},
     {PERF_COUNT_HW_CACHE_DTLB, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_RESULT_ACCESS}}};

static constexpr std::array<uint8_t, NUM_PERF_HW_EVENTS> SUPPORTED_HW_EVENTS{
    PERF_COUNT_HW_CPU_CYCLES,   PERF_COUNT_HW_REF_CPU_CYCLES,      PERF_COUNT_HW_INSTRUCTIONS, PERF_COUNT_HW_CACHE_REFERENCES,
    PERF_COUNT_HW_CACHE_MISSES, PERF_COUNT_HW_BRANCH_INSTRUCTIONS, PERF_COUNT_HW_BRANCH_MISSES};

static constexpr std::array<uint8_t, NUM_PERF_SW_EVENTS> SUPPORTED_SW_EVENTS{PERF_COUNT_SW_PAGE_FAULTS, PERF_COUNT_SW_PAGE_FAULTS_MIN,
                                                                             PERF_COUNT_SW_PAGE_FAULTS_MAJ, PERF_COUNT_SW_ALIGNMENT_FAULTS};

static constexpr uint64_t AMD_L3_DEVICE = 9;

static constexpr uint32_t AMD_ANY_CPU = static_cast<uint32_t>(-1);
static constexpr uint32_t AMD_CPU0 = 0;

static constexpr uint32_t AMD_ANY_PID = static_cast<uint32_t>(-1);
static constexpr uint32_t AMD_PID0 = 0;

static constexpr uint8_t AMD_EXCLUDE_KERNEL = 1;
static constexpr uint8_t AMD_DONT_EXCLUDE_KERNEL = 0;

static constexpr uint8_t AMD_EXCLUDE_HV = 1;
static constexpr uint8_t AMD_DONT_EXCLUDE_HV = 0;

static constexpr std::array<std::array<uint64_t, 6>, NUM_PERF_RAW_EVENTS> SUPPORTED_RAW_EVENTS{
    {{0xFE06, AMD_L3_DEVICE, AMD_CPU0, AMD_ANY_PID, AMD_DONT_EXCLUDE_KERNEL, AMD_DONT_EXCLUDE_HV},
     {0x0106, AMD_L3_DEVICE, AMD_CPU0, AMD_ANY_PID, AMD_DONT_EXCLUDE_KERNEL, AMD_DONT_EXCLUDE_HV},
     {0xFF04, AMD_L3_DEVICE, AMD_CPU0, AMD_ANY_PID, AMD_DONT_EXCLUDE_KERNEL, AMD_DONT_EXCLUDE_HV},
     {0x8001, AMD_L3_DEVICE, AMD_CPU0, AMD_ANY_PID, AMD_DONT_EXCLUDE_KERNEL, AMD_DONT_EXCLUDE_HV},
     {0x3F9A, AMD_L3_DEVICE, AMD_CPU0, AMD_ANY_PID, AMD_DONT_EXCLUDE_KERNEL, AMD_DONT_EXCLUDE_HV},
     {0x40, PERF_TYPE_RAW, AMD_ANY_CPU, AMD_PID0, AMD_EXCLUDE_KERNEL, AMD_EXCLUDE_HV},
     {0x0129, PERF_TYPE_RAW, AMD_ANY_CPU, AMD_PID0, AMD_EXCLUDE_KERNEL, AMD_EXCLUDE_HV},
     {0x0429, PERF_TYPE_RAW, AMD_ANY_CPU, AMD_PID0, AMD_EXCLUDE_KERNEL, AMD_EXCLUDE_HV}}};
#endif
/**
 * Represents the result of custom performance monitoring class. Uses rusage (cf.
 * https://man7.org/linux/man-pages/man2/getrusage.2.html) and perf (cf.
 * https://man7.org/linux/man-pages/man2/perf_event_open.2.html) to obtain measurements.
 * Currently gathered stats:
 * - user CPU time (rusage - ru_utime)
 * - system CPU time (rusage - ru_stime)
 * - page faults (rusage - ru_majflt)
 * - voluntary context switches (rusage - ru_nvcsw)
 * - Level 1 Data Cache read misses (perf - PERF_COUNT_HW_CACHE_L1D - PERF_COUNT_HW_CACHE_OP_READ -
 * PERF_COUNT_HW_CACHE_RESULT_MISS)
 * - Level 1 Data Cache read accesses (perf - PERF_COUNT_HW_CACHE_L1D - PERF_COUNT_HW_CACHE_OP_READ -
 * PERF_COUNT_HW_CACHE_RESULT_ACCESS)
 * - Level 1 Data Cache write accesses (perf - PERF_COUNT_HW_CACHE_L1D - PERF_COUNT_HW_CACHE_OP_WRITE -
 * PERF_COUNT_HW_CACHE_RESULT_ACCESS)
 * - Last-Level Cache read misses (perf - PERF_COUNT_HW_CACHE_LL - PERF_COUNT_HW_CACHE_OP_READ -
 * PERF_COUNT_HW_CACHE_RESULT_MISS)
 * - Last-Level Cache read accesses (perf - PERF_COUNT_HW_CACHE_LL - PERF_COUNT_HW_CACHE_OP_READ -
 * PERF_COUNT_HW_CACHE_RESULT_ACCESS)
 * - Last-Level Cache write misses (perf - PERF_COUNT_HW_CACHE_LL - PERF_COUNT_HW_CACHE_OP_WRITE -
 * PERF_COUNT_HW_CACHE_RESULT_MISS)
 * - Last-Level Cache write accesses (perf - PERF_COUNT_HW_CACHE_LL - PERF_COUNT_HW_CACHE_OP_WRITE -
 * PERF_COUNT_HW_CACHE_RESULT_ACCESS)
 * - Data TLB read misses (perf - PERF_COUNT_HW_CACHE_DTLB - PERF_COUNT_HW_CACHE_OP_READ -
 * PERF_COUNT_HW_CACHE_RESULT_MISS)
 * - Data TLB read accesses (perf - PERF_COUNT_HW_CACHE_DTLB - PERF_COUNT_HW_CACHE_OP_READ -
 * PERF_COUNT_HW_CACHE_RESULT_ACCESS)
 * - Data TLB write misses (perf - PERF_COUNT_HW_CACHE_DTLB - PERF_COUNT_HW_CACHE_OP_WRITE -
 * PERF_COUNT_HW_CACHE_RESULT_MISS)
 * - Data TLB write accesses (perf - PERF_COUNT_HW_CACHE_DTLB - PERF_COUNT_HW_CACHE_OP_WRITE -
 * PERF_COUNT_HW_CACHE_RESULT_ACCESS)
 */
struct PerfResult {
  /** User CPU time used - from rusage. */
  uint64_t user_time;

  /** System CPU time used - from rusage. */
  uint64_t system_time;

  /** Page faults (hard page faults) - from rusage. */
  uint64_t page_faults;

  /** Voluntary context switches - from rusage. */
  uint64_t nv_context_switches;

  /** Level 1 Data Cache read misses - from perf. */
  uint64_t l1d_read_misses;

  /** Level 1 Data Cache read accesses - from perf. */
  uint64_t l1d_read_accesses;

  /** Level 1 Data Cache write accesses - from perf. */
  uint64_t l1d_write_accesses;

  /** Last-Level Cache read misses - from perf. */
  uint64_t llc_read_misses;

  /** Last-Level Cache read accesses - from perf. */
  uint64_t llc_read_accesses;

  /** Last-Level Cache write misses - from perf. */
  uint64_t llc_write_misses;

  /** Last-Level Cache write accesses - from perf. */
  uint64_t llc_write_accesses;

  /** Data TLB read misses - from perf. */
  uint64_t data_tlb_read_misses;

  /** Data TLB read accesses - from perf. */
  uint64_t data_tlb_read_accesses;

  /** Data TLB write misses - from perf. */
  uint64_t data_tlb_write_misses;

  /** Data TLB write accesses - from perf. */
  uint64_t data_tlb_write_accesses;

  /** CPU Cycles - from perf. */
  uint64_t cycles;

  /** CPU Cycles (no scaling) - from perf. */
  uint64_t cycles_noscale;

  /** CPU Instructions - from perf. */
  uint64_t instructions;

  /** Cache References - from perf. */
  uint64_t cache_references;

  /** Cache Misses - from perf. */
  uint64_t cache_misses;

  /** Number of Branch Instructions - from perf. */
  uint64_t branches;

  /** Branch Misses - from perf. */
  uint64_t branch_misses;

  /** Software page faults - from perf. */
  uint64_t sw_page_faults;

  /** Minor page faults - from perf. */
  uint64_t minor_page_faults;

  /** Major page faults - from perf. */
  uint64_t major_page_faults;

  /** Aligenment faults - from perf. */
  uint64_t alignment_faults;

  /** AMD: other_l3_miss_typs - from perf. */
  uint64_t other_l3_miss_typs;

  /** AMD: request_miss - from perf. */
  uint64_t request_miss;

  /** AMD: all_l3_req_typs - from perf. */
  uint64_t all_l3_req_typs;

  /** AMD: caching_l3_cache_accesses - from perf. */
  uint64_t caching_l3_cache_accesses;

  /** AMD: all_l3_miss_req_typs - from perf. */
  uint64_t all_l3_miss_req_typs;

  /** AMD: ls_dc_accesses - from perf. */
  uint64_t ls_dc_accesses;

  /** AMD: ls_dispatch.ld_dispatch - from perf. */
  uint64_t ls_dispatch;

  /** AMD: ls_dispatch.ld_st_dispatch - from perf. */
  uint64_t ld_st_dispatch;

  std::string to_string() {
    return fmt::format(
        "\n -- RUSAGE Info --\nuser_time: {}\nsystem_time: {}\npage_faults: {}\nnv_context_switches: {}\n\n -- PERF Info --\ncycles: {}\ncycles "
        "(without freq. scaling impact: {}\ninstructions: {}\ncache references: {}\ncache misses: {}\nbranches: {}\nbranch misses: {}\nsw page "
        "faults: {}\nminor page faults: {}\nmajor page faults: {}\nalignment faults: {}\nl1d_read_misses: {}\nl1d_read_accesses: "
        "{}\nl1d_write_accesses: "
        "{}\nllc_read_misses: {}\nllc_read_accesses:{}\nllc_write_misses: {}\nllc_write_accesses:{}\ndata_tlb_read_misses: "
        "{}\ndata_tlb_read_accesses: {}\ndata_tlb_write_misses: {}\ndata_tlb_write_accesses: {}\n\n--AMD--\nother_l3_miss_typs (CPU0 only): "
        "{}\nrequest_miss "
        "(0x0106, L3 Misses, CPU0 only): "
        "{}\nall_l3_req_typs (0xFF04, L3 Lookups, CPU0 only): {}\ncaching_l3_cache_accesses (CPU0 only): {}\nall_l3_miss_req_typs (CPU0 only): "
        "{}\nls_dc_accesses: {}\nls_dispatch: "
        "{}\nld_st_dispatch: {}",
        user_time, system_time, page_faults, nv_context_switches, cycles, cycles_noscale, instructions, cache_references, cache_misses, branches,
        branch_misses, sw_page_faults, minor_page_faults, major_page_faults, alignment_faults, l1d_read_misses, l1d_read_accesses, l1d_write_accesses,
        llc_read_misses, llc_read_accesses, llc_write_misses, llc_write_accesses, data_tlb_read_misses, data_tlb_read_accesses, data_tlb_write_misses,
        data_tlb_write_accesses, other_l3_miss_typs, request_miss, all_l3_req_typs, caching_l3_cache_accesses, all_l3_miss_req_typs, ls_dc_accesses,
        ls_dispatch, ld_st_dispatch);
  }

  void benchmark_print() {
    std::cout << cycles << std::endl;
    std::cout << instructions << std::endl;
    std::cout << cache_references << std::endl;
    std::cout << cache_misses << std::endl;
    std::cout << l1d_read_accesses << std::endl;
    std::cout << l1d_read_misses << std::endl;
    std::cout << llc_read_accesses << std::endl;
    std::cout << llc_read_misses << std::endl;
    std::cout << data_tlb_read_accesses << std::endl;
    std::cout << data_tlb_read_misses << std::endl;
    std::cout << all_l3_req_typs << std::endl;
    std::cout << request_miss << std::endl;
  }
};

class PerfMonitor {
 public:
  PerfMonitor() : rusage_begin_(), rusage_end_(), event_fds_{-1} {};

  // Also delete move c'tor as long as it is not needed
  PerfMonitor operator=(PerfMonitor&& other) noexcept = delete;
  PerfMonitor(PerfMonitor&& other) noexcept = delete;

  ~PerfMonitor();

  void start();
  void stop();
  [[nodiscard]] PerfResult get_result() const;

 private:
  /**  Returns timeval struct diff of rusage struct as milliseconds. */
  [[nodiscard]] static uint64_t rusage_timeval_diff(const timeval& end, const timeval& begin);

  rusage rusage_begin_;
  rusage rusage_end_;
  std::array<int, NUM_PERF_TOTAL_EVENTS> event_fds_;
};

}  // namespace benchmark