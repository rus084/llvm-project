//===-- I8051TargetMachine.h - Define TargetMachine for I8051 -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the I8051 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_I8051_TARGET_MACHINE_H
#define LLVM_I8051_TARGET_MACHINE_H

#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

#include <optional>

namespace llvm {

/// A generic I8051 implementation.
class I8051TargetMachine : public LLVMTargetMachine {
public:
  I8051TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   std::optional<Reloc::Model> RM,
                   std::optional<CodeModel::Model> CM, CodeGenOptLevel OL, bool JIT = false);

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;

private:
};

} // end namespace llvm

#endif // LLVM_I8051_TARGET_MACHINE_H
