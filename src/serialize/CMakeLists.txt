set_source_files_properties(serialize.cc PROPERTIES COMPILE_FLAGS -mavx)

add_executable(serialize "serialize.cc")
target_link_libraries(serialize util wabt)
