set(HASHMAP_SOURCES
        misc/stringkey.cpp
        utils.cpp
)

set(HASHMAP_HEADERS
        ../include/hashmap/entries/aosentry.hpp
        ../include/hashmap/hashes/buckethash.hpp
        ../include/hashmap/hashes/hasher.hpp
        ../include/hashmap/hashes/identity.hpp
        ../include/hashmap/hashes/multaddshifthasher.hpp
        ../include/hashmap/hashes/multshifthasher.hpp
        ../include/hashmap/hashes/murmurhasher.hpp
        ../include/hashmap/hashes/statichasher.hpp
        ../include/hashmap/hashes/stdhasher.hpp
        ../include/hashmap/hashes/xxhasher.hpp
        ../include/hashmap/hashmaps/abseil.hpp
        ../include/hashmap/hashmaps/bucketing_simd.hpp
        ../include/hashmap/hashmaps/chained.hpp
        ../include/hashmap/hashmaps/f14.hpp
        ../include/hashmap/hashmaps/fingerprinting_simd_soa.hpp
        ../include/hashmap/hashmaps/hashmap.hpp
        ../include/hashmap/hashmaps/linear_probing_aos.hpp
        ../include/hashmap/hashmaps/linear_probing_soa_packed.hpp
        ../include/hashmap/hashmaps/linear_probing_soa.hpp
        ../include/hashmap/hashmaps/martinus.hpp
        ../include/hashmap/hashmaps/quadratic_probing_aos.hpp
        ../include/hashmap/hashmaps/recalc_robin_hood_aos.hpp
        ../include/hashmap/hashmaps/simple_simd_soa.hpp
        ../include/hashmap/hashmaps/storing_robin_hood_aos.hpp
        ../include/hashmap/misc/stringkey.hpp
        ../include/hashmap/simd_utils.hpp
        ../include/hashmap/utils.hpp
)

target_sources(hashmap PRIVATE ${HASHMAP_SOURCES})
target_sources(hashmap PRIVATE ${HASHMAP_HEADERS})

target_include_directories(hashmap PUBLIC ../include)

include(${HASHMAP_CMAKE_DIR}/dependencies.cmake)
target_link_libraries(hashmap PUBLIC fmt spdlog hedley INTERFACE gcem xxHash::xxhash)

if (${HASHMAP_BUILD_EXTERNAL})
   message(STATUS "External hashmaps are _enabled_")
   target_link_libraries(hashmap INTERFACE absl::flat_hash_map robin-hood-hashmap)
   if (NOT DEFINED HASHMAP_IS_POWER)
	   message("Not on Power, hence we can link folly")
	   target_link_libraries(hashmap INTERFACE folly)
   endif()
endif()

# Copied from Darwin:
# This adds a `INCLUDE_DIRECTORIES` definition containing all include directories, separate by comma.
# The definition is set to PRIVATE, so it will not be exposed if the target is itself a dependency.
set(INCLUDE_EXPR "$<TARGET_PROPERTY:hashmap,INCLUDE_DIRECTORIES>")
set(INCLUDE_FILTER "$<FILTER:${INCLUDE_EXPR},INCLUDE,.*/.*>")
set(INCLUDE_JOINED "$<JOIN:${INCLUDE_FILTER},,>")
target_compile_definitions(hashmap PRIVATE "INCLUDE_DIRECTORIES=\"${INCLUDE_JOINED}\"")
