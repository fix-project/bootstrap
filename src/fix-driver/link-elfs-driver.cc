#include "link-elfs.hh"

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
extern void attach_tree_ro_table_1( externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "attach_tree_ro_table_1" ) ) );
extern int32_t size_ro_table_1( void )
  __attribute__( ( import_module( "asm" ), import_name( "size_ro_table_1" ) ) );

externref fixpoint_apply( externref encode )
{
  attach_tree_ro_table_0( encode );

  attach_tree_ro_table_1( get_ro_table_0( 2 ) );
  std::vector<char*> dep_files;
  std::vector<size_t> dep_file_sizes;

  size_t n = size_ro_table_1();
  for ( size_t i = 0; i < n; i++ ) {
    attach_blob_ro_mem_0( get_ro_table_1( i ) );
    char* buffer = (char*)malloc( size_ro_mem_0() + 1 );
    ro_0_to_program_memory( buffer, 0, size_ro_mem_0() );
    buffer[size_ro_mem_0()] = '\0';
    dep_files.push_back( buffer );
    dep_file_sizes.push_back( size_ro_mem_0() );
  }

  auto [success, res] = link_elfs( dep_files, dep_file_sizes );

  if ( ( res.size() >> 16 ) > 0 ) {
    grow_rw_0( res.size() >> 16 );
  }
  program_memory_to_rw_0( 0, res.data(), res.size() );
  return create_blob_rw_mem_0( res.size() );
}
