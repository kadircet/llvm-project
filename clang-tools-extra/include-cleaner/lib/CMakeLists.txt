set(LLVM_LINK_COMPONENTS Support)

add_clang_library(clangIncludeCleaner
  Analysis.cpp
  WalkAST.cpp
  WalkMacros.cpp

  LINK_LIBS
  clangBasic
  clangAST
  clangToolingSyntax
  )

