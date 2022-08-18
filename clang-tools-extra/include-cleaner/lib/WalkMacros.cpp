//===--- WalkMacros.cpp - Find macro references in files ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AnalysisInternal.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Tooling/Syntax/Tokens.h"

namespace clang::include_cleaner {
void walkMacros(FileID FID, Preprocessor &PP,
                llvm::function_ref<void(SourceLocation, MacroInfo *MI)> CB) {
  auto &SM = PP.getSourceManager();
  syntax::TokenBuffer TB(SM);
  for (auto &Tok : syntax::tokenize(FID, SM, PP.getLangOpts())) {
    if (Tok.kind() != tok::identifier)
      continue;
    auto *II = PP.getIdentifierInfo(Tok.text(SM));
    if (!II)
      continue;
    auto Spelled = Tok.location();
    // Don't need to care for #define/#undef cases, as they won't introduce
    // references outside of the current file.
    auto MD = PP.getMacroDefinitionAtLoc(II, Spelled);
    // FIXME: We might miss some references, especially from modules.
    if (!MD)
      continue;
    CB(Spelled, MD.getMacroInfo());
    // FIXME: support stdlib macros
  }
}
} // namespace clang::include_cleaner
