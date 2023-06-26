include(FetchContent)

##################### GoogleTest ####################
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)

set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

get_target_property(HASHMAP_INCLUDES hashmap INCLUDE_DIRECTORIES)

##################################################
# UNIT TESTS
##################################################
set(
  HASHMAP_TEST_SOURCES
  unit/hashmaps/bucketing_simd_test.cpp
  unit/hashmaps/chained_test.cpp
  unit/hashmaps/external_hashmap_test.cpp
  unit/hashmaps/fingerprinting_simd_test.cpp
  unit/hashmaps/hashers.hpp
  unit/hashmaps/hashmap_test_impl.hpp
  unit/hashmaps/linear_probing_aos_test.cpp
  unit/hashmaps/linear_probing_soa_packed_test.cpp
  unit/hashmaps/linear_probing_soa_test.cpp
  unit/hashmaps/quadratic_probing_aos_test.cpp
  unit/hashmaps/robin_hood_aos_test.cpp
  unit/hashmaps/simple_simd_test.cpp
  unit/other/simd_utils_test.cpp
)

add_executable(hashmap-test hashmap_test.cpp ${HASHMAP_TEST_SOURCES})
add_test(hashmap-test hashmap-test)
target_include_directories(hashmap-test PRIVATE ${HASHMAP_INCLUDES})
target_include_directories(hashmap-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hashmap-test PRIVATE gtest gmock fmt spdlog hashmap gcem hedley)
