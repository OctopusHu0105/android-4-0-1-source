//===-- PPCMCTargetDesc.cpp - PowerPC Target Descriptions -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides PowerPC specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "PPCMCTargetDesc.h"
#include "PPCMCAsmInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Target/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "PPCGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "PPCGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "PPCGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createPPCMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitPPCMCInstrInfo(X);
  return X;
}

extern "C" void LLVMInitializePowerPCMCInstrInfo() {
  TargetRegistry::RegisterMCInstrInfo(ThePPC32Target, createPPCMCInstrInfo);
  TargetRegistry::RegisterMCInstrInfo(ThePPC64Target, createPPCMCInstrInfo);
}

static MCRegisterInfo *createPPCMCRegisterInfo(StringRef TT) {
  Triple TheTriple(TT);
  bool isPPC64 = (TheTriple.getArch() == Triple::ppc64);
  unsigned Flavour = isPPC64 ? 0 : 1;
  unsigned RA = isPPC64 ? PPC::LR8 : PPC::LR;

  MCRegisterInfo *X = new MCRegisterInfo();
  InitPPCMCRegisterInfo(X, RA, Flavour, Flavour);
  return X;
}

extern "C" void LLVMInitializePowerPCMCRegisterInfo() {
  TargetRegistry::RegisterMCRegInfo(ThePPC32Target, createPPCMCRegisterInfo);
  TargetRegistry::RegisterMCRegInfo(ThePPC64Target, createPPCMCRegisterInfo);
}

static MCSubtargetInfo *createPPCMCSubtargetInfo(StringRef TT, StringRef CPU,
                                                 StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitPPCMCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

extern "C" void LLVMInitializePowerPCMCSubtargetInfo() {
  TargetRegistry::RegisterMCSubtargetInfo(ThePPC32Target,
                                          createPPCMCSubtargetInfo);
  TargetRegistry::RegisterMCSubtargetInfo(ThePPC64Target,
                                          createPPCMCSubtargetInfo);
}

static MCAsmInfo *createPPCMCAsmInfo(const Target &T, StringRef TT) {
  Triple TheTriple(TT);
  bool isPPC64 = TheTriple.getArch() == Triple::ppc64;

  MCAsmInfo *MAI;
  if (TheTriple.isOSDarwin())
    MAI = new PPCMCAsmInfoDarwin(isPPC64);
  else
    MAI = new PPCLinuxMCAsmInfo(isPPC64);

  // Initial state of the frame pointer is R1.
  MachineLocation Dst(MachineLocation::VirtualFP);
  MachineLocation Src(PPC::R1, 0);
  MAI->addInitialFrameState(0, Dst, Src);

  return MAI;
}

extern "C" void LLVMInitializePowerPCMCAsmInfo() {
  RegisterMCAsmInfoFn C(ThePPC32Target, createPPCMCAsmInfo);
  RegisterMCAsmInfoFn D(ThePPC64Target, createPPCMCAsmInfo);  
}

MCCodeGenInfo *createPPCMCCodeGenInfo(StringRef TT, Reloc::Model RM) {
  MCCodeGenInfo *X = new MCCodeGenInfo();

  if (RM == Reloc::Default) {
    Triple T(TT);
    if (T.isOSDarwin())
      RM = Reloc::DynamicNoPIC;
    else
      RM = Reloc::Static;
  }
  X->InitMCCodeGenInfo(RM);
  return X;
}

extern "C" void LLVMInitializePowerPCMCCodeGenInfo() {
  TargetRegistry::RegisterMCCodeGenInfo(ThePPC32Target, createPPCMCCodeGenInfo);
  TargetRegistry::RegisterMCCodeGenInfo(ThePPC64Target, createPPCMCCodeGenInfo);
}
