//===-- I8051MCCodeEmitter.cpp - Convert I8051 Code to Machine Code -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the I8051MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "I8051MCCodeEmitter.h"

//#include "MCTargetDesc/I8051MCExpr.h"
#include "MCTargetDesc/I8051MCTargetDesc.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "mccodeemitter"

#define GET_INSTRMAP_INFO
#include "I8051GenInstrInfo.inc"
#undef GET_INSTRMAP_INFO

namespace llvm {

void I8051MCCodeEmitter::encodeInstruction(const MCInst &MI,
                                         SmallVectorImpl<char> &CB,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());

  // Get byte count of instruction
  unsigned Size = Desc.getSize();

  assert(Size > 0 && "Instruction size cannot be zero");

  uint64_t BinaryOpCode = getBinaryCodeForInstr(MI, Fixups, STI);

  for (int64_t i = Size / 2 - 1; i >= 0; --i) {
    uint16_t Word = (BinaryOpCode >> (i * 16)) & 0xFFFF;
    support::endian::write(CB, Word, llvm::endianness::little);
  }
  llvm_unreachable("Not implemented yet!");
}

MCCodeEmitter *createI8051MCCodeEmitter(const MCInstrInfo &MCII,
                                      MCContext &Ctx) {
  return new I8051MCCodeEmitter(MCII, Ctx);
}

#include "I8051GenMCCodeEmitter.inc"

} // end of namespace llvm
