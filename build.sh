#!/bin/bash

if [ "$1" == "fast_plans" ]; then
    # build the fast executable using cmake
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev && cmake --build build -- -j $(nproc) fast && echo "Build succeeded" || echo "Build failed"

elif [ "$1" == "tests" ]; then
    # build the unit tests using cmake
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev && cmake --build build -- -j $(nproc) unit_tests && echo "Build succeeded" || echo "Build failed"
elif [ "$1" == "main" ]; then
    # build default plans executable (selection removed)
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release  -Wno-dev && cmake --build build -- -j $(nproc) && echo "Build succeeded" || echo "Build failed"
else
    echo "Unknown argument"
fi
