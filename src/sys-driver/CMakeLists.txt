set (CMAKE_CXX_STANDARD 23)

add_executable(wasm-to-c-sys "wasm2c-driver.cc")
target_link_libraries(wasm-to-c-sys util)
target_link_libraries(wasm-to-c-sys wasm-to-c-lib wabt)

add_executable(c-to-elf-sys "c-to-elf-driver.cc" "mmap.cc")
target_link_libraries(c-to-elf-sys util)
target_link_libraries(c-to-elf-sys clang-lib ${CLANG_LIBS} ${LLVM_LIBS})

add_executable(link-elfs-sys "link-elfs-driver.cc")
target_link_libraries(link-elfs-sys util)
target_link_libraries(link-elfs-sys lld-lib ${LLD_LIBS} ${LLVM_LIBS})
