#include "initcomposer.hh"

#include <format>
#include <sstream>

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
  ostringstream result_;
  wasminspector::WasmInspector* inspector_;

  void write_attach_tree();
  void write_attach_blob();
  void write_memory_size();
  void write_create_blob();
  void write_create_tree();
  void write_init_read_only_mem_table();
  void write_get_instance_size();
  void write_unsafe_io();
  void write_get_attached_tree();
  void write_get_attached_blob();

  struct function
  {
    string return_type;
    string name;
    vector<string> parameter_types;
  };
  void write_function( function fn );
};

void InitComposer::write_attach_tree()
{
  result_ << "extern void fixpoint_attach_tree(__m256i, wasm_rt_externref_table_t*);" << endl;
  auto ro_tables = inspector_->GetExportedROTables();
  for ( uint32_t idx : ro_tables ) {
    result_ << "void " << ExportName( "fixpoint", "attach_tree_ro_table_" + to_string( idx ) )
            << "(struct w2c_fixpoint* instance, __m256i ro_handle) {" << endl;
    result_ << "  wasm_rt_externref_table_t* ro_table = "
            << ExportName( module_prefix_, "ro_table_" + to_string( idx ) ) << "((" << state_info_type_name_
            << "*)instance);" << endl;
    result_ << "  fixpoint_attach_tree(ro_handle, ro_table);" << endl;
    result_ << "}\n" << endl;
  }
}

void InitComposer::write_attach_blob()
{
  auto ro_mems = inspector_->GetExportedROMems();
  result_ << "extern void fixpoint_attach_blob(__m256i, wasm_rt_memory_t*);" << endl;
  for ( uint32_t idx : ro_mems ) {
    result_ << "void " << ExportName( "fixpoint", "attach_blob_ro_mem_" + to_string( idx ) )
            << "(struct w2c_fixpoint* instance, __m256i ro_handle) {" << endl;
    result_ << "  wasm_rt_memory_t* ro_mem = " << ExportName( module_prefix_, "ro_mem_" + to_string( idx ) ) << "(("
            << state_info_type_name_ << "*)instance);" << endl;
    result_ << "  fixpoint_attach_blob(ro_handle, ro_mem);" << endl;
    result_ << "}\n" << endl;
  }
}

void InitComposer::write_memory_size()
{
  auto ro_mems = inspector_->GetExportedROMems();
  for ( uint32_t idx : ro_mems ) {
    result_ << "uint32_t " << ExportName( "fixpoint", "size_ro_mem_" + to_string( idx ) )
            << "(struct w2c_fixpoint* instance) {" << endl;
    result_ << "  wasm_rt_memory_t* ro_mem = " << ExportName( module_prefix_, "ro_mem_" + to_string( idx ) ) << "(("
            << state_info_type_name_ << "*)instance);" << endl;
    result_ << "  return ro_mem->size;" << endl;
    result_ << "}\n" << endl;
  }
}

void InitComposer::write_create_blob()
{
  auto rw_mems = inspector_->GetExportedMems();
  result_ << "extern __m256i fixpoint_create_blob( wasm_rt_memory_t*, uint32_t );" << endl;
  for ( uint32_t idx : rw_mems ) {
    result_ << "__m256i " << ExportName( "fixpoint", "create_blob_rw_mem_" + to_string( idx ) )
            << "(struct w2c_fixpoint* instance, uint32_t size) {" << endl;
    result_ << "  wasm_rt_memory_t* rw_mem = " << ExportName( module_prefix_, "rw_mem_" + to_string( idx ) ) << "(("
            << state_info_type_name_ << "*)instance);" << endl;
    result_ << "  return fixpoint_create_blob(rw_mem, size);" << endl;
    result_ << "}\n" << endl;
  }
}

void InitComposer::write_create_tree()
{
  auto rw_tables = inspector_->GetExportedTables();
  result_ << "extern __m256i fixpoint_create_tree( wasm_rt_externref_table_t*, uint32_t );" << endl;
  for ( auto rw_table : rw_tables ) {
    result_ << "__m256i " << ExportName( "fixpoint", "create_tree_rw_table_" + to_string( rw_table ) )
            << "(struct w2c_fixpoint* instance, uint32_t size) {" << endl;
    result_ << "  wasm_rt_externref_table_t* rw_table = "
            << ExportName( module_prefix_, "rw_table_" + to_string( rw_table ) ) << "((" << state_info_type_name_
            << "*)instance);" << endl;
    result_ << "  return fixpoint_create_tree(rw_table, size);" << endl;
    result_ << "}\n" << endl;
  }
}

void InitComposer::write_init_read_only_mem_table()
{
  result_ << "void init_mems(" << state_info_type_name_ << "* instance) {" << endl;
  for ( const auto& ro_mem : inspector_->GetExportedROMems() ) {
    result_ << "  " << ExportName( module_prefix_, inspector_->GetMemoryName( ro_mem ) )
            << "(instance)->read_only = true;" << endl;
  }
  result_ << "  return;" << endl;
  result_ << "}" << endl;
  result_ << endl;

  result_ << "void init_tabs(" << state_info_type_name_ << "* instance) {" << endl;
  for ( const auto& ro_table : inspector_->GetExportedROTables() ) {
    result_ << "  " << ExportName( module_prefix_, inspector_->GetTableName( ro_table ) )
            << "(instance)->read_only = true;" << endl;
  }
  result_ << "  return;" << endl;
  result_ << "}" << endl;
  result_ << endl;
}

void InitComposer::write_get_instance_size()
{
  result_ << "size_t get_instance_size() {" << endl;
  result_ << "  return sizeof(" << state_info_type_name_ << ");" << endl;
  result_ << "}\n" << endl;
}

void InitComposer::write_unsafe_io()
{

  result_ << "extern void fixpoint_unsafe_io(uint32_t index, uint32_t length, wasm_rt_memory_t* main_mem);" << endl;
  result_ << "void " << ExportName( "fixpoint", "unsafe_io" );
  result_ << "(struct w2c_fixpoint* instance, uint32_t index, uint32_t length) {" << endl;

  // Only call fixpoint_unsafe_io if there is a memory called "memory"
  // Otherwise unsafe_io is a no op.
  if ( inspector_->ExportsMainMemory() ) {
    result_ << "  wasm_rt_memory_t* main_mem = " << ExportName( module_prefix_, "memory" ) << "(("
            << state_info_type_name_ << "*)instance);" << endl;
    result_ << "  fixpoint_unsafe_io(index, length, main_mem);" << endl;
  }
  result_ << "}" << endl;
}

void InitComposer::write_get_attached_tree()
{
  result_ << "extern __m256i fixpoint_get_attached_tree(wasm_rt_externref_table_t*);" << endl;
  auto ro_tables = inspector_->GetExportedROTables();
  for ( uint32_t idx : ro_tables ) {
    result_ << "__m256i " << ExportName( "fixpoint", "get_attached_tree_ro_table_" + to_string( idx ) )
            << "(struct w2c_fixpoint* instance) {" << endl;
    result_ << "  wasm_rt_externref_table_t* ro_table = "
            << ExportName( module_prefix_, "ro_table_" + to_string( idx ) ) << "((" << state_info_type_name_
            << "*)instance);" << endl;
    result_ << "if ( ro_table->read_only && ro_table->size > 0 ) return ro_table->ref;" << endl;
    result_ << "wasm_rt_trap( WASM_RT_TRAP_OOB );" << endl;
    result_ << "}\n" << endl;
  }
}

void InitComposer::write_get_attached_blob()
{
  auto ro_mems = inspector_->GetExportedROMems();
  result_ << "extern __m256i fixpoint_get_attached_blob(wasm_rt_memory_t*);" << endl;
  for ( uint32_t idx : ro_mems ) {
    result_ << "__m256i " << ExportName( "fixpoint", "get_attached_blob_ro_mem_" + to_string( idx ) )
            << "(struct w2c_fixpoint* instance) {" << endl;
    result_ << "  wasm_rt_memory_t* ro_mem = " << ExportName( module_prefix_, "ro_mem_" + to_string( idx ) ) << "(("
            << state_info_type_name_ << "*)instance);" << endl;
    result_ << "if ( ro_mem->read_only && ro_mem->size > 0 ) return ro_mem->ref;" << endl;
    result_ << "wasm_rt_trap( WASM_RT_TRAP_OOB );" << endl;
    result_ << "}\n" << endl;
  }
}

void InitComposer::write_function( function fn )
{
  result_ << "extern " << fn.return_type << " fixpoint_" << fn.name << "(";
  for ( size_t i = 0; i < fn.parameter_types.size(); i++ ) {
    if ( i > 0 )
      result_ << ", ";
    result_ << fn.parameter_types[i];
  }
  result_ << ");" << endl;

  result_ << fn.return_type << " " << ExportName( "fixpoint", fn.name ) << "(struct w2c_fixpoint *instance";
  for ( size_t i = 0; i < fn.parameter_types.size(); i++ ) {
    result_ << ", " << fn.parameter_types[i] << " x" << i;
  }
  result_ << ") {" << endl;
  result_ << "  return fixpoint_" << fn.name << "(";
  for ( size_t i = 0; i < fn.parameter_types.size(); i++ ) {
    if ( i > 0 )
      result_ << ", ";
    result_ << "x" << i;
  }
  result_ << ");" << endl;
  result_ << "}\n" << endl;
}

struct functype
{
  string return_type;
  string name;
  vector<string> parameter_types;
};

const char* structs = R"INLINECODE(
struct w2c_fixpoint {
  void *runtime;
  struct api *api;
};

)INLINECODE";

inline string arg_name( size_t i )
{
  return std::format( "x{}", i );
}

inline vector<string> arg_names( size_t size )
{
  vector<string> result;
  result.reserve( size );
  for ( size_t i = 0; i < size; i++ ) {
    result.push_back( arg_name( i ) );
  }
  return result;
}

inline vector<string> zip( vector<string> a, vector<string> b )
{
  vector<string> result;
  result.reserve( a.size() );
  for ( size_t i = 0; i < a.size() and i < b.size(); i++ ) {
    result.push_back( std::format( "{} {}", a[i], b[i] ) );
  }
  return result;
}

inline string comma_separate( vector<string> items )
{
  ostringstream s;
  for ( size_t i = 0; i < items.size(); i++ ) {
    s << items[i];
    if ( i != items.size() - 1 ) {
      s << ", ";
    }
  }
  return s.str();
}

string api_struct( vector<functype> helpers, vector<functype> functions )
{
  ostringstream s;

  s << "struct api {\n";
  for ( const functype& f : functions ) {
    s << std::format( "  {}(*{})({});\n", f.return_type, f.name, comma_separate( f.parameter_types ) );
  }
  for ( const functype& f : helpers ) {
    s << std::format( "  {}(*{})({});\n", f.return_type, f.name, comma_separate( f.parameter_types ) );
  }
  s << "};\n";

  return s.str();
};

string api_function( functype f, bool exported )
{
  ostringstream s;

  const auto args = f.parameter_types;

  s << std::format( "{} {}({}) {{\n",
                    f.return_type,
                    exported ? ExportName( "fixpoint", f.name ) : std::format( "fixpoint_{}", f.name ),
                    comma_separate( zip( args, arg_names( args.size() ) ) ) );
  s << std::format(
    "  return {}->api->{}({});\n", arg_name( 0 ), f.name, comma_separate( arg_names( args.size() ) ) );
  s << "}\n\n";
  return s.str();
};

/* void InitComposer::write_attach_blob() */
/* { */
/*   auto ro_mems = inspector_->GetExportedROMems(); */
/*   result_ << "extern void fixpoint_attach_blob(__m256i, wasm_rt_memory_t*);" << endl; */
/*   for ( uint32_t idx : ro_mems ) { */
/*     result_ << "void " << ExportName( "fixpoint", "attach_blob_ro_mem_" + to_string( idx ) ) */
/*             << "(struct w2c_fixpoint* instance, __m256i ro_handle) {" << endl; */
/*     result_ << "  wasm_rt_memory_t* ro_mem = " << ExportName( module_prefix_, "ro_mem_" + to_string( idx ) ) <<
 * "((" */
/*             << state_info_type_name_ << "*)instance);" << endl; */
/*     result_ << "  fixpoint_attach_blob(ro_handle, ro_mem);" << endl; */
/*     result_ << "}\n" << endl; */
/*   } */
/* } */

string blob_function( string mem, functype f, std::string wasm )
{
  ostringstream s;

  const auto args = f.parameter_types;

  s << std::format( "{} {}({}) {{\n",
                    f.return_type,
                    ExportName( "fixpoint", std::format( "{}_{}", f.name, mem ) ),
                    comma_separate( zip( args, arg_names( args.size() ) ) ) );
  s << std::format( "  wasm_rt_memory_t* mem = {}(({}*){});\n",
                    ExportName( MangleName( wasm ), mem ),
                    ModuleInstanceTypeName( wasm ),
                    arg_name( 0 ) );
  s << std::format( "  return fixpoint_{}({}, mem);\n", f.name, comma_separate( arg_names( args.size() ) ) );
  s << "}\n\n";
  return s.str();
};

string tree_function( string tbl, functype f, std::string wasm )
{
  ostringstream s;

  const auto args = f.parameter_types;

  s << std::format( "{} {}({}) {{\n",
                    f.return_type,
                    ExportName( "fixpoint", std::format( "{}_{}", f.name, tbl ) ),
                    comma_separate( zip( args, arg_names( args.size() ) ) ) );
  s << std::format( "  wasm_rt_externref_table_t* tbl = {}(({}*){});\n",
                    ExportName( MangleName( wasm ), tbl ),
                    ModuleInstanceTypeName( wasm ),
                    arg_name( 0 ) );
  s << std::format( "  return fixpoint_{}({}, tbl);\n", f.name, comma_separate( arg_names( args.size() ) ) );
  s << "}\n\n";
  return s.str();
};

string InitComposer::compose_header()
{
  result_ = ostringstream();

  result_ << "#include <immintrin.h>" << endl;
  result_ << "#include \"" << wasm_name_ << ".h\"" << endl;
  result_ << endl;

  vector<functype> helper_functions = {
    { "void", "attach_blob", { "struct w2c_fixpoint*", "__m256i", "wasm_rt_memory_t*" } },
    { "void", "attach_tree", { "struct w2c_fixpoint*", "__m256i", "wasm_rt_externref_table_t*" } },
    { "__m256i", "get_attached_blob", { "struct w2c_fixpoint*", "wasm_rt_memory_t*" } },
    { "__m256i", "get_attached_tree", { "struct w2c_fixpoint*", "wasm_rt_externref_table_t*" } },
    { "__m256i", "create_blob", { "struct w2c_fixpoint*", "uint32_t", "wasm_rt_memory_t*" } },
    { "__m256i", "create_tree", { "struct w2c_fixpoint*", "uint32_t", "wasm_rt_externref_table_t*" } },
  };

  vector<functype> api_functions = {
    { "__m256i", "create_blob_i32", { "struct w2c_fixpoint*", "uint32_t" } },
    { "__m256i", "create_tag", { "struct w2c_fixpoint*", "__m256i", "__m256i" } },
    { "__m256i", "create_thunk", { "struct w2c_fixpoint*", "__m256i" } },
    { "__m256i", "debug_try_evaluate", { "struct w2c_fixpoint*", "__m256i" } },
    { "__m256i", "debug_try_inspect", { "struct w2c_fixpoint*", "__m256i" } },
    { "__m256i", "debug_try_lift", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "equality", { "struct w2c_fixpoint*", "__m256i", "__m256i" } },
    { "__m256i", "lower", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "get_access", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "get_length", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "get_value_type", { "struct w2c_fixpoint*", "__m256i" } },
  };

  result_ << api_struct( helper_functions, api_functions ) << endl;
  result_ << structs;

  write_get_instance_size();
  write_init_read_only_mem_table();
  write_memory_size();
  write_unsafe_io();

  unordered_map<string, functype> fns_map {};
  for ( const auto& fn : api_functions ) {
    fns_map.insert( std::make_pair( fn.name, fn ) );
  }

  const auto& imported = inspector_->GetImportedFunctions();

  for ( const auto& f : fns_map ) {
    if ( imported.find( f.first ) != imported.end() ) {
      result_ << api_function( f.second, true );
    }
  }
  for ( const auto& f : helper_functions ) {
    result_ << api_function( f, false );
  }
  auto ro_mems = inspector_->GetExportedROMems();
  auto rw_mems = inspector_->GetExportedMems();
  auto ro_tables = inspector_->GetExportedROTables();
  auto rw_tables = inspector_->GetExportedTables();

  vector<functype> ro_blob_functions = {
    { "void", "attach_blob", { "struct w2c_fixpoint*", "__m256i" } },
    { "__m256i", "create_blob", { "struct w2c_fixpoint*", "uint32_t" } },
    { "__m256i", "get_attached_blob", { "struct w2c_fixpoint*" } },
  };
  vector<functype> rw_blob_functions = {
    { "__m256i", "create_blob", { "struct w2c_fixpoint*", "uint32_t" } },
    { "__m256i", "get_attached_blob", { "struct w2c_fixpoint*" } },
  };

  for ( const auto& mem : ro_mems ) {
    for ( const auto& f : ro_blob_functions ) {
      result_ << blob_function( inspector_->GetMemoryName( mem ), f, wasm_name_ );
    }
  }
  for ( const auto& mem : rw_mems ) {
    for ( const auto& f : rw_blob_functions ) {
      result_ << blob_function( inspector_->GetMemoryName( mem ), f, wasm_name_ );
    }
  }

  vector<functype> ro_tree_functions = {
    { "void", "attach_tree", { "struct w2c_fixpoint*", "__m256i" } },
  };

  vector<functype> rw_tree_functions = {
    { "__m256i", "create_tree", { "struct w2c_fixpoint*", "uint32_t" } },
    { "__m256i", "get_attached_tree", { "struct w2c_fixpoint*" } },
  };

  for ( const auto& table : ro_tables ) {
    for ( const auto& f : ro_tree_functions ) {
      result_ << tree_function( inspector_->GetTableName( table ), f, wasm_name_ );
    }
  }
  for ( const auto& table : rw_tables ) {
    for ( const auto& f : rw_tree_functions ) {
      result_ << tree_function( inspector_->GetTableName( table ), f, wasm_name_ );
    }
  }

  result_ << "void initProgram(void* ptr) {" << endl;
  result_ << "  " << state_info_type_name_ << "* instance = (" << state_info_type_name_ << "*)ptr;" << endl;
  result_ << "  wasm2c_" << module_prefix_ << "_instantiate(instance, (struct w2c_fixpoint*)instance);" << endl;
  result_ << "  init_mems(instance);" << endl;
  result_ << "  init_tabs(instance);" << endl;
  result_ << "  return;" << endl;
  result_ << "}" << endl;

  return result_.str();
}

string compose_header( string wasm_name, Module* module, Errors* error, wasminspector::WasmInspector* inspector )
{
  InitComposer composer( wasm_name, module, error, inspector );
  return composer.compose_header();
}
}
