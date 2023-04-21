#include <array>
#include <string>
#include <tuple>
#include <utility>

#define NUM_OUTPUT 256

std::tuple<std::array<std::string, NUM_OUTPUT>, std::string, std::string> wasm_to_c( const void* wasm_source,
                                                                                     size_t source_size );
