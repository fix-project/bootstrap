# bootstrap
This repository builds a compilation toolchain in Fix that compiles WASM modules to ELF.

## Prerequisite
To build this project, you need to first build [fix-project/wasm-toolchain](https://github.com/fix-project/wasm-toolchain), and `wasm-toolchain` needs to be placed at `$ENV{HOME}/wasm-toolchain`.

## Build
```
cmake -S . -B build
cmake --build build --parallel {# of parallelism}
cmake -S . -B fix-build -DBUILD_SYS_DRIVER=OFF
cmake --build fix-build --parallel {# of parallelism}
./build.sh
./serialize.sh
```
After building, there should be a `boot/` and a `.fix/` in the source directory of the project. Copy both directories to the source directory of Fix.
