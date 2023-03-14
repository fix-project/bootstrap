#include <cstdlib>
#include <iostream>
#include <string>

#include "depfile.h"
#include "file_names.hh"
#include "name.hh"
#include "serialize.hh"

using namespace std;

int main( int argc, char* argv [] )
{
  if ( argc != 3 ) {
    cerr << "Usage: " << argv[0]
         << " path_to_base_path path_to_file\n";
  }

  string base_path = string( argv[1] );
  Name file_name = serialize_file( base_path, argv[2] );
  cout << "Serialized file: " << base64::encode( file_name ) << endl;

  return 0;
}
