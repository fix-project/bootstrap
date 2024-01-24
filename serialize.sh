#!/bin/bash
SRC_REL=`dirname $0`
SRC=`realpath ${SRC_REL}`
FIX=${SRC}/.fix

mkdir -p ${FIX}
mkdir -p ${FIX}/data
mkdir -p ${FIX}/relations
mkdir -p ${FIX}/labels
mkdir -p ${FIX}/pins

${SRC}/build/src/serialize/serialize ${SRC}/ ${SRC}/build/llvm-project/llvm/lib/clang/18/include/
