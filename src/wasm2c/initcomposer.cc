#include "initcomposer.hh"

#include "wabt/c-writer.h"
#include "wabt/error.h"
#include "memorystringstream.hh"

using namespace std;
using namespace wabt;

namespace initcomposer {
string MangleName( string_view name )
{
  const char kPrefix = 'Z';
  std::string result = "Z_";

  if ( !name.empty() ) {
    for ( char c : name ) {
      if ( ( isalnum( c ) && c != kPrefix ) || c == '_' ) {
        result += c;
      } else {
        result += kPrefix;
        result += wabt::StringPrintf( "%02X", static_cast<uint8_t>( c ) );
      }
    }
  }

  return result;
}

string MangleStateInfoTypeName( const string& wasm_name )
{
  return MangleName( wasm_name ) + "_instance_t";
}

class InitComposer
{
public:
  InitComposer( const string& wasm_name, Module* module, Errors* errors, wasminspector::WasmInspector* inspector )
    : current_module_( module )
    , errors_( errors )
    , wasm_name_( wasm_name )
    , state_info_type_name_( MangleStateInfoTypeName( wasm_name ) )
    , module_prefix_( MangleName( wasm_name ) )
    , result_()
    , inspector_( inspector )
  {
  }

  string compose_header();

private:
  wabt::Module* current_module_;
  wabt::Errors* errors_;
  string wasm_name_;
  string state_info_type_name_;
  string module_prefix_;
  MemoryStringStream result_;
  wasminspector::WasmInspector* inspector_;

  void Write(string content);
  void write_attach_tree();
  void write_attach_blob();
  void write_memory_size();
  void write_create_blob();
  void write_create_blob_i32();
  void write_value_type();
  void write_create_tree();
  void write_create_thunk();
  void write_init_read_only_mem_table();
  void write_get_instance_size();
  void write_context();
};

void InitComposer::Write(string content) {
  result_.WriteData( content.data(), content.size() );
}

void InitComposer::write_context()
{ 
  string buf = "";
 
  buf = buf + "typedef struct Context {\n";
  buf = buf + "  __m256i return_value;\n";
  buf = buf + "  size_t memory_usage;\n";
  buf = buf + "} Context;\n\n";

  buf = buf + "Context* get_context_ptr( void* instance ) {\n";
  buf = buf + "  return (Context*)((char*)instance + get_instance_size());\n";
  buf = buf + "}\n\n";

  Write( buf );
}

void InitComposer::write_attach_tree()
{
  string buf = "";
  buf = buf + "extern void fixpoint_attach_tree(__m256i, wasm_rt_externref_table_t*);\n";
  auto ro_tables = inspector_->GetExportedROTables();
  for ( uint32_t idx : ro_tables ) {
    buf = buf + "void Z_fixpointZ_attach_tree_ro_table_" + to_string(idx)
            + "(struct Z_fixpoint_instance_t* instance, __m256i ro_handle) {\n";
    buf = buf + "  wasm_rt_externref_table_t* ro_table = " + module_prefix_ + "Z_ro_table_" + to_string(idx) + "(("
            + state_info_type_name_ + "*)instance);" + "\n";
    buf = buf + "  fixpoint_attach_tree(ro_handle, ro_table);" + "\n";
    buf = buf + "}\n" + "\n";
  }
  Write( buf );
}

void InitComposer::write_attach_blob()
{
  string buf = "";
  auto ro_mems = inspector_->GetExportedROMems();
  buf = buf + "extern void fixpoint_attach_blob(__m256i, wasm_rt_memory_t*);" + "\n";
  for ( uint32_t idx : ro_mems ) {
    buf = buf + "void Z_fixpointZ_attach_blob_ro_mem_" + to_string(idx)
            + "(struct Z_fixpoint_instance_t* instance, __m256i ro_handle) {" + "\n";
    buf = buf + "  wasm_rt_memory_t* ro_mem = " + module_prefix_ + "Z_ro_mem_" + to_string(idx) + "(("
            + state_info_type_name_ + "*)instance);" + "\n";
    buf = buf + "  fixpoint_attach_blob(ro_handle, ro_mem);" + "\n";
    buf = buf + "}\n" + "\n";
  }
  Write( buf );
}

void InitComposer::write_memory_size()
{
  auto ro_mems = inspector_->GetExportedROMems();
  for ( uint32_t idx : ro_mems ) {
    string buf = "";
    buf = buf + "uint32_t Z_fixpointZ_size_ro_mem_" + to_string(idx) + "(struct Z_fixpoint_instance_t* instance) {" + "\n";
    buf = buf + "  wasm_rt_memory_t* ro_mem = " + module_prefix_ + "Z_ro_mem_" + to_string(idx) + "(("
            + state_info_type_name_ + "*)instance);" + "\n";
    buf = buf + "  return ro_mem->size;" + "\n";
    buf = buf + "}\n" + "\n";
    Write( buf );
  }
}

void InitComposer::write_create_blob()
{
  auto rw_mems = inspector_->GetExportedRWMems();
  string buf = "";
  buf = buf + "extern __m256i fixpoint_create_blob( wasm_rt_memory_t*, uint32_t );" + "\n";
  for ( uint32_t idx : rw_mems ) {
    buf = buf + "__m256i Z_fixpointZ_create_blob_rw_mem_" + to_string(idx)
            + "(struct Z_fixpoint_instance_t* instance, uint32_t size) {" + "\n";
    buf = buf + "  wasm_rt_memory_t* rw_mem = " + module_prefix_ + "Z_rw_mem_" + to_string(idx) + "(("
            + state_info_type_name_ + "*)instance);" + "\n";
    buf = buf + "  return fixpoint_create_blob(rw_mem, size);" + "\n";
    buf = buf + "}\n" + "\n";
  }
  Write( buf );
}

void InitComposer::write_create_blob_i32()
{ 
  string buf = "";
  buf = buf + "extern __m256i fixpoint_create_blob_i32( uint32_t );" + "\n";
  buf = buf + "__m256i Z_fixpointZ_create_blob_i32"
          + "(struct Z_fixpoint_instance_t* instance, uint32_t content) {" + "\n";
  buf = buf + "  return fixpoint_create_blob_i32( content );" + "\n";
  buf = buf + "}\n" + "\n";
  Write( buf );
}

void InitComposer::write_value_type()
{
  string buf = "";
  buf = buf + "extern uint32_t fixpoint_value_type( __m256i );" + "\n";
  buf = buf + "uint32_t Z_fixpointZ_value_type"
          + "(struct Z_fixpoint_instance_t* instance, __m256i handle ) {" + "\n";
  buf = buf + "  return fixpoint_value_type( handle );" + "\n";
  buf = buf + "}\n" + "\n";
  Write( buf );
}

void InitComposer::write_create_tree()
{
  string buf = "";
  auto rw_tables = inspector_->GetExportedRWTables();
  buf = buf + "extern __m256i fixpoint_create_tree( wasm_rt_externref_table_t*, uint32_t );" + "\n";
  for ( auto rw_table : rw_tables ) {
    buf = buf + "__m256i Z_fixpointZ_create_tree_rw_table_" + to_string(rw_table)
            + "(struct Z_fixpoint_instance_t* instance, uint32_t size) {" + "\n";
    buf = buf + "  wasm_rt_externref_table_t* rw_table = " + module_prefix_ + "Z_rw_table_" + to_string(rw_table) + "(("
            + state_info_type_name_ + "*)instance);" + "\n";
    buf = buf + "  return fixpoint_create_tree(rw_table, size);" + "\n";
    buf = buf + "}\n" + "\n";
  }
  Write( buf );
}

void InitComposer::write_create_thunk()
{
  string buf = "";
  buf = buf + "extern __m256i fixpoint_create_thunk(__m256i);" + "\n";
  buf = buf + "__m256i " + module_prefix_
          + "Z_fixpointZ_create_thunk(struct Z_fixpoint_instance_t* instance, __m256i handle) {" + "\n";
  buf = buf + "  return fixpoint_create_thunk(handle);" + "\n";
  buf = buf + "}\n" + "\n";
  Write( buf );
}

void InitComposer::write_init_read_only_mem_table()
{
  string buf = "";
  buf = buf + "void init_mems(" + state_info_type_name_ + "* instance) {" + "\n";
  for ( const auto& ro_mem : inspector_->GetExportedROMems() ) {
    buf = buf + "  " + module_prefix_ + "Z_ro_mem_" + to_string(ro_mem) + "(instance)->read_only = true;" + "\n";
  }
  buf = buf + "  return;" + "\n";
  buf = buf + "}" + "\n";
  buf = buf + "\n";

  buf = buf + "void init_tabs(" + state_info_type_name_ + "* instance) {" + "\n";
  for ( const auto& ro_table : inspector_->GetExportedROTables() ) {
    buf = buf + "  " + module_prefix_ + "Z_ro_table_" + to_string(ro_table) + "(instance)->read_only = true;" + "\n";
  }
  buf = buf + "  return;" + "\n";
  buf = buf + "}" + "\n";
  buf = buf + "\n";
  Write( buf );
}

void InitComposer::write_get_instance_size()
{
  string buf = "";
  buf = buf + "size_t get_instance_size() {" + "\n";
  buf = buf + "  return sizeof(" + state_info_type_name_ + ");" + "\n";
  buf = buf + "}\n" + "\n";
  Write( buf );
}

string InitComposer::compose_header()
{
  string buf = "";
  buf = buf + "#include <immintrin.h>" + "\n";
  buf = buf + "#include \"" + wasm_name_ + "_fixpoint.h\"" + "\n";
  buf = buf + "\n";
  Write( buf );

  write_get_instance_size();
  write_context();
  write_init_read_only_mem_table();
  write_attach_tree();
  write_attach_blob();
  write_memory_size();
  write_create_tree();
  write_create_blob();
  write_create_blob_i32();
  write_value_type();
  write_create_thunk();

  buf = "";
  buf = buf + "void initProgram(void* ptr) {" + "\n";
  buf = buf + "  " + state_info_type_name_ + "* instance = (" + state_info_type_name_ + "*)ptr;" + "\n";
  buf = buf + "  " + module_prefix_ + "_instantiate(instance, (struct Z_fixpoint_instance_t*)instance);" + "\n";
  buf = buf + "  init_mems(instance);" + "\n";
  buf = buf + "  init_tabs(instance);" + "\n";
  buf = buf + "  return;" + "\n";
  buf = buf + "}" + "\n";
  Write( buf );

  return result_.ReleaseStringBuf();
}

string compose_header( string wasm_name, Module* module, Errors* error, wasminspector::WasmInspector* inspector )
{
  InitComposer composer( wasm_name, module, error, inspector );
  return composer.compose_header();
}
}
