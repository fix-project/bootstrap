#include <fstream>
#include <iostream>
#include <string>

#include "mmap.hh"
#include "wasm-to-c.hh"

using namespace std;

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

  auto [c_outputs, h_header, h_impl_header, fixpoint_c] = wasm_to_c( wasm_content.addr(), wasm_content.length() );

  for ( unsigned int i = 0; i < NUM_OUTPUT; i++ ) {
    ofstream fout_c( output_path + "function" + std::to_string( i ) + ".c" );
    fout_c << c_outputs[i];
    fout_c.close();
  }

  ofstream fout_h_impl( output_path + "function-impl.h" );
  fout_h_impl << h_impl_header;
  fout_h_impl.close();

  ofstream fout_h( output_path + "function.h" );
  fout_h << h_header;
  fout_h.close();

  ofstream fout_c_fixpoint( output_path + "function-fixpoint.c" );
  fout_c_fixpoint << fixpoint_c;
  fout_c_fixpoint.close();

  return 0;
}
