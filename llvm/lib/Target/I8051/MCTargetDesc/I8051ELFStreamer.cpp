#include "I8051ELFStreamer.h"
#include "I8051MCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/TargetParser/SubtargetFeature.h"

namespace llvm {

static unsigned getEFlagsForFeatureSet(const FeatureBitset &Features) {
  unsigned EFlags = 0;

  return EFlags;
}

I8051ELFStreamer::I8051ELFStreamer(MCStreamer &S, const MCSubtargetInfo &STI)
    : I8051TargetStreamer(S) {
  ELFObjectWriter &W = getStreamer().getWriter();
  unsigned EFlags = W.getELFHeaderEFlags();

  EFlags |= getEFlagsForFeatureSet(STI.getFeatureBits());
  EFlags |= ELF::EF_I8051_LINKRELAX_PREPARED;

  W.setELFHeaderEFlags(EFlags);
}

} // end namespace llvm
