#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "mmap.hh"
#include "name.hh"
#include "wabt/sha256.h"
#include "base64.hh"

static Name serialize_file( std::string base_path, std::string file_path ) {
  ReadOnlyFile file ( file_path );
  std::string hash;
  wabt::sha256( file, hash );
  Name blob_name ( hash, file.length(), ContentType::Blob);
  std::string file_name = base64::encode( blob_name );
  std::ofstream fout ( base_path + ".fix/" + file_name );
  fout << file.addr();
  fout.close();
  return blob_name;
}

static Name serialize_tree( std::string base_path, const std::vector<Name> & tree ) {
  std::string_view view( reinterpret_cast<const char*>( tree.data() ), tree.size() * sizeof( Name ) );
  std::string hash;
  wabt::sha256( view, hash );
  Name tree_name ( hash, view.size(), ContentType::Tree );
  std::string file_name = base64::encode( tree_name );
  std::ofstream fout ( base_path + ".fix/" + file_name );
  for ( auto name : tree ) {
    fout << base64::encode( name );
  }
  fout.close();
  return tree_name;
 }
