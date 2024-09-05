// RUN: rm -rf %t/
// RUN: mkdir -p %t/
// RUN: echo "*/diag-enablement.cpp" > %t/foo.txt
// RUN: echo "namespace { void bar(); }" > %t/a.h
// RUN: %clang -Xclang=-verify -Wunused=%t/foo.txt -fsyntax-only -I%t %s

#include "a.h"

namespace {
void foo(); // expected-warning {{unused function 'foo'}}
} // namespace
