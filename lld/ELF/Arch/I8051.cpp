//===- I8051.cpp ------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Since it is a baremetal programming, there's usually no loader to load
// ELF files on I8051s. You are expected to link your program against address
// 0 and pull out a .text section from the result using objcopy, so that you
// can write the linked code to on-chip flash memory. You can do that with
// the following commands:
//
//   ld.lld -Ttext=0 -o foo foo.o
//   objcopy -O binary --only-section=.text foo output.bin
//
// Note that the current I8051 support is very preliminary so you can't
// link any useful program yet, though.
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "Symbols.h"
#include "Target.h"
#include "Thunks.h"
#include "lld/Common/ErrorHandler.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {
class I8051 final : public TargetInfo {
public:
  I8051(Ctx &ctx) : TargetInfo(ctx) { needsThunks = true; }
  uint32_t calcEFlags() const override;
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  bool needsThunk(RelExpr expr, RelType type, const InputFile *file,
                  uint64_t branchAddr, const Symbol &s,
                  int64_t a) const override;
  void relocate(uint8_t *loc, const Relocation &rel,
                uint64_t val) const override;
};
} // namespace

RelExpr I8051::getRelExpr(RelType type, const Symbol &s,
                        const uint8_t *loc) const {
  switch (type) {
  default:
    error(getErrorLoc(ctx, loc) + "unknown relocation (" + Twine(type) +
          ") against symbol " + toString(s));
    return R_NONE;
  }
}

bool I8051::needsThunk(RelExpr expr, RelType type, const InputFile *file,
                     uint64_t branchAddr, const Symbol &s, int64_t a) const {
  llvm_unreachable("todo: needsThunk");
  switch (type) {
  default:
    return false;
  }
}

void I8051::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {
  switch (rel.type) {
  default:
    llvm_unreachable("unknown relocation");
  }
}

void elf::setI8051TargetInfo(Ctx &ctx) { ctx.target.reset(new I8051(ctx)); }

static uint32_t getEFlags(InputFile *file) {
  return cast<ObjFile<ELF32LE>>(file)->getObj().getHeader().e_flags;
}

uint32_t I8051::calcEFlags() const {
  assert(!ctx.objectFiles.empty());

  uint32_t flags = getEFlags(ctx.objectFiles[0]);
  bool hasLinkRelaxFlag = flags & EF_I8051_LINKRELAX_PREPARED;

  for (InputFile *f : ArrayRef(ctx.objectFiles).slice(1)) {
    uint32_t objFlags = getEFlags(f);
    if ((objFlags & EF_I8051_ARCH_MASK) != (flags & EF_I8051_ARCH_MASK))
      error(toString(f) +
            ": cannot link object files with incompatible target ISA");
    if (!(objFlags & EF_I8051_LINKRELAX_PREPARED))
      hasLinkRelaxFlag = false;
  }

  if (!hasLinkRelaxFlag)
    flags &= ~EF_I8051_LINKRELAX_PREPARED;

  return flags;
}
