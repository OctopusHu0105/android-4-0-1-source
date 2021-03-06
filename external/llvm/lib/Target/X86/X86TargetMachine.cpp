//===-- X86TargetMachine.cpp - Define TargetMachine for the X86 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the X86 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "X86TargetMachine.h"
#include "X86.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetRegistry.h"
using namespace llvm;

static MCStreamer *createMCStreamer(const Target &T, const std::string &TT,
                                    MCContext &Ctx, TargetAsmBackend &TAB,
                                    raw_ostream &_OS,
                                    MCCodeEmitter *_Emitter,
                                    bool RelaxAll,
                                    bool NoExecStack) {
  Triple TheTriple(TT);

  if (TheTriple.isOSDarwin() || TheTriple.getEnvironment() == Triple::MachO)
    return createMachOStreamer(Ctx, TAB, _OS, _Emitter, RelaxAll);

  if (TheTriple.isOSWindows())
    return createWinCOFFStreamer(Ctx, TAB, *_Emitter, _OS, RelaxAll);

  return createELFStreamer(Ctx, TAB, _OS, _Emitter, RelaxAll, NoExecStack);
}

extern "C" void LLVMInitializeX86Target() {
  // Register the target.
  RegisterTargetMachine<X86_32TargetMachine> X(TheX86_32Target);
  RegisterTargetMachine<X86_64TargetMachine> Y(TheX86_64Target);

  // Register the code emitter.
  TargetRegistry::RegisterCodeEmitter(TheX86_32Target,
                                      createX86MCCodeEmitter);
  TargetRegistry::RegisterCodeEmitter(TheX86_64Target,
                                      createX86MCCodeEmitter);

  // Register the asm backend.
  TargetRegistry::RegisterAsmBackend(TheX86_32Target,
                                     createX86_32AsmBackend);
  TargetRegistry::RegisterAsmBackend(TheX86_64Target,
                                     createX86_64AsmBackend);

  // Register the object streamer.
  TargetRegistry::RegisterObjectStreamer(TheX86_32Target,
                                         createMCStreamer);
  TargetRegistry::RegisterObjectStreamer(TheX86_64Target,
                                         createMCStreamer);
}


X86_32TargetMachine::X86_32TargetMachine(const Target &T, StringRef TT,
                                         StringRef CPU, StringRef FS,
                                         Reloc::Model RM)
  : X86TargetMachine(T, TT, CPU, FS, RM, false),
    DataLayout(getSubtargetImpl()->isTargetDarwin() ?
               "e-p:32:32-f64:32:64-i64:32:64-f80:128:128-f128:128:128-n8:16:32" :
               (getSubtargetImpl()->isTargetCygMing() ||
                getSubtargetImpl()->isTargetWindows()) ?
               "e-p:32:32-f64:64:64-i64:64:64-f80:32:32-f128:128:128-n8:16:32" :
               "e-p:32:32-f64:32:64-i64:32:64-f80:32:32-f128:128:128-n8:16:32"),
    InstrInfo(*this),
    TSInfo(*this),
    TLInfo(*this),
    JITInfo(*this) {
}


X86_64TargetMachine::X86_64TargetMachine(const Target &T, StringRef TT,
                                         StringRef CPU, StringRef FS,
                                         Reloc::Model RM)
  : X86TargetMachine(T, TT, CPU, FS, RM, true),
    DataLayout("e-p:64:64-s:64-f64:64:64-i64:64:64-f80:128:128-f128:128:128-n8:16:32:64"),
    InstrInfo(*this),
    TSInfo(*this),
    TLInfo(*this),
    JITInfo(*this) {
}

/// X86TargetMachine ctor - Create an X86 target.
///
X86TargetMachine::X86TargetMachine(const Target &T, StringRef TT,
                                   StringRef CPU, StringRef FS,
                                   Reloc::Model RM, bool is64Bit)
  : LLVMTargetMachine(T, TT, CPU, FS, RM),
    Subtarget(TT, CPU, FS, StackAlignmentOverride, is64Bit),
    FrameLowering(*this, Subtarget),
    ELFWriterInfo(is64Bit, true) {
  // Determine the PICStyle based on the target selected.
  if (getRelocationModel() == Reloc::Static) {
    // Unless we're in PIC or DynamicNoPIC mode, set the PIC style to None.
    Subtarget.setPICStyle(PICStyles::None);
  } else if (Subtarget.is64Bit()) {
    // PIC in 64 bit mode is always rip-rel.
    Subtarget.setPICStyle(PICStyles::RIPRel);
  } else if (Subtarget.isTargetCygMing()) {
    Subtarget.setPICStyle(PICStyles::None);
  } else if (Subtarget.isTargetDarwin()) {
    if (getRelocationModel() == Reloc::PIC_)
      Subtarget.setPICStyle(PICStyles::StubPIC);
    else {
      assert(getRelocationModel() == Reloc::DynamicNoPIC);
      Subtarget.setPICStyle(PICStyles::StubDynamicNoPIC);
    }
  } else if (Subtarget.isTargetELF()) {
    Subtarget.setPICStyle(PICStyles::GOT);
  }

  // default to hard float ABI
  if (FloatABIType == FloatABI::Default)
    FloatABIType = FloatABI::Hard;    
}

//===----------------------------------------------------------------------===//
// Pass Pipeline Configuration
//===----------------------------------------------------------------------===//

bool X86TargetMachine::addInstSelector(PassManagerBase &PM,
                                       CodeGenOpt::Level OptLevel) {
  // Install an instruction selector.
  PM.add(createX86ISelDag(*this, OptLevel));

  // For 32-bit, prepend instructions to set the "global base reg" for PIC.
  if (!Subtarget.is64Bit())
    PM.add(createGlobalBaseRegPass());

  return false;
}

bool X86TargetMachine::addPreRegAlloc(PassManagerBase &PM,
                                      CodeGenOpt::Level OptLevel) {
  PM.add(createX86MaxStackAlignmentHeuristicPass());
  return false;  // -print-machineinstr shouldn't print after this.
}

bool X86TargetMachine::addPostRegAlloc(PassManagerBase &PM,
                                       CodeGenOpt::Level OptLevel) {
  PM.add(createX86FloatingPointStackifierPass());
  return true;  // -print-machineinstr should print after this.
}

bool X86TargetMachine::addPreEmitPass(PassManagerBase &PM,
                                      CodeGenOpt::Level OptLevel) {
  if (OptLevel != CodeGenOpt::None && Subtarget.hasSSE2()) {
    PM.add(createSSEDomainFixPass());
    return true;
  }
  return false;
}

bool X86TargetMachine::addCodeEmitter(PassManagerBase &PM,
                                      CodeGenOpt::Level OptLevel,
                                      JITCodeEmitter &JCE) {
  PM.add(createX86JITCodeEmitterPass(*this, JCE));

  return false;
}

void X86TargetMachine::setCodeModelForStatic() {

    if (getCodeModel() != CodeModel::Default) return;

    // For static codegen, if we're not already set, use Small codegen.
    setCodeModel(CodeModel::Small);
}


void X86TargetMachine::setCodeModelForJIT() {

  if (getCodeModel() != CodeModel::Default) return;

  // 64-bit JIT places everything in the same buffer except external functions.
  if (Subtarget.is64Bit())
    setCodeModel(CodeModel::Large);
  else
    setCodeModel(CodeModel::Small);
}
