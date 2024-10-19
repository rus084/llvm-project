//===-- I8051.h - Top-level interface for I8051 representation ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// I8051 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_I8051_H
#define LLVM_I8051_H

#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class I8051TargetMachine;
class FunctionPass;
class PassRegistry;

/// Contains the I8051 backend.
namespace I8051 {

} // end of namespace I8051

} // end namespace llvm

#endif // LLVM_I8051_H
