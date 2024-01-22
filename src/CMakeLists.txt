include_directories(wasm2c)
add_subdirectory(wasm2c)

include_directories(clang)
add_subdirectory(clang)

include_directories(lld)
add_subdirectory(lld)

# Build corresponding driver
if(BUILD_SYS_DRIVER)
  include_directories(util)
  add_subdirectory(sys-driver)
  add_subdirectory(serialize)
else()
  add_subdirectory(fix-driver)
endif()
