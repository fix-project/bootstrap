#include <clang/Basic/FileManager.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/CodeGen/BackendUtil.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/VirtualFileSystem.h>

#include "cc1args.h"
#include "depfile.h"
#include "wasm-rt-content.h"

using namespace std;

typedef char __attribute__((address_space(10))) * externref;
externref fixpoint_apply(externref encode)
    __attribute__((export_name("_fixpoint_apply")));

extern void program_memory_to_rw_0(int32_t, const void *, int32_t)
    __attribute__((import_module("asm"),
                   import_name("program_memory_to_rw_0")));
extern void ro_0_to_program_memory(const void *, int32_t, int32_t)
    __attribute__((import_module("asm"),
                   import_name("ro_0_to_program_memory")));
extern int32_t grow_rw_0(int32_t)
    __attribute__((import_module("asm"), import_name("grow_rw_0")));

extern void attach_blob_ro_mem_0(externref)
    __attribute__((import_module("fixpoint"),
                   import_name("attach_blob_ro_mem_0")));
extern int32_t size_ro_mem_0(void)
    __attribute__((import_module("fixpoint"), import_name("size_ro_mem_0")));
extern externref create_blob_rw_mem_0(int32_t)
    __attribute__((import_module("fixpoint"),
                   import_name("create_blob_rw_mem_0")));

extern externref get_ro_table_0(int32_t)
    __attribute__((import_module("asm"), import_name("get_ro_table_0")));
extern void attach_tree_ro_table_0(externref)
    __attribute__((import_module("fixpoint"),
                   import_name("attach_tree_ro_table_0")));
extern externref get_ro_table_1(int32_t)
    __attribute__((import_module("asm"), import_name("get_ro_table_1")));
extern void attach_tree_ro_table_1(externref)
    __attribute__((import_module("fixpoint"),
                   import_name("attach_tree_ro_table_1")));

using namespace clang;
using namespace llvm;

string c_to_elf(char *system_dep_files[], char *clang_dep_files[],
                char *function_c_buffer, char *function_h_buffer,
                char *function_fixpoint_h_buffer) {
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  // Create compiler instance
  clang::CompilerInstance compilerInstance;

  // Create diagnostic engine
  string diagOutput;
  raw_string_ostream diagOS(diagOutput);
  auto diagPrinter =
      make_unique<TextDiagnosticPrinter>(diagOS, new DiagnosticOptions());

  IntrusiveRefCntPtr<DiagnosticsEngine> diagEngine =
      CompilerInstance::createDiagnostics(new DiagnosticOptions(),
                                          diagPrinter.get(), false);

  // Create File System
  IntrusiveRefCntPtr<vfs::InMemoryFileSystem> InMemFS(
      new vfs::InMemoryFileSystem());

  for (size_t i = 0; i < 63; i++) {
    InMemFS->addFile(system_deps[i], 0,
                     MemoryBuffer::getMemBuffer(system_dep_files[i]));
  }

  for (size_t i = 0; i < 92; i++) {
    InMemFS->addFile(clang_deps[i], 0,
                     MemoryBuffer::getMemBuffer(clang_dep_files[i]));
  }

  InMemFS->addFile("/fix/function.c", 0,
                   MemoryBuffer::getMemBuffer(function_c_buffer));
  InMemFS->addFile("/fix/function.h", 0,
                   MemoryBuffer::getMemBuffer(function_h_buffer));
  InMemFS->addFile("/fix/function_fixpoint.h", 0,
                   MemoryBuffer::getMemBuffer(function_fixpoint_h_buffer));
  InMemFS->addFile("/fix/wasm-rt.h", 0,
                   MemoryBuffer::getMemBuffer(wasm_rt_content));

  InMemFS->setCurrentWorkingDirectory("/fix");

  FileManager *FM = new FileManager(FileSystemOptions{}, InMemFS);
  compilerInstance.setFileManager(FM);

  auto compilerInvocation = std::make_shared<CompilerInvocation>();
  if (!CompilerInvocation::CreateFromArgs(*compilerInvocation, cc1args,
                                          *diagEngine)) {
    return diagOS.str() + "\nFailed to create compiler invocation.\n";
  }

  LLVMContext context;
  std::unique_ptr<CodeGenAction> action(new EmitLLVMOnlyAction(&context));
  compilerInstance.createDiagnostics(diagPrinter.get(), false);
  compilerInstance.setInvocation(compilerInvocation);

  auto &codegenOptions = compilerInstance.getCodeGenOpts();
  codegenOptions.CodeModel = "large";
  codegenOptions.RelocationModel = llvm::Reloc::Static;

  if (!compilerInstance.ExecuteAction(*action)) {
    return diagOS.str() + "\nFailed to emit llvm\n";
  }

  std::unique_ptr<llvm::Module> module = action->takeModule();
  if (!module) {
    return "\nFailed to take module\n";
  }

  std::string llvm_res;
  raw_string_ostream Str_OS(llvm_res);
  auto OS = make_unique<buffer_ostream>(Str_OS);

  EmitBackendOutput(
      compilerInstance.getDiagnostics(), compilerInstance.getHeaderSearchOpts(),
      compilerInstance.getCodeGenOpts(), compilerInstance.getTargetOpts(),
      compilerInstance.getLangOpts(),
      compilerInstance.getTarget().getDataLayoutString(), module.get(),
      Backend_EmitObj, std::move(OS));

  return llvm_res;
}

externref fixpoint_apply(externref encode) {
  attach_tree_ro_table_0(encode);

  attach_tree_ro_table_1(get_ro_table_0(2));
  char *system_dep_files[63];
  for (size_t i = 0; i < 63; i++) {
    attach_blob_ro_mem_0(get_ro_table_1(i));
    char *buffer = (char *)malloc(size_ro_mem_0() + 1);
    ro_0_to_program_memory(buffer, 0, size_ro_mem_0());
    buffer[size_ro_mem_0()] = '\0';
    system_dep_files[i] = buffer;
  }

  attach_tree_ro_table_1(get_ro_table_0(3));
  char *clang_dep_files[92];
  for (size_t i = 0; i < 92; i++) {
    attach_blob_ro_mem_0(get_ro_table_1(i));
    char *buffer = (char *)malloc(size_ro_mem_0() + 1);
    ro_0_to_program_memory(buffer, 0, size_ro_mem_0());
    buffer[size_ro_mem_0()] = '\0';
    clang_dep_files[i] = buffer;
  }

  attach_tree_ro_table_1(get_ro_table_0(4));
  attach_blob_ro_mem_0(get_ro_table_1(0));
  char *function_c_buffer = (char *)malloc(size_ro_mem_0() + 1);
  ro_0_to_program_memory(function_c_buffer, 0, size_ro_mem_0());
  function_c_buffer[size_ro_mem_0()] = '\0';

  attach_blob_ro_mem_0(get_ro_table_1(1));
  char *function_h_buffer = (char *)malloc(size_ro_mem_0() + 1);
  ro_0_to_program_memory(function_h_buffer, 0, size_ro_mem_0());
  function_h_buffer[size_ro_mem_0()] = '\0';

  attach_blob_ro_mem_0(get_ro_table_1(2));
  char *function_fixpoint_h_buffer = (char *)malloc(size_ro_mem_0() + 1);
  ro_0_to_program_memory(function_fixpoint_h_buffer, 0, size_ro_mem_0());
  function_fixpoint_h_buffer[size_ro_mem_0()] = '\0';

  string res = c_to_elf(system_dep_files, clang_dep_files, function_c_buffer,
                        function_h_buffer, function_fixpoint_h_buffer);

  if ((res.size() >> 16) > 0) {
    grow_rw_0(res.size() >> 16);
  }
  program_memory_to_rw_0(0, res.data(), res.size());
  return create_blob_rw_mem_0(res.size());
}
