add_llvm_component_group(I8051)

set(LLVM_TARGET_DEFINITIONS I8051.td)

tablegen(LLVM I8051GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM I8051GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM I8051GenCallingConv.inc -gen-callingconv)
tablegen(LLVM I8051GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM I8051GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM I8051GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM I8051GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM I8051GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM I8051GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(I8051CommonTableGen)

add_llvm_target(I8051CodeGen
  I8051AsmPrinter.cpp
  I8051FrameLowering.cpp
  I8051InstrInfo.cpp
  I8051ISelLowering.cpp
  I8051RegisterInfo.cpp
  I8051Subtarget.cpp
  I8051TargetMachine.cpp
  I8051TargetObjectFile.cpp

  DEPENDS
  intrinsics_gen

  LINK_COMPONENTS
  I8051Desc
  I8051Info
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  Support
  Target

  ADD_TO_COMPONENT
  I8051
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
