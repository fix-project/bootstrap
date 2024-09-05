#!/bin/bash
SRC_REL=`dirname $0`
SRC=`realpath ${SRC_REL}`
FIX=${SRC}/.fix
BIN_PATH=$1
BIN_NUM=$2
BIN_PATH=`realpath ${BIN_PATH}`

rm -rf ${BIN_PATH}
mkdir -p ${BIN_PATH}
for i in $( seq 0 $((BIN_NUM - 1)) )
do
  mkdir -p ${BIN_PATH}/bin$i
  mkdir -p ${BIN_PATH}/bin$i/.fix
  mkdir -p ${BIN_PATH}/bin$i/.fix/data
  mkdir -p ${BIN_PATH}/bin$i/.fix/labels
  mkdir -p ${BIN_PATH}/bin$i/.fix/relations
  mkdir -p ${BIN_PATH}/bin$i/.fix/pins
done

echo "${SRC}/build/src/serialize/serialize-to-bins ${SRC}/ ${SRC}/build/llvm-project/llvm/lib/clang/19/include/ ${BIN_PATH} ${BIN_NUM}"
${SRC}/build/src/serialize/serialize-to-bins ${SRC}/ ${SRC}/build/llvm-project/llvm/lib/clang/19/include/ ${BIN_PATH} ${BIN_NUM}
