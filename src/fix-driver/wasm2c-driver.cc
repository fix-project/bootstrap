#include "wasm-to-c.hh"

typedef char __attribute__( ( address_space( 10 ) ) ) * externref;
externref fixpoint_apply( externref encode ) __attribute__( ( export_name( "_fixpoint_apply" ) ) );

extern void program_memory_to_rw_0( int32_t, const void*, int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_0" ) ) );
extern void program_memory_to_rw_1( int32_t, const void*, int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_1" ) ) );
extern void program_memory_to_rw_2( int32_t, const void*, int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_2" ) ) );
extern void ro_0_to_program_memory( const void*, int32_t, int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "ro_0_to_program_memory" ) ) );
extern int32_t grow_rw_0( int32_t ) __attribute__( ( import_module( "asm" ), import_name( "grow_rw_0" ) ) );
extern int32_t grow_rw_1( int32_t ) __attribute__( ( import_module( "asm" ), import_name( "grow_rw_1" ) ) );
extern int32_t grow_rw_2( int32_t ) __attribute__( ( import_module( "asm" ), import_name( "grow_rw_2" ) ) );

extern void attach_blob_ro_mem_0( externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "attach_blob_ro_mem_0" ) ) );
extern int32_t size_ro_mem_0( void )
  __attribute__( ( import_module( "fixpoint" ), import_name( "size_ro_mem_0" ) ) );
extern externref create_blob_rw_mem_0( int32_t )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_0" ) ) );
extern externref create_blob_rw_mem_1( int32_t )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_1" ) ) );
extern externref create_blob_rw_mem_2( int32_t )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_2" ) ) );

extern externref get_ro_table_0( int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "get_ro_table_0" ) ) );
extern void attach_tree_ro_table_0( externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "attach_tree_ro_table_0" ) ) );
extern void set_rw_table_0( int32_t, externref )
  __attribute__( ( import_module( "asm" ), import_name( "set_rw_table_0" ) ) );
extern int32_t grow_rw_table_0( int32_t size, externref pointer )
  __attribute__( ( import_module( "asm" ), import_name( "grow_rw_table_0" ) ) );
extern externref create_tree_rw_table_0( int32_t )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_tree_rw_table_0" ) ) );
extern void set_rw_table_1( int32_t, externref )
  __attribute__( ( import_module( "asm" ), import_name( "set_rw_table_1" ) ) );
extern int32_t grow_rw_table_1( int32_t size, externref pointer )
  __attribute__( ( import_module( "asm" ), import_name( "grow_rw_table_1" ) ) );
extern externref create_tree_rw_table_1( int32_t )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_tree_rw_table_1" ) ) );

externref fixpoint_apply( externref encode )
{
  attach_tree_ro_table_0( encode );
  attach_blob_ro_mem_0( get_ro_table_0( 2 ) );

  char* buffer = (char*)malloc( size_ro_mem_0() );
  ro_0_to_program_memory( buffer, 0, size_ro_mem_0() );

  auto [c_outputs, h_header, h_impl_header] = wasm_to_c( buffer, size_ro_mem_0() );

  int c_written = 0;
  for ( int i = 0; i < 256; i++ ) {
    std::string c_output = c_outputs.at( i );
    if ( c_output.rfind( "/* Empty wasm2c", 0 ) == 0 ) {
      continue;
    }
    grow_rw_0( ( c_output.size() >> 16 ) + 1 );
    program_memory_to_rw_0( 0, c_output.data(), c_output.size() );
    externref c_blob = create_blob_rw_mem_0( c_output.size() );
    set_rw_table_1( c_written, c_blob );
    c_written++;
  }

  externref c_tree = create_tree_rw_table_1( c_written );

  grow_rw_0( ( h_header.size() >> 16 ) + 1 );
  program_memory_to_rw_0( 0, h_header.data(), h_header.size() );
  externref h_blob = create_blob_rw_mem_0( h_header.size() );

  grow_rw_0( ( h_impl_header.size() >> 16 ) + 1 );
  program_memory_to_rw_0( 0, h_impl_header.data(), h_impl_header.size() );
  externref h_impl_blob = create_blob_rw_mem_0( h_impl_header.size() );

  free( buffer );

  grow_rw_table_1( 2, h_impl_blob );
  set_rw_table_1( 0, h_impl_blob );
  set_rw_table_1( 1, h_blob );
  externref h_tree = create_tree_rw_table_1( 2 );

  set_rw_table_0( 0, h_tree );
  set_rw_table_0( 1, c_tree );
  return create_tree_rw_table_0( 2 );
}
