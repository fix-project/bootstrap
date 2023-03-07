add_link_options(-nostdlib -Wl,--no-entry -Wl,--allow-undefined
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc++.a
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc++abi.a
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc.a)

add_executable(wasm-to-c-fix "wasm2c-driver.cc")
target_link_libraries(wasm-to-c-fix wasm-to-c-lib wabt)
add_executable(c-to-elf-fix "c-to-elf-driver.cc")
target_link_libraries(c-to-elf-fix clang-lib ${CLANG_LIBS} ${LLVM_LIBS})
add_executable(link-elfs-fix "link-elfs-driver.cc")
target_link_libraries(link-elfs-fix lld-lib ${LLD_LIBS} ${LLVM_LIBS})

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
  OUTPUT "wasm-to-c-no-wasi.wasm"
  DEPENDS asm.wasm
          wasm-to-c-fix
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --enable-multi-memory
	        --enable-exceptions
          wasm-to-c-fix
          asm.wasm
          -m clang
          -n asm
          -o wasm-to-c-no-wasi.wasm
)

add_custom_command(
  OUTPUT "wasm-to-c-fix.wasm"
  DEPENDS wasisnapshot.wasm
          wasm-to-c-no-wasi.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --enable-multi-memory
	        --enable-exceptions
          wasm-to-c-no-wasi.wasm
          wasisnapshot.wasm
          -m wasm2c
          -n wasi_snapshot_preview1
          -o wasm-to-c-fix.wasm
)

add_custom_target(
  wasm-to-c-fix-wasm ALL
  DEPENDS wasm-to-c-fix.wasm
)

add_custom_command(
  OUTPUT "c-to-elf-no-wasi.wasm"
  DEPENDS asm.wasm
          c-to-elf-fix
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --enable-multi-memory
	        --enable-exceptions
          c-to-elf-fix
          asm.wasm
          -m clang
          -n asm
          -o c-to-elf-no-wasi.wasm
)

add_custom_command(
  OUTPUT "c-to-elf-fix.wasm"
  DEPENDS wasisnapshot.wasm
          c-to-elf-no-wasi.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --enable-multi-memory
	        --enable-exceptions
          c-to-elf-no-wasi.wasm
          wasisnapshot.wasm
          -m wasm2c
          -n wasi_snapshot_preview1
          -o c-to-elf-fix.wasm
)

add_custom_target(
  c-to-elf-fix-wasm ALL
  DEPENDS c-to-elf-fix.wasm
)

add_custom_command(
  OUTPUT "link-elfs-no-wasi.wasm"
  DEPENDS asm.wasm
          link-elfs-fix
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --enable-multi-memory
	        --enable-exceptions
          link-elfs-fix
          asm.wasm
          -m clang
          -n asm
          -o link-elfs-no-wasi.wasm
)

add_custom_command(
  OUTPUT "link-elfs-fix.wasm"
  DEPENDS wasisnapshot.wasm
          link-elfs-no-wasi.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --enable-multi-memory
	        --enable-exceptions
          link-elfs-no-wasi.wasm
          wasisnapshot.wasm
          -m wasm2c
          -n wasi_snapshot_preview1
          -o link-elfs-fix.wasm
)

add_custom_target(
  link-elfs-fix-wasm ALL
  DEPENDS link-elfs-fix.wasm
)
