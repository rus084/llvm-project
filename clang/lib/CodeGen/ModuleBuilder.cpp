//===--- ModuleBuilder.cpp - Emit LLVM Code from ASTs ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This builds an AST and converts it to LLVM Code.
//
//===----------------------------------------------------------------------===//

#include "clang/CodeGen/ModuleBuilder.h"
#include "CGCXXABI.h"
#include "CGDebugInfo.h"
#include "CodeGenModule.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/Expr.h"
#include "clang/AST/StmtCXX.h"
#include "clang/Basic/CodeGenOptions.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Transforms/Coroutines/CoroCleanup.h"
#include "llvm/Transforms/Coroutines/CoroEarly.h"
#include "llvm/Transforms/Coroutines/CoroSplit.h"
#include <memory>

using namespace clang;
using namespace CodeGen;

EarlyCoroutineLayout::EarlyCoroutineLayout() = default;
EarlyCoroutineLayout::~EarlyCoroutineLayout() = default;

llvm::Expected<std::unique_ptr<EarlyCoroutineLayout>>
clang::computeEarlyCoroutineLayout(const CompilerInstance &CI,
                                  const FunctionDecl &F) {
  if (!F.doesThisDeclarationHaveABody() ||
      !isa<CoroutineBodyStmt>(F.getBody()))
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "a completed coroutine definition is required");

  auto Result = std::make_unique<EarlyCoroutineLayout>();
  Result->Context = std::make_unique<llvm::LLVMContext>();
  auto CG = CreateLLVMCodeGen(CI, "early-coroutine-layout", *Result->Context);
  CG->Initialize(CI.getASTContext());
  std::string Name = CG->GetMangledName(GlobalDecl(&F)).str();
  CG->HandleTopLevelDecl(DeclGroupRef(const_cast<FunctionDecl *>(&F)));
  CG->GetAddrOfGlobal(GlobalDecl(&F), /*isForDefinition=*/true);
  CG->HandleTranslationUnit(CI.getASTContext());
  Result->LoweredIR = CG->ReleaseModule();
  CG.reset();
  if (!Result->LoweredIR)
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "early coroutine IR generation failed");

  // Capture the layout from the same ABI implementation which outlines the
  // retained resume/destroy functions. No independent estimate or second
  // final-emission layout calculation is involved.
  class LayoutABI final : public llvm::coro::SwitchABI {
    EarlyCoroutineLayout &Result;

  public:
    LayoutABI(llvm::Function &F, llvm::coro::Shape &Shape,
              EarlyCoroutineLayout &Result)
        : SwitchABI(F, Shape, llvm::coro::isTriviallyMaterializable),
          Result(Result) {}

    void buildCoroutineFrame(bool OptimizeFrame) override {
      SwitchABI::buildCoroutineFrame(/*OptimizeFrame=*/false);
      Result.SizeBytes = Shape.FrameSize;
      Result.AlignBytes = Shape.FrameAlign.value();
    }
  };

  llvm::CoroSplitPass Split(/*OptimizeFrame=*/false);
  auto DefaultABI = Split.CreateAndInitABI;
  Split.CreateAndInitABI =
      [&](llvm::Function &Fn, llvm::coro::Shape &Shape)
          -> std::unique_ptr<llvm::coro::BaseABI> {
    if (Fn.getName() != Name)
      return DefaultABI(Fn, Shape);
    auto ABI = std::make_unique<LayoutABI>(Fn, Shape, *Result);
    ABI->init();
    return ABI;
  };

  llvm::LoopAnalysisManager LAM;
  llvm::FunctionAnalysisManager FAM;
  llvm::CGSCCAnalysisManager CGAM;
  llvm::ModuleAnalysisManager MAM;
  llvm::PassBuilder PB;
  PB.registerLoopAnalyses(LAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerModuleAnalyses(MAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
  llvm::ModulePassManager MPM;
  MPM.addPass(llvm::CoroEarlyPass());
  MPM.addPass(llvm::createModuleToPostOrderCGSCCPassAdaptor(std::move(Split)));
  MPM.addPass(llvm::CoroCleanupPass());
  MPM.run(*Result->LoweredIR, MAM);

  if (!Result->SizeBytes || !Result->AlignBytes)
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "coroutine splitter did not produce a layout");
  std::string Errors;
  llvm::raw_string_ostream OS(Errors);
  if (llvm::verifyModule(*Result->LoweredIR, &OS))
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "invalid early coroutine IR: %s",
                                   Errors.c_str());
  return Result;
}

namespace {
  class CodeGeneratorImpl final : public CodeGenerator {
    DiagnosticsEngine &Diags;
    ASTContext *Ctx;
    IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS; // Only used for debug info.
    const HeaderSearchOptions &HeaderSearchOpts; // Only used for debug info.
    const PreprocessorOptions &PreprocessorOpts; // Only used for debug info.
    const CodeGenOptions &CodeGenOpts;

    unsigned HandlingTopLevelDecls;

    /// Use this when emitting decls to block re-entrant decl emission. It will
    /// emit all deferred decls on scope exit. Set EmitDeferred to false if decl
    /// emission must be deferred longer, like at the end of a tag definition.
    struct HandlingTopLevelDeclRAII {
      CodeGeneratorImpl &Self;
      bool EmitDeferred;
      HandlingTopLevelDeclRAII(CodeGeneratorImpl &Self,
                               bool EmitDeferred = true)
          : Self(Self), EmitDeferred(EmitDeferred) {
        ++Self.HandlingTopLevelDecls;
      }
      ~HandlingTopLevelDeclRAII() {
        unsigned Level = --Self.HandlingTopLevelDecls;
        if (Level == 0 && EmitDeferred)
          Self.EmitDeferredDecls();
      }
    };

    CoverageSourceInfo *CoverageInfo;
    std::unique_ptr<llvm::Module> M;
    std::unique_ptr<CodeGen::CodeGenModule> Builder;
    SmallVector<FunctionDecl *, 8> DeferredInlineMemberFuncDefs;

    static llvm::StringRef ExpandModuleName(llvm::StringRef ModuleName,
                                            const CodeGenOptions &CGO) {
      if (ModuleName == "-" && !CGO.MainFileName.empty())
        return CGO.MainFileName;
      return ModuleName;
    }

  public:
    CodeGeneratorImpl(DiagnosticsEngine &diags, llvm::StringRef ModuleName,
                      IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS,
                      const HeaderSearchOptions &HSO,
                      const PreprocessorOptions &PPO, const CodeGenOptions &CGO,
                      llvm::LLVMContext &C,
                      CoverageSourceInfo *CoverageInfo = nullptr)
        : Diags(diags), Ctx(nullptr), FS(std::move(FS)), HeaderSearchOpts(HSO),
          PreprocessorOpts(PPO), CodeGenOpts(CGO), HandlingTopLevelDecls(0),
          CoverageInfo(CoverageInfo),
          M(new llvm::Module(ExpandModuleName(ModuleName, CGO), C)) {
      C.setDiscardValueNames(CGO.DiscardValueNames);
    }

    ~CodeGeneratorImpl() override {
      // There should normally not be any leftover inline method definitions.
      assert(DeferredInlineMemberFuncDefs.empty() ||
             Diags.hasErrorOccurred());
    }

    CodeGenModule &CGM() {
      return *Builder;
    }

    llvm::Module *GetModule() {
      return M.get();
    }

    CGDebugInfo *getCGDebugInfo() {
      return Builder->getModuleDebugInfo();
    }

    std::unique_ptr<llvm::Module> ReleaseModule() {
      return std::exchange(M, nullptr);
    }

    const Decl *GetDeclForMangledName(StringRef MangledName) {
      GlobalDecl Result;
      if (!Builder->lookupRepresentativeDecl(MangledName, Result))
        return nullptr;
      const Decl *D = Result.getCanonicalDecl().getDecl();
      if (auto FD = dyn_cast<FunctionDecl>(D)) {
        if (FD->hasBody(FD))
          return FD;
      } else if (auto TD = dyn_cast<TagDecl>(D)) {
        if (auto Def = TD->getDefinition())
          return Def;
      }
      return D;
    }

    llvm::StringRef GetMangledName(GlobalDecl GD) {
      return Builder->getMangledName(GD);
    }

    llvm::Constant *GetAddrOfGlobal(GlobalDecl global, bool isForDefinition) {
      return Builder->GetAddrOfGlobal(global, ForDefinition_t(isForDefinition));
    }

    llvm::Constant *GetAddrOfVTable(BaseSubobject subobject,
                                    const CXXRecordDecl *decl) {
      return Builder->getCXXABI().getVTableAddressPoint(subobject, decl);
    }

    llvm::Module *StartModule(llvm::StringRef ModuleName,
                              llvm::LLVMContext &C) {
      assert(!M && "Replacing existing Module?");
      M.reset(new llvm::Module(ExpandModuleName(ModuleName, CodeGenOpts), C));

      IRGenFinished = false;

      std::unique_ptr<CodeGenModule> OldBuilder = std::move(Builder);

      assert(Ctx && "must call Initialize() before calling StartModule()");
      Initialize(*Ctx);

      if (OldBuilder)
        OldBuilder->moveLazyEmissionStates(Builder.get());

      return M.get();
    }

    void Initialize(ASTContext &Context) override {
      Ctx = &Context;

      M->setTargetTriple(Ctx->getTargetInfo().getTriple());
      M->setDataLayout(Ctx->getTargetInfo().getDataLayoutString());
      const auto &SDKVersion = Ctx->getTargetInfo().getSDKVersion();
      if (!SDKVersion.empty())
        M->setSDKVersion(SDKVersion);
      if (const auto *TVT = Ctx->getTargetInfo().getDarwinTargetVariantTriple())
        M->setDarwinTargetVariantTriple(TVT->getTriple());
      if (auto TVSDKVersion =
              Ctx->getTargetInfo().getDarwinTargetVariantSDKVersion())
        M->setDarwinTargetVariantSDKVersion(*TVSDKVersion);
      Builder.reset(new CodeGen::CodeGenModule(Context, FS, HeaderSearchOpts,
                                               PreprocessorOpts, CodeGenOpts,
                                               *M, Diags, CoverageInfo));

      for (auto &&Lib : CodeGenOpts.DependentLibraries)
        Builder->AddDependentLib(Lib);
      for (auto &&Opt : CodeGenOpts.LinkerOptions)
        Builder->AppendLinkerOptions(Opt);
    }

    void HandleCXXStaticMemberVarInstantiation(VarDecl *VD) override {
      if (Diags.hasErrorOccurred())
        return;

      Builder->HandleCXXStaticMemberVarInstantiation(VD);
    }

    bool HandleTopLevelDecl(DeclGroupRef DG) override {
      // Ignore interesting decls from the AST reader after IRGen is finished.
      if (IRGenFinished)
        return true; // We can't CodeGen more but pass to other consumers.

      // FIXME: Why not return false and abort parsing?
      if (Diags.hasUnrecoverableErrorOccurred())
        return true;

      HandlingTopLevelDeclRAII HandlingDecl(*this);

      // Make sure to emit all elements of a Decl.
      for (auto &I : DG)
        Builder->EmitTopLevelDecl(I);

      return true;
    }

    void EmitDeferredDecls() {
      if (DeferredInlineMemberFuncDefs.empty())
        return;

      // Emit any deferred inline method definitions. Note that more deferred
      // methods may be added during this loop, since ASTConsumer callbacks
      // can be invoked if AST inspection results in declarations being added.
      HandlingTopLevelDeclRAII HandlingDecl(*this);
      for (unsigned I = 0; I != DeferredInlineMemberFuncDefs.size(); ++I)
        Builder->EmitTopLevelDecl(DeferredInlineMemberFuncDefs[I]);
      DeferredInlineMemberFuncDefs.clear();
    }

    void HandleInlineFunctionDefinition(FunctionDecl *D) override {
      if (Diags.hasUnrecoverableErrorOccurred())
        return;

      assert(D->doesThisDeclarationHaveABody());

      // We may want to emit this definition. However, that decision might be
      // based on computing the linkage, and we have to defer that in case we
      // are inside of something that will change the method's final linkage,
      // e.g.
      //   typedef struct {
      //     void bar();
      //     void foo() { bar(); }
      //   } A;
      DeferredInlineMemberFuncDefs.push_back(D);

      // Provide some coverage mapping even for methods that aren't emitted.
      // Don't do this for templated classes though, as they may not be
      // instantiable. Also skip consteval methods as they are never emitted.
      if (!D->getLexicalDeclContext()->isDependentContext() &&
          !D->getAsFunction()->isImmediateFunction())
        Builder->AddDeferredUnusedCoverageMapping(D);
    }

    /// HandleTagDeclDefinition - This callback is invoked each time a TagDecl
    /// to (e.g. struct, union, enum, class) is completed. This allows the
    /// client hack on the type, which can occur at any point in the file
    /// (because these can be defined in declspecs).
    void HandleTagDeclDefinition(TagDecl *D) override {
      if (Diags.hasUnrecoverableErrorOccurred())
        return;

      // Don't allow re-entrant calls to CodeGen triggered by PCH
      // deserialization to emit deferred decls.
      HandlingTopLevelDeclRAII HandlingDecl(*this, /*EmitDeferred=*/false);

      Builder->UpdateCompletedType(D);

      // For MSVC compatibility, treat declarations of static data members with
      // inline initializers as definitions.
      assert(Ctx && "Initialize() not called");
      if (Ctx->getTargetInfo().getCXXABI().isMicrosoft()) {
        for (Decl *Member : D->decls()) {
          if (VarDecl *VD = dyn_cast<VarDecl>(Member)) {
            if (Ctx->isMSStaticDataMemberInlineDefinition(VD) &&
                Ctx->DeclMustBeEmitted(VD)) {
              Builder->EmitGlobal(VD);
            }
          }
        }
      }

      // Emit dllexport inherited constructors. These are synthesized during
      // Sema (in checkClassLevelDLLAttribute) but have no written definition,
      // so they must be emitted now while visiting the class definition.
      if (auto *RD = dyn_cast<CXXRecordDecl>(D);
          RD && RD->hasAttr<DLLExportAttr>()) {
        for (Decl *Member : RD->decls()) {
          if (auto *CD = dyn_cast<CXXConstructorDecl>(Member)) {
            if (CD->getInheritedConstructor() && CD->hasAttr<DLLExportAttr>() &&
                !CD->isDeleted())
              Builder->EmitTopLevelDecl(CD);
          }
        }
      }

      // For OpenMP emit declare reduction functions, if required.
      if (Ctx->getLangOpts().OpenMP) {
        for (Decl *Member : D->decls()) {
          if (auto *DRD = dyn_cast<OMPDeclareReductionDecl>(Member)) {
            if (Ctx->DeclMustBeEmitted(DRD))
              Builder->EmitGlobal(DRD);
          } else if (auto *DMD = dyn_cast<OMPDeclareMapperDecl>(Member)) {
            if (Ctx->DeclMustBeEmitted(DMD))
              Builder->EmitGlobal(DMD);
          }
        }
      }
    }

    void HandleTagDeclRequiredDefinition(const TagDecl *D) override {
      if (Diags.hasUnrecoverableErrorOccurred())
        return;

      // Don't allow re-entrant calls to CodeGen triggered by PCH
      // deserialization to emit deferred decls.
      HandlingTopLevelDeclRAII HandlingDecl(*this, /*EmitDeferred=*/false);

      if (CodeGen::CGDebugInfo *DI = Builder->getModuleDebugInfo())
        if (const RecordDecl *RD = dyn_cast<RecordDecl>(D))
          DI->completeRequiredType(RD);
    }

    void HandleTranslationUnit(ASTContext &Ctx) override {
      // Release the Builder when there is no error.
      if (!Diags.hasUnrecoverableErrorOccurred() && Builder)
        Builder->Release();

      // If there are errors before or when releasing the Builder, reset
      // the module to stop here before invoking the backend.
      if (Diags.hasErrorOccurred()) {
        if (Builder)
          Builder->clear();
        M.reset();
      }

      IRGenFinished = true;
    }

    void AssignInheritanceModel(CXXRecordDecl *RD) override {
      if (Diags.hasUnrecoverableErrorOccurred())
        return;

      Builder->RefreshTypeCacheForClass(RD);
    }

    void CompleteTentativeDefinition(VarDecl *D) override {
      if (Diags.hasUnrecoverableErrorOccurred())
        return;

      Builder->EmitTentativeDefinition(D);
    }

    void CompleteExternalDeclaration(DeclaratorDecl *D) override {
      Builder->EmitExternalDeclaration(D);
    }

    void HandleVTable(CXXRecordDecl *RD) override {
      if (Diags.hasUnrecoverableErrorOccurred())
        return;

      Builder->EmitVTable(RD);
    }
  };
}

void CodeGenerator::anchor() { }

CodeGenModule &CodeGenerator::CGM() {
  return static_cast<CodeGeneratorImpl*>(this)->CGM();
}

llvm::Module *CodeGenerator::GetModule() {
  return static_cast<CodeGeneratorImpl*>(this)->GetModule();
}

std::unique_ptr<llvm::Module> CodeGenerator::ReleaseModule() {
  return static_cast<CodeGeneratorImpl*>(this)->ReleaseModule();
}

CGDebugInfo *CodeGenerator::getCGDebugInfo() {
  return static_cast<CodeGeneratorImpl*>(this)->getCGDebugInfo();
}

const Decl *CodeGenerator::GetDeclForMangledName(llvm::StringRef name) {
  return static_cast<CodeGeneratorImpl*>(this)->GetDeclForMangledName(name);
}

llvm::StringRef CodeGenerator::GetMangledName(GlobalDecl GD) {
  return static_cast<CodeGeneratorImpl *>(this)->GetMangledName(GD);
}

llvm::Constant *CodeGenerator::GetAddrOfGlobal(GlobalDecl global,
                                               bool isForDefinition) {
  return static_cast<CodeGeneratorImpl*>(this)
           ->GetAddrOfGlobal(global, isForDefinition);
}

llvm::Constant *CodeGenerator::GetAddrOfVTable(BaseSubobject base,
                                               const CXXRecordDecl *decl) {
  return static_cast<CodeGeneratorImpl *>(this)->GetAddrOfVTable(base, decl);
}

llvm::Module *CodeGenerator::StartModule(llvm::StringRef ModuleName,
                                         llvm::LLVMContext &C) {
  return static_cast<CodeGeneratorImpl*>(this)->StartModule(ModuleName, C);
}

std::unique_ptr<CodeGenerator>
clang::CreateLLVMCodeGen(DiagnosticsEngine &Diags, llvm::StringRef ModuleName,
                         IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS,
                         const HeaderSearchOptions &HeaderSearchOpts,
                         const PreprocessorOptions &PreprocessorOpts,
                         const CodeGenOptions &CGO, llvm::LLVMContext &C,
                         CoverageSourceInfo *CoverageInfo) {
  return std::make_unique<CodeGeneratorImpl>(Diags, ModuleName, std::move(FS),
                                             HeaderSearchOpts, PreprocessorOpts,
                                             CGO, C, CoverageInfo);
}

std::unique_ptr<CodeGenerator>
clang::CreateLLVMCodeGen(const CompilerInstance &CI, StringRef ModuleName,
                         llvm::LLVMContext &C,
                         CoverageSourceInfo *CoverageInfo) {
  return CreateLLVMCodeGen(CI.getDiagnostics(), ModuleName,
                           CI.getVirtualFileSystemPtr(),
                           CI.getHeaderSearchOpts(), CI.getPreprocessorOpts(),
                           CI.getCodeGenOpts(), C, CoverageInfo);
}

namespace clang {
namespace CodeGen {
std::optional<std::pair<StringRef, StringRef>>
DemangleTrapReasonInDebugInfo(StringRef FuncName) {
  static const auto TrapRegex =
      llvm::Regex(llvm::formatv("^{0}\\$(.*)\\$(.*)$", ClangTrapPrefix).str());
  llvm::SmallVector<llvm::StringRef, 3> Matches;
  std::string *ErrorPtr = nullptr;
#ifndef NDEBUG
  std::string Error;
  ErrorPtr = &Error;
#endif
  if (!TrapRegex.match(FuncName, &Matches, ErrorPtr)) {
    assert(ErrorPtr && ErrorPtr->empty() && "Invalid regex pattern");
    return {};
  }

  if (Matches.size() != 3) {
    assert(0 && "Expected 3 matches from Regex::match");
    return {};
  }

  // Returns { Trap Category, Trap Message }
  return std::make_pair(Matches[1], Matches[2]);
}
} // namespace CodeGen
} // namespace clang
