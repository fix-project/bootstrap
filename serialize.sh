#!/bin/bash
SRC_REL=`dirname $0`
SRC=`realpath ${SRC_REL}`
FIX=${SRC}/.fix

mkdir -p ${FIX}
${SRC}/build/src/serialize/serialize ${SRC}/ ${SRC}/build/llvm-project/llvm/lib/clang/18/include/
