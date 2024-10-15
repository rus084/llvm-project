add_llvm_component_group(I8051)

set(LLVM_TARGET_DEFINITIONS I8051.td)

tablegen(LLVM I8051GenRegisterInfo.inc -gen-register-info)

add_public_tablegen_target(I8051CommonTableGen)

add_llvm_target(I8051CodeGen
  I8051TargetMachine.cpp

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
