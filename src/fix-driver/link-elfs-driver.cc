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
extern int32_t get_memory_size_ro_mem_0( void )
  __attribute__( ( import_module( "fixpoint" ), import_name( "get_memory_size_ro_mem_0" ) ) );
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

extern void program_memory_to_rw_1( int32_t, const void*, int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_1" ) ) );
extern externref create_blob_rw_mem_1( int32_t )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_1" ) ) );
extern void program_memory_to_rw_2( int32_t, const void*, int32_t )
  __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_2" ) ) );
extern externref create_blob_rw_mem_2( int32_t )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_2" ) ) );

extern externref create_tag( externref, externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "create_tag" ) ) );
extern uint32_t get_value_type( externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "get_value_type" ) ) );
extern uint32_t fixpoint_equality( externref, externref )
  __attribute__( ( import_module( "fixpoint" ), import_name( "equality" ) ) );

// The input is a result Tag from map.wat.  If it's an Okay, then it contains a Tree of results from the compiler.
externref fixpoint_apply( externref encode )
{
  attach_tree_ro_table_0( encode );

  program_memory_to_rw_2( 0, "Okay", 4 );
  externref okay = create_blob_rw_mem_2( 4 );

  // Look at the Tag
  attach_tree_ro_table_1( get_ro_table_0( 2 ) );

  if ( not fixpoint_equality( get_ro_table_1( 2 ), okay ) ) {
    // Propagate the error
    return get_ro_table_0( 2 );
  }

  // Look at the Object
  attach_tree_ro_table_1( get_ro_table_1( 0 ) );
  std::vector<char*> dep_files;
  std::vector<size_t> dep_file_sizes;

  size_t n = size_ro_table_1();
  for ( size_t i = 0; i < n; i++ ) {
    externref tag = get_ro_table_1(i);
    attach_tree_ro_table_0(tag);
    if (not fixpoint_equality(get_ro_table_0(2), okay)) {
      // not okay, return the error message
      return tag;
    }
    // unwrap the result
    attach_blob_ro_mem_0(get_ro_table_0(0));
    char* buffer = (char*)malloc( get_memory_size_ro_mem_0() + 1 );
    ro_0_to_program_memory( buffer, 0, get_memory_size_ro_mem_0() );
    buffer[get_memory_size_ro_mem_0()] = '\0';
    dep_files.push_back( buffer );
    dep_file_sizes.push_back( get_memory_size_ro_mem_0() );
  }

  auto [success, res] = link_elfs( dep_files, dep_file_sizes );

  if ( ( res.size() >> 16 ) > 0 ) {
    grow_rw_0( res.size() >> 16 );
  }
  program_memory_to_rw_0( 0, res.data(), res.size() );
  externref blob = create_blob_rw_mem_0( res.size() );
  std::string msg = success ? "Okay" : "Error";
  program_memory_to_rw_1( 0, msg.data(), msg.size() );
  externref msg_blob = create_blob_rw_mem_1( msg.size() );
  return create_tag( blob, msg_blob );
}
