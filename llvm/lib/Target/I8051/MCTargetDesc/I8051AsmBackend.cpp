//===-- I8051AsmBackend.cpp - I8051 Asm Backend  ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the I8051AsmBackend class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/I8051AsmBackend.h"
//#include "MCTargetDesc/I8051FixupKinds.h"
#include "MCTargetDesc/I8051MCTargetDesc.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

// Prepare value for the target space for it
void I8051AsmBackend::adjustFixupValue(const MCFixup &Fixup,
                                     const MCValue &Target, uint64_t &Value,
                                     MCContext *Ctx) const {
  // The size of the fixup in bits.
  uint64_t Size = I8051AsmBackend::getFixupKindInfo(Fixup.getKind()).TargetSize;

  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("unhandled fixup");
  }
}

std::unique_ptr<MCObjectTargetWriter>
I8051AsmBackend::createObjectTargetWriter() const {
  return createI8051ELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType));
}

void I8051AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                               const MCValue &Target,
                               MutableArrayRef<char> Data, uint64_t Value,
                               bool IsResolved,
                               const MCSubtargetInfo *STI) const {
  llvm_unreachable("unhandled fixup");
}

std::optional<MCFixupKind> I8051AsmBackend::getFixupKind(StringRef Name) const {
  llvm_unreachable("unhandled fixup");
  return std::nullopt;
}

MCFixupKindInfo const &I8051AsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  llvm_unreachable("unhandled fixup");
}

bool I8051AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                 const MCSubtargetInfo *STI) const {
  OS.write_zeros(Count);
  return true;
}

bool I8051AsmBackend::shouldForceRelocation(const MCAssembler &Asm,
                                          const MCFixup &Fixup,
                                          const MCValue &Target,
                                          const MCSubtargetInfo *STI) {
  llvm_unreachable("unhandled fixup");
}

MCAsmBackend *createI8051AsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const llvm::MCTargetOptions &TO) {
  return new I8051AsmBackend(STI.getTargetTriple().getOS());
}

} // end of namespace llvm
