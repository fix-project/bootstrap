#include <bitset>
#include <fstream>
#include <iostream>
#include <memory>

#include "c-to-elf.hh"
#include "depfile.hh"
#include "file_names.hh"
#include "mmap.hh"

using namespace std;

int main( int argc, char* argv[] )
{
  if ( argc != 6 ) {
    cerr << "Usage: " << argv[0] << " path_to_c path_to_h_impl path_to_h path_to_resource_headers path_to_obj\n";
  }

  ReadOnlyFile c_content { argv[1] };
  ReadOnlyFile h_impl_content { argv[2] };
  ReadOnlyFile h_content { argv[3] };

  std::vector<ReadOnlyFile> system_dep_files;
  std::vector<char*> system_dep_content;
  for ( auto system_dep_path : system_deps ) {
    system_dep_files.push_back( ReadOnlyFile( system_dep_path ) );
    system_dep_content.push_back( system_dep_files.back().addr() );
  }
  std::vector<ReadOnlyFile> clang_dep_files;
  std::vector<char*> clang_dep_content;
  for ( auto clang_dep_path : clang_deps ) {
    std::string resource_dir_path = std::string( argv[4] ) + get_base_name( clang_dep_path );
    clang_dep_files.push_back( ReadOnlyFile( resource_dir_path ) );
    clang_dep_content.push_back( clang_dep_files.back().addr() );
  }

  string elf_res
    = c_to_elf( system_dep_content, clang_dep_content, c_content.addr(), h_impl_content.addr(), h_content.addr() );
  ofstream fout_obj( argv[5] );
  fout_obj << elf_res;
  fout_obj.close();

  return 0;
}
