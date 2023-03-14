set_source_files_properties(serialize-clang-dep.cc PROPERTIES COMPILE_FLAGS -mavx)
set_source_files_properties(serialize-file.cc PROPERTIES COMPILE_FLAGS -mavx)

add_executable(serialize-clang-dep "serialize-clang-dep.cc")
target_link_libraries(serialize-clang-dep util wabt)

add_executable(serialize-file "serialize-file.cc")
target_link_libraries(serialize-file util wabt)
