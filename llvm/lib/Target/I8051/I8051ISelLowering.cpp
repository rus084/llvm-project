//===-- I8051ISelLowering.cpp - I8051 DAG Lowering Implementation -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that I8051 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "I8051ISelLowering.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"

#include "I8051.h"
//#include "I8051MachineFunctionInfo.h"
#include "I8051Subtarget.h"
#include "I8051TargetMachine.h"
#include "MCTargetDesc/I8051MCTargetDesc.h"

namespace llvm {

I8051TargetLowering::I8051TargetLowering(const I8051TargetMachine &TM,
                                     const I8051Subtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {
}

const char *I8051TargetLowering::getTargetNodeName(unsigned Opcode) const {
  llvm_unreachable("Not implemented yet!");
  switch (Opcode) {
  default:
    return nullptr;
  }
}

EVT I8051TargetLowering::getSetCCResultType(const DataLayout &DL, LLVMContext &,
                                          EVT VT) const {
  llvm_unreachable("Not implemented yet!");
  assert(!VT.isVector() && "No I8051 SetCC type for vectors!");
  return MVT::i8;
}

/// IntCCToI8051CC - Convert a DAG integer condition code to an I8051 CC.
static I8051CC::CondCodes intCCToI8051CC(ISD::CondCode CC) {
  llvm_unreachable("Not implemented yet!");
  switch (CC) {
  default:
    llvm_unreachable("Unknown condition code!");
  }
}

SDValue I8051TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    llvm_unreachable("Don't know how to custom lower this!");
  }

  return SDValue();
}

/// Replace a node with an illegal result type
/// with a new node built out of custom code.
void I8051TargetLowering::ReplaceNodeResults(SDNode *N,
                                           SmallVectorImpl<SDValue> &Results,
                                           SelectionDAG &DAG) const {
  llvm_unreachable("Not implemented yet!");
  SDLoc DL(N);

  switch (N->getOpcode()) {
  default: {
    SDValue Res = LowerOperation(SDValue(N, 0), DAG);

    for (unsigned I = 0, E = Res->getNumValues(); I != E; ++I)
      Results.push_back(Res.getValue(I));

    break;
  }
  }
}

/// Return true if the addressing mode represented
/// by AM is legal for this target, for a load/store of the specified type.
bool I8051TargetLowering::isLegalAddressingMode(const DataLayout &DL,
                                              const AddrMode &AM, Type *Ty,
                                              unsigned AS,
                                              Instruction *I) const {
  llvm_unreachable("Not implemented yet!");
  return false;
}

/// Returns true by value, base pointer and
/// offset pointer and addressing mode by reference if the node's address
/// can be legally represented as pre-indexed load / store address.
bool I8051TargetLowering::getPreIndexedAddressParts(SDNode *N, SDValue &Base,
                                                  SDValue &Offset,
                                                  ISD::MemIndexedMode &AM,
                                                  SelectionDAG &DAG) const {
  llvm_unreachable("Not implemented yet!");
  return false;
}

/// Returns true by value, base pointer and
/// offset pointer and addressing mode by reference if this node can be
/// combined with a load / store to form a post-indexed load / store.
bool I8051TargetLowering::getPostIndexedAddressParts(SDNode *N, SDNode *Op,
                                                   SDValue &Base,
                                                   SDValue &Offset,
                                                   ISD::MemIndexedMode &AM,
                                                   SelectionDAG &DAG) const {
  llvm_unreachable("Not implemented yet!");
  return false;
}

bool I8051TargetLowering::isOffsetFoldingLegal(
    const GlobalAddressSDNode *GA) const {
  llvm_unreachable("Not implemented yet!");
  return true;
}

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "I8051GenCallingConv.inc"


//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Custom Inserters
//===----------------------------------------------------------------------===//

MachineBasicBlock *
I8051TargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                               MachineBasicBlock *MBB) const {
  llvm_unreachable("EmitInstrWithCustomInserter");
  return MBB;
}

//===----------------------------------------------------------------------===//
//  Inline Asm Support
//===----------------------------------------------------------------------===//

I8051TargetLowering::ConstraintWeight
I8051TargetLowering::getSingleConstraintMatchWeight(
    AsmOperandInfo &info, const char *constraint) const {
  ConstraintWeight weight = CW_Invalid;
  llvm_unreachable("Not implemented yet!");
  return weight;
}

std::pair<unsigned, const TargetRegisterClass *>
I8051TargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                StringRef Constraint,
                                                MVT VT) const {
  llvm_unreachable("Not implemented yet!");
  return TargetLowering::getRegForInlineAsmConstraint(
      Subtarget.getRegisterInfo(), Constraint, VT);
}

I8051TargetLowering::ConstraintType
I8051TargetLowering::getConstraintType(StringRef Constraint) const {
  if (Constraint.size() == 1) {
    // See http://www.nongnu.org/I8051-libc/user-manual/inline_asm.html
    switch (Constraint[0]) {
    default:
      break;
    }
  }

  llvm_unreachable("Not implemented yet!");
  return TargetLowering::getConstraintType(Constraint);
}

InlineAsm::ConstraintCode
I8051TargetLowering::getInlineAsmMemConstraint(StringRef ConstraintCode) const {
  llvm_unreachable("Not implemented yet!");
  return TargetLowering::getInlineAsmMemConstraint(ConstraintCode);
}

void I8051TargetLowering::LowerAsmOperandForConstraint(SDValue Op,
                                                     StringRef Constraint,
                                                     std::vector<SDValue> &Ops,
                                                     SelectionDAG &DAG) const {
  llvm_unreachable("Not implemented yet!");
  return TargetLowering::LowerAsmOperandForConstraint(Op, Constraint, Ops, DAG);
}

Register I8051TargetLowering::getRegisterByName(const char *RegName, LLT VT,
                                                const MachineFunction &MF) const {
  report_fatal_error(
      Twine("Invalid register name \"" + StringRef(RegName) + "\"."));
}

} // end of namespace llvm
