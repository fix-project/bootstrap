#include "wabt/apply-names.h"
#include "wabt/binary-reader-ir.h"
#include "wabt/binary-reader.h"
#include "wabt/error-formatter.h"
#include "wabt/feature.h"
#include "wabt/filenames.h"
#include "wabt/generate-names.h"
#include "wabt/ir.h"
#include "wabt/option-parser.h"
#include "wabt/stream.h"
#include "wabt/validator.h"
#include "wabt/wast-lexer.h"

#include "wabt/c-writer.h"

#include "wasminspector.hh"
#include "initcomposer.hh"
#include "memorystringstream.hh"

using namespace wabt;
using namespace std;

typedef char __attribute__( ( address_space( 10 ) ) ) * externref;
externref fixpoint_apply( externref encode ) __attribute__( ( export_name( "_fixpoint_apply" ) ) );

extern void program_memory_to_rw_0( int32_t, const void*, int32_t ) __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_0" ) ) );
extern void program_memory_to_rw_1( int32_t, const void*, int32_t ) __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_1" ) ) );
extern void program_memory_to_rw_2( int32_t, const void*, int32_t ) __attribute__( ( import_module( "asm" ), import_name( "program_memory_to_rw_2" ) ) );
extern void ro_0_to_program_memory( const void*, int32_t, int32_t ) __attribute__( ( import_module( "asm" ), import_name( "ro_0_to_program_memory" ) ) );
extern int32_t grow_rw_0( int32_t ) __attribute__( ( import_module( "asm" ), import_name( "grow_rw_0" ) ) );
extern int32_t grow_rw_1( int32_t ) __attribute__( ( import_module( "asm" ), import_name( "grow_rw_1" ) ) );
extern int32_t grow_rw_2( int32_t ) __attribute__( ( import_module( "asm" ), import_name( "grow_rw_2" ) ) );

extern void attach_blob_ro_mem_0( externref ) __attribute__( ( import_module( "fixpoint" ), import_name( "attach_blob_ro_mem_0" ) ) );
extern int32_t size_ro_mem_0( void ) __attribute__( ( import_module( "fixpoint" ), import_name( "size_ro_mem_0" ) ) );
extern externref create_blob_rw_mem_0( int32_t ) __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_0" ) ) );
extern externref create_blob_rw_mem_1( int32_t ) __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_1" ) ) );
extern externref create_blob_rw_mem_2( int32_t ) __attribute__( ( import_module( "fixpoint" ), import_name( "create_blob_rw_mem_2" ) ) );

extern externref get_ro_table_0( int32_t ) __attribute__( ( import_module( "asm" ), import_name( "get_ro_table_0" ) ) );
extern void attach_tree_ro_table_0( externref ) __attribute__( ( import_module( "fixpoint" ), import_name( "attach_tree_ro_table_0" ) ) );
extern void set_rw_table_0( int32_t, externref ) __attribute__( ( import_module( "asm" ), import_name( "set_rw_table_0" ) ) );
extern externref create_tree_rw_table_0( int32_t ) __attribute__( ( import_module( "fixpoint" ), import_name( "create_tree_rw_table_0" ) ) );

tuple<string, string, string> wasm_to_c( const void* wasm_source, size_t source_size ) __attribute__( ( export_name( "wasmtoc" ) ) ) {
  Errors errors;
  Module module;

  ReadBinaryOptions options;
  options.features.enable_multi_memory();
  options.features.enable_exceptions();
 
  ReadBinaryIr( "function", wasm_source, source_size, options, &errors, &module );
  ValidateModule( &module, &errors, options.features );
  GenerateNames( &module );
  ApplyNames( &module );
  
  wasminspector::WasmInspector inspector( &module, &errors );
  inspector.Validate();

  for ( auto index : inspector.GetExportedROMemIndex() ) {
   module.memories[index]->bounds_checked = true;
  }
  for ( auto index : inspector.GetExportedRWMemIndex() ) {
   module.memories[index]->bounds_checked = true;
  }
  
  WriteCOptions write_c_options;
  write_c_options.module_name = "function";
  MemoryStringStream c_stream;
  MemoryStringStream h_stream;
  WriteC( &c_stream, &h_stream, "function.h", &module, write_c_options );

  string fix_header (initcomposer::compose_header( "function", &module, &errors, &inspector ));

  return { c_stream.ReleaseStringBuf(), h_stream.ReleaseStringBuf(), fix_header };
}

externref fixpoint_apply( externref encode ) {
  attach_tree_ro_table_0( encode );
  attach_blob_ro_mem_0( get_ro_table_0( 2 ) );

  char* buffer = (char*)malloc( size_ro_mem_0() );
  ro_0_to_program_memory( buffer, 0, size_ro_mem_0() );

  auto result = wasm_to_c( buffer, size_ro_mem_0() );
  auto& c_output = get<0>( result );
  auto& h_output = get<1>( result );
  auto& fix_header = get<2>( result );

  if ( ( c_output.size() >> 12 ) > 0 ) {
    grow_rw_0( c_output.size() >> 12 );
  }
  if ( ( h_output.size() >> 12 ) > 0 ) {
    grow_rw_1( h_output.size() >> 12 );
  }
  if ( ( fix_header.size() >> 12 ) > 0 ) {
    grow_rw_2( fix_header.size() >> 12 );
  }

  program_memory_to_rw_0(0, c_output.data(), c_output.size());
  program_memory_to_rw_1(0, h_output.data(), h_output.size());
  program_memory_to_rw_2(0, fix_header.data(), fix_header.size());

  externref c_blob = create_blob_rw_mem_0( c_output.size() );
  externref h_blob = create_blob_rw_mem_1( h_output.size() );
  externref fix_header_blob = create_blob_rw_mem_2( fix_header.size() );

  free( buffer );

  set_rw_table_0( 0, c_blob );
  set_rw_table_0( 1, fix_header_blob );
  set_rw_table_0( 2, h_blob );
  return create_tree_rw_table_0( 3 );
}
