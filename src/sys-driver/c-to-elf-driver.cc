#include <bitset>
#include <fstream>
#include <iostream>
#include <memory>

#include "c-to-elf.hh"
#include "depfile.h"
#include "filenames.hh"
#include "mmap.hh"

using namespace std;

int main( int argc, char* argv[] )
{
  if ( argc != 4 ) {
    cerr << "Usage: " << argv[0]
         << " path_to_c path_to_h_impl path_to_h path_to_fixpoint_header path_to_resource_headers path_to_obj\n";
  }

  ReadOnlyFile c_content { argv[1] };
  ReadOnlyFile h_impl_content { argv[2] };
  ReadOnlyFile h_content { argv[3] };
  ReadOnlyFile h_fixpoint_content { argv[4] };

  std::vector<ReadOnlyFile> system_dep_files;
  char* system_dep_content[63];
  size_t dep_index = 0;
  for ( auto system_dep_path : system_deps ) {
    system_dep_files.push_back( ReadOnlyFile( system_dep_path ) );
    system_dep_content[dep_index] = system_dep_files.back().addr();
    dep_index++;
  }
  dep_index = 0;
  std::vector<ReadOnlyFile> clang_dep_files;
  char* clang_dep_content[92];
  for ( auto clang_dep_path : clang_deps ) {
    std::string resource_dir_path = std::string( argv[5] ) + get_base_name( clang_dep_path );
    clang_dep_files.push_back( ReadOnlyFile( resource_dir_path ) );
    clang_dep_content[dep_index] = clang_dep_files.back().addr();
  }

  string elf_res = c_to_elf( system_dep_content,
                             clang_dep_content,
                             c_content.addr(),
                             h_impl_content.addr(),
                             h_content.addr(),
                             h_fixpoint_content.addr() );
  ofstream fout_obj( argv[6] );
  fout_obj << elf_res;
  fout_obj.close();

  return 0;
}
