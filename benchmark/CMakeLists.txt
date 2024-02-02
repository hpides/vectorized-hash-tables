set(BENCHMARK_SOURCES
        src/benchmark_shared.cpp
        src/benchmark_utils.cpp
        src/perf_monitor.cpp
        src/read_benchmark.cpp
        src/write_benchmark.cpp
)

set(BENCHMARK_HEADERS
        ../benchmark-include/benchmark/benchmark_hashmaps_a.hpp
        ../benchmark-include/benchmark/benchmark_hashmaps_b.hpp
        ../benchmark-include/benchmark/benchmark_hashmaps_c.hpp
        ../benchmark-include/benchmark/benchmark_hashmaps_d.hpp
        ../benchmark-include/benchmark/benchmark_hashmaps_f.hpp
        ../benchmark-include/benchmark/benchmark_hashmaps_g.hpp
        ../benchmark-include/benchmark/benchmark_hashmaps_h.hpp
        ../benchmark-include/benchmark/benchmark_hashmaps_i.hpp
        ../benchmark-include/benchmark/benchmark_hashmaps_rep.hpp
        ../benchmark-include/benchmark/benchmark_shared.hpp
        ../benchmark-include/benchmark/benchmark_utils.hpp
        ../benchmark-include/benchmark/perf_monitor.hpp
        ../benchmark-include/benchmark/read_benchmark.hpp
        ../benchmark-include/benchmark/write_benchmark.hpp
)

get_target_property(HASHMAP_INCLUDES hashmap INCLUDE_DIRECTORIES)

set(BENCHMARK_TARGETS "hashmap-read-benchmark;hashmap-write-benchmark;hashmap-benchmark-profiler-app;hashmap-alignedness-benchmark;hashmap-hugepage-benchmark")
foreach (benchmark_target ${BENCHMARK_TARGETS})
        target_include_directories(${benchmark_target} PRIVATE ${HASHMAP_INCLUDES})
        target_include_directories(${benchmark_target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../benchmark-include/)

        target_sources(${benchmark_target} PRIVATE ${BENCHMARK_SOURCES})
        target_sources(${benchmark_target} PRIVATE ${BENCHMARK_HEADERS})

        target_link_libraries(${benchmark_target} PUBLIC fmt spdlog hedley)
endforeach()
