file(GLOB LIB_SOURCES "*.cc")
add_library(wasm-to-c-lib STATIC ${LIB_SOURCES})
set_property(TARGET wasm-to-c-lib PROPERTY CXX_STANDARD 20)
set_property(TARGET wasm-to-c-lib PROPERTY CXX_STANDARD_REQUIRED ON)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/wasm-rt-impl.c.h"
  COMMAND xxd -n wasm_rt_impl_c -i "${CMAKE_CURRENT_SOURCE_DIR}/wasm-rt-impl.c" "${CMAKE_CURRENT_SOURCE_DIR}/wasm-rt-impl.c.h"
  COMMAND sed -i "s/unsigned/static/" "${CMAKE_CURRENT_SOURCE_DIR}/wasm-rt-impl.c.h" 
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/wasm-rt-impl.c"
)
add_custom_target(wasm-rt-impl-fixpoint DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/wasm-rt-impl.c.h")
add_dependencies(wasm-to-c-lib wasm-rt-impl-fixpoint)
