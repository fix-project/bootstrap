#include <string>

std::string c_to_elf( char* system_dep_files[],
                      char* clang_dep_files[],
                      char* function_c_buffer,
                      char* function_h_impl_buffer,
                      char* function_h_buffer,
                      char* function_fixpoint_h_buffer );
