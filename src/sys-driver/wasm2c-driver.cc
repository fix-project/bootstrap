#include <fstream>
#include <iostream>
#include <string>

#include "mmap.hh"
#include "wasm-to-c.hh"

using namespace std;

int main( int argc, char* argv[] )
{
  if ( argc != 3 ) {
    cerr << "Usage: " << argv[0] << " path_to_wasm_file\n";
  }

  ReadOnlyFile wasm_content { argv[2] };

  auto [c_outputs, h_header, h_impl_header, fixpoint_header]
    = wasm_to_c( wasm_content.addr(), wasm_content.length() );

  for ( unsigned int i = 0; i < NUM_OUTPUT; i++ ) {
    ofstream fout_c( "function" + std::to_string( i ) + ".c" );
    fout_c << c_outputs[i];
    fout_c.close();
  }

  ofstream fout_h_impl( "function-impl.h" );
  fout_h_impl << h_impl_header;
  fout_h_impl.close();

  ofstream fout_h( "function.h" );
  fout_h << fixpoint_header;
  fout_h.close();

  ofstream fout_h_fixpoint( "function-fixpoint.h" );
  fout_h_fixpoint << h_header;
  fout_h_fixpoint.close();

  return 0;
}
