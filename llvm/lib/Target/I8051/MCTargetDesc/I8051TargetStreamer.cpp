//===-- I8051TargetStreamer.cpp - I8051 Target Streamer Methods ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides I8051 specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "I8051TargetStreamer.h"

#include "llvm/MC/MCContext.h"

namespace llvm {

I8051TargetStreamer::I8051TargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

I8051TargetAsmStreamer::I8051TargetAsmStreamer(MCStreamer &S)
    : I8051TargetStreamer(S) {}

} // end namespace llvm
