include(FetchContent)

message(STATUS "Making dependencies available.")

################### fmt ####################
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 8.1.1
)

if (${HASHMAP_BUILD_EXTERNAL})
  set(FMT_INSTALL ON)
endif()

FetchContent_MakeAvailable(fmt)

################### spdlog ####################
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.10.0
)
FetchContent_MakeAvailable(spdlog)

################## gcem ###################
FetchContent_Declare(
        gcem
        GIT_REPOSITORY https://github.com/kthohr/gcem.git
        GIT_TAG "v1.14.1"
)

FetchContent_MakeAvailable(gcem)

################## xxhash ###################
FetchContent_Declare(
        xxhash
        GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
        GIT_TAG "v0.8.1"
)

FetchContent_GetProperties(xxhash)
if(NOT xxhash_POPULATED)
    FetchContent_Populate(xxhash)
    option(BUILD_SHARED_LIBS "Build shared libs" OFF)
    set(XXHASH_BUILD_XXHSUM OFF)
    add_subdirectory(${xxhash_SOURCE_DIR}/cmake_unofficial/ ${xxhash_SOURCE_DIR}/build/ EXCLUDE_FROM_ALL)
endif()

################## Hedley ###################
FetchContent_Declare(hedley
  GIT_REPOSITORY https://github.com/nemequ/hedley.git
  GIT_TAG master
)

FetchContent_GetProperties(hedley)
if(NOT hedley_POPULATED)
  FetchContent_Populate(hedley)
endif()

add_library(hedley INTERFACE)
target_include_directories(hedley INTERFACE ${hedley_SOURCE_DIR})


if (${HASHMAP_BUILD_EXTERNAL})
  ################## Abseil ###################
  message(STATUS "Fetching and building Abseil, this might take a while.")
  set(ABSL_BUILD_TESTING OFF)
  FetchContent_Declare(
    absl
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20211102.0
  )
  FetchContent_GetProperties(absl)
  if(NOT absl_POPULATED)
      FetchContent_Populate(absl)
      add_subdirectory(${absl_SOURCE_DIR} ${absl_SOURCE_DIR}/build)
  endif()

  # Function to help us fix compiler warnings for all abseil targets
  function(get_all_targets src_dir var)
      set(targets)
      get_all_targets_recursive(targets ${src_dir})
      set(${var} ${targets} PARENT_SCOPE)
  endfunction()

  macro(get_all_targets_recursive targets dir)
      get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
      foreach(subdir ${subdirectories})
          get_all_targets_recursive(${targets} ${subdir})
      endforeach()

      get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
      list(APPEND ${targets} ${current_targets})
  endmacro()

  macro(remove_flag_from_target _target _flag)
      get_target_property(_target_cxx_flags ${_target} COMPILE_OPTIONS)
      if(_target_cxx_flags)
          list(REMOVE_ITEM _target_cxx_flags ${_flag})
          set_target_properties(${_target} PROPERTIES COMPILE_OPTIONS "${_target_cxx_flags}")
      endif()
  endmacro()

  get_all_targets(${absl_SOURCE_DIR} all_absl_targets)
  foreach(_absl_target IN LISTS all_absl_targets)
    target_compile_options(${_absl_target} INTERFACE -Wno-conversion -Wno-pedantic -Wno-float-equal)
    remove_flag_from_target(${_absl_target} "-march=armv8-a+crypto")
  endforeach()

  ################## Martinus Robin Hood ###################
  FetchContent_Declare(
    robin-hood-hashmap
    GIT_REPOSITORY https://github.com/martinus/robin-hood-hashing.git
    GIT_TAG 3.11.5
  )
  FetchContent_GetProperties(robin-hood-hashmap)
  if(NOT robin-hood-hashmap_POPULATED)
    FetchContent_Populate(robin-hood-hashmap)
  endif()
  
  add_library(robin-hood-hashmap INTERFACE)
  target_include_directories(robin-hood-hashmap INTERFACE ${robin-hood-hashmap_SOURCE_DIR}/src/include)

  ################## Folly ###################

  if (${HASHMAP_IS_POWER})
    message(STATUS "Skipping Folly on Power due to incompatability.")
  else()
    message(STATUS "Fetching and building Folly, this might take a while.")

    # If this is the first time building folly, we need to download some dependencies
    if (NOT EXISTS "${HASHMAP_EXTERNAL_TEMP_DIR}/opt") 
      message(STATUS "Building some folly dependencies in ${HASHMAP_EXTERNAL_TEMP_DIR}/opt using bash")	  
      execute_process(COMMAND mkdir -p ${HASHMAP_EXTERNAL_TEMP_DIR}/opt)
      execute_process(COMMAND bash ${HASHMAP_CMAKE_DIR}/build_folly_deps.sh ${HASHMAP_EXTERNAL_TEMP_DIR} WORKING_DIRECTORY ${HASHMAP_EXTERNAL_TEMP_DIR})
    endif() 

    set(CMAKE_LIBRARY_PATH "${HASHMAP_EXTERNAL_TEMP_DIR}/opt/lib" "${HASHMAP_EXTERNAL_TEMP_DIR}/opt/lib64" ${CMAKE_LIBRARY_PATH})
    set(CMAKE_INCLUDE_PATH "${HASHMAP_EXTERNAL_TEMP_DIR}/opt/include" ${CMAKE_INCLUDE_PATH})

    FetchContent_Declare(
      folly
      GIT_REPOSITORY https://github.com/facebook/folly.git
      GIT_TAG v2022.04.04.00
    )
    FetchContent_GetProperties(folly)

    MACRO(SUBDIRLIST result curdir)
      FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
      SET(dirlist "")
      FOREACH(child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
          LIST(APPEND dirlist "${curdir}/${child}")
        ENDIF()
      ENDFOREACH()
      SET(${result} ${dirlist})
    ENDMACRO()


    if(NOT folly_POPULATED)
      FetchContent_Populate(folly)

      execute_process(COMMAND rm ${folly_SOURCE_DIR}/CMakeLists.txt)
      execute_process(COMMAND cp ${HASHMAP_CMAKE_DIR}/Folly_FixedCMakeLists.txt ${folly_SOURCE_DIR}/CMakeLists.txt)

      # We need to disable that CMake is built, as we can assume that we setup a recent enough version of cmake and the cmake built is broken on systems without openssl, such as our ARM system
      # We need to disable Python for Boost, as some HPI DELAB nodes don't expose the Python headers  
      # We need to enable static boost because on systems with an broken version of boost (i.e., HPI x86 nodes) everything breaks together
      execute_process(COMMAND bash ${HASHMAP_CMAKE_DIR}/fix_folly_boost.sh ${folly_SOURCE_DIR} WORKING_DIRECTORY ${folly_SOURCE_DIR})

      # If this is the first time building folly, we need to download the dependencies using the getdeps.py script
      if (NOT EXISTS "${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch")
        message(STATUS "Calling Folly's getdeps.py script, this will download boost 'n stuff. Do not worry if the next step seems to fail and go grab a coffee ☕")
        execute_process(COMMAND mkdir -p ${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch)
        execute_process(COMMAND mkdir -p ${HASHMAP_EXTERNAL_TEMP_DIR}/folly_install)
	      execute_process(COMMAND python3 ${folly_SOURCE_DIR}/build/fbcode_builder/getdeps.py build --only-deps --scratch-path ${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch --install-dir ${HASHMAP_EXTERNAL_TEMP_DIR}/folly_install --num-jobs 8 WORKING_DIRECTORY ${folly_SOURCE_DIR})
      endif() 

      SUBDIRLIST(FOLLY_DEP_SUBDIRS "${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch/installed" "folly_scratch/installed")
      SUBDIRLIST(FOLLY_DEP_BUILD_SUBDIRS "${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch/build" "folly_scratch/build")
      
      set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${FOLLY_DEP_SUBDIRS})
      set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${FOLLY_DEP_BUILD_SUBDIRS} ${FOLLY_DEP_SUBDIRS})
      set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${FOLLY_DEP_BUILD_SUBDIRS} ${FOLLY_DEP_SUBDIRS})
      set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${FOLLY_DEP_BUILD_SUBDIRS} ${FOLLY_DEP_SUBDIRS})

      list(APPEND FOLLY_CXX_FLAGS -Wno-conversion -Wno-float-equal -fpermissive)

      add_subdirectory(${folly_SOURCE_DIR} ${folly_SOURCE_DIR}/build)
    endif()

    # Just to be sure (on x86, this is for some reason requried), we overwrite this every time.
    execute_process(COMMAND rm ${folly_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND cp ${HASHMAP_CMAKE_DIR}/Folly_FixedCMakeLists.txt ${folly_SOURCE_DIR}/CMakeLists.txt)

    SUBDIRLIST(FOLLY_DEP_SUBDIRS "${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch/installed" "folly_scratch/installed")
    SUBDIRLIST(FOLLY_DEP_BUILD_SUBDIRS "${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch/build" "folly_scratch/build")
    
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${FOLLY_DEP_SUBDIRS})
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${FOLLY_DEP_BUILD_SUBDIRS} ${FOLLY_DEP_SUBDIRS})
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${FOLLY_DEP_BUILD_SUBDIRS} ${FOLLY_DEP_SUBDIRS})
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${FOLLY_DEP_BUILD_SUBDIRS} ${FOLLY_DEP_SUBDIRS})
    
    # glog fetched by the script is faulty, need to fix that...
    message(STATUS "Fixing glog")
    execute_process(COMMAND bash "-c" "ls ${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch/build | grep glog" OUTPUT_VARIABLE FOLLY_GLOG_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(FOLLY_GLOG_DIR ${HASHMAP_EXTERNAL_TEMP_DIR}/folly_scratch/build/${FOLLY_GLOG_DIR})
    message(STATUS "Found glog dir ${FOLLY_GLOG_DIR}")

    execute_process(COMMAND bash "-c" "rm ${FOLLY_GLOG_DIR}/*.so*")
    execute_process(COMMAND bash "-c" "find $(pwd -P) -name 'libglog.so.0.6.0'" OUTPUT_VARIABLE LIBGLOG_SO_PATH OUTPUT_STRIP_TRAILING_WHITESPACE WORKING_DIRECTORY "${HASHMAP_EXTERNAL_TEMP_DIR}/opt")

    execute_process(COMMAND cp ${LIBGLOG_SO_PATH} ${FOLLY_GLOG_DIR}/libglog.so)
    execute_process(COMMAND cp ${LIBGLOG_SO_PATH} ${FOLLY_GLOG_DIR}/libglog.so.1)
    execute_process(COMMAND cp ${LIBGLOG_SO_PATH} ${FOLLY_GLOG_DIR}/libglog.so.0)
    execute_process(COMMAND cp ${LIBGLOG_SO_PATH} ${FOLLY_GLOG_DIR}/libglog.so.0.6.0)

    execute_process(COMMAND bash "-c" "cp ${HASHMAP_EXTERNAL_TEMP_DIR}/opt/include/glog/*.h ${FOLLY_GLOG_DIR}/glog")

    file(READ "${folly_SOURCE_DIR}/folly/container/detail/F14MapFallback.h" FallbackMapSource)
    string(FIND "${FallbackMapSource}" "uses_vector_instructions()" matchres)
    if(${matchres} EQUAL -1)
      message(STATUS "Applying F14 SIMD Information Method")
      execute_process(COMMAND bash ${HASHMAP_CMAKE_DIR}/fix_f14.sh ${folly_SOURCE_DIR} WORKING_DIRECTORY ${folly_SOURCE_DIR})
    else()
      message(STATUS "F14 Info already applied")
    endif()
    
    get_all_targets(${folly_SOURCE_DIR} all_folly_targets)
    foreach(_folly_target IN LISTS all_folly_targets)
      target_compile_options(${_folly_target} INTERFACE -Wno-conversion)
    endforeach()
  endif()
endif()
