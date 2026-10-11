//===- CoroutineFrameLayout.h - Explicit coroutine storage --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_COROUTINEFRAMELAYOUT_H
#define LLVM_CLANG_AST_COROUTINEFRAMELAYOUT_H

#include "llvm/Support/Error.h"
#include <cstdint>

namespace llvm {
class Module;
}

namespace clang {
class FunctionDecl;

struct CoroutineFrameLayout {
  uint64_t SizeBytes;
  uint64_t AlignBytes;
};

/// AST/Sema depend on this interface, without depending on the CodeGen library.
/// One provider belongs to one ASTContext and one target configuration.
class CoroutineFrameLayoutProvider {
public:
  virtual ~CoroutineFrameLayoutProvider() = default;
  virtual llvm::Expected<CoroutineFrameLayout>
  getLayout(const FunctionDecl *F) = 0;
  virtual llvm::Error importInto(llvm::Module &M) = 0;
};
} // namespace clang

#endif
