#!/bin/bash

#build the fast executable using cmake 
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev && cmake --build build -- -j $(nproc) fast && echo "Build succeeded" || echo "Build failed"