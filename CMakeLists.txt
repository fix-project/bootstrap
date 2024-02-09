cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0115 NEW)

set(WASI_SDK $ENV{HOME}/wasm-toolchain/sysroot)
set(WASIX_DIR $ENV{HOME}/wasm-toolchain/sysroot/wasix)

option(BUILD_SYS_DRIVER "Build local machine version" ON)

find_program(CCACHE ccache)

if(CCACHE)
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  message(STATUS "Using ${CCACHE} as compiler launcher.")
endif(CCACHE)

# Use compiler from wasm-toolchain if building to wasm
if(NOT BUILD_SYS_DRIVER)
  set(CMAKE_C_COMPILER ${WASI_SDK}/bin/clang)
  set(CMAKE_CXX_COMPILER ${WASI_SDK}/bin/clang++)
  set(CMAKE_AR ${WASI_SDK}/bin/llvm-ar)
  set(CMAKE_RANLIB ${WASI_SDK}/bin/llvm-ranlib)
  LIST(APPEND CMAKE_PROGRAM_PATH ${WASI_SDK}/bin)
  set(CMAKE_C_FLAGS "-I${WASIX_DIR}/include -I${WASI_SDK}/include -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_MMAN -L${WASIX_DIR}/lib -lwasix -L${WASI_SDK}/lib -L${WASI_SDK}/lib/wasm32-wasi -lwasi-emulated-signal -lwasi-emulated-mman -z stack-size=1048576")
  set(CMAKE_CXX_FLAGS "-I${WASIX_DIR}/include -I${WASI_SDK}/include -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_MMAN -D_LIBCPP_HAS_NO_EXCEPTIONS -D_LIBCXXABI_NO_EXCEPTIONS -fno-exceptions -mreference-types -include ${CMAKE_SOURCE_DIR}/src/fix-driver/ifstream-stub.hh")
  add_link_options(-L${WASIX_DIR}/lib -lwasix -L${WASI_SDK}/lib -L${WASI_SDK}/lib/wasm32-wasi -lwasi-emulated-signal -lwasi-emulated-mman -z stack-size=1048576)
endif()

project(bootstrap)

include(etc/build_type.cmake)

# LLVM setups
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON CACHE BOOL "" FORCE)
set(LLVM_ENABLE_PROJECTS "clang;lld" CACHE STRING "" FORCE)
set(LLVM_TARGET_ARCH "x86_64" CACHE STRING "" FORCE)
set(LLVM_DEFAULT_TARGET_TRIPLE "x86_64-pc-linux-gnu" CACHE STRING "" FORCE)
set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "" FORCE)
if (NOT BUILD_SYS_DRIVER)
  # set(CMAKE_CROSSCOMPILING ON CACHE BOOL "" FORCE)
  set(LLVM_TABLEGEN "$ENV{HOME}/wasm-toolchain/sysroot/bin/llvm-tblgen" CACHE STRING "" FORCE)
  set(CLANG_TABLEGEN "$ENV{HOME}/wasm-toolchain/sysroot/bin/clang-tblgen" CACHE STRING "" FORCE)
endif()
set(LLVM_ENABLE_THREADS OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_TESTS OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_UTILS OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(LIBCLANG_BUILD_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory($ENV{HOME}/wasm-toolchain/llvm-project/llvm llvm-project/llvm EXCLUDE_FROM_ALL)

include_directories($ENV{HOME}/wasm-toolchain/llvm-project/llvm/include)
include_directories(${CMAKE_BINARY_DIR}/llvm-project/llvm/include)
include_directories($ENV{HOME}/wasm-toolchain/llvm-project/clang/include)
include_directories(${CMAKE_BINARY_DIR}/llvm-project/llvm/tools/clang/include)
include_directories($ENV{HOME}/wasm-toolchain/llvm-project/lld/include)

# wabt setups
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_LIBWASM OFF CACHE BOOL "" FORCE)
include_directories($ENV{HOME}/wasm-toolchain/wabt/include)
add_subdirectory($ENV{HOME}/wasm-toolchain/wabt wabt EXCLUDE_FROM_ALL)

set(CLANG_LIBS clangFrontend clangDriver clangSerialization clangParse clangCodeGen clangSema clangAnalysis clangEdit clangAST clangLex clangBasic)
set(LLD_LIBS lldCommon lldELF)
set(LLVM_LIBS LLVMX86AsmParser LLVMX86CodeGen LLVMX86Desc LLVMX86Disassembler LLVMX86Info LLVMX86TargetMCA)

include(etc/clang_format.cmake)

include_directories(${CMAKE_BINARY_DIR}/wabt/include)
add_subdirectory(src)
