#include <fstream>
#include <iostream>
#include <string>

#include "mmap.hh"
#include "wasm-to-c.hh"

using namespace std;

void write_c_output( string output_path, size_t index, string content )
{
  if ( content.rfind( "/* Empty wasm2c", 0 ) == 0 ) {
    return;
  }

  ofstream fout_c( output_path + "function" + to_string( index ) + ".c" );
  fout_c << content;
  fout_c.close();
}

int main( int argc, char* argv[] )
{
  if ( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " path_to_wasm_file output_path\n";
  }

  string output_path = "";
  if ( argc == 3 ) {
    output_path = string( argv[2] ) + "/";
  }
  ReadOnlyFile wasm_content { argv[1] };

  auto stream_finish_callback = bind( write_c_output, output_path, placeholders::_1, placeholders::_2 );

  auto [h_header, h_impl_header, errors]
    = wasm_to_c( wasm_content.addr(), wasm_content.length(), stream_finish_callback );

  ofstream fout_h_impl( output_path + "function-impl.h" );
  fout_h_impl << h_impl_header;
  fout_h_impl.close();

  ofstream fout_h( output_path + "function.h" );
  fout_h << h_header;
  fout_h.close();

  if ( errors ) {
    cerr << *errors << endl;
    return 1;
  }

  return 0;
}
