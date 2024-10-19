//===-- I8051RegisterInfo.cpp - I8051 Register Information --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the I8051 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "I8051RegisterInfo.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/IR/Function.h"

#include "I8051.h"
#include "I8051InstrInfo.h"
//#include "I8051MachineFunctionInfo.h"
#include "I8051TargetMachine.h"
#include "MCTargetDesc/I8051MCTargetDesc.h"

#define GET_REGINFO_TARGET_DESC
#include "I8051GenRegisterInfo.inc"

namespace llvm {

I8051RegisterInfo::I8051RegisterInfo() : I8051GenRegisterInfo(0) {}

const uint16_t *
I8051RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  llvm_unreachable("todo: getCalleeSavedRegs");
  return nullptr;
}

const uint32_t *
I8051RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                      CallingConv::ID CC) const {
  llvm_unreachable("todo: getCallPreservedMask");

  return nullptr;
}

BitVector I8051RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  llvm_unreachable("todo: getReservedRegs");

  BitVector Reserved(getNumRegs());

  // Reserve the intermediate result register B
  // The result of instructions like 'mul' is always stored here.
  Reserved.set(I8051::B);

  // Reserve the stack pointer.
  Reserved.set(I8051::SP);

  return Reserved;
}

const TargetRegisterClass *
I8051RegisterInfo::getLargestLegalSuperClass(const TargetRegisterClass *RC,
                                           const MachineFunction &MF) const {
  llvm_unreachable("todo: getLargestLegalSuperClass");

  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
  if (TRI->isTypeLegalForClass(*RC, MVT::i16)) {
    return &I8051::GR16RegClass;
  }

  if (TRI->isTypeLegalForClass(*RC, MVT::i8)) {
    return &I8051::GR8RegClass;
  }

  llvm_unreachable("Invalid register size");
}

bool I8051RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  llvm_unreachable("todo: eliminateFrameIndex");

  return false;
}

Register I8051RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  llvm_unreachable("todo: getFrameRegister");
  return I8051::SP;
}

const TargetRegisterClass *
I8051RegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                    unsigned Kind) const {
  llvm_unreachable("todo: getPointerRegClass");

  return &I8051::GR16RegClass;
}

void I8051RegisterInfo::splitReg(Register Reg, Register &LoReg,
                               Register &HiReg) const {
  llvm_unreachable("todo: splitReg");

  assert(I8051::GR16RegClass.contains(Reg) && "can only split 16-bit registers");

  LoReg = getSubReg(Reg, I8051::sub_lo);
  HiReg = getSubReg(Reg, I8051::sub_hi);
}

bool I8051RegisterInfo::shouldCoalesce(
    MachineInstr *MI, const TargetRegisterClass *SrcRC, unsigned SubReg,
    const TargetRegisterClass *DstRC, unsigned DstSubReg,
    const TargetRegisterClass *NewRC, LiveIntervals &LIS) const {
  llvm_unreachable("todo: shouldCoalesce");
  return TargetRegisterInfo::shouldCoalesce(MI, SrcRC, SubReg, DstRC, DstSubReg,
                                            NewRC, LIS);
}

} // end of namespace llvm
