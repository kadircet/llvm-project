//===--- WalkMacrosTest.cpp -------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AnalysisInternal.h"
#include "TestUtils.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Testing/TestAST.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Testing/Support/Annotations.h"
#include "gtest/gtest.h"

namespace clang::include_cleaner {
namespace {
void testReferences(llvm::StringRef TargetCode,
                    llvm::StringRef ReferencingCode) {
  llvm::Annotations Target(TargetCode);

  TestInputs Inputs(ReferencingCode);
  Inputs.ExtraFiles["target.h"] = Target.code().str();
  Inputs.ExtraArgs.push_back("-include");
  Inputs.ExtraArgs.push_back("target.h");
  Inputs.ExtraArgs.push_back("-std=c++17");
  TestAST AST(Inputs);
  const auto &SM = AST.sourceManager();

  // We're only going to record references from the nominated point,
  // to the target file.
  FileID ReferencingFile = SM.getMainFileID();
  FileID TargetFile = SM.translateFile(
      llvm::cantFail(AST.fileManager().getFileRef("target.h")));

  // Perform the walk, and capture the offsets of the referenced targets.
  std::vector<size_t> ReferencedOffsets;
  walkMacros(ReferencingFile, AST.preprocessor(),
             [&](SourceLocation Loc, MacroInfo *MI) {
               assert(Loc.isFileID());
               auto MDLoc =
                   SM.getDecomposedLoc(SM.getFileLoc(MI->getDefinitionLoc()));
               if (MDLoc.first != TargetFile)
                 return;
               ReferencedOffsets.push_back(MDLoc.second);
             });
  llvm::sort(ReferencedOffsets);
  auto Diags =
      diagnosePoints(AST, TargetFile, Target.points(), ReferencedOffsets);
  if (!Diags.empty())
    ADD_FAILURE() << Diags << "\nfrom code:\n" << ReferencingCode;
}
} // namespace
} // namespace clang::include_cleaner
