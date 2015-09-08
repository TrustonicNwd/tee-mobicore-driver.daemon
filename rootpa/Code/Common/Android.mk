#
# Copyright © Trustonic Limited 2013
#
# All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification, 
#  are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice, this 
#     list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright notice, 
#     this list of conditions and the following disclaimer in the documentation 
#     and/or other materials provided with the distribution.
#
#  3. Neither the name of the Trustonic Limited nor the names of its contributors 
#     may be used to endorse or promote products derived from this software 
#     without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
# OF THE POSSIBILITY OF SUCH DAMAGE.
#


#
# makefile for building the provisioning agent Common part for android. build the code by executing 
# $NDK_ROOT/ndk-build in the folder where this file resides
#
# naturally the right way to build is to use build script under Build folder. It then uses this file.
#



LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DANDROID_ARM=1
LOCAL_CFLAGS += -DANDROID 
LOCAL_CFLAGS +=-fstack-protector
ifeq ($(DEBUG), 1)
    LOCAL_CFLAGS += -D__DEBUG=1
endif    


LOCAL_SRC_FILES += pacmp3.c
LOCAL_SRC_FILES += pacmtl.c
LOCAL_SRC_FILES += trustletchannel.c
LOCAL_SRC_FILES += registry.c
LOCAL_SRC_FILES += seclient.c
LOCAL_SRC_FILES += base64.c
LOCAL_SRC_FILES += xmlmessagehandler.c
LOCAL_SRC_FILES += provisioningengine.c
LOCAL_SRC_FILES += contentmanager.c
LOCAL_SRC_FILES += commandhandler.c


LOCAL_C_INCLUDES +=  $(MOBICORE_DIR_INC)
LOCAL_C_INCLUDES +=  external/curl/include
LOCAL_C_INCLUDES +=  external/icu/icu4c/source/common
LOCAL_C_INCLUDES +=  external/icu4c/common
LOCAL_C_INCLUDES +=  external/libxml2/include
LOCAL_C_INCLUDES +=  .
LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/include

ifeq ($(ROOTPA_MODULE_TEST), 1)
    LOCAL_STATIC_LIBRARIES +=  McStub
    LOCAL_MODULE    := provisioningagent_test
else
    LOCAL_MODULE    := provisioningagent
endif

LOCAL_MODULE_TAGS := debug eng optional

LOCAL_STATIC_LIBRARIES = MobiCoreTlcm
LOCAL_SHARED_LIBRARIES = libMcClient libMcRegistry

APP_PIE := true
LOCAL_32_BIT_ONLY := true
include $(BUILD_STATIC_LIBRARY)
