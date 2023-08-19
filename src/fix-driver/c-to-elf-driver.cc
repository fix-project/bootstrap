#include "c-to-elf.hh"

typedef char __attribute__( ( address_space( 10 ) ) ) * externref;
externref fixpoint_apply( externref encode ) __attribute__( ( export_name( "_fixpoint_apply" ) ) );

extern void program_memory_to_rw_0( int32_t, const void*, int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_0" ) ) );
extern void ro_0_to_program_memory( const void*, int32_t, int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "ro_0_to_program_memory" ) ) );
extern int32_t grow_rw_0( int32_t ) __attribute__( ( import_module( "asm" ), import_name( "grow_rw_0" ) ) );

extern void attach_blob_ro_mem_0( externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "attach_blob_ro_mem_0" ) ) );
extern int32_t size_ro_mem_0( void )
  __attribute__( ( import_module( "fixpoint" ), import_name( "size_ro_mem_0" ) ) );
extern externref create_blob_rw_mem_0( int32_t )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_0" ) ) );

extern externref get_ro_table_0( int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "get_ro_table_0" ) ) );
extern void attach_tree_ro_table_0( externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "attach_tree_ro_table_0" ) ) );
extern externref get_ro_table_1( int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "get_ro_table_1" ) ) );
extern size_t size_ro_table_1( void ) __attribute__( ( import_module( "asm" ), import_name( "size_ro_table_1" ) ) );
extern void attach_tree_ro_table_1( externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "attach_tree_ro_table_1" ) ) );

// resource_limits | { resource_limits | clang | system_dep | clang_dep } | { h_impl | h } | c
externref fixpoint_apply( externref encode )
{
  attach_tree_ro_table_0( encode );
  attach_tree_ro_table_1( get_ro_table_0( 1 ) );

  externref system_dep_tree = get_ro_table_1( 2 );
  externref clang_dep_tree = get_ro_table_1( 3 );

  attach_tree_ro_table_1( system_dep_tree );
  std::vector<char*> system_dep_files;
  for ( size_t i = 0; i < size_ro_table_1(); i++ ) {
    attach_blob_ro_mem_0( get_ro_table_1( i ) );
    char* buffer = (char*)malloc( size_ro_mem_0() + 1 );
    ro_0_to_program_memory( buffer, 0, size_ro_mem_0() );
    buffer[size_ro_mem_0()] = '\0';
    system_dep_files.push_back( buffer );
  }

  attach_tree_ro_table_1( clang_dep_tree );
  std::vector<char*> clang_dep_files;
  for ( size_t i = 0; i < size_ro_table_1(); i++ ) {
    attach_blob_ro_mem_0( get_ro_table_1( i ) );
    char* buffer = (char*)malloc( size_ro_mem_0() + 1 );
    ro_0_to_program_memory( buffer, 0, size_ro_mem_0() );
    buffer[size_ro_mem_0()] = '\0';
    clang_dep_files.push_back( buffer );
  }

  attach_tree_ro_table_1( get_ro_table_0( 2 ) );
  externref h_impl_blob = get_ro_table_1( 0 );
  externref h_blob = get_ro_table_1( 1 );

  attach_blob_ro_mem_0( h_impl_blob );
  char* h_impl_buffer = (char*)malloc( size_ro_mem_0() + 1 );
  ro_0_to_program_memory( h_impl_buffer, 0, size_ro_mem_0() );
  h_impl_buffer[size_ro_mem_0()] = '\0';

  attach_blob_ro_mem_0( h_blob );
  char* h_buffer = (char*)malloc( size_ro_mem_0() + 1 );
  ro_0_to_program_memory( h_buffer, 0, size_ro_mem_0() );
  h_buffer[size_ro_mem_0()] = '\0';

  externref c_blob = get_ro_table_0( 3 );
  attach_blob_ro_mem_0( c_blob );
  char* c_buffer = (char*)malloc( size_ro_mem_0() + 1 );
  ro_0_to_program_memory( c_buffer, 0, size_ro_mem_0() );
  c_buffer[size_ro_mem_0()] = '\0';

  std::pair<bool, std::string> result
    = c_to_elf( system_dep_files, clang_dep_files, c_buffer, h_impl_buffer, h_buffer );

  std::string res = result.second;

  if ( ( res.size() >> 16 ) > 0 ) {
    grow_rw_0( res.size() >> 16 );
  }
  program_memory_to_rw_0( 0, res.data(), res.size() );
  return create_blob_rw_mem_0( res.size() );
}
