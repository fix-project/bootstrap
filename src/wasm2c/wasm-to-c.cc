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

#include "initcomposer.hh"
#include "memorystringstream.hh"
#include "wasminspector.hh"

#include "wasm-to-c.hh"

using namespace wabt;
using namespace std;

tuple<array<string, NUM_OUTPUT>, string, string, string> wasm_to_c( const void* wasm_source, size_t source_size )
{
  Errors errors;
  Module module;

  ReadBinaryOptions options;
  options.features.enable_multi_memory();
  options.features.enable_exceptions();
  options.read_debug_names = true;

  ReadBinaryIr( "function", wasm_source, source_size, options, &errors, &module );

  array<MemoryStringStream, NUM_OUTPUT> c_streams;
  vector<Stream*> c_stream_ptrs;
  for ( auto& s : c_streams ) {
    c_stream_ptrs.emplace_back( &s );
  }
  MemoryStringStream h_impl_stream;
  MemoryStringStream h_stream;
  string fixpoint_c;

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
  WriteC( std::move( c_stream_ptrs ),
          &h_stream,
          &h_impl_stream,
          "function.h",
          "function-impl.h",
          &module,
          write_c_options );

  fixpoint_c = initcomposer::compose_header( "function", &module, &errors, &inspector );

  array<string, NUM_OUTPUT> c_outputs;
  for ( unsigned int i = 0; i < NUM_OUTPUT; i++ ) {
    c_outputs[i] = c_streams[i].ReleaseStringBuf();
  }
  return { c_outputs, h_stream.ReleaseStringBuf(), h_impl_stream.ReleaseStringBuf(), fixpoint_c };
}
