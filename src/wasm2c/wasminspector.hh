#pragma once

#include <map>
#include <set>

#include "wabt/cast.h"
#include "wabt/common.h"
#include "wabt/error.h"
#include "wabt/expr-visitor.h"
#include "wabt/ir.h"
#include "wabt/wast-lexer.h"

namespace wasminspector {

class WasmInspector : public wabt::ExprVisitor::DelegateNop
{
public:
  WasmInspector( wabt::Module* module, wabt::Errors* errors );
  WasmInspector( const WasmInspector& ) = default;
  WasmInspector& operator=( const WasmInspector& ) = default;

  wabt::Result Validate();
  const std::set<std::string>& GetImportedFunctions() { return imported_functions_; }
  std::set<wabt::Index> GetExportedROMems() { return exported_ro_mems_; }
  std::set<wabt::Index> GetExportedMems() { return exported_mems_; }
  std::set<wabt::Index> GetExportedROTables() { return exported_ro_tables_; }
  std::set<wabt::Index> GetExportedTables() { return exported_tables_; }
  bool ExportsMainMemory(); // True if module exports memory named memory
  std::string GetMemoryName( wabt::Index idx );
  std::string GetTableName( wabt::Index idx );

  // Implementation of ExprVisitor::DelegateNop.
  wabt::Result OnMemoryCopyExpr( wabt::MemoryCopyExpr* ) override;
  wabt::Result OnMemoryFillExpr( wabt::MemoryFillExpr* ) override;
  wabt::Result OnMemoryGrowExpr( wabt::MemoryGrowExpr* ) override;
  wabt::Result OnMemoryInitExpr( wabt::MemoryInitExpr* ) override;
  wabt::Result OnTableSetExpr( wabt::TableSetExpr* ) override;
  wabt::Result OnTableCopyExpr( wabt::TableCopyExpr* ) override;
  wabt::Result OnTableGrowExpr( wabt::TableGrowExpr* ) override;
  wabt::Result OnTableFillExpr( wabt::TableFillExpr* ) override;
  wabt::Result OnTableInitExpr( wabt::TableInitExpr* ) override;
  wabt::Result OnStoreExpr( wabt::StoreExpr* ) override;

private:
  void VisitFunc( wabt::Func* func );
  void VisitExport( wabt::Export* export_ );
  void VisitGlobal( wabt::Global* global );
  void VisitElemSegment( wabt::ElemSegment* segment );
  void VisitDataSegment( wabt::DataSegment* segment );
  void VisitScriptModule( wabt::ScriptModule* script_module );
  void VisitCommand( wabt::Command* command );
  wabt::Result MarkMemoryWriteable( wabt::Var* memidx );
  wabt::Result MarkTableWriteable( wabt::Var* tableidx );
  wabt::Result ValidateAccess();
  wabt::Result ValidateImports();

  wabt::Errors* errors_ = nullptr;
  wabt::Module* current_module_ = nullptr;
  wabt::Func* current_func_ = nullptr;
  wabt::ExprVisitor visitor_;
  wabt::Result result_ = wabt::Result::Ok;

  std::set<std::string> imported_functions_;
  std::set<wabt::Index> exported_ro_mems_ {};
  std::set<wabt::Index> exported_mems_ {};
  std::set<wabt::Index> exported_ro_tables_ {};
  std::set<wabt::Index> exported_tables_ {};
};

} // namespace wasminspector
