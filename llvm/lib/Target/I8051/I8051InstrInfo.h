//===-- I8051InstrInfo.h - I8051 Instruction Information ------------*- C++ -*-===//
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

#ifndef LLVM_I8051_INSTR_INFO_H
#define LLVM_I8051_INSTR_INFO_H

#include "llvm/CodeGen/TargetInstrInfo.h"

#include "I8051RegisterInfo.h"

#define GET_INSTRINFO_HEADER
#include "I8051GenInstrInfo.inc"
#undef GET_INSTRINFO_HEADER

namespace llvm {

class I8051Subtarget;

namespace I8051CC {

/// I8051 specific condition codes.
enum CondCodes {
  COND_INVALID
};

} // end of namespace I8051CC

namespace I8051II {

/// Specifies a target operand flag.
enum TOF {
  MO_NO_FLAG,
};

} // end of namespace I8051II

/// Utilities related to the I8051 instruction set.
class I8051InstrInfo : public I8051GenInstrInfo {
public:
  explicit I8051InstrInfo(I8051Subtarget &STI);

  const I8051RegisterInfo &getRegisterInfo() const { return RI; }
  const MCInstrDesc &getBrCond(I8051CC::CondCodes CC) const;
  I8051CC::CondCodes getCondFromBranchOpc(unsigned Opc) const;
  I8051CC::CondCodes getOppositeCondition(I8051CC::CondCodes CC) const;
  unsigned getInstSizeInBytes(const MachineInstr &MI) const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;
  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, Register SrcReg,
                           bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI,
                           Register VReg) const override;
  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI, Register DestReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const override;
  Register isLoadFromStackSlot(const MachineInstr &MI,
                               int &FrameIndex) const override;
  Register isStoreToStackSlot(const MachineInstr &MI,
                              int &FrameIndex) const override;

  // Branch analysis.
  bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                     MachineBasicBlock *&FBB,
                     SmallVectorImpl<MachineOperand> &Cond,
                     bool AllowModify = false) const override;
  unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        const DebugLoc &DL,
                        int *BytesAdded = nullptr) const override;
  unsigned removeBranch(MachineBasicBlock &MBB,
                        int *BytesRemoved = nullptr) const override;
  bool
  reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const override;

  MachineBasicBlock *getBranchDestBlock(const MachineInstr &MI) const override;

  bool isBranchOffsetInRange(unsigned BranchOpc,
                             int64_t BrOffset) const override;

  void insertIndirectBranch(MachineBasicBlock &MBB,
                            MachineBasicBlock &NewDestBB,
                            MachineBasicBlock &RestoreBB, const DebugLoc &DL,
                            int64_t BrOffset, RegScavenger *RS) const override;

private:
  const I8051RegisterInfo RI;

protected:
  const I8051Subtarget &STI;
};

} // end namespace llvm

#endif // LLVM_I8051_INSTR_INFO_H
