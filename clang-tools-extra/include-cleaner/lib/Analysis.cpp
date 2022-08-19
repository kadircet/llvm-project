//===--- Analysis.cpp - Analysis building blocks ------------------- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AnalysisInternal.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/STLFunctionalExtras.h"

namespace clang::include_cleaner {

void findReferencedLocations(ASTContext &Ctx, Preprocessor &PP,
                             llvm::function_ref<void(SourceLocation)> CB) {
  walkAST(*Ctx.getTranslationUnitDecl(),
          [CB](SourceLocation Loc, NamedDecl &ND) { CB(Loc); });
  auto &SM = PP.getSourceManager();
  walkMacros(SM.getMainFileID(), PP,
             [CB](SourceLocation Loc, MacroInfo *MI) { CB(Loc); });
}
} // namespace clang::include_cleaner
