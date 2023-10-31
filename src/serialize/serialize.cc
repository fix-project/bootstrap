#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "base64.hh"
#include "depfile.hh"
#include "file_names.hh"
#include "handle.hh"
#include "mmap.hh"
#include "wabt/sha256.h"

using namespace std;
namespace fs = std::filesystem;

Handle serialize_file( fs::path objects, string file_path )
{
  ReadOnlyFile file( file_path );
  string hash;
  wabt::sha256( file, hash );
  Handle blob_name( hash, file.length(), ContentType::Blob );
  string file_name = base64::encode( blob_name );
  ofstream fout( objects / file_name );
  fout << string_view( file );
  fout.close();
  return blob_name;
}

Handle serialize_tree( fs::path objects, const vector<Handle>& tree )
{
  string_view view( reinterpret_cast<const char*>( tree.data() ), tree.size() * sizeof( Handle ) );
  string hash;
  wabt::sha256( view, hash );
  Handle tree_name( hash, view.size() / sizeof( Handle ), ContentType::Tree );
  string file_name = base64::encode( tree_name );
  ofstream fout( objects / file_name );
  for ( auto name : tree ) {
    fout << base64::encode( name );
  }
  fout.close();
  return tree_name;
}

Handle serialize_tag( fs::path objects, const vector<Handle>& tag )
{
  string_view view( reinterpret_cast<const char*>( tag.data() ), tag.size() * sizeof( Handle ) );
  string hash;
  wabt::sha256( view, hash );
  Handle tag_name( hash, view.size() / sizeof( Handle ), ContentType::Tag );
  string file_name = base64::encode( tag_name );
  ofstream fout( objects / file_name );
  for ( auto name : tag ) {
    fout << base64::encode( name );
  }
  fout.close();
  return tag_name;
}

int main( int argc, char* argv[] )
{
  if ( argc != 3 ) {
    cerr << "Usage: " << argv[0] << " path_to_base_path path_to_resource_headers\n";
  }

  string path_str = string( argv[1] );
  fs::path base_path( path_str );
  auto fix = base_path / ".fix";
  auto objects = fix / "objects";
  auto refs = fix / "refs";
  fs::create_directories( objects );
  fs::create_directories( refs );

  vector<Handle> system_dep_tree;
  for ( const char* file_name : system_deps ) {
    system_dep_tree.push_back( serialize_file( objects, file_name ) );
  }
  Handle system_dep_tree_name = serialize_tree( objects, system_dep_tree );

  vector<Handle> clang_dep_tree;
  string resource_dir_path( argv[2] );
  for ( const char* file_name : clang_deps ) {
    string file_path = resource_dir_path + get_base_name( file_name );
    clang_dep_tree.push_back( serialize_file( objects, file_path.c_str() ) );
  }
  Handle clang_dep_tree_name = serialize_tree( objects, clang_dep_tree );

  array<string, 5> files = { "wasm-to-c-fix", "c-to-elf-fix", "link-elfs-fix", "map", "compile" };
  vector<Handle> wasm_names;
  vector<Handle> elf_names;
  for ( auto file : files ) {
    wasm_names.push_back( serialize_file( objects, base_path / ( "fix-build/src/fix-driver/" + file + ".wasm" ) ) );
    elf_names.push_back( serialize_file( objects, base_path / "tmp" / ( file + ".o" ) ) );
  }

  vector<Handle> runnable_tags;
  for ( size_t i = 0; i < 4; i++ ) {
    vector<Handle> runnable_tag;
    runnable_tag.push_back( elf_names[i] );
    runnable_tag.push_back( elf_names[4] );
    runnable_tag.push_back( Handle( "Runnable" ) );
    runnable_tags.push_back( serialize_tag( objects, runnable_tag ) );
  }

  vector<Handle> runnable_compile;
  runnable_compile.push_back( elf_names[4] );
  runnable_compile.push_back( elf_names[4] );
  runnable_compile.push_back( Handle( "Runnable" ) );
  Handle runnable_compile_name = serialize_tag( objects, runnable_compile );

  // {runnable-wasm2c.elf, runnable-clang.elf, runnable-lld.elf, system_dep_tree, clang_dep_tree, runnable-map.elf }
  vector<Handle> compile_tool_tree;
  compile_tool_tree.push_back( runnable_tags[0] );
  compile_tool_tree.push_back( runnable_tags[1] );
  compile_tool_tree.push_back( runnable_tags[2] );
  compile_tool_tree.push_back( system_dep_tree_name );
  compile_tool_tree.push_back( clang_dep_tree_name );
  compile_tool_tree.push_back( runnable_tags[3] );
  Handle compile_tool_tree_name = serialize_tree( objects, compile_tool_tree );

  // Tag compile_tool_tree bootstrap
  vector<Handle> compile_tool_tree_tag;
  compile_tool_tree_tag.push_back( compile_tool_tree_name );
  compile_tool_tree_tag.push_back( elf_names[4] );
  compile_tool_tree_tag.push_back( Handle( "Bootstrap" ) );
  Handle compile_tool_tree_tag_name = serialize_tag( objects, compile_tool_tree_tag );

  // Encode: {r, runnable-compile.elf, tagged-compile-tool-tree}
  vector<Handle> compile_encode;
  compile_encode.push_back( Handle( "unused" ) );
  compile_encode.push_back( runnable_compile_name );
  compile_encode.push_back( compile_tool_tree_tag_name );
  Handle compile_encode_name = serialize_tag( objects, compile_encode );

  std::ofstream compile_tool_out( refs / "compile-encode" );
  compile_tool_out << base64::encode( compile_encode_name );
  compile_tool_out.close();

  size_t index = 0;
  for ( auto file : files ) {
    std::ofstream fout( refs / ( file + "-wasm" ) );
    fout << base64::encode( wasm_names[index] );
    fout.close();
    index++;
  }

  for ( size_t i = 0; i < 4; i++ ) {
    std::ofstream fout( refs / ( files[i] + "-runnable-tag" ) );
    fout << base64::encode( runnable_tags[i] );
    fout.close();
  }

  {
    std::ofstream fout ( refs / ( "compile-runnable-tag" ) );
    fout << base64::encode( runnable_compile_name );
    fout.close();
  }

  {
    std::ofstream fout ( refs / ( "compile-elf" ) );
    fout << base64::encode( elf_names[4] );
    fout.close();
  }

  std::ofstream st_out( refs / "system-dep-tree" );
  st_out << base64::encode( system_dep_tree_name );
  st_out.close();
  std::ofstream ct_out( refs / "clang-dep-tree" );
  ct_out << base64::encode( clang_dep_tree_name );
  ct_out.close();

  return 0;
}
