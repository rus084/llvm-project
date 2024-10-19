//===-- I8051InstrInfo.td - I8051 Instruction defs -------------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the I8051 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "I8051InstrFormats.td"

// NOP
// No-operation instruction
// ---------
// <|0000|0000>
def NOP : F8<0b00000000, (outs), (ins), "nop", []>;
