//===-- I8051TargetMachine.cpp - Define TargetMachine for I8051 ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the I8051 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "I8051TargetMachine.h"

#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"

#include "TargetInfo/I8051TargetInfo.h"

#include <optional>

namespace llvm {

static const char *I8051DataLayout =
    "e-P1-p:16:8-i8:8-i16:8-i32:8-i64:8-f32:8-f64:8-n8-a:8";

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

I8051TargetMachine::I8051TargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   std::optional<Reloc::Model> RM,
                                   std::optional<CodeModel::Model> CM,
                                   CodeGenOptLevel OL, bool JIT)
    : LLVMTargetMachine(T, I8051DataLayout, TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL) {
  initAsmInfo();
}

namespace {
/// I8051 Code Generator Pass Configuration Options.
class I8051PassConfig : public TargetPassConfig {
public:
  I8051PassConfig(I8051TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  I8051TargetMachine &getI8051TargetMachine() const {
    return getTM<I8051TargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreSched2() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *I8051TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new I8051PassConfig(*this, PM);
}

void I8051PassConfig::addIRPasses() {
  TargetPassConfig::addIRPasses();
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeI8051Target() {
  // Register the target.
  RegisterTargetMachine<I8051TargetMachine> X(getTheI8051Target());

  auto &PR = *PassRegistry::getPassRegistry();
}

MachineFunctionInfo *I8051TargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return nullptr;
}

//===----------------------------------------------------------------------===//
// Pass Pipeline Configuration
//===----------------------------------------------------------------------===//

bool I8051PassConfig::addInstSelector() {
  return false;
}

void I8051PassConfig::addPreSched2() {
}

void I8051PassConfig::addPreEmitPass() {
  addPass(&BranchRelaxationPassID);
}

} // end of namespace llvm
