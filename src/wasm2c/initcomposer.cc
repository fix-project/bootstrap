#include "initcomposer.hh"

#include "memorystringstream.hh"
#include "wabt/c-writer.h"
#include "wabt/error.h"

using namespace std;
using namespace wabt;

namespace initcomposer {
static constexpr char kSymbolPrefix[] = "w2c_";

/*
 * Hardcoded "C"-locale versions of isalpha/isdigit/isalnum/isxdigit for use
 * in CWriter::Mangle(). We don't use the standard isalpha/isdigit/isalnum
 * because the caller might have changed the current locale.
 */
static bool internal_isalpha( uint8_t ch )
{
  return ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' );
}

static bool internal_isdigit( uint8_t ch )
{
  return ( ch >= '0' && ch <= '9' );
}

static bool internal_isalnum( uint8_t ch )
{
  return internal_isalpha( ch ) || internal_isdigit( ch );
}

static bool internal_ishexdigit( uint8_t ch )
{
  return internal_isdigit( ch ) || ( ch >= 'A' && ch <= 'F' ); // capitals only
}

// static
std::string Mangle( std::string_view name, bool double_underscores )
{
  /*
   * Name mangling transforms arbitrary Wasm names into "safe" C names
   * in a deterministic way. To avoid collisions, distinct Wasm names must be
   * transformed into distinct C names.
   *
   * The rules implemented here are:
   * 1) any hex digit ('A' through 'F') that follows the sequence "0x"
   *    is escaped
   * 2) any underscore at the beginning, at the end, or following another
   *    underscore, is escaped
   * 3) if double_underscores is set, underscores are replaced with
   *    two underscores.
   * 4) otherwise, any alphanumeric character is kept as-is,
   *    and any other character is escaped
   *
   * "Escaped" means the character is represented with the sequence "0xAB",
   * where A B are hex digits ('0'-'9' or 'A'-'F') representing the character's
   * numeric value.
   *
   * Module names are mangled with double_underscores=true to prevent
   * collisions between, e.g., a module "alfa" with export
   * "bravo_charlie" vs. a module "alfa_bravo" with export "charlie".
   */

  enum State
  {
    Any,
    Zero,
    ZeroX,
    ZeroXHexDigit
  } state { Any };
  bool last_was_underscore = false;

  std::string result;
  auto append_escaped = [&]( const uint8_t ch ) {
    result += "0x" + StringPrintf( "%02X", ch );
    last_was_underscore = false;
    state = Any;
  };

  auto append_verbatim = [&]( const uint8_t ch ) {
    result += ch;
    last_was_underscore = ( ch == '_' );
  };

  for ( auto it = name.begin(); it != name.end(); ++it ) {
    const uint8_t ch = *it;
    switch ( state ) {
      case Any:
        state = ( ch == '0' ) ? Zero : Any;
        break;
      case Zero:
        state = ( ch == 'x' ) ? ZeroX : Any;
        break;
      case ZeroX:
        state = internal_ishexdigit( ch ) ? ZeroXHexDigit : Any;
        break;
      case ZeroXHexDigit:
        WABT_UNREACHABLE;
        break;
    }

    /* rule 1 */
    if ( state == ZeroXHexDigit ) {
      append_escaped( ch );
      continue;
    }

    /* rule 2 */
    if ( ( ch == '_' ) && ( ( it == name.begin() ) || ( std::next( it ) == name.end() ) || last_was_underscore ) ) {
      append_escaped( ch );
      continue;
    }

    /* rule 3 */
    if ( double_underscores && ch == '_' ) {
      append_verbatim( ch );
      append_verbatim( ch );
      continue;
    }

    /* rule 4 */
    if ( internal_isalnum( ch ) || ( ch == '_' ) ) {
      append_verbatim( ch );
    } else {
      append_escaped( ch );
    }
  }

  return result;
}

std::string MangleName( std::string_view name )
{
  return Mangle( name, false );
}

std::string MangleModuleName( std::string_view name )
{
  return Mangle( name, true );
}

/* The type name of an instance of an arbitrary module. */
std::string ModuleInstanceTypeName( std::string_view module_name )
{
  return kSymbolPrefix + MangleModuleName( module_name );
}

/* The C symbol for an export from an arbitrary module. */
std::string ExportName( std::string_view module_name, std::string_view export_name )
{
  return kSymbolPrefix + MangleModuleName( module_name ) + '_' + MangleName( export_name );
}

class InitComposer
{
public:
  InitComposer( const string& wasm_name, Module* module, Errors* errors, wasminspector::WasmInspector* inspector )
    : current_module_( module )
    , errors_( errors )
    , wasm_name_( wasm_name )
    , state_info_type_name_( ModuleInstanceTypeName( wasm_name ) )
    , module_prefix_( MangleName( wasm_name ) )
    , result_()
    , inspector_( inspector )
  {}

  string compose_header();

private:
  wabt::Module* current_module_;
  wabt::Errors* errors_;
  string wasm_name_;
  string state_info_type_name_;
  string module_prefix_;
  MemoryStringStream result_;
  wasminspector::WasmInspector* inspector_;

  void Write( string content );
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

void InitComposer::Write( string content )
{
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
    buf = buf + "void " + ExportName( "fixpoint", "attach_tree_ro_table_" + to_string( idx ) )
          + "(struct w2c_fixpoint* instance, __m256i ro_handle) {\n";
    buf = buf + "  wasm_rt_externref_table_t* ro_table = "
          + ExportName( module_prefix_, "ro_table_" + to_string( idx ) ) + "((" + state_info_type_name_
          + "*)instance);" + "\n";
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
    buf = buf + "void " + ExportName( "fixpoint", "attach_blob_ro_mem_" + to_string( idx ) )
          + "(struct w2c_fixpoint* instance, __m256i ro_handle) {" + "\n";
    buf = buf + "  wasm_rt_memory_t* ro_mem = " + ExportName( module_prefix_, "ro_mem_" + to_string( idx ) ) + "(("
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
    buf = buf + "uint32_t " + ExportName( "fixpoint", "size_ro_mem_" + to_string( idx ) )
          + "(struct w2c_fixpoint* instance) {" + "\n";
    buf = buf + "  wasm_rt_memory_t* ro_mem = " + ExportName( module_prefix_, "ro_mem_" + to_string( idx ) ) + "(("
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
    buf = buf + "__m256i " + ExportName( "fixpoint", "create_blob_rw_mem_" + to_string( idx ) )
          + "(struct w2c_fixpoint* instance, uint32_t size) {" + "\n";
    buf = buf + "  wasm_rt_memory_t* rw_mem = " + ExportName( module_prefix_, "rw_mem_" + to_string( idx ) ) + "(("
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
  buf = buf + "__m256i " + ExportName( "fixpoint", "create_blob_i32" )
        + "(struct w2c_fixpoint* instance, uint32_t content) {" + "\n";
  buf = buf + "  return fixpoint_create_blob_i32( content );" + "\n";
  buf = buf + "}\n" + "\n";
  Write( buf );
}

void InitComposer::write_value_type()
{
  string buf = "";
  buf = buf + "extern uint32_t fixpoint_value_type( __m256i );" + "\n";
  buf = buf + "uint32_t " + ExportName( "fixpoint", "value_type" )
        + "(struct w2c_fixpoint* instance, __m256i handle ) {" + "\n";
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
    buf = buf + "__m256i " + ExportName( "fixpoint", "create_tree_rw_table_" + to_string( rw_table ) )
          + "(struct w2c_fixpoint* instance, uint32_t size) {" + "\n";
    buf = buf + "  wasm_rt_externref_table_t* rw_table = "
          + ExportName( module_prefix_, "rw_table_" + to_string( rw_table ) ) + "((" + state_info_type_name_
          + "*)instance);" + "\n";
    buf = buf + "  return fixpoint_create_tree(rw_table, size);" + "\n";
    buf = buf + "}\n" + "\n";
  }
  Write( buf );
}

void InitComposer::write_create_thunk()
{
  string buf = "";
  buf = buf + "extern __m256i fixpoint_create_thunk(__m256i);" + "\n";
  buf = buf + "__m256i " + ExportName( "fixpoint", "create_thunk" )
        + "(struct w2c_fixpoint* instance, __m256i handle) {" + "\n";
  buf = buf + "  return fixpoint_create_thunk(handle);" + "\n";
  buf = buf + "}\n" + "\n";
  Write( buf );
}

void InitComposer::write_init_read_only_mem_table()
{
  string buf = "";
  buf = buf + "void init_mems(" + state_info_type_name_ + "* instance) {" + "\n";
  for ( const auto& ro_mem : inspector_->GetExportedROMems() ) {
    buf = buf + "  " + ExportName( module_prefix_, "ro_mem_" + to_string( ro_mem ) )
          + "(instance)->read_only = true;" + "\n";
  }
  buf = buf + "  return;" + "\n";
  buf = buf + "}" + "\n";
  buf = buf + "\n";

  buf = buf + "void init_tabs(" + state_info_type_name_ + "* instance) {" + "\n";
  for ( const auto& ro_table : inspector_->GetExportedROTables() ) {
    buf = buf + "  " + ExportName( module_prefix_, "ro_table_" + to_string( ro_table ) )
          + "(instance)->read_only = true;" + "\n";
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
  buf = buf + "#include \"" + wasm_name_ + ".h\"" + "\n";
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
  buf = buf + "  wasm2c_" + module_prefix_ + "_instantiate(instance, (struct w2c_fixpoint*)instance);" + "\n";
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
