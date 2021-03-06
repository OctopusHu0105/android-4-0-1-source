//===-- XCoreMCTargetDesc.cpp - XCore Target Descriptions -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides XCore specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "XCoreMCTargetDesc.h"
#include "XCoreMCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Target/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "XCoreGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "XCoreGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "XCoreGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createXCoreMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitXCoreMCInstrInfo(X);
  return X;
}

extern "C" void LLVMInitializeXCoreMCInstrInfo() {
  TargetRegistry::RegisterMCInstrInfo(TheXCoreTarget, createXCoreMCInstrInfo);
}

static MCRegisterInfo *createXCoreMCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitXCoreMCRegisterInfo(X, XCore::LR);
  return X;
}

extern "C" void LLVMInitializeXCoreMCRegisterInfo() {
  TargetRegistry::RegisterMCRegInfo(TheXCoreTarget, createXCoreMCRegisterInfo);
}

static MCSubtargetInfo *createXCoreMCSubtargetInfo(StringRef TT, StringRef CPU,
                                                   StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitXCoreMCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

extern "C" void LLVMInitializeXCoreMCSubtargetInfo() {
  TargetRegistry::RegisterMCSubtargetInfo(TheXCoreTarget,
                                          createXCoreMCSubtargetInfo);
}

static MCAsmInfo *createXCoreMCAsmInfo(const Target &T, StringRef TT) {
  MCAsmInfo *MAI = new XCoreMCAsmInfo(T, TT);

  // Initial state of the frame pointer is SP.
  MachineLocation Dst(MachineLocation::VirtualFP);
  MachineLocation Src(XCore::SP, 0);
  MAI->addInitialFrameState(0, Dst, Src);

  return MAI;
}

extern "C" void LLVMInitializeXCoreMCAsmInfo() {
  RegisterMCAsmInfoFn X(TheXCoreTarget, createXCoreMCAsmInfo);
}

MCCodeGenInfo *createXCoreMCCodeGenInfo(StringRef TT, Reloc::Model RM) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  X->InitMCCodeGenInfo(RM);
  return X;
}

extern "C" void LLVMInitializeXCoreMCCodeGenInfo() {
  TargetRegistry::RegisterMCCodeGenInfo(TheXCoreTarget,
                                        createXCoreMCCodeGenInfo);
}
