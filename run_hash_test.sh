#!/bin/bash


g++ ./main.cpp -o main -I ./include && time ./main || echo "Compilation or execution failed"