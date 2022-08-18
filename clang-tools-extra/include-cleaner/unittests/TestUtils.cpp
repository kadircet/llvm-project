//===--- TestUtils.cpp ------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TestUtils.h"

namespace clang::include_cleaner {

std::string diagnosePoints(TestAST &AST, FileID TargetFile,
                           llvm::ArrayRef<size_t> TargetOffsets,
                           llvm::ArrayRef<size_t> ReferencedOffsets) {
  const auto &SM = AST.sourceManager();
  // Compare results to the expected points.
  // For each difference, show the target point in context, like a diagnostic.
  std::string DiagBuf;
  llvm::raw_string_ostream DiagOS(DiagBuf);
  auto *DiagOpts = new DiagnosticOptions();
  DiagOpts->ShowLevel = 0;
  DiagOpts->ShowNoteIncludeStack = 0;
  TextDiagnostic Diag(DiagOS, AST.context().getLangOpts(), DiagOpts);
  auto DiagnosePoint = [&](const char *Message, unsigned Offset) {
    Diag.emitDiagnostic(
        FullSourceLoc(SM.getComposedLoc(TargetFile, Offset), SM),
        DiagnosticsEngine::Note, Message, {}, {});
  };
  for (auto Expected : TargetOffsets)
    if (!llvm::is_contained(ReferencedOffsets, Expected))
      DiagnosePoint("location not marked used", Expected);
  for (auto Actual : ReferencedOffsets)
    if (!llvm::is_contained(TargetOffsets, Actual))
      DiagnosePoint("location unexpectedly used", Actual);

  return DiagBuf;
}
} // namespace clang::include_cleaner
