#include "wasminspector.hh"

#include <string>

using namespace std;

namespace wasminspector {

using namespace wabt;

wabt::Error createErrorWithMessage( string message )
{
  wabt::Error error = wabt::Error();
  error.message = message;
  return error;
}

WasmInspector::WasmInspector( Module* module, Errors* errors )
  : errors_( errors )
  , current_module_( module )
  , visitor_( this )
{
  for ( Export* export_ : module->exports ) {
    if ( export_->kind == ExternalKind::Memory ) {
      exported_ro_mems_.insert( current_module_->GetMemoryIndex( export_->var ) );
      exported_mems_.insert( current_module_->GetMemoryIndex( export_->var ) );
    }

    if ( export_->kind == ExternalKind::Table ) {
      exported_ro_tables_.insert( current_module_->GetTableIndex( export_->var ) );
      exported_tables_.insert( current_module_->GetTableIndex( export_->var ) );
    }
  }

  for ( Import* import_ : module->imports ) {
    if ( import_->kind() == ExternalKind::Func ) {
      imported_functions_.insert( import_->field_name );
    }
  }
}

bool WasmInspector::ExportsMainMemory()
{
  for ( Export* export_ : current_module_->exports ) {
    if ( export_->kind == ExternalKind::Memory && export_->name == "memory" )
      return true;
  }
  return false;
}

string WasmInspector::GetMemoryName( wabt::Index idx )
{
  return current_module_->memories[idx]->name.substr( 1 );
}

string WasmInspector::GetTableName( wabt::Index idx )
{
  return current_module_->tables[idx]->name.substr( 1 );
}

Result WasmInspector::OnMemoryCopyExpr( MemoryCopyExpr* expr )
{
  return MarkMemoryWriteable( &expr->destmemidx );
}

Result WasmInspector::OnMemoryFillExpr( MemoryFillExpr* expr )
{
  return MarkMemoryWriteable( &expr->memidx );
}

Result WasmInspector::OnMemoryGrowExpr( MemoryGrowExpr* expr )
{
  return MarkMemoryWriteable( &expr->memidx );
}

Result WasmInspector::OnMemoryInitExpr( MemoryInitExpr* expr )
{
  return MarkMemoryWriteable( &expr->memidx );
}

Result WasmInspector::OnStoreExpr( StoreExpr* expr )
{
  return MarkMemoryWriteable( &expr->memidx );
}

Result WasmInspector::OnTableSetExpr( TableSetExpr* expr )
{
  return MarkTableWriteable( &expr->var );
}

Result WasmInspector::OnTableCopyExpr( TableCopyExpr* expr )
{
  return MarkTableWriteable( &expr->dst_table );
}

Result WasmInspector::OnTableGrowExpr( TableGrowExpr* expr )
{
  return MarkTableWriteable( &expr->var );
}

Result WasmInspector::OnTableFillExpr( TableFillExpr* expr )
{
  return MarkTableWriteable( &expr->var );
}

Result WasmInspector::OnTableInitExpr( TableInitExpr* expr )
{
  return MarkTableWriteable( &expr->table_index );
}

void WasmInspector::VisitFunc( Func* func )
{
  current_func_ = func;
  visitor_.VisitFunc( func );
  current_func_ = nullptr;
}

void WasmInspector::VisitExport( Export* export_ )
{
  result_ = MarkMemoryWriteable( &export_->var );
}

void WasmInspector::VisitGlobal( Global* global )
{
  visitor_.VisitExprList( global->init_expr );
}

void WasmInspector::VisitElemSegment( ElemSegment* segment )
{
  visitor_.VisitExprList( segment->offset );
}

void WasmInspector::VisitDataSegment( DataSegment* segment )
{
  visitor_.VisitExprList( segment->offset );
}

Result WasmInspector::ValidateAccess()
{
  for ( Func* func : current_module_->funcs )
    VisitFunc( func );
  for ( Export* export_ : current_module_->exports )
    VisitExport( export_ );
  for ( Global* global : current_module_->globals )
    VisitGlobal( global );
  for ( ElemSegment* elem_segment : current_module_->elem_segments )
    VisitElemSegment( elem_segment );
  for ( DataSegment* data_segment : current_module_->data_segments )
    VisitDataSegment( data_segment );
  return result_;
}

Result WasmInspector::ValidateImports()
{
  // Module does not contain imports from other modules
  for ( Import* import : current_module_->imports ) {
    switch ( import->kind() ) {
      case ExternalKind::Global:
      case ExternalKind::Table: {
        break;
      }

      case ExternalKind::Memory:
      case ExternalKind::Func: {
        if ( import->module_name != "fixpoint" ) {
          string message = "ValidateImports: module name is not fixpoint " + import->field_name;
          errors_->push_back( createErrorWithMessage( message ) );
          return Result::Error;
        }
        break;
      }

      default:
        string message = "ValidateImports: ExternalKind not found";
        errors_->push_back( createErrorWithMessage( message ) );
        return Result::Error;
    }
  }

  // Only rw memory can have nonzero initial size
  for ( auto index : this->exported_ro_mems_ ) {
    Memory* memory = current_module_->memories[index];
    if ( memory->page_limits.initial > 0 ) {
      return Result::Error;
    }
  }

  // Only rw table can have nonzero initial size
  for ( auto index : this->exported_ro_tables_ ) {
    Table* table = current_module_->tables[index];
    if ( table->elem_limits.initial > 0 ) {
      return Result::Error;
    }
  }

  // unsafe_io requires memory called "memory"
  if ( !ExportsMainMemory() ) {
    for ( auto it = imported_functions_.begin(); it != imported_functions_.end(); it++ ) {
      if ( *it == "unsafe_io" ) {
        string message = "ValidateImports: If unsafe_io is imported, memory with name memory must be exported.";
        errors_->push_back( createErrorWithMessage( message ) );
        return Result::Error;
      }
    }
  }
  return Result::Ok;
}

Result WasmInspector::Validate()
{
  auto result = ValidateAccess();
  result |= ValidateImports();
  return result;
}

Result WasmInspector::MarkMemoryWriteable( Var* memidx )
{
  exported_ro_mems_.erase( current_module_->GetMemoryIndex( *memidx ) );
  return Result::Ok;
}

Result WasmInspector::MarkTableWriteable( Var* tableidx )
{
  exported_ro_tables_.erase( current_module_->GetMemoryIndex( *tableidx ) );
  return Result::Ok;
}

} // namespace wasminspector
