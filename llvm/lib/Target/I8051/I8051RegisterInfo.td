//===- I8051RegisterInfo.td - Describe the I8051 Register Info ----*- tblgen -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Declaration that describes the I8051 register file
//===----------------------------------------------------------------------===//

class I8051Reg<string name, bits<16> num = 0, list<Register> subregs = []>
  : RegisterWithSubRegs<name, subregs> {
  field bits<16> Num = num;

  let HWEncoding = num;
  let Namespace = "I8051";
  let SubRegs = subregs;
}

// Subregister indices.
let Namespace = "I8051" in {
  def sub_lo : SubRegIndex<8>;
  def sub_hi : SubRegIndex<8, 8>;
}

let Namespace = "I8051" in { def ptr : RegAltNameIndex; }

//===----------------------------------------------------------------------===//
//  8-bit general purpose registers
//===----------------------------------------------------------------------===//

// 8 bit registers
def A : I8051Reg<"a", 7>, DwarfRegNum<[0]>;
def B : I8051Reg<"b", 8>, DwarfRegNum<[1]>;
def R0 : I8051Reg<"r0", 0>, DwarfRegNum<[2]>;
def R1 : I8051Reg<"r1", 1>, DwarfRegNum<[3]>;
def R2 : I8051Reg<"r2", 2>, DwarfRegNum<[4]>;
def R3 : I8051Reg<"r3", 3>, DwarfRegNum<[5]>;
def R4 : I8051Reg<"r4", 4>, DwarfRegNum<[6]>;
def R5 : I8051Reg<"r5", 5>, DwarfRegNum<[7]>;
def R6 : I8051Reg<"r6", 6>, DwarfRegNum<[8]>;
def R7 : I8051Reg<"r7", 7>, DwarfRegNum<[9]>;
def SP : I8051Reg<"sp">, DwarfRegNum<[10]>;
def DPH : I8051Reg<"dph", 9>, DwarfRegNum<[11]>;
def DPL : I8051Reg<"dpl", 9>, DwarfRegNum<[12]>;

let SubRegIndices = [sub_lo, sub_hi], CoveredBySubRegs = 1 in {
  def DPTR : I8051Reg<"dptr", 0, [DPH, DPL]>, DwarfRegNum<[13]>;
}

def PC : I8051Reg<"pc">;

class I8051Reg8Class<dag reglist>
  : RegisterClass<"I8051", [i8], 8, reglist> {
}

class I8051Reg16Class<dag reglist>
  : RegisterClass<"I8051", [i16], 8, reglist> {
}

def GR8  : I8051Reg8Class<(add A, B, R0, R1, R2, R3, R4, R5, R6, R7, SP, DPH, DPL)>;
def GR16 : I8051Reg16Class<(add DPTR, PC)>;
