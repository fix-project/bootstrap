#!/bin/bash
SRC_REL=`dirname $0`
SRC=`realpath ${SRC_REL}`
WASM2C=${SRC}/tmp/wasm2c
CLANG=${SRC}/tmp/clang
LLD=${SRC}/tmp/lld

mkdir -p ${WASM2C}
mkdir -p ${CLANG}
mkdir -p ${LLD}
# cmake -S . -B fix_build -DBUILD_SYS_DRIVER:BOOL=OFF
# cmake -S . -B build
# cmake --build fix_build --parallel 256
# cmake --build build --parallel 256

# wasm2c
${SRC}/build/src/sys-driver/wasm-to-c-sys fix_build/src/fix-driver/wasm-to-c-fix.wasm ${WASM2C}
for i in {0..255}
do
  ${SRC}/build/src/sys-driver/c-to-elf-sys ${WASM2C}/function${i}.c ${WASM2C}/function-impl.h ${WASM2C}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${WASM2C}/function${i}.o &
done
${SRC}/build/src/sys-driver/c-to-elf-sys ${WASM2C}/function-fixpoint.c ${WASM2C}/function-impl.h ${WASM2C}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${WASM2C}/function-fixpoint.o &
wait
LLDSYS="${SRC}/build/src/sys-driver/link-elfs-sys ${WASM2C}/wasm-to-c-fix.o"
for i in {0..255}
do
  LLDSYS+=" ${WASM2C}/function${i}.o"
done
LLDSYS+=" ${WASM2C}/function-fixpoint.o"
eval "${LLDSYS}"

# clang
${SRC}/build/src/sys-driver/wasm-to-c-sys fix_build/src/fix-driver/c-to-elf-fix.wasm ${CLANG}
for i in {0..255}
do
  ${SRC}/build/src/sys-driver/c-to-elf-sys ${CLANG}/function${i}.c ${CLANG}/function-impl.h ${CLANG}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${CLANG}/function${i}.o &
done
${SRC}/build/src/sys-driver/c-to-elf-sys ${CLANG}/function-fixpoint.c ${CLANG}/function-impl.h ${CLANG}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${CLANG}/function-fixpoint.o &
wait
LLDSYS="${SRC}/build/src/sys-driver/link-elfs-sys ${CLANG}/c-to-elf-fix.o"
for i in {0..255}
do
  LLDSYS+=" ${CLANG}/function${i}.o"
done
LLDSYS+=" ${CLANG}/function-fixpoint.o"
eval "${LLDSYS}"

# lld
${SRC}/build/src/sys-driver/wasm-to-c-sys fix_build/src/fix-driver/link-elfs-fix.wasm ${LLD}
for i in {0..255}
do
  ${SRC}/build/src/sys-driver/c-to-elf-sys ${LLD}/function${i}.c ${LLD}/function-impl.h ${LLD}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${LLD}/function${i}.o &
done
${SRC}/build/src/sys-driver/c-to-elf-sys ${LLD}/function-fixpoint.c ${LLD}/function-impl.h ${LLD}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${LLD}/function-fixpoint.o &
wait
LLDSYS="${SRC}/build/src/sys-driver/link-elfs-sys ${LLD}/link-elfs-fix.o"
for i in {0..255}
do
  LLDSYS+=" ${LLD}/function${i}.o"
done
LLDSYS+=" ${LLD}/function-fixpoint.o"
eval "${LLDSYS}"
