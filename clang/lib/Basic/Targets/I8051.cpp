//===--- I8051.cpp - Implement I8051 target feature support -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements I8051 TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "I8051.h"
#include "clang/Basic/MacroBuilder.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;


void I8051TargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("I8051");
  Builder.defineMacro("__I8051");
  Builder.defineMacro("__I8051__");
  Builder.defineMacro("__I8051_ARCH__", Arch);

  //Builder.defineMacro("__idata", "__attribute__((__address_space__(1)))");
}
