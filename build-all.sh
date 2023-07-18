#!/bin/bash -xe

PARALLEL=${1:-$(nproc)}

cmake -S . -B build -G Ninja
cmake --build build --parallel $PARALLEL
cmake -S . -B fix-build -DBUILD_SYS_DRIVER=OFF -G Ninja
cmake --build fix-build --parallel $PARALLEL

./build.sh
./serialize.sh
