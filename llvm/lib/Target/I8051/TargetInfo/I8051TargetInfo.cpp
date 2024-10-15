//===-- I8051TargetInfo.cpp - I8051 Target Implementation ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/I8051TargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
namespace llvm {
Target &getTheI8051Target() {
  static Target TheI8051Target;
  return TheI8051Target;
}
} // namespace llvm

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeI8051TargetInfo() {
  llvm::RegisterTarget<llvm::Triple::i8051> X(llvm::getTheI8051Target(), "i8051",
                                            "Intel I8051 Microcontroller", "I8051");
}
