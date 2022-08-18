#ifndef LLVM_CLANG_TOOLS_EXTRA_INCLUDE_CLEANER_INCLUDE_ANALYSIS_H_
#define LLVM_CLANG_TOOLS_EXTRA_INCLUDE_CLEANER_INCLUDE_ANALYSIS_H_

#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/STLFunctionalExtras.h"

namespace clang {
class Decl;
class NamedDecl;
class Preprocessor;
class MacroInfo;
namespace include_cleaner {

/// Traverses part of the AST from \p Root, finding uses of symbols.
///
/// Each use is reported to the callback:
/// - the SourceLocation describes where the symbol was used. This is usually
///   the primary location of the AST node found under Root.
/// - the NamedDecl is the symbol referenced. It is canonical, rather than e.g.
///   the redecl actually found by lookup.
///
/// walkAST is typically called once per top-level declaration in the file
/// being analyzed, in order to find all references within it.
void walkAST(Decl &Root, llvm::function_ref<void(SourceLocation, NamedDecl &)>);

/// Traverses spelled tokens in a given file and collects macro uses.
void walkMacros(FileID FID, Preprocessor &PP,
                llvm::function_ref<void(SourceLocation, MacroInfo *MI)>);
} // namespace include_cleaner
} // namespace clang
#endif
