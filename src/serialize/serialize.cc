#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "handle.hh"
#include "handle_post.hh"
#include "handle_util.hh"
#include "repository.hh"

#include "depfile.hh"
#include "file_names.hh"

using namespace std;
namespace fs = std::filesystem;

Repository rp;

Handle<Fix> to_fix( Handle<AnyTree> h )
{
  return h.visit<Handle<Fix>>( []( auto h ) { return h.template into<Fix>(); } );
}

template<FixHandle... Args>
Handle<Fix> create_tree( Args... args )
{
  OwnedMutTree tree = OwnedMutTree::allocate( sizeof...( args ) );
  size_t i = 0;
  (
    [&] {
      tree[i] = args;
      i++;
    }(),
    ... );
  return to_fix( rp.create( make_shared<OwnedTree>( std::move( tree ) ) ) );
}

Handle<Fix> create_tree( vector<Handle<Fix>> vec )
{
  OwnedMutTree tree = OwnedMutTree::allocate( vec.size() );
  for ( size_t i = 0; i < vec.size(); i++ ) {
    tree[i] = vec[i];
  }
  return to_fix( rp.create( make_shared<OwnedTree>( std::move( tree ) ) ) );
}

Handle<Fix> create_blob( fs::path p )
{
  return rp.create( make_shared<OwnedBlob>( p ) );
}

Handle<Fix> create_blob_ref( Handle<Fix> arg )
{
  return handle::extract<Named>( arg )
    .transform( []( auto handle ) { return Handle<BlobRef>( Handle<Blob>( handle ) ); } )
    .or_else( []() -> optional<Handle<BlobRef>> { throw runtime_error( "Not reffable" ); } )
    .value();
}

Handle<Fix> create_tag( Handle<Fix> arg0, Handle<Fix> arg1, Handle<Fix> arg2 )
{
  OwnedMutTree mut_tree = OwnedMutTree::allocate( 3 );
  mut_tree[0] = arg0;
  mut_tree[1] = arg1;
  mut_tree[2] = arg2;

  auto tree = make_shared<OwnedTree>( std::move( mut_tree ) );
  auto handle = handle::create( tree );
  auto tagged = handle.visit<Handle<AnyTree>>( []( auto h ) { return h.tag(); } );

  rp.put( tagged, tree );
  return to_fix( tagged );
}

Handle<Fix> create_tree_ref( Handle<Fix> arg0 )
{
  Handle<AnyTreeRef> h
    = handle::extract<ObjectTree>( arg0 )
        .transform( [&]( auto handle ) -> Handle<AnyTreeRef> {
          return handle.template into<ObjectTreeRef>( rp.get( handle ).value()->size() );
        } )
        .or_else( [&]() -> optional<Handle<AnyTreeRef>> {
          return handle::extract<ValueTree>( arg0 ).transform( [&]( auto handle ) -> Handle<AnyTreeRef> {
            return handle.template into<ValueTreeRef>( rp.get( handle ).value()->size() );
          } );
        } )
        .or_else( []() -> optional<Handle<AnyTreeRef>> { throw runtime_error( "Not reffable" ); } )
        .value();
  return handle::fix( h );
}

int main( int argc, char* argv[] )
{
  if ( argc != 3 ) {
    cerr << "Usage: " << argv[0] << " path_to_base_path path_to_resource_headers\n";
  }

  string path_str = string( argv[1] );
  fs::path base_path( path_str );

  vector<Handle<Fix>> system_dep_tree;
  for ( const char* file_name : system_deps ) {
    system_dep_tree.push_back( create_blob_ref( create_blob( file_name ) ) );
  }
  auto system_dep_tree_name = create_tree( system_dep_tree );

  vector<Handle<Fix>> clang_dep_tree;
  string resource_dir_path( argv[2] );
  for ( const char* file_name : clang_deps ) {
    string file_path = resource_dir_path + get_base_name( file_name );
    clang_dep_tree.push_back( create_blob_ref( create_blob( file_path ) ) );
  }
  auto clang_dep_tree_name = create_tree( clang_dep_tree );

  array<string, 5> files = { "wasm-to-c-fix", "c-to-elf-fix", "link-elfs-fix", "map", "compile" };
  vector<Handle<Fix>> wasm_names;
  vector<Handle<Fix>> elf_names;
  for ( auto file : files ) {
    wasm_names.push_back( create_blob( base_path / ( "fix-build/src/fix-driver/" + file + ".wasm" ) ) );
    elf_names.push_back( create_blob( base_path / "tmp" / ( file + ".o" ) ) );
  }

  vector<Handle<Fix>> runnable_tags;
  auto compile_fixedpoint = create_tag( elf_names[4], elf_names[4], Handle<Literal>( "Runnable" ) );
  auto compile_runnable_tag = create_tag( compile_fixedpoint, elf_names[4], Handle<Literal>( "Runnable" ) );

  for ( size_t i = 0; i < 4; i++ ) {
    runnable_tags.push_back( create_tag( compile_fixedpoint, elf_names[i], Handle<Literal>( "Runnable" ) ) );
  }

  // {runnable-wasm2c.elf, runnable-clang.elf, runnable-lld.elf, system_dep_tree, clang_dep_tree, runnable-map.elf }
  auto compile_tool_tree_name = create_tree( create_tree_ref( runnable_tags[0] ),
                                             create_tree_ref( runnable_tags[1] ),
                                             create_tree_ref( runnable_tags[2] ),
                                             create_tree_ref( system_dep_tree_name ),
                                             create_tree_ref( clang_dep_tree_name ),
                                             create_tree_ref( runnable_tags[3] ) );

  // Tag compile_tool_tree bootstrap
  auto tagged_compile_tool_tree
    = create_tag( compile_fixedpoint, compile_tool_tree_name, Handle<Literal>( "Bootstrap" ) );

  // Encode: {r, runnable-compile.elf, tagged-compile-tool-tree}
  auto compile_encode_name
    = create_tree( Handle<Literal>( "compile" ), compile_fixedpoint, tagged_compile_tool_tree );

  rp.label( "compile-encode", compile_encode_name );

  size_t index = 0;
  for ( auto file : files ) {
    rp.label( file + "-wasm", wasm_names[index] );
    index++;
  }

  for ( size_t i = 0; i < 4; i++ ) {
    rp.label( files[i] + "-runnable-tag", runnable_tags[i] );
  }

  rp.label( "compile-fixed-point", compile_fixedpoint );
  rp.label( "compile-runnable-tag", compile_runnable_tag );
  rp.label( "compile-elf", elf_names[4] );
  rp.label( "system-dep-tree", system_dep_tree_name );
  rp.label( "clang-dep-tree", clang_dep_tree_name );

  return 0;
}
