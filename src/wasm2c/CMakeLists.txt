file(GLOB LIB_SOURCES "*.cc")
add_library(wasm-to-c-lib STATIC ${LIB_SOURCES})
set_property(TARGET wasm-to-c-lib PROPERTY CXX_STANDARD 20)
set_property(TARGET wasm-to-c-lib PROPERTY CXX_STANDARD_REQUIRED ON)
