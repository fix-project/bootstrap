#!/bin/bash
# cmake -S . -B build
# cmake --build build
# cmake -S . -B fix-build -DBUILD_SYS_DRIVER=OFF
# cmake --build fix-build

progress_bar() {
  if [ -z $TERM ]
  then
    return
  fi
  msg=$1
  completed=$2
  in_progress=$3
  total=$4
  prefix=$(printf "%s [" "$msg")
  suffix=$(printf "][%d/%d]" "$completed" "$total")
  n=$((${#prefix} + ${#suffix}))
  cols=$(($(tput cols) - "$n" - 1))

  scale=$(("$cols" * 100 / "$total"))
  cutoff1=$(("$completed"*"$scale" / 100))
  cutoff2=$(( ("$completed" + "$in_progress") * "$scale" / 100))

  bar=""
  for i in $(seq 1 $((cols - 1)))
  do
    if [ "$i" -le "$cutoff1" ]
    then
      bar="$bar#"
    elif [ "$i" -le "$cutoff2" ]
    then
      bar="$bar*"
    else
      bar="$bar "
    fi
  done

  printf "%s%s%s" "$prefix" "$bar" "$suffix"
}

PARALLEL=${1:-$(nproc)}
SRC_REL=$(dirname "$0")
SRC=$(realpath "${SRC_REL}")
OUTPUT=${SRC}/tmp
Tasks='wasm-to-c-fix c-to-elf-fix link-elfs-fix map compile'
for task in ${Tasks}; do
  echo "Converting $task to C..."
  mkdir -p "${OUTPUT}/${task}"
  "${SRC}"/build/src/sys-driver/wasm-to-c-sys "${SRC}"/fix-build/src/fix-driver/"${task}".wasm "${OUTPUT}"/"${task}"

  echo "Compiling $task to ELF..."
  for i in {0..255}
  do
    "${SRC}"/build/src/sys-driver/c-to-elf-sys "${OUTPUT}"/"${task}"/function"${i}".c "${OUTPUT}"/"${task}"/function-impl.h "${OUTPUT}"/"${task}"/function.h "${SRC}"/build/llvm-project/llvm/lib/clang/16.0.0/include/ "${OUTPUT}"/"${task}"/function"${i}".o &
    running=$(jobs -r | wc -l)
    if [ "$running" -ge $(("$PARALLEL"-1)) ]
    then
      wait -n
    fi
    printf "\r"
    progress_bar "$task" $(("$i"-"$running")) "$running" 256
  done

  while [ "$running" != 0 ]
  do
    wait -n
    running=$(jobs -r | wc -l)
    printf "\r"
    progress_bar "$task" $((256-"$running")) "$running" 256
  done
  echo

  echo "Linking $task..."
  LLDSYS="${SRC}/build/src/sys-driver/link-elfs-sys ${OUTPUT}/${task}.o"

  for i in {0..255}
  do
    LLDSYS+=" "
    LLDSYS+="${OUTPUT}/${task}/function${i}.o"
  done
  eval "${LLDSYS}"
done
