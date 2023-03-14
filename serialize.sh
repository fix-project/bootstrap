#!/bin/bash
SRC_REL=`dirname $0`
SRC=`realpath ${SRC_REL}`
WASM2C=${SRC}/tmp/wasm2c
CLANG=${SRC}/tmp/clang
LLD=${SRC}/tmp/lld
FIX=${SRC}/.fix

mkdir -p ${FIX}
${SRC}/build/src/serialize/serialize-clang-dep ${SRC} ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/

${SRC}/build/src/serialize/serialize-file ${SRC} ${SRC}/fix_build/src/fix-driver/wasm-to-c-fix.wasm
${SRC}/build/src/serialize/serialize-file ${SRC} ${WASM2C}/wasm-to-c-fix.o

${SRC}/build/src/serialize/serialize-file ${SRC} ${SRC}/fix_build/src/fix-driver/c-to-elf-fix.wasm
${SRC}/build/src/serialize/serialize-file ${SRC} ${CLANG}/c-to-elf-fix.o

${SRC}/build/src/serialize/serialize-file ${SRC} ${SRC}/fix_build/src/fix-driver/link-elfs-fix.wasm
${SRC}/build/src/serialize/serialize-file ${SRC} ${LLD}/link-elfs-fix.o

${SRC}/build/src/serialize/serialize-file ${SRC} ${SRC}/fix_build/src/fix-driver/map.wasm
