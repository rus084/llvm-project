//===-- I8051InstrInfo.cpp - I8051 Instruction Information --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the I8051 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "I8051InstrInfo.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#include "I8051.h"
//#include "I8051MachineFunctionInfo.h"
#include "I8051RegisterInfo.h"
#include "I8051TargetMachine.h"
#include "MCTargetDesc/I8051MCTargetDesc.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "I8051GenInstrInfo.inc"

namespace llvm {

I8051InstrInfo::I8051InstrInfo(I8051Subtarget &STI)
    : I8051GenInstrInfo(), RI(),
      STI(STI) {}

void I8051InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI,
                               const DebugLoc &DL, MCRegister DestReg,
                               MCRegister SrcReg, bool KillSrc,
                               bool RenamableDest, bool RenamableSrc) const {
  llvm_unreachable("Not implemented yet!");
}

Register I8051InstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                           int &FrameIndex) const {
  llvm_unreachable("Not implemented yet!");
  return 0;
}

Register I8051InstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                          int &FrameIndex) const {
  llvm_unreachable("Not implemented yet!");
  return 0;
}

void I8051InstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg,
    bool isKill, int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg) const {
  llvm_unreachable("Not implemented yet!");
}

void I8051InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator MI,
                                        Register DestReg, int FrameIndex,
                                        const TargetRegisterClass *RC,
                                        const TargetRegisterInfo *TRI,
                                        Register VReg) const {
  llvm_unreachable("Not implemented yet!");
}

const MCInstrDesc &I8051InstrInfo::getBrCond(I8051CC::CondCodes CC) const {
  switch (CC) {
  default:
    llvm_unreachable("Unknown condition code!");
  }
}

I8051CC::CondCodes I8051InstrInfo::getCondFromBranchOpc(unsigned Opc) const {
  switch (Opc) {
  default:
    return I8051CC::COND_INVALID;
  }
}

I8051CC::CondCodes I8051InstrInfo::getOppositeCondition(I8051CC::CondCodes CC) const {
  switch (CC) {
  default:
    llvm_unreachable("Invalid condition!");
  }
}

bool I8051InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                 MachineBasicBlock *&TBB,
                                 MachineBasicBlock *&FBB,
                                 SmallVectorImpl<MachineOperand> &Cond,
                                 bool AllowModify) const {
  llvm_unreachable("analyzeBranch");
  return false;
}

unsigned I8051InstrInfo::insertBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *TBB,
                                    MachineBasicBlock *FBB,
                                    ArrayRef<MachineOperand> Cond,
                                    const DebugLoc &DL, int *BytesAdded) const {
  llvm_unreachable("insertBranch");
  return 0;
}

unsigned I8051InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                    int *BytesRemoved) const {
  llvm_unreachable("removeBranch");
  return 0;
}

bool I8051InstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  assert(Cond.size() == 1 && "Invalid I8051 branch condition!");

  I8051CC::CondCodes CC = static_cast<I8051CC::CondCodes>(Cond[0].getImm());
  Cond[0].setImm(getOppositeCondition(CC));

  llvm_unreachable("Not implemented yet!");
  return false;
}

unsigned I8051InstrInfo::getInstSizeInBytes(const MachineInstr &MI) const {
  unsigned Opcode = MI.getOpcode();
    llvm_unreachable("Not implemented yet!");

  switch (Opcode) {
  // A regular instruction
  default: {
    const MCInstrDesc &Desc = get(Opcode);
    return Desc.getSize();
  }
  case TargetOpcode::EH_LABEL:
  case TargetOpcode::IMPLICIT_DEF:
  case TargetOpcode::KILL:
  case TargetOpcode::DBG_VALUE:
    return 0;
  case TargetOpcode::INLINEASM:
  case TargetOpcode::INLINEASM_BR: {
    const MachineFunction &MF = *MI.getParent()->getParent();
    const I8051TargetMachine &TM =
        static_cast<const I8051TargetMachine &>(MF.getTarget());
    const TargetInstrInfo &TII = *STI.getInstrInfo();
    return TII.getInlineAsmLength(MI.getOperand(0).getSymbolName(),
                                  *TM.getMCAsmInfo());
  }
  }
}

MachineBasicBlock *
I8051InstrInfo::getBranchDestBlock(const MachineInstr &MI) const {
  switch (MI.getOpcode()) {
  default:
    llvm_unreachable("unexpected opcode!");
  }
  return nullptr;
}

bool I8051InstrInfo::isBranchOffsetInRange(unsigned BranchOp,
                                         int64_t BrOffset) const {
  switch (BranchOp) {
  default:
    llvm_unreachable("unexpected opcode!");
  }
  return false;
}

void I8051InstrInfo::insertIndirectBranch(MachineBasicBlock &MBB,
                                        MachineBasicBlock &NewDestBB,
                                        MachineBasicBlock &RestoreBB,
                                        const DebugLoc &DL, int64_t BrOffset,
                                        RegScavenger *RS) const {
  llvm_unreachable("insertIndirectBranch");
}

} // end of namespace llvm
