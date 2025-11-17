#!/bin/bash

if [ "$1" == "fast_plans" ]; then
    # build the fast executable using cmake
    #based on $2 arg do hash map selection default is 0 and is robin hood, 1 hopscotch, 2 cuckoo, if $2 not present use robin hood
    if [ -z "$2" ]; then
        set -- "$2" "1"
    fi
    if [ "$2" == "0" ]; then
        echo "-- Using hash map: Robin Hood"
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DHASH_MAP=0 -Wno-dev && cmake --build build -- -j $(nproc) fast && echo "Build succeeded" || echo "Build failed"
    elif [ "$2" == "1" ]; then
        echo "-- Using hash map: Hopscotch"
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DHASH_MAP=1 -Wno-dev && cmake --build build -- -j $(nproc) fast && echo "Build succeeded" || echo "Build failed"
    elif [ "$2" == "2" ]; then
        echo "-- Using hash map: Cuckoo"
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DHASH_MAP=2 -Wno-dev && cmake --build build -- -j $(nproc) fast && echo "Build succeeded" || echo "Build failed"
    else
        echo "Unknown hash map selection, use 0 for robin hood, 1 for hopscotch, 2 for cuckoo"
        exit 1
    fi

elif [ "$1" == "tests" ]; then
    # build the unit tests using cmake
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev && cmake --build build -- -j $(nproc) unit_tests && echo "Build succeeded" || echo "Build failed"
elif [ "$1" == "main" ]; then
    # build default plans executable
    # same as fast plans
    if [ -z "$2" ]; then
        set -- "$2" "1"
    fi
    if [ "$2" == "0" ]; then
        echo "-- Using hash map: Robin Hood"
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DHASH_MAP=0 -Wno-dev && cmake --build build -- -j $(nproc) && echo "Build succeeded" || echo "Build failed"
    elif [ "$2" == "1" ]; then
        echo "-- Using hash map: Hopscotch" 
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DHASH_MAP=1 -Wno-dev && cmake --build build -- -j $(nproc) && echo "Build succeeded" || echo "Build failed"
    elif [ "$2" == "2" ]; then
        echo "-- Using hash map: Cuckoo"
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DHASH_MAP=2 -Wno-dev && cmake --build build -- -j $(nproc) && echo "Build succeeded" || echo "Build failed"
    else
        echo "Unknown hash map selection, use 0 for robin hood, 1 for hopscotch, 2 for cuckoo"
        exit 1
    fi
else
    echo "Unknown argument"
fi
