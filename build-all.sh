#!/bin/bash

PARALLEL=${1:-$(nproc)}

cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13
cmake --build build --parallel $PARALLEL
cmake -S . -B fix-build -DBUILD_SYS_DRIVER=OFF -G Ninja
cmake --build fix-build --parallel $PARALLEL

./build.sh
./serialize.sh
