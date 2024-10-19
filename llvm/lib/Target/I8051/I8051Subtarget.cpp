//===-- I8051Subtarget.cpp - I8051 Subtarget Information ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the I8051 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "I8051Subtarget.h"

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/TargetRegistry.h"

#include "I8051.h"
#include "I8051TargetMachine.h"
#include "MCTargetDesc/I8051MCTargetDesc.h"

#define DEBUG_TYPE "I8051-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "I8051GenSubtargetInfo.inc"

namespace llvm {

I8051Subtarget::I8051Subtarget(const Triple &TT, const std::string &CPU,
                           const std::string &FS, const I8051TargetMachine &TM)
    : I8051GenSubtargetInfo(TT, CPU, /*TuneCPU*/ CPU, FS), 
    InstrInfo(*this), 
    TLInfo(TM, initializeSubtargetDependencies(CPU, FS, TM)) {
  // Parse features string.
  ParseSubtargetFeatures(CPU, /*TuneCPU*/ CPU, FS);
}

I8051Subtarget &
I8051Subtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                              const TargetMachine &TM) {
  // Parse features string.
  ParseSubtargetFeatures(CPU, /*TuneCPU*/ CPU, FS);
  return *this;
}

} // end of namespace llvm
