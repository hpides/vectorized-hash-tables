#!/bin/bash
set -e

CLANG_TIDY=${CLANG_TIDY:-clang-tidy}
BUILD_DIR=${BUILD_DIR:-cmake-build-debug/clang-tidy-build}

function run_build() {
    echo "Running cmake build..."
    set -x
    cmake -S . -B ${BUILD_DIR} \
        -DCMAKE_BUILD_TYPE=DEBUG \
        -DCMAKE_UNITY_BUILD=ON \
        -DCMAKE_UNITY_BUILD_BATCH_SIZE=0
    set +x
}

function run_tidy() {
    echo "Running clang-tidy..."
    set -x
    ${CLANG_TIDY} -p ${BUILD_DIR} \
        -header-filter='.*/ma-code/src/.*|.*include/ma-code/.*' \
        --checks='-bugprone-suspicious-include' \
        ${BUILD_DIR}/CMakeFiles/hashmap-read-benchmark.dir/Unity/*.cxx

    ${CLANG_TIDY} -p ${BUILD_DIR} \
        -header-filter='.*/ma-code/src/.*|.*include/ma-code/.*' \
        --checks='-bugprone-suspicious-include' \
        ${BUILD_DIR}/CMakeFiles/hashmap-write-benchmark.dir/Unity/*.cxx

    set +x
}

case $1 in
    "build")
        run_build
        ;;
    "run_tidy")
        run_tidy
        ;;
    *)
        run_build
        run_tidy
        ;;
esac
