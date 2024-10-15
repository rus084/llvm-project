add_llvm_component_library(LLVMI8051Desc
  I8051MCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  I8051Info
  Support
  TargetParser

  ADD_TO_COMPONENT
  I8051
)

add_dependencies(LLVMI8051Desc I8051CommonTableGen)
