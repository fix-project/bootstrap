# bootstrap
This repository builds a compilation toolchain in Fix that compiles WASM modules to ELF.

## Prerequisite
To build this project, you need to first build [fix-project/wasm-toolchain](https://github.com/fix-project/wasm-toolchain), and `wasm-toolchain` needs to be placed at `$ENV{HOME}/wasm-toolchain`.

## Build
```
./build-all.sh
```
After building, there should be a `boot/` and a `.fix/` in the source directory of the project. Copy both directories to the source directory of Fix.
