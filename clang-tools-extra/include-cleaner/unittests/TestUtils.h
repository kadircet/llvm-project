//===--- TestUtils.h --------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_TOOLS_EXTRA_INCLUDE_CLEANER_UNITTESTS_TESTUTILS_H_
#define LLVM_CLANG_TOOLS_EXTRA_INCLUDE_CLEANER_UNITTESTS_TESTUTILS_H_

#include "clang/Frontend/TextDiagnostic.h"
#include "clang/Testing/TestAST.h"
#include "llvm/ADT/ArrayRef.h"
#include <string>

namespace clang::include_cleaner {
std::string diagnosePoints(TestAST &AST, FileID TargetFile,
                           llvm::ArrayRef<size_t> TargetOffsets,
                           llvm::ArrayRef<size_t> ReferencedOffsets);
} // namespace clang::include_cleaner

#endif
