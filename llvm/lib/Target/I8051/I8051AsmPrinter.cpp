//===-- I8051AsmPrinter.cpp - I8051 LLVM assembly writer ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format I8051 assembly language.
//
//===----------------------------------------------------------------------===//

#include "I8051.h"
//#include "I8051MCInstLower.h"
//#include "I8051Subtarget.h"
#include "I8051TargetMachine.h"
//#include "MCTargetDesc/I8051InstPrinter.h"
//#include "MCTargetDesc/I8051MCExpr.h"
#include "TargetInfo/I8051TargetInfo.h"

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLoweringObjectFile.h"

#define DEBUG_TYPE "I8051-asm-printer"

namespace llvm {

/// An I8051 assembly code printer.
class I8051AsmPrinter : public AsmPrinter {
public:
  I8051AsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), MRI(*TM.getMCRegisterInfo()) {}

  StringRef getPassName() const override { return "I8051 Assembly Printer"; }

  void printOperand(const MachineInstr *MI, unsigned OpNo, raw_ostream &O);

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                       const char *ExtraCode, raw_ostream &O) override;

  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum,
                             const char *ExtraCode, raw_ostream &O) override;

  void emitInstruction(const MachineInstr *MI) override;

  const MCExpr *lowerConstant(const Constant *CV) override;

  void emitXXStructor(const DataLayout &DL, const Constant *CV) override;

  bool doFinalization(Module &M) override;

  void emitStartOfAsmFile(Module &M) override;

private:
  const MCRegisterInfo &MRI;
  bool EmittedStructorSymbolAttrs = false;
};

void I8051AsmPrinter::printOperand(const MachineInstr *MI, unsigned OpNo,
                                 raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(OpNo);

  switch (MO.getType()) {
  default:
    llvm_unreachable("Not implemented yet!");
  }
}

bool I8051AsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
                                    const char *ExtraCode, raw_ostream &O) {
  llvm_unreachable("Not implemented yet!");
  return true;
}

bool I8051AsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                          unsigned OpNum, const char *ExtraCode,
                                          raw_ostream &O) {
  llvm_unreachable("Not implemented yet!");
  return true; // Unknown modifier
}

void I8051AsmPrinter::emitInstruction(const MachineInstr *MI) {
  // todo
  llvm_unreachable("Not implemented yet!");
}

const MCExpr *I8051AsmPrinter::lowerConstant(const Constant *CV) {
  // todo
  llvm_unreachable("Not implemented yet!");
  return AsmPrinter::lowerConstant(CV);
}

void I8051AsmPrinter::emitXXStructor(const DataLayout &DL, const Constant *CV) {
  llvm_unreachable("Not implemented yet!");
  if (!EmittedStructorSymbolAttrs) {
    OutStreamer->emitRawComment(
        " Emitting these undefined symbol references causes us to link the"
        " libgcc code that runs our constructors/destructors");
    OutStreamer->emitRawComment(" This matches GCC's behavior");

    MCSymbol *CtorsSym = OutContext.getOrCreateSymbol("__do_global_ctors");
    OutStreamer->emitSymbolAttribute(CtorsSym, MCSA_Global);

    MCSymbol *DtorsSym = OutContext.getOrCreateSymbol("__do_global_dtors");
    OutStreamer->emitSymbolAttribute(DtorsSym, MCSA_Global);

    EmittedStructorSymbolAttrs = true;
  }

  AsmPrinter::emitXXStructor(DL, CV);
}

bool I8051AsmPrinter::doFinalization(Module &M) {
  // todo
  llvm_unreachable("Not implemented yet!");
  return AsmPrinter::doFinalization(M);
}

void I8051AsmPrinter::emitStartOfAsmFile(Module &M) {
  llvm_unreachable("Not implemented yet!");
}

} // end of namespace llvm

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeI8051AsmPrinter() {
  llvm::RegisterAsmPrinter<llvm::I8051AsmPrinter> X(llvm::getTheI8051Target());
}
