//===-- I8051InstrInfo.td - I8051 Instruction Formats ----------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// I8051 Instruction Format Definitions.
//
//===----------------------------------------------------------------------===//

// A generic I8051 instruction.
class I8051Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  let Namespace = "I8051";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;

  field bits<32> SoftFail = 0;
}

/// A 8-bit I8051 instruction.
class I8051Inst8<dag outs, dag ins, string asmstr, list<dag> pattern>
    : I8051Inst<outs, ins, asmstr, pattern> {
  field bits<8> Inst;

  let Size = 1;
}

/// A 16-bit I8051 instruction.
class I8051Inst16<dag outs, dag ins, string asmstr, list<dag> pattern>
    : I8051Inst<outs, ins, asmstr, pattern> {
  field bits<16> Inst;

  let Size = 2;
}

/// a 24-bit I8051 instruction.
class I8051Inst24<dag outs, dag ins, string asmstr, list<dag> pattern>
    : I8051Inst<outs, ins, asmstr, pattern> {
  field bits<24> Inst;

  let Size = 3;
}

//===----------------------------------------------------------------------===//
// Special, opcode only instructions: <|opcode|>
//===----------------------------------------------------------------------===//

class F8<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : I8051Inst8<outs, ins, asmstr, pattern> {
  let Inst = opcode;
}
