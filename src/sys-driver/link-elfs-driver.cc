#include <fstream>
#include <iostream>
#include <vector>

#include "link-elfs.hh"
#include "mmap.hh"

using namespace std;

int main( int argc, char* argv[] )
{
  if ( argc < 3 ) {
    cerr << "Usage: " << argv[0] << " output_path input_file_1_path input_file_2_path ...";
  }

  vector<char*> dep_files;
  vector<size_t> dep_file_sizes;

  for ( int i = 2; i < argc; i++ ) {
    ReadOnlyFile file { argv[i] };
    char* buffer = (char*)malloc( file.length() + 1 );
    memcpy( buffer, file.addr(), file.length() );
    buffer[file.length()] = '\0';
    dep_files.push_back( buffer );
    dep_file_sizes.push_back( file.length() );
  }

  auto [success, res] = link_elfs( dep_files, dep_file_sizes );
  ofstream fout( argv[1] );
  fout << res;
  fout.close();

  for ( auto ptr : dep_files ) {
    free( ptr );
  }

  if ( not success ) {
    cerr << res << "\n";
    return 1;
  }
  return 0;
}
