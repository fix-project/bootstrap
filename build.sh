#!/bin/bash
# cmake -S . -B build
# cmake --build build
# cmake -S . -B fix-build -DBUILD_SYS_DRIVER=OFF
# cmake --build fix-build

PARALLEL=${1:-256}
SRC_REL=`dirname $0`
SRC=`realpath ${SRC_REL}`
OUTPUT=${SRC}/tmp
Tasks='wasm-to-c-fix c-to-elf-fix link-elfs-fix map compile'
for task in ${Tasks}; do
  echo $task
  mkdir -p ${OUTPUT}/${task}
  ${SRC}/build/src/sys-driver/wasm-to-c-sys ${SRC}/fix-build/src/fix-driver/${task}.wasm ${OUTPUT}/${task}

  for i in {0..255}
  do
    ${SRC}/build/src/sys-driver/c-to-elf-sys ${OUTPUT}/${task}/function${i}.c ${OUTPUT}/${task}/function-impl.h ${OUTPUT}/${task}/function.h ${SRC}/build/llvm-project/llvm/lib/clang/16.0.0/include/ ${OUTPUT}/${task}/function${i}.o &
    if [ $(((${i}+1) % ${PARALLEL})) == 0 ]
    then
      wait
    fi
  done
  wait

  LLDSYS="${SRC}/build/src/sys-driver/link-elfs-sys ${OUTPUT}/${task}.o"

  for i in {0..255}
  do
    LLDSYS+=" "
    LLDSYS+="${OUTPUT}/${task}/function${i}.o"
  done
  eval "${LLDSYS}"
done
