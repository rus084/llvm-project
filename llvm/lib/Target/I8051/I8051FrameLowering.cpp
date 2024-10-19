//===-- I8051FrameLowering.cpp - I8051 Frame Information ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the I8051 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "I8051FrameLowering.h"

#include "I8051.h"
#include "I8051InstrInfo.h"
//#include "I8051MachineFunctionInfo.h"
#include "I8051TargetMachine.h"
#include "MCTargetDesc/I8051MCTargetDesc.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Function.h"

namespace llvm {

I8051FrameLowering::I8051FrameLowering()
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(1), -2) {}

bool I8051FrameLowering::canSimplifyCallFramePseudos(
    const MachineFunction &MF) const {
  // Always simplify call frame pseudo instructions, even when
  // hasReservedCallFrame is false.
  llvm_unreachable("Not implemented yet!");
  return true;
}

bool I8051FrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  llvm_unreachable("Not implemented yet!");
  return !MFI.hasVarSizedObjects();
}

void I8051FrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  llvm_unreachable("Not implemented yet!");
}

void I8051FrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  llvm_unreachable("Not implemented yet!");
}

bool I8051FrameLowering::hasFP(const MachineFunction &MF) const {
  llvm_unreachable("Not implemented yet!");
  return false;
}

bool I8051FrameLowering::spillCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    ArrayRef<CalleeSavedInfo> CSI, const TargetRegisterInfo *TRI) const {
  llvm_unreachable("Not implemented yet!");
  return true;
}

bool I8051FrameLowering::restoreCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    MutableArrayRef<CalleeSavedInfo> CSI, const TargetRegisterInfo *TRI) const {
  llvm_unreachable("Not implemented yet!");
  return true;
}

MachineBasicBlock::iterator I8051FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {
  llvm_unreachable("Not implemented yet!");
  return MBB.erase(MI);
}

void I8051FrameLowering::determineCalleeSaves(MachineFunction &MF,
                                            BitVector &SavedRegs,
                                            RegScavenger *RS) const {
  llvm_unreachable("Not implemented yet!");
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
}
/// The frame analyzer pass.
///
/// Scans the function for allocas and used arguments
/// that are passed through the stack.
struct I8051FrameAnalyzer : public MachineFunctionPass {
  static char ID;
  I8051FrameAnalyzer() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    llvm_unreachable("Not implemented yet!");
    return false;
  }

  StringRef getPassName() const override { return "I8051 Frame Analyzer"; }
};

char I8051FrameAnalyzer::ID = 0;

/// Creates instance of the frame analyzer pass.
FunctionPass *createI8051FrameAnalyzerPass() { return new I8051FrameAnalyzer(); }

} // end of namespace llvm
