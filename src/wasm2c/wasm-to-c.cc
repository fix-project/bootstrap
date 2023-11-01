#include "wabt/apply-names.h"
#include "wabt/binary-reader-ir.h"
#include "wabt/binary-reader.h"
#include "wabt/c-writer.h"
#include "wabt/error-formatter.h"
#include "wabt/feature.h"
#include "wabt/filenames.h"
#include "wabt/generate-names.h"
#include "wabt/ir.h"
#include "wabt/option-parser.h"
#include "wabt/sha256.h"
#include "wabt/stream.h"
#include "wabt/validator.h"
#include "wabt/wast-lexer.h"

#include "initcomposer.hh"
#include "memorystringstream.hh"
#include "wasminspector.hh"

#include "wasm-to-c.hh"
#include <functional>

using namespace wabt;
using namespace std;

static vector<size_t> consistent_hashing_name_to_output_file_index( vector<Func*>::const_iterator func_begin,
                                                                    vector<Func*>::const_iterator func_end,
                                                                    size_t num_imports,
                                                                    size_t num_streams,
                                                                    size_t num_parallelism )
{
  vector<size_t> result;
  size_t num_outputs = min( num_streams, num_parallelism );
  result.resize( distance( func_begin, func_end ) );
  if ( num_outputs == 1 ) {
    return result;
  }

  map<string, pair<bool, size_t>> hash_to_index;

  // Insert file indexes to the map
  for ( size_t i = 0; i < num_outputs; i++ ) {
    string hash_key;
    wabt::sha256( { reinterpret_cast<char*>( &i ), sizeof( size_t ) }, hash_key );
    hash_to_index[hash_key] = { true, i };
  }

  // Insert function indexes to the map
  size_t func_index = 0;
  for ( auto func = func_begin; func != func_end; func++ ) {
    string hash_key;
    wabt::sha256( ( *func )->name, hash_key );
    hash_to_index[hash_key] = { false, func_index };
    func_index++;
  }

  // Find the file with the largest hash key
  size_t file_index = 0;
  for ( auto entry = hash_to_index.rbegin(); entry != hash_to_index.rend(); entry++ ) {
    if ( entry->second.first ) {
      file_index = entry->second.second;
      break;
    }
  }

  // A function should be write to the file on the left side
  for ( auto entry = hash_to_index.begin(); entry != hash_to_index.end(); entry++ ) {
    if ( entry->second.first ) {
      file_index = entry->second.second;
    } else {
      result[entry->second.second] = file_index;
    }
  }

  return result;
}

tuple<array<string, NUM_OUTPUT>, string, string, optional<string>> wasm_to_c( const void* wasm_source,
                                                                              size_t source_size )
{
  Errors errors;
  Module module;

  size_t parallelism = 1 + ( source_size >> 14 );

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

  ValidateModule( &module, &errors, options.features );
  GenerateNames( &module );
  ApplyNames( &module );

  wasminspector::WasmInspector inspector( &module, &errors );
  inspector.Validate();

  for ( auto index : inspector.GetExportedMems() ) {
    module.memories[index]->bounds_checked = true;
  }

  WriteCOptions write_c_options;
  write_c_options.module_name = "function";
  write_c_options.name_to_output_file_index = bind( consistent_hashing_name_to_output_file_index,
                                                    placeholders::_1,
                                                    placeholders::_2,
                                                    placeholders::_3,
                                                    placeholders::_4,
                                                    parallelism );

  auto [fixpoint_start, fixpoint_c] = initcomposer::compose_header( "function", &module, &errors, &inspector );
  // Make sure the start function is the first thing in the binary.
  c_streams[0].WriteData( fixpoint_start.data(), fixpoint_start.size() );
  WriteC( std::move( c_stream_ptrs ),
          &h_stream,
          &h_impl_stream,
          "function.h",
          "function-impl.h",
          &module,
          write_c_options );
  c_streams[0].WriteData( fixpoint_c.data(), fixpoint_c.size() );

  array<string, NUM_OUTPUT> c_outputs;
  for ( unsigned int i = 0; i < NUM_OUTPUT; i++ ) {
    c_outputs[i] = c_streams[i].ReleaseStringBuf();
  }
  string error_string = FormatErrorsToString( errors, Location::Type::Text );
  return { c_outputs,
           h_stream.ReleaseStringBuf(),
           h_impl_stream.ReleaseStringBuf(),
           errors.empty() ? nullopt : make_optional( error_string ) };
}
