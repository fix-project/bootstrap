add_link_options(-nostdlib -Wl,--no-entry -Wl,--allow-undefined
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc++.a
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc++abi.a
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc.a
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/clang/18/lib/wasi/libclang_rt.builtins-wasm32.a)

add_library(stub-obj OBJECT "stubs.c")

add_executable(wasm-to-c-fix "wasm2c-driver.cc")
target_link_libraries(wasm-to-c-fix wasm-to-c-lib wabt)
add_executable(c-to-elf-fix "c-to-elf-driver.cc")
target_link_libraries(c-to-elf-fix clang-lib ${CLANG_LIBS} ${LLVM_LIBS} stub-obj)
add_executable(link-elfs-fix "link-elfs-driver.cc")
target_link_libraries(link-elfs-fix lld-lib ${LLD_LIBS} ${LLVM_LIBS} stub-obj)

add_custom_command(
  OUTPUT "asm.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/asm.wat
  COMMAND $ENV{HOME}/wasm-toolchain/wabt/build/wat2wasm
          --enable-multi-memory
	        --enable-exceptions
          ${CMAKE_CURRENT_SOURCE_DIR}/asm.wat
          -o asm.wasm
)

add_custom_command(
  OUTPUT "wasisnapshot.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/wasisnapshot.wat
  COMMAND $ENV{HOME}/wasm-toolchain/wabt/build/wat2wasm
          ${CMAKE_CURRENT_SOURCE_DIR}/wasisnapshot.wat
          -o wasisnapshot.wasm
)


add_custom_command(
  OUTPUT "wasm-to-c-no-asm.wasm"
  DEPENDS wasisnapshot.wasm
          wasm-to-c-fix
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --debug-names
          --enable-multi-memory
	        --enable-exceptions
          wasm-to-c-fix
          wasisnapshot.wasm
          -m wasm2c
          -n wasi_snapshot_preview1
          -o wasm-to-c-no-asm.wasm
)

add_custom_command(
  OUTPUT "wasm-to-c-fix.wasm"
  DEPENDS asm.wasm
          wasm-to-c-no-asm.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --debug-names
          --enable-multi-memory
	        --enable-exceptions
          wasm-to-c-no-asm.wasm
          asm.wasm
          -m clang
          -n asm
          -o wasm-to-c-fix.wasm
)

add_custom_target(
  wasm-to-c-fix-wasm ALL
  DEPENDS wasm-to-c-fix.wasm
)

add_custom_command(
  OUTPUT "c-to-elf-no-asm.wasm"
  DEPENDS wasisnapshot.wasm
          c-to-elf-fix
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --debug-names
          --enable-multi-memory
	        --enable-exceptions
          c-to-elf-fix
          wasisnapshot.wasm
          -m wasm2c
          -n wasi_snapshot_preview1
          -o c-to-elf-no-asm.wasm
)

add_custom_command(
  OUTPUT "c-to-elf-fix.wasm"
  DEPENDS asm.wasm
          c-to-elf-no-asm.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --debug-names
          --enable-multi-memory
	        --enable-exceptions
          c-to-elf-no-asm.wasm
          asm.wasm
          -m clang
          -n asm
          -o c-to-elf-fix.wasm
)

add_custom_target(
  c-to-elf-fix-wasm ALL
  DEPENDS c-to-elf-fix.wasm
)

add_custom_command(
  OUTPUT "link-elfs-no-asm.wasm"
  DEPENDS wasisnapshot.wasm
          link-elfs-fix
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --debug-names
          --enable-multi-memory
	        --enable-exceptions
          link-elfs-fix
          wasisnapshot.wasm
          -m wasm2c
          -n wasi_snapshot_preview1
          -o link-elfs-no-asm.wasm
)

add_custom_command(
  OUTPUT "link-elfs-fix.wasm"
  DEPENDS asm.wasm
          link-elfs-no-asm.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --debug-names
          --enable-multi-memory
	        --enable-exceptions
          link-elfs-no-asm.wasm
          asm.wasm
          -m clang
          -n asm
          -o link-elfs-fix.wasm
)

add_custom_target(
  link-elfs-fix-wasm ALL
  DEPENDS link-elfs-fix.wasm
)

add_custom_command(
  OUTPUT "map.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/map.wat
  COMMAND $ENV{HOME}/wasm-toolchain/wabt/build/wat2wasm
          --enable-multi-memory
          ${CMAKE_CURRENT_SOURCE_DIR}/map.wat
          -o map.wasm)

add_custom_target(map-wasm ALL DEPENDS map.wasm)

add_custom_command(
  OUTPUT "compile.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/compile.wat
  COMMAND $ENV{HOME}/wasm-toolchain/wabt/build/wat2wasm
          --enable-multi-memory
          ${CMAKE_CURRENT_SOURCE_DIR}/compile.wat
          -o compile.wasm)

add_custom_target(compile-wasm ALL DEPENDS compile.wasm)
