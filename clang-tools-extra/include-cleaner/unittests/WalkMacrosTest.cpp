//===--- WalkMacrosTest.cpp -------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
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
void testWalk(llvm::StringRef TargetCode, llvm::StringRef ReferencingCode) {
  llvm::Annotations Target(TargetCode);
  llvm::Annotations Referencing(ReferencingCode);

  TestInputs Inputs(Referencing.code());
  Inputs.ExtraFiles["target.h"] = Target.code().str();
  Inputs.ExtraArgs.push_back("-include");
  Inputs.ExtraArgs.push_back("target.h");
  Inputs.ExtraArgs.push_back("-std=c++17");
  TestAST AST(Inputs);
  const auto &SM = AST.sourceManager();

  // We're only going to record references from the nominated point,
  // to the target file.
  FileID ReferencingFile = SM.getMainFileID();
  SourceLocation ReferencingLoc =
      SM.getComposedLoc(ReferencingFile, Referencing.point());
  FileID TargetFile = SM.translateFile(
      llvm::cantFail(AST.fileManager().getFileRef("target.h")));

  // Perform the walk, and capture the offsets of the referenced targets.
  std::vector<size_t> ReferencedOffsets;
  walkMacros(ReferencingFile, AST.preprocessor(),
             [&](SourceLocation Loc, MacroInfo *MI) {
               assert(Loc.isFileID());
               if (Loc != ReferencingLoc)
                 return;
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

TEST(WalkMacros, General) {
  testWalk("#define ^FOO 42", "int x = ^FOO;");
  // Refs only to active definition.
  testWalk("#define FOO 42", "#define FOO 42\nint x = ^FOO;");
  // Refs from macro bodies.
  testWalk("#define ^FOO 42", "#define BAR ^FOO");
  // Refs only to outer-most macro definition.
  testWalk("#define FOO 42\n#define ^BAR FOO", "int x = ^BAR;");
  // Refs from macro args.
  testWalk("#define ^FOO 42\n#define BAR(X) X", "int x = BAR(^FOO);");
  // Refs from disabled branches.
  testWalk("#define ^FOO", R"cpp(
  #if 0
  ^FOO
  #endif)cpp");
}
} // namespace
} // namespace clang::include_cleaner
