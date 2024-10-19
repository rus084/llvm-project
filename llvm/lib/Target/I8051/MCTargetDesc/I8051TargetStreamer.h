//===-- I8051TargetStreamer.h - I8051 Target Streamer --------------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_I8051_TARGET_STREAMER_H
#define LLVM_I8051_TARGET_STREAMER_H

#include "llvm/MC/MCELFStreamer.h"

namespace llvm {
class MCStreamer;

/// A generic I8051 target output stream.
class I8051TargetStreamer : public MCTargetStreamer {
public:
  explicit I8051TargetStreamer(MCStreamer &S);
};

/// A target streamer for textual I8051 assembly code.
class I8051TargetAsmStreamer : public I8051TargetStreamer {
public:
  explicit I8051TargetAsmStreamer(MCStreamer &S);
};

} // end namespace llvm

#endif // LLVM_I8051_TARGET_STREAMER_H
