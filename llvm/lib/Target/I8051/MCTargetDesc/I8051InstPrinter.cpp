//===-- I8051InstPrinter.cpp - Convert I8051 MCInst to assembly syntax --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an I8051 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "I8051InstPrinter.h"

#include "MCTargetDesc/I8051MCTargetDesc.h"

#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

#include <cstring>

#define DEBUG_TYPE "asm-printer"

namespace llvm {

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "I8051GenAsmWriter.inc"

void I8051InstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &O) {
  unsigned Opcode = MI->getOpcode();

  switch (Opcode) {
  default:
    printAnnotation(O, Annot);
    break;
  }
  llvm_unreachable("Not implemented yet!");
}

const char *I8051InstPrinter::getPrettyRegisterName(MCRegister Reg,
                                                  MCRegisterInfo const &MRI) {
  // GCC prints register pairs by just printing the lower register
  // If the register contains a subregister, print it instead
  if (MRI.getNumSubRegIndices() > 0) {
    MCRegister RegLo = MRI.getSubReg(Reg, I8051::sub_lo);
    Reg = (RegLo != I8051::NoRegister) ? RegLo : Reg;
  }

  llvm_unreachable("Not implemented yet!");
  return getRegisterName(Reg);
}


} // end of namespace llvm
