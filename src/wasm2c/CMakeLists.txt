file(GLOB LIB_SOURCES "*.cc")
add_library(wasm-to-c-lib STATIC ${LIB_SOURCES})
set_property(TARGET wasm-to-c-lib PROPERTY CXX_STANDARD 20)
set_property(TARGET wasm-to-c-lib PROPERTY CXX_STANDARD_REQUIRED ON)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/shims.h.inc"
  COMMAND xxd -n shims_data -i "${CMAKE_CURRENT_SOURCE_DIR}/shims.h" "${CMAKE_CURRENT_SOURCE_DIR}/shims.h.inc"
  COMMAND sed -i "s/unsigned/static/" "${CMAKE_CURRENT_SOURCE_DIR}/shims.h.inc"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shims.h"
)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/shims.c.inc"
  COMMAND xxd -n shims_impl_data -i "${CMAKE_CURRENT_SOURCE_DIR}/shims.c" "${CMAKE_CURRENT_SOURCE_DIR}/shims.c.inc"
  COMMAND sed -i "s/unsigned/static/" "${CMAKE_CURRENT_SOURCE_DIR}/shims.c.inc"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shims.c"
)

add_custom_target(fixpoint-shims DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shims.h.inc")
add_custom_target(fixpoint-shims-impl DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shims.c.inc")
add_dependencies(wasm-to-c-lib fixpoint-shims fixpoint-shims-impl)
