#!/bin/bash
SRC_REL=`dirname $0`
SRC=`realpath ${SRC_REL}`
WASM2C=${SRC}/tmp/wasm2c
CLANG=${SRC}/tmp/clang
LLD=${SRC}/tmp/lld
MAP=${SRC}/tmp/map
COMPILE=${SRC}/tmp/compile

mkdir -p ${WASM2C}
mkdir -p ${CLANG}
mkdir -p ${LLD}
mkdir -p ${MAP}
mkdir -p ${COMPILE}
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

# map
${SRC}/build/src/sys-driver/wasm-to-c-sys fix_build/src/fix-driver/map.wasm ${MAP}
for i in {0..255}
do
  ${SRC}/build/src/sys-driver/c-to-elf-sys ${MAP}/function${i}.c ${MAP}/function-impl.h ${MAP}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${MAP}/function${i}.o &
done
${SRC}/build/src/sys-driver/c-to-elf-sys ${MAP}/function-fixpoint.c ${MAP}/function-impl.h ${MAP}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${MAP}/function-fixpoint.o &
wait
LLDSYS="${SRC}/build/src/sys-driver/link-elfs-sys ${MAP}/map.o"
for i in {0..255}
do
  LLDSYS+=" ${MAP}/function${i}.o"
done
LLDSYS+=" ${MAP}/function-fixpoint.o"
eval "${LLDSYS}"

# compile
${SRC}/build/src/sys-driver/wasm-to-c-sys fix_build/src/fix-driver/compile.wasm ${COMPILE}
for i in {0..255}
do
  ${SRC}/build/src/sys-driver/c-to-elf-sys ${COMPILE}/function${i}.c ${COMPILE}/function-impl.h ${COMPILE}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${COMPILE}/function${i}.o &
done
${SRC}/build/src/sys-driver/c-to-elf-sys ${COMPILE}/function-fixpoint.c ${COMPILE}/function-impl.h ${COMPILE}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${COMPILE}/function-fixpoint.o &
wait
LLDSYS="${SRC}/build/src/sys-driver/link-elfs-sys ${COMPILE}/compile.o"
for i in {0..255}
do
  LLDSYS+=" ${COMPILE}/function${i}.o"
done
LLDSYS+=" ${COMPILE}/function-fixpoint.o"
eval "${LLDSYS}"
