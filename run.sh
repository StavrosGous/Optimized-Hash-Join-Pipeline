#!/bin/bash

if [ $# -eq 0 ]; then
    set -- "fast_plans"
fi

# if argument is "main" run this, otherwise if "fast_plans" run ./build/fast plans.json
if [ "$1" == "main" ]; then
    g++ ./main.cpp -o main -I ./include && time ./main || echo "Compilation or execution failed"
elif [ "$1" == "fast_plans" ]; then
    ./build/fast plans.json
elif [ "$1" == "tests" ]; then
    ./build/unit_tests
else
    echo "Unknown argument"
fi