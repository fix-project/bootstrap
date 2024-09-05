set_source_files_properties(serialize.cc PROPERTIES COMPILE_FLAGS -mavx)
set_source_files_properties(serialize-to-bins.cc PROPERTIES COMPILE_FLAGS -mavx)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory( ${PROJECT_SOURCE_DIR}/fix fixpoint )
include_directories( ${PROJECT_SOURCE_DIR}/fix )

add_executable(serialize "serialize.cc")
target_link_libraries(serialize storage)

add_executable(serialize-to-bins "serialize-to-bins.cc")
target_link_libraries(serialize-to-bins storage)
