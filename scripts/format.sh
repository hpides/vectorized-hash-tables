#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." >/dev/null 2>&1 && pwd )"

function run_format() {
    local subdir=$1
    find "$DIR/$subdir" -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
}

run_format "benchmark"
run_format "benchmark-include"
run_format "include"
run_format "playground"
run_format "test"
