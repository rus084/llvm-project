//===-- I8051MCTargetDesc.cpp - I8051 Target Descriptions ---------------------===//
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

#include "I8051ELFStreamer.h"
#include "I8051InstPrinter.h"
#include "I8051MCTargetDesc.h"
#include "I8051MCAsmInfo.h"
#include "I8051TargetStreamer.h"

#include "TargetInfo/I8051TargetInfo.h"

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "I8051GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "I8051GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "I8051GenRegisterInfo.inc"

using namespace llvm;

MCInstrInfo *llvm::createI8051MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitI8051MCInstrInfo(X);

  return X;
}

static MCRegisterInfo *createI8051MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitI8051MCRegisterInfo(X, 0);

  return X;
}

static MCSubtargetInfo *createI8051MCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  return createI8051MCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCInstPrinter *createI8051MCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  if (SyntaxVariant == 0) {
    return new I8051InstPrinter(MAI, MII, MRI);
  }

  return nullptr;
}

static MCStreamer *createMCStreamer(const Triple &T, MCContext &Context,
                                    std::unique_ptr<MCAsmBackend> &&MAB,
                                    std::unique_ptr<MCObjectWriter> &&OW,
                                    std::unique_ptr<MCCodeEmitter> &&Emitter) {
  return createELFStreamer(Context, std::move(MAB), std::move(OW),
                           std::move(Emitter));
}

static MCTargetStreamer *
createI8051ObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  return new I8051ELFStreamer(S, STI);
}

static MCTargetStreamer *createMCAsmTargetStreamer(MCStreamer &S,
                                                   formatted_raw_ostream &OS,
                                                   MCInstPrinter *InstPrint) {
  return new I8051TargetAsmStreamer(S);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeI8051TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfo<I8051MCAsmInfo> X(getTheI8051Target());

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(getTheI8051Target(), createI8051MCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(getTheI8051Target(), createI8051MCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(getTheI8051Target(),
                                          createI8051MCSubtargetInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(getTheI8051Target(),
                                        createI8051MCInstPrinter);

  // Register the MC Code Emitter
  TargetRegistry::RegisterMCCodeEmitter(getTheI8051Target(),
                                        createI8051MCCodeEmitter);

  // Register the obj streamer
  TargetRegistry::RegisterELFStreamer(getTheI8051Target(), createMCStreamer);

  // Register the obj target streamer.
  TargetRegistry::RegisterObjectTargetStreamer(getTheI8051Target(),
                                               createI8051ObjectTargetStreamer);

  // Register the asm target streamer.
  TargetRegistry::RegisterAsmTargetStreamer(getTheI8051Target(),
                                            createMCAsmTargetStreamer);

  // Register the asm backend (as little endian).
  TargetRegistry::RegisterMCAsmBackend(getTheI8051Target(), createI8051AsmBackend);
}
