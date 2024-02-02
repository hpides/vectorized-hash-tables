cmake_minimum_required(VERSION 3.16)

project(multiarch-hashmap)
set(CMAKE_CXX_STANDARD 20)

set(HASHMAP_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# This is not the default because Folly/Abseil fiddle around with global compiler flags
option(HASHMAP_BUILD_EXTERNAL "Set ON to build external hashmaps (Abseil, Folly, Martinus RH)" OFF)
option(HASHMAP_ONLY_BENCH_EXTERNAL "Set ON if you only want to benchmark external hashmaps" OFF)
option(HASHMAP_ONLY_BENCH_SIMD "Set ON if you only want to benchmark SIMD hashmaps" OFF)

# Handle meta info
option(HASHMAP_COLLECT_META_INFO "Set ON if you want to collect meta information, e.g., #probes" OFF)
if (${HASHMAP_COLLECT_META_INFO})
    message(STATUS "Collecting meta information, don't use this for maximum performance.")
    add_compile_definitions(HASHMAP_COLLECT_META_INFO=1)
endif()

# Larger matrix
option(HASHMAP_LARGE_MATRIX "Set ON if you want to run a larger benchmark matrix" OFF)
if (${HASHMAP_LARGE_MATRIX})
    message(STATUS "Running benchmarks with large matrix.")
    add_compile_definitions(HASHMAP_LARGE_MATRIX=1)
endif()

# Hash functions
option(HASHMAP_HASHFUNCTIONS "Set ON if you want to benchmark the hash functions" OFF)
if (${HASHMAP_HASHFUNCTIONS})
    message(STATUS "Running hash function benchmarks.")
    add_compile_definitions(HASHMAP_HASHFUNCTIONS=1)
endif()

# String keys
add_compile_definitions(HASHMAP_STRINGKEY_SIZE=32)
option(HASHMAP_STRINGKEYS "Set ON if you want to benchmark string keys" OFF)
if (${HASHMAP_STRINGKEYS})
    message(STATUS "Running string key benchmarks with 32 character strings.")
    add_compile_definitions(HASHMAP_STRINGKEYS=1)
endif()

# Pointer values
option(HASHMAP_POINTERVALUES "Set ON if you want to benchmark values as pointers (hash index)" OFF)
if (${HASHMAP_POINTERVALUES})
    message(STATUS "Running pointer value benchmarks.")
    add_compile_definitions(HASHMAP_POINTERVALUES=1)
endif()

# Large values
option(HASHMAP_LARGEVALUES "Set ON if you want to benchmark large values (64 byte, stream processing tuples)" OFF)
if (${HASHMAP_LARGEVALUES})
    message(STATUS "Running large value benchmarks.")
    add_compile_definitions(HASHMAP_LARGEVALUES=1)
endif()

# Dense keys
option(HASHMAP_DENSEKEYS "Set ON if you want to benchmark dense keys" OFF)
if (${HASHMAP_DENSEKEYS})
    message(STATUS "Running dense keys benchmarks.")
    add_compile_definitions(HASHMAP_DENSEKEYS=1)
endif()

# Prefetching
option(HASHMAP_PREFETCHING "Set ON if you want to benchmark prefetching" OFF)
if (${HASHMAP_PREFETCHING})
    message(STATUS "Running prefetching benchmarks.")
    add_compile_definitions(HASHMAP_PREFETCHING=1)
endif()

# Scalar Impl
option(HASHMAP_USE_SCALAR_IMPL "Set ON if you want to use the scalar implementation of the SIMD Utils" OFF)
if (${HASHMAP_USE_SCALAR_IMPL})
    message(STATUS "Using scalar impl.")
    add_compile_definitions(HASHMAP_USE_SCALAR_IMPL=1)
endif()

option(HASHMAP_BENCHMARK_FALLBACK "Set ON if you want to run the fallback benchmark" OFF)
if (${HASHMAP_BENCHMARK_FALLBACK})
    message(STATUS "Running fallback benchmarks.")
    add_compile_definitions(HASHMAP_BENCHMARK_FALLBACK=1)
endif()

option(HASHMAP_BENCHMARK_CHAINED_ONLY "Set ON if you want to run the chained only benchmarks" OFF)
if (${HASHMAP_BENCHMARK_CHAINED_ONLY})
    message(STATUS "Running chained only BMs.")
    add_compile_definitions(HASHMAP_BENCHMARK_CHAINED_ONLY=1)
endif()

option(HASHMAP_MT "Set ON if you want to run the MT benchmarks" OFF)
if (${HASHMAP_MT})
    message(STATUS "Running MT BMs.")
    add_compile_definitions(HASHMAP_MT=1)
endif()

option(HASHMAP_ZIPF "Set ON if you want to run the zipf benchmarks" OFF)
if (${HASHMAP_ZIPF})
    message(STATUS "Running Zipf BMs.")
    add_compile_definitions(HASHMAP_ZIPF=1)
endif()

option(HASHMAP_REPR "Set ON if you want to run the reproducibility benchmarks" OFF)
if (${HASHMAP_REPR})
    message(STATUS "Running REPR BMs.")
    add_compile_definitions(HASHMAP_REPR=1)
endif()


#### INTERNAL OPTIONS ####
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE)
    string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
else()
    message(STATUS "No build type given, defaulting to release.")
    set(BUILD_TYPE "RELEASE")
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_compile_definitions(IS_LINUX=1)
    message(STATUS "Found Linux system. Automatically detecting system info.")
    ### Get cacheline size ###
    execute_process(COMMAND getconf LEVEL1_DCACHE_LINESIZE OUTPUT_VARIABLE CACHELINE_SIZE OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Obtained cache line size ${CACHELINE_SIZE} from `getconf`")

    ### Get regular size ###
    execute_process(COMMAND getconf PAGESIZE OUTPUT_VARIABLE PAGE_SIZE OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Obtained regular page size ${PAGE_SIZE} from `getconf`")
    add_compile_definitions(PAGE_SIZE=${PAGE_SIZE})

    ### Get hugepage size ###
    execute_process(COMMAND bash -c "cat /proc/meminfo | grep Hugepagesize | cut -d' ' -f2- | tr -d ' \t\n\r' | awk '{ print toupper($0) }' | numfmt --from=iec --suffix=B  | sed 's/[^0-9]*//g'" OUTPUT_VARIABLE HUGEPAGE_SIZE OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Obtained huge page size ${HUGEPAGE_SIZE} from `/proc/meminfo`")
    add_compile_definitions(HUGEPAGE_SIZE=${HUGEPAGE_SIZE})

else ()
  message(STATUS "Found non-Linux system ${CMAKE_SYSTEM_NAME}, using hard-coded default values. This platform should just be used for development purposes.")
  set(CACHELINE_SIZE 64)
  set(PAGE_SIZE 4096)
  set(HUGEPAGE_SIZE 2097152)  # 2MiB
  message(STATUS "Using scalar impl.")
  add_compile_definitions(HASHMAP_USE_SCALAR_IMPL=1)
endif ()


### Compile Flags for gcc/clang ###
add_compile_definitions(COMPILER_IDENTIFIER=${CMAKE_CXX_COMPILER_ID})

### Macro for external ###
if (${HASHMAP_BUILD_EXTERNAL})
    add_compile_definitions(HASHMAP_BUILD_EXTERNAL=1)
    set(HASHMAP_EXTERNAL_TEMP_DIR ${HASHMAP_CMAKE_DIR}/tmp_external)
endif()

if (${HASHMAP_ONLY_BENCH_EXTERNAL})
    if (NOT ${HASHMAP_BUILD_EXTERNAL})
        message(FATAL_ERROR "HASHMAP_ONLY_BENCH_EXTERNAL can only be used if HASHMAP_BUILD_EXTERNAL is activated.")
    endif()
    add_compile_definitions(HASHMAP_ONLY_BENCH_EXTERNAL=1)
endif()

if (${HASHMAP_ONLY_BENCH_SIMD})
    add_compile_definitions(HASHMAP_ONLY_BENCH_SIMD=1)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Building with gcc or clang.")
    add_compile_options(-Wall -Wextra -Wpedantic -Wfloat-equal -Wconversion -Wno-sign-compare)

    if ((NOT ${HASHMAP_BUILD_EXTERNAL}) AND (${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
        # Unfortunately, the external libraries (especially Folly) have too many warnings that we cannot silence
        # Hence, we only do Werror if we are not building these
        # Furthermore, mac has some more annoying errors in external libs
        add_compile_options(-Werror -Wno-interference-size)
    endif()

    # Figure out relase flags
    EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    if(${ARCHITECTURE} STREQUAL "x86_64")
        message(STATUS "Assuming build is happening on x86_64 system.")
        set(HASHMAP_ARCHITECTURE_RELEASE_FLAGS -march=native)
	    set(HASHMAP_ARCHITECTURE_DEBUG_FLAGS -fcf-protection -march=native)
        set(DEFAULT_WORKLOAD 1024)
        add_compile_definitions(HASHMAP_IS_X86=1)
        set(HASHMAP_IS_X86 1)
    elseif((${ARCHITECTURE} STREQUAL "aarch64"))
        message(STATUS "Assuming build is happening on ARM system.")
        set(HASHMAP_ARCHITECTURE_RELEASE_FLAGS -mcpu=native)
	    set(HASHMAP_ARCHITECTURE_DEBUG_FLAGS -mcpu=native)
        set(DEFAULT_WORKLOAD 1024)
        add_compile_definitions(HASHMAP_IS_ARM=1)
        set(HASHMAP_IS_ARM 1)

        message(STATUS "Checking for SVE availability and size.")
	    try_run(HASHMAP_IGNORE_THIS HASHMAP_HAS_SVE ${CMAKE_CURRENT_BINARY_DIR}/compile_tests ${HASHMAP_CMAKE_DIR}/compile_tests/sve_size.cpp RUN_OUTPUT_VARIABLE SVE_REGISTER_SIZE  COMPILE_DEFINITIONS ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})

        if(${HASHMAP_HAS_SVE})
            message(STATUS "Obtained SVE size ${SVE_REGISTER_SIZE}")
        else()
            message(STATUS "SVE not supported on this system.")
        endif()

        ### HPE Apollo 80 - Reduce Workload ###
        execute_process(COMMAND bash -c "gcc -mcpu=native -Q --help=target | grep mtune | awk '{print $2}' | xargs" OUTPUT_VARIABLE ARM_CPU OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(${ARM_CPU} STREQUAL "a64fx")
            set(DEFAULT_WORKLOAD 256)
            message(STATUS "Found A64FX CPU, reduced workload to 256.")    
        endif()

    elseif(${ARCHITECTURE} STREQUAL "ppc64le")
        message(STATUS "Assuming build is happening on Power system.")
        set(HASHMAP_ARCHITECTURE_RELEASE_FLAGS -mcpu=native -mtune=native)
	    set(HASHMAP_ARCHITECTURE_DEBUG_FLAGS -mcpu=native -mtune=native)
        set(DEFAULT_WORKLOAD 1024)
	    add_compile_definitions(HASHMAP_IS_POWER=1)
        set(HASHMAP_IS_POWER 1)

        ### Power9 Cacheline Size Fix ###
        execute_process(COMMAND bash -c "gcc -mcpu=native -mtune=native -Q --help=target | grep mtune | awk '{print $2}' | xargs | cut -d ' ' -f1" OUTPUT_VARIABLE POWER_CPU OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(${POWER_CPU} STREQUAL "power9")
            set(CACHELINE_SIZE 128)	
            message(STATUS "Found Power9 CPU, fixing cache line size to be 128.")    
        else()
            message(STATUS "Unknown Power CPU: ${POWER_CPU}. Double-check the cache line size.")
        endif()

    else()
        set(DEFAULT_WORKLOAD 1024)
        message(STATUS "Unknown platform ${ARCHITECTURE}. Not using any native flags.")
    endif()

    if (${BUILD_TYPE} MATCHES "DEBUG")
        message(STATUS "Building in Debug mode.")
        if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    	    add_compile_options(-O0 -g3 -ggdb3 -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all -fstack-protector-strong -fstack-clash-protection -D_GLIBCXX_ASSERTIONS ${HASHMAP_ARCHITECTURE_DEBUG_FLAGS})
            add_link_options(-fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
        else()
            add_compile_options(-O0 -g3 ${HASHMAP_ARCHITECTURE_DEBUG_FLAGS})
            add_link_options(-fno-omit-frame-pointer)
        endif()
    elseif(${BUILD_TYPE} MATCHES "RELEASE")
        message(STATUS "Building in Release mode.")
        add_compile_options(-O3 -flto ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
        add_link_options(-flto)
    elseif(${BUILD_TYPE} MATCHES "REL_NONATIVE")
        message(STATUS "Building in Release mode without native flags.")
        add_compile_options(-O3 -flto)
        add_link_options(-flto)
    elseif(${BUILD_TYPE} MATCHES "REL_NOLTO")
        message(STATUS "Building in NOLTO Release mode.")
        add_compile_options(-O3 ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
    elseif(${BUILD_TYPE} MATCHES "PROFILE_VTUNE")
        message(STATUS "Building in Profile for VTUNE mode.")
        add_compile_options(-O2 -flto -g3 -ggdb3 -fno-omit-frame-pointer ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
        add_link_options(-flto)
        add_compile_definitions(HASHMAP_IS_VTUNEPROFILE_BUILD)
        add_compile_definitions(HASHMAP_IS_PROFILE_BUILD)
        include_directories(SYSTEM /opt/intel/oneapi/vtune/latest/sdk/include)
        link_directories(/opt/intel/oneapi/vtune/latest/sdk/lib32)
        link_directories(/opt/intel/oneapi/vtune/latest/sdk/lib64)
        link_libraries(ittnotify dl)
    elseif(${BUILD_TYPE} MATCHES "PROFILE_UPROF")
        message(STATUS "Building in Profile for UPROF mode.")
        add_compile_options(-O2 -flto -g3 -ggdb3 -fno-omit-frame-pointer ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
        add_link_options(-flto)
        add_compile_definitions(HASHMAP_IS_UPROF_BUILD)
        add_compile_definitions(HASHMAP_IS_PROFILE_BUILD)
        include_directories(SYSTEM /opt/AMDuProf_3.4-475/include)
        link_directories(/opt/AMDuProf_3.4-475/lib/x64)
        link_libraries(AMDProfileController rt)
    elseif(${BUILD_TYPE} MATCHES "PROFILE_VALGRIND")
        message(STATUS "Building in Profile for VALGRIND mode.")
        add_compile_options(-O2 -flto -g3 -ggdb3 -fno-omit-frame-pointer ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
        add_link_options(-flto)
        add_compile_definitions(HASHMAP_IS_PROFILE_BUILD)
        add_compile_definitions(HASHMAP_IS_VALGRINDPROFILE_BUILD)
    elseif(${BUILD_TYPE} MATCHES "PROFILE_GPROF")
        message(STATUS "Building in Profile for GPROF mode.")
        add_compile_options(-O2 -flto -gstrict-dwarf -gdwarf-3 -pg -fno-omit-frame-pointer ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
        add_link_options(-flto -gstrict-dwarf -gdwarf-3 -pg)
        add_compile_definitions(HASHMAP_IS_PROFILE_BUILD)
        add_compile_definitions(HASHMAP_IS_GPROFPROFILE_BUILD)
    elseif(${BUILD_TYPE} MATCHES "PROFILE_PERF")
        message(STATUS "Building in perf mode.")
        add_compile_options(-O2 -g3 -ggdb3 -flto -fno-omit-frame-pointer ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
        add_link_options(-flto)
        add_compile_definitions(HASHMAP_IS_PROFILE_BUILD)
        add_compile_definitions(HASHMAP_IS_PERFPROFILE_BUILD)
    elseif(${BUILD_TYPE} MATCHES "PROFILE_FASTPERF")
        message(STATUS "Building in fast perf mode (perf with o3).")
        add_compile_options(-O3 -flto ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
        add_link_options(-flto)
        add_compile_definitions(HASHMAP_IS_PROFILE_BUILD)
        add_compile_definitions(HASHMAP_IS_PERFPROFILE_BUILD)
    elseif(${BUILD_TYPE} MATCHES "PROFILE")
        message(STATUS "Building in general purpose profile/RelWithDebugInfo mode.")
        add_compile_options(-O2 -g3 -ggdb3 -flto -fno-omit-frame-pointer ${HASHMAP_ARCHITECTURE_RELEASE_FLAGS})
        add_link_options(-flto)
        add_compile_definitions(HASHMAP_IS_PROFILE_BUILD)
    else()
        message(FATAL_ERROR "Unknown build mode ${BUILD_TYPE}.")
    endif()
else()
    # Fujitsu/cray/icc/xlc Flags might be different, we fail until we have validated this.
    message(FATAL_ERROR "Unknown compiler ${CMAKE_CXX_COMPILER_ID}.")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Allow spdlog and gcem and the tests to build with clang...
    # TODO: This can probably be refactored to something similar we do with abseil
    add_compile_options(-Wno-deprecated-copy -Wno-sign-conversion -Wno-gnu-zero-variadic-macro-arguments)
endif()

#### Support custom compiler ####
if (DEFINED ENV{NM})
    set(CMAKE_NM $ENV{NM})	
    message(STATUS "Found NM environment variable, set CMAKE_NM to ${CMAKE_NM}")
endif()

if (DEFINED ENV{AR})
    set(CMAKE_AR $ENV{AR})	
    message(STATUS "Found AR environment variable, set CMAKE_AR to ${CMAKE_AR}")
endif()

if (DEFINED ENV{RANLIB})
    set(CMAKE_RANLIB $ENV{RANLIB})	
    message(STATUS "Found RANLIB environment variable, set CMAKE_RANLIB to ${CMAKE_RANLIB}")
endif()

if (DEFINED ENV{SVE_REGISTER_SIZE})
    set(SVE_REGISTER_SIZE $ENV{SVE_REGISTER_SIZE})	
    message(STATUS "Found SVE_REGISTER_SIZE environment variable, set SVE_REGISTER_SIZE to ${SVE_REGISTER_SIZE}")
endif()

add_compile_definitions(CACHELINE_SIZE=${CACHELINE_SIZE})
add_compile_definitions(DEFAULT_WORKLOAD=${DEFAULT_WORKLOAD})

if (DEFINED SVE_REGISTER_SIZE)
    add_compile_definitions(SVE_REGISTER_SIZE=${SVE_REGISTER_SIZE})	
endif()

#### HASHMAP LIB ####
add_library(hashmap)
target_include_directories(hashmap INTERFACE include/
                                  PRIVATE   include/ src/)
add_subdirectory(src/)

#### Benchmarks ####
add_executable(hashmap-read-benchmark benchmark/app/read_bm_app.cpp)
target_link_libraries(hashmap-read-benchmark PRIVATE hashmap)

add_executable(hashmap-write-benchmark benchmark/app/write_bm_app.cpp)
target_link_libraries(hashmap-write-benchmark PRIVATE hashmap)

add_executable(hashmap-benchmark-profiler-app benchmark/app/profile_app.cpp)
target_link_libraries(hashmap-benchmark-profiler-app PRIVATE hashmap)

add_executable(hashmap-alignedness-benchmark benchmark/app/alignedness-bench.cpp)
target_link_libraries(hashmap-alignedness-benchmark PRIVATE hashmap)
file(COPY benchmark/app/run_alignedness_bench.sh
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(hashmap-hugepage-benchmark benchmark/app/hugepage-bench.cpp)
target_link_libraries(hashmap-hugepage-benchmark PRIVATE hashmap)
file(COPY benchmark/app/run_hugepage_bench.sh
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
     
add_subdirectory(benchmark/)

#### Playground ####
add_executable(playground playground/main.cpp)
target_link_libraries(playground PRIVATE hashmap)

##### TESTING #####
enable_testing()
add_subdirectory(test)
