add_llvm_component_library(LLVMI8051Desc
  I8051AsmBackend.cpp
  I8051ELFObjectWriter.cpp
  I8051ELFStreamer.cpp
  I8051InstPrinter.cpp
  I8051MCAsmInfo.cpp
  I8051MCCodeEmitter.cpp
  I8051MCTargetDesc.cpp
  I8051TargetStreamer.cpp

  LINK_COMPONENTS
  MC
  I8051Info
  Support
  TargetParser

  ADD_TO_COMPONENT
  I8051
)

add_dependencies(LLVMI8051Desc I8051CommonTableGen)
