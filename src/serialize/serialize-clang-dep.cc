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
         << " path_to_base_path path_to_resource_headers\n";
  }

  string base_path = string( argv[1]);
  vector<Name> system_dep_tree;
  for ( const char* file_name : system_deps ) {
    system_dep_tree.push_back( serialize_file( base_path, file_name ) );
  }
  Name system_dep_tree_name = serialize_tree( base_path, system_dep_tree );

  vector<Name> clang_dep_tree;
  string resource_dir_path ( argv[2] );
  for ( const char* file_name : clang_deps ) {
    string file_path = resource_dir_path + get_base_name(file_name);
    clang_dep_tree.push_back( serialize_file( base_path, file_path.c_str() ));
  }
  Name clang_dep_tree_name = serialize_tree( base_path, clang_dep_tree );

  cout << "Serialized system dependencies: " << base64::encode( system_dep_tree_name ) << endl;
  cout << "Serialized clang dependencies: " << base64::encode( clang_dep_tree_name ) << endl;

  return 0;
}
