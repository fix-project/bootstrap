#include "initcomposer.hh"

#include <cmath>
#include <format>
#include <sstream>

#include "wabt/c-writer.h"
#include "wabt/error.h"

using namespace std;
using namespace wabt;

namespace initcomposer {

#include "shims.c.inc"
#include "shims.h.inc"

string_view shims { shims_data, (size_t)shims_data_len };
string_view shims_impl { shims_impl_data, (size_t)shims_impl_data_len };

namespace types {
struct functype
{
  string return_type;
  string name;
  vector<string> parameter_types;
  bool noreturn = false;
};

template<typename T>
concept Pointer = is_pointer<T>::is_pointer;

template<typename T>
extern string name;
template<>
string name<float> = "float";
template<>
string name<double> = "double";
template<>
string name<size_t> = "size_t";
template<>
string name<void> = "void";
template<>
string name<int> = "int";
template<typename T>
string name<T const> = name<T> + " const";
template<typename T>
string name<T*> = name<T> + "*";

template<typename R, typename... As>
R return_type( R( As... ) );

template<typename A, typename... As>
void names( vector<string>& args )
{
  args.push_back( name<A> );
  if constexpr ( sizeof...( As ) > 0 ) {
    names<As...>( args );
  }
};

template<typename R, typename... As>
std::vector<string> arg_names( R( As... ) )
{
  vector<string> args;
  args.reserve( sizeof...( As ) );
  names<As...>( args );
  return args;
}

template<typename F>
functype type( F f, string n )
{
  return { name<decltype( return_type( f ) )>, n, arg_names( f ) };
}

}

using functype = types::functype;
#define CTYPE( F ) types::type( F, #F )
#define MTYPE( F ) types::type<double( double )>( F, #F )

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
  string compose_stub();

private:
  wabt::Module* current_module_;
  wabt::Errors* errors_;
  string wasm_name_;
  string state_info_type_name_;
  string module_prefix_;
  ostringstream result_;
  wasminspector::WasmInspector* inspector_;

  void write_unsafe_io();
};

void InitComposer::write_unsafe_io() {}

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

string make_api_struct( string_view name, vector<vector<functype>> functions )
{
  ostringstream s;

  s << std::format( "struct {} {{\n", name );
  for ( const auto& fs : functions ) {
    for ( const functype& f : fs ) {
      s << std::format( "  {}(*{})({});\n", f.return_type, f.name, comma_separate( f.parameter_types ) );
    }
  }
  s << "};\n";

  return s.str();
};

string api_function( functype f, bool exported )
{
  ostringstream s;

  const auto args = f.parameter_types;

  s << std::format( "{}{} {}({}) {{\n",
                    f.noreturn ? "WASM_RT_NO_RETURN " : "",
                    f.return_type,
                    exported ? ExportName( "fixpoint", f.name ) : std::format( "fixpoint_{}", f.name ),
                    comma_separate( zip( args, arg_names( args.size() ) ) ) );
  s << std::format( "  {}{}->api->{}({});\n",
                    f.noreturn ? "" : "return ",
                    arg_name( 0 ),
                    f.name,
                    comma_separate( arg_names( args.size() ) ) );
  if ( f.noreturn ) {
    s << "  __builtin_unreachable();";
  }
  s << "}\n\n";
  return s.str();
};

string rt_function( functype f )
{
  ostringstream s;

  const auto args = f.parameter_types;

  s << std::format( "{}{} wasm_rt_{}({}) {{\n",
                    f.noreturn ? "WASM_RT_NO_RETURN " : "",
                    f.return_type,
                    f.name,
                    args.empty() ? "void" : comma_separate( zip( args, arg_names( args.size() ) ) ) );
  s << std::format( "  {}(CONTEXT)->rt->{}({});\n",
                    f.noreturn ? "" : "return ",
                    f.name,
                    comma_separate( arg_names( args.size() ) ) );
  if ( f.noreturn ) {
    s << "  __builtin_unreachable();";
  }
  s << "}\n\n";
  return s.str();
};

string os_function( functype f )
{
  ostringstream s;

  const auto args = f.parameter_types;

  s << std::format( "{}{} {}({}) {{\n",
                    f.noreturn ? "WASM_RT_NO_RETURN " : "",
                    f.return_type,
                    f.name,
                    comma_separate( zip( args, arg_names( args.size() ) ) ) );
  s << std::format( "  {}(CONTEXT)->os->{}({});\n",
                    f.noreturn ? "" : "return ",
                    f.name,
                    comma_separate( arg_names( args.size() ) ) );
  if ( f.noreturn ) {
    s << "  __builtin_unreachable();";
  }
  s << "}\n\n";
  return s.str();
};

string mem_function( string mem, functype f, std::string wasm )
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

string tbl_function( string tbl, functype f, std::string wasm )
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

  result_ << shims;

  vector<functype> helper_functions = {
    { "void", "attach_blob", { "struct w2c_fixpoint*", "__m256i", "wasm_rt_memory_t*" } },
    { "void", "attach_tree", { "struct w2c_fixpoint*", "__m256i", "wasm_rt_externref_table_t*" } },
    { "__m256i", "create_blob", { "struct w2c_fixpoint*", "uint32_t", "wasm_rt_memory_t*" } },
    { "__m256i", "create_tree", { "struct w2c_fixpoint*", "uint32_t", "wasm_rt_externref_table_t*" } },
    { "void", "unsafely_log", { "struct w2c_fixpoint*", "uint32_t", "uint32_t", "wasm_rt_memory_t*" } },
  };

  vector<functype> api_functions = {
    { "__m256i", "create_blob_i32", { "struct w2c_fixpoint*", "uint32_t" } },
    { "__m256i", "create_tag", { "struct w2c_fixpoint*", "__m256i", "__m256i" } },
    { "__m256i", "create_thunk", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "equality", { "struct w2c_fixpoint*", "__m256i", "__m256i" } },
    { "__m256i", "lower", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "get_access", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "get_length", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "get_value_type", { "struct w2c_fixpoint*", "__m256i" } },
  };

  vector<functype> rt_functions = {
    { "void", "trap", { "wasm_rt_trap_t" }, true },
    { "const char *", "strerror", { "wasm_rt_trap_t" } },
    { "void", "load_exception", { "const wasm_rt_tag_t", "uint32_t", "const void*" } },
    { "void", "throw", {}, true },
    { "WASM_RT_UNWIND_TARGET*", "get_unwind_target", {} },
    { "void", "set_unwind_target", { "WASM_RT_UNWIND_TARGET*" } },
    { "wasm_rt_tag_t", "exception_tag", {} },
    { "uint32_t", "exception_size", {} },
    { "void *", "exception", {} },
    { "void", "allocate_memory", { "wasm_rt_memory_t*", "uint64_t", "uint64_t", "bool" } },
    { "void", "allocate_memory_sw_checked", { "wasm_rt_memory_t*", "uint64_t", "uint64_t", "bool" } },
    { "uint64_t", "grow_memory", { "wasm_rt_memory_t*", "uint64_t" } },
    { "uint64_t", "grow_memory_sw_checked", { "wasm_rt_memory_t*", "uint64_t" } },
    { "void", "free_memory", { "wasm_rt_memory_t*" } },
    { "void", "free_memory_sw_checked", { "wasm_rt_memory_t*" } },
    { "void", "allocate_funcref_table", { "wasm_rt_funcref_table_t*", "uint32_t", "uint32_t" } },
    { "void", "free_funcref_table", { "wasm_rt_funcref_table_t*" } },
    { "uint32_t", "grow_funcref_table", { "wasm_rt_funcref_table_t*", "uint32_t", "wasm_rt_funcref_t" } },
    { "void", "allocate_externref_table", { "wasm_rt_externref_table_t*", "uint32_t", "uint32_t" } },
    { "void", "free_externref_table", { "wasm_rt_externref_table_t*" } },
    { "uint32_t", "grow_externref_table", { "wasm_rt_externref_table_t*", "uint32_t", "wasm_rt_externref_t" } },
  };

  vector<functype> os_functions = {
    CTYPE( aligned_alloc ),
    CTYPE( free ),
    { "void", "__assert_fail", { "const char *", "const char *", "unsigned int", "const char *" }, true },
    CTYPE( memcpy ),
    CTYPE( memmove ),
    CTYPE( memset ),
    MTYPE( sqrt ),
    CTYPE( ceilf ),
    MTYPE( ceil ),
    MTYPE( floor ),
    MTYPE( nearbyint ),
  };

  result_ << make_api_struct( "api", { helper_functions, api_functions } ) << endl;
  result_ << make_api_struct( "rt", { rt_functions } ) << endl;
  result_ << make_api_struct( "os", { os_functions } ) << endl;

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

  for ( const auto& f : os_functions ) {
    result_ << os_function( f );
  }
  for ( const auto& f : rt_functions ) {
    result_ << rt_function( f );
  }
  auto ro_mems = inspector_->GetExportedROMems();
  auto mems = inspector_->GetExportedMems();
  auto ro_tables = inspector_->GetExportedROTables();
  auto tables = inspector_->GetExportedTables();

  result_ << std::format( "void protect_memories_and_tables({} *instance) {{\n", state_info_type_name_ );
  for ( const auto& ro_mem : ro_mems ) {
    result_ << "  " << ExportName( module_prefix_, inspector_->GetMemoryName( ro_mem ) )
            << "(instance)->read_only = true;" << endl;
  }
  for ( const auto& ro_table : ro_tables ) {
    result_ << "  " << ExportName( module_prefix_, inspector_->GetTableName( ro_table ) )
            << "(instance)->read_only = true;" << endl;
  }
  result_ << "}\n\n";

  vector<functype> ro_mem_functions = {
    { "void", "attach_blob", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "get_memory_size", { "struct w2c_fixpoint*" } },
    { "__m256i", "get_attached_blob", { "struct w2c_fixpoint*" } },
  };

  vector<functype> rw_mem_functions = {
    { "__m256i", "create_blob", { "struct w2c_fixpoint*", "uint32_t" } },
    { "void", "unsafely_log", { "struct w2c_fixpoint*", "uint32_t", "uint32_t" } },
  };

  vector<functype> ro_tbl_functions = {
    { "void", "attach_tree", { "struct w2c_fixpoint*", "__m256i" } },
    { "uint32_t", "get_table_size", { "struct w2c_fixpoint*" } },
    { "__m256i", "get_attached_tree", { "struct w2c_fixpoint*" } },
  };

  vector<functype> rw_tbl_functions = {
    { "__m256i", "create_tree", { "struct w2c_fixpoint*", "uint32_t" } },
  };

  result_ << "/* RO Mems:\n";
  for ( const auto& mem : ro_mems ) {
    result_ << inspector_->GetMemoryName( mem ) << "\n";
  }
  result_ << "-- Mems:\n";
  for ( const auto& mem : mems ) {
    result_ << inspector_->GetMemoryName( mem ) << "\n";
  }
  result_ << "*/\n\n";
  result_ << "/* RO Tables:\n";
  for ( const auto& mem : ro_tables ) {
    result_ << inspector_->GetTableName( mem ) << "\n";
  }
  result_ << "-- Tables:\n";
  for ( const auto& mem : tables ) {
    result_ << inspector_->GetTableName( mem ) << "\n";
  }
  result_ << "*/\n\n";

  for ( const auto& mem : ro_mems ) {
    for ( const auto& f : ro_mem_functions ) {
      result_ << mem_function( inspector_->GetMemoryName( mem ), f, wasm_name_ );
    }
  }
  for ( const auto& mem : mems ) {
    for ( const auto& f : rw_mem_functions ) {
      result_ << mem_function( inspector_->GetMemoryName( mem ), f, wasm_name_ );
    }
  }

  for ( const auto& table : ro_tables ) {
    for ( const auto& f : ro_tbl_functions ) {
      result_ << tbl_function( inspector_->GetTableName( table ), f, wasm_name_ );
    }
  }
  for ( const auto& table : tables ) {
    for ( const auto& f : rw_tbl_functions ) {
      result_ << tbl_function( inspector_->GetTableName( table ), f, wasm_name_ );
    }
  }

  if ( inspector_->ExportsMainMemory() ) {
    result_ << std::format( R"IO(
void {} (struct w2c_fixpoint* instance, uint32_t index, uint32_t length) {{
{}(instance, index, length);
}}
)IO",
                            ExportName( "fixpoint", "unsafe_io" ),
                            ExportName( "fixpoint", "unsafely_log_memory" ) );
  }

  result_ << std::format( R"RUN(
unsigned long _apply(struct w2c_fixpoint *ctx, wasm_rt_externref_t encode) {{
  {0} *instance = ctx->os->aligned_alloc(_Alignof(__m256i), sizeof({0}));
  SET_CONTEXT(ctx);
  wasm2c_{1}_instantiate(instance, ctx);
  protect_memories_and_tables(instance);

  wasm_rt_externref_t result = {2}(instance, encode);

  wasm2c_{1}_free(instance);
  free(instance);
  /* return result; */
  return 0;
}}
)RUN",
                          state_info_type_name_,
                          module_prefix_,
                          ExportName( module_prefix_, "_fixpoint_apply" ) );

  result_ << shims_impl;

  return result_.str();
}

string InitComposer::compose_stub()
{
  ostringstream result_;
  result_ << R"ASM(
asm(
    "_start:\n"
    "jmp _apply\n"
);
)ASM";
  return result_.str();
}

pair<string, string> compose_header( string wasm_name,
                                     Module* module,
                                     Errors* error,
                                     wasminspector::WasmInspector* inspector )
{
  InitComposer composer( wasm_name, module, error, inspector );
  return { composer.compose_stub(), composer.compose_header() };
}
}
