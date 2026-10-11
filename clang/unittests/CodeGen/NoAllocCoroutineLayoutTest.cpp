//===- NoAllocCoroutineLayoutTest.cpp - Early coroutine layout tests --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TestCompiler.h"
#include "clang/AST/Decl.h"
#include "clang/AST/StmtCXX.h"
#include "clang/Basic/Builtins.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Sema.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Support/Error.h"
#include "gtest/gtest.h"

using namespace clang;

namespace {

// Keep the fixture freestanding: the queried target need not have a C++ sysroot.
constexpr char Program[] = R"cpp(
namespace std {
template<class T = void> struct coroutine_handle;
template<> struct coroutine_handle<void> {
  void *p;
  static coroutine_handle from_address(void *p) { return {p}; }
  void *address() const { return p; }
};
template<class T> struct coroutine_handle {
  void *p;
  static coroutine_handle from_address(void *p) { return {p}; }
  void *address() const { return p; }
  operator coroutine_handle<void>() const { return {p}; }
};
template<class R, class... Args> struct coroutine_traits {
  using promise_type = typename R::promise_type;
};
struct suspend_always {
  bool await_ready() const noexcept { return false; }
  void await_suspend(coroutine_handle<>) const noexcept {}
  void await_resume() const noexcept {}
};
}
void consume(void *);
struct task {
  struct promise_type {
    task get_return_object() { return {}; }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
  };
};
task early(int value) {
  alignas(64) unsigned char data[64];
  data[0] = value;
  co_await std::suspend_always{};
  consume(data);
}
// The consumer must have completed the query before this declaration is parsed.
static_assert(sizeof(int) >= 2);
int after_query() { return std::suspend_always{}.await_ready() ? 0 : 42; }
)cpp";

struct ProbeConsumer final : ASTConsumer {
  const CompilerInstance &CI;
  std::unique_ptr<CodeGenerator> NormalCG;
  unsigned Queries = 0;
  bool SawLaterDeclaration = false;
  unsigned ExpectedQueries;
  bool EmitFinalIR;
  std::unique_ptr<EarlyCoroutineLayout> Retained;

  ProbeConsumer(const CompilerInstance &CI, std::unique_ptr<CodeGenerator> CG,
                unsigned ExpectedQueries, bool EmitFinalIR)
      : CI(CI), NormalCG(std::move(CG)), ExpectedQueries(ExpectedQueries),
        EmitFinalIR(EmitFinalIR) {}

  void Initialize(ASTContext &Ctx) override { NormalCG->Initialize(Ctx); }

  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    for (Decl *D : DG) {
      auto *F = dyn_cast<FunctionDecl>(D);
      if (F && F->getName() == "consume") {
        auto Rejected = computeEarlyCoroutineLayout(CI, *F);
        EXPECT_FALSE(bool(Rejected));
        if (!Rejected)
          llvm::consumeError(Rejected.takeError());
      }
      if (!F || !F->doesThisDeclarationHaveABody())
        continue;
      if (F->getName() == "early") {
        EXPECT_FALSE(SawLaterDeclaration);
        EXPECT_TRUE(isa<CoroutineBodyStmt>(F->getBody()));
        for (unsigned I = 0; I != ExpectedQueries; ++I) {
          auto Result = computeEarlyCoroutineLayout(CI, *F);
          if (!Result) {
            ADD_FAILURE() << llvm::toString(Result.takeError());
            continue;
          }
          ++Queries;
          EXPECT_GT((*Result)->SizeBytes, 0u);
          EXPECT_GE((*Result)->AlignBytes, 64u);
          EXPECT_EQ((*Result)->SizeBytes % (*Result)->AlignBytes, 0u);
          EXPECT_FALSE(llvm::verifyModule(*(*Result)->LoweredIR, &llvm::errs()));
          unsigned Definitions = 0;
          unsigned FrameUsers = 0;
          for (const auto &Fn : *(*Result)->LoweredIR) {
            Definitions += !Fn.isDeclaration();
            if (Fn.getName() == "_Z5earlyi.resume" ||
                Fn.getName() == "_Z5earlyi.destroy") {
              ++FrameUsers;
              // LLVM's switch ABI records the actual frame extent/alignment
              // on the outlined functions' frame pointer parameters.
              EXPECT_EQ(Fn.getParamDereferenceableBytes(0),
                        (*Result)->SizeBytes);
              EXPECT_TRUE(Fn.getParamAlign(0).has_value());
              if (auto Alignment = Fn.getParamAlign(0))
                EXPECT_EQ(Alignment->value(), (*Result)->AlignBytes);
            }
          }
          EXPECT_EQ(FrameUsers, 2u);
          EXPECT_GE(Definitions, 3u);
          if (Retained) {
            EXPECT_EQ(Retained->SizeBytes, (*Result)->SizeBytes);
            EXPECT_EQ(Retained->AlignBytes, (*Result)->AlignBytes);
          }
          Retained = std::move(*Result);
        }
        // Final emission uses the retained definition, not a second layout run.
        return true;
      }
      if (F->getName() == "after_query") {
        SawLaterDeclaration = true;
        EXPECT_EQ(Queries, ExpectedQueries);
      }
    }
    return !EmitFinalIR || NormalCG->HandleTopLevelDecl(DG);
  }

  void HandleTranslationUnit(ASTContext &Ctx) override {
    if (!EmitFinalIR)
      return;
    NormalCG->HandleTranslationUnit(Ctx);
    ASSERT_NE(NormalCG->GetModule(), nullptr);
    ASSERT_NE(Retained, nullptr);
    // Serialized IR crosses LLVMContexts without dangling cached LLVM values.
    llvm::SmallVector<char, 0> Buffer;
    llvm::raw_svector_ostream OS(Buffer);
    llvm::WriteBitcodeToFile(*Retained->LoweredIR, OS);
    auto Imported = llvm::parseBitcodeFile(
        llvm::MemoryBufferRef(llvm::StringRef(Buffer.data(), Buffer.size()),
                              "early-layout"),
        NormalCG->GetModule()->getContext());
    if (!Imported) {
      ADD_FAILURE() << llvm::toString(Imported.takeError());
      return;
    }
    EXPECT_FALSE(llvm::Linker::linkModules(*NormalCG->GetModule(),
                                         std::move(*Imported)));
    EXPECT_FALSE(llvm::verifyModule(*NormalCG->GetModule(), &llvm::errs()));
    unsigned Ramps = 0;
    for (const auto &Fn : *NormalCG->GetModule())
      Ramps += !Fn.isDeclaration() && Fn.getName() == "_Z5earlyi";
    EXPECT_EQ(Ramps, 1u);
  }
};

void runProbe(llvm::StringRef Target = {}, unsigned ExpectedQueries = 1,
              bool EmitFinalIR = true) {
  LangOptions LO;
  LO.CPlusPlus = LO.CPlusPlus11 = LO.CPlusPlus14 = LO.CPlusPlus17 =
      LO.CPlusPlus20 = LO.Coroutines = 1;
  LO.Bool = 1;
  llvm::TestCompiler Compiler(LO, CodeGenOptions(), Target);
  auto &PP = Compiler.compiler.getPreprocessor();
  PP.getBuiltinInfo().initializeBuiltins(PP.getIdentifierTable(), LO);
  auto Consumer = std::make_unique<ProbeConsumer>(Compiler.compiler,
      std::move(Compiler.CG), ExpectedQueries, EmitFinalIR);
  ProbeConsumer *Probe = Consumer.get();
  Compiler.init(Program, std::move(Consumer));
  Compiler.compiler.getDiagnosticClient().BeginSourceFile(
      LO, &Compiler.compiler.getPreprocessor());
  ParseAST(Compiler.compiler.getSema(), false, false);
  Compiler.compiler.getDiagnosticClient().EndSourceFile();
  EXPECT_FALSE(Compiler.compiler.getDiagnostics().hasErrorOccurred());
  EXPECT_EQ(Probe->Queries, ExpectedQueries);
  EXPECT_TRUE(Probe->SawLaterDeclaration);
}

TEST(NoAllocCoroutineLayout, EarlyQuery) { runProbe(); }
TEST(NoAllocCoroutineLayout, RepeatedQuery) { runProbe({}, 2); }
TEST(NoAllocCoroutineLayout, ThumbLayout) {
  runProbe("thumbv6m-unknown-none-eabi");
}
TEST(NoAllocCoroutineLayout, SyntaxOnly) { runProbe({}, 1, false); }

} // namespace
