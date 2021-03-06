LOCAL_PATH := $(call my-dir)

arm_mc_desc_TBLGEN_TABLES :=	\
	ARMGenRegisterInfo.inc	\
	ARMGenInstrInfo.inc	\
	ARMGenSubtargetInfo.inc

arm_mc_desc_SRC_FILES :=   \
	ARMMCAsmInfo.cpp \
	ARMMCTargetDesc.cpp

# For the host
# =====================================================
include $(CLEAR_VARS)
include $(CLEAR_TBLGEN_VARS)

TBLGEN_TABLES := $(arm_mc_desc_TBLGEN_TABLES)

LOCAL_SRC_FILES := $(arm_mc_desc_SRC_FILES)

LOCAL_MODULE:= libLLVMARMDesc

LOCAL_MODULE_TAGS := optional

include $(LLVM_HOST_BUILD_MK)
include $(LLVM_TBLGEN_RULES_MK)
include $(LLVM_GEN_INTRINSICS_MK)
include $(BUILD_HOST_STATIC_LIBRARY)

# For the device only
# =====================================================
ifeq ($(TARGET_ARCH),arm)
include $(CLEAR_VARS)
include $(CLEAR_TBLGEN_VARS)

TBLGEN_TABLES := $(arm_mc_desc_TBLGEN_TABLES)

LOCAL_SRC_FILES := $(arm_mc_desc_SRC_FILES)

LOCAL_MODULE:= libLLVMARMDesc

LOCAL_MODULE_TAGS := optional

include $(LLVM_DEVICE_BUILD_MK)
include $(LLVM_TBLGEN_RULES_MK)
include $(LLVM_GEN_INTRINSICS_MK)
include $(BUILD_STATIC_LIBRARY)
endif
