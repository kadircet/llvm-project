#ifndef LLVM_CLANG_TOOLS_EXTRA_INCLUDE_CLEANER_INCLUDE_ANALYSIS_H_
#define LLVM_CLANG_TOOLS_EXTRA_INCLUDE_CLEANER_INCLUDE_ANALYSIS_H_

#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/STLFunctionalExtras.h"

namespace clang {
class ASTContext;
class Preprocessor;
namespace include_cleaner {

void walkUsed(ASTContext &Ctx, Preprocessor &PP,
              llvm::function_ref<void(SourceLocation)> CB);
} // namespace include_cleaner
} // namespace clang
#endif
