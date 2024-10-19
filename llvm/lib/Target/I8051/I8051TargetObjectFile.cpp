//===-- I8051TargetObjectFile.cpp - I8051 Object Files ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "I8051TargetObjectFile.h"
#include "I8051TargetMachine.h"

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"

#include "I8051.h"

namespace llvm {
void I8051TargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM) {
  llvm_unreachable("Not implemented yet!");
  Base::Initialize(Ctx, TM);
}

MCSection *I8051TargetObjectFile::SelectSectionForGlobal(
  const GlobalObject *GO, SectionKind Kind, const TargetMachine &TM) const {
  llvm_unreachable("Not implemented yet!");
  // Otherwise, we work the same way as ELF.
  return Base::SelectSectionForGlobal(GO, Kind, TM);
}
} // end of namespace llvm
