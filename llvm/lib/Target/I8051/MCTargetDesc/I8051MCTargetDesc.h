//===-- I8051MCTargetDesc.h - I8051 Target Descriptions -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides I8051 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_I8051_MCTARGET_DESC_H
#define LLVM_I8051_MCTARGET_DESC_H

#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {

class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class Target;

MCInstrInfo *createI8051MCInstrInfo();

/// Creates a machine code emitter for AVR.
MCCodeEmitter *createI8051MCCodeEmitter(const MCInstrInfo &MCII,
                                      MCContext &Ctx);

/// Creates an assembly backend for AVR.
MCAsmBackend *createI8051AsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const llvm::MCTargetOptions &TO);

/// Creates an ELF object writer for AVR.
std::unique_ptr<MCObjectTargetWriter> createI8051ELFObjectWriter(uint8_t OSABI);

} // end namespace llvm

#define GET_REGINFO_ENUM
#include "I8051GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "I8051GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "I8051GenSubtargetInfo.inc"

#endif // LLVM_I8051_MCTARGET_DESC_H
