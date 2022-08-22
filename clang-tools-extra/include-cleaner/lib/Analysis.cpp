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
#include "clang/AST/DeclCXX.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/STLFunctionalExtras.h"
#include "llvm/Support/Casting.h"
#include <vector>

namespace clang::include_cleaner {

std::vector<const Decl *> locateDecls(NamedDecl &ND) {
  const auto &SM = ND.getASTContext().getSourceManager();
  // FIXME: Signals
  if (const auto *TD = llvm::dyn_cast<RecordDecl>(&ND)) {
    if (auto *Def = TD->getDefinition())
      return {Def};
    // FIXME: Signals.
    // FIXME: Handle canonical forward decl.
    auto LastDeclLoc = TD->getMostRecentDecl()->getLocation();
    if (SM.isInMainFile(LastDeclLoc))
      return {TD->getMostRecentDecl()};
  }
  // FIXME: Signals.
  std::vector<const Decl *> Result;
  for (auto *Redecl : ND.redecls())
    Result.emplace_back(Redecl);

  if (auto *MD = llvm::dyn_cast<CXXMethodDecl>(&ND))
    Result.emplace_back(MD->getParent());
  else if (auto *FD = llvm::dyn_cast<FieldDecl>(&ND))
    Result.emplace_back(FD->getParent());
  else if (auto *EC = llvm::dyn_cast<EnumConstantDecl>(&ND))
    Result.emplace_back(EC->getType()->getAsTagDecl());
  if (auto *FT = ND.getFunctionType())
    if (auto *TD = FT->getReturnType()->getAsTagDecl())
      Result.emplace_back(TD);
  return Result;
}

void walkUsed(ASTContext &Ctx, Preprocessor &PP,
              llvm::function_ref<void(SourceLocation)> CB) {
  auto &SM = PP.getSourceManager();
  walkAST(*Ctx.getTranslationUnitDecl(),
          [&](SourceLocation Loc, NamedDecl &ND) {
            for (auto *D : locateDecls(ND))
              CB(D->getLocation());
          });
  walkMacros(SM.getMainFileID(), PP, [CB](SourceLocation Loc, MacroInfo *MI) {
    CB(MI->getDefinitionLoc());
  });
}
} // namespace clang::include_cleaner
