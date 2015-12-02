# =============================================================================
#
# Module: libMcTeeKeymaster.so - Client library handling key operations
# with TEE Keymaster trustlet
#
# =============================================================================

LOCAL_PATH := $(call my-dir)
MY_CLIENTLIB_PATH := $(LOCAL_PATH)

# =============================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := libMcTeeKeymaster
LOCAL_SRC_FILES := src/tlcTeeKeymaster_if.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SHARED_LIBRARIES := libMcClient
ifeq ($(APP_PROJECT_PATH),)
LOCAL_SHARED_LIBRARIES += liblog
else
LOCAL_C_INCLUDES += \
	$(COMP_PATH_TlSdk)/inc
LOCAL_LDLIBS := -llog
endif

include $(BUILD_SHARED_LIBRARY)

# =============================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := testTeeKeymaster
LOCAL_SRC_FILES := src/testTeeKeymaster.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SHARED_LIBRARIES := libMcTeeKeymaster libMcClient libcrypto
ifeq ($(APP_PROJECT_PATH),)
LOCAL_C_INCLUDES += \
	external/openssl/include
LOCAL_SHARED_LIBRARIES += liblog
else
LOCAL_LDLIBS := -llog
endif

include $(BUILD_EXECUTABLE)

# =============================================================================

$(call import-add-path,/)
$(call import-module,$(COMP_PATH_AndroidOpenSsl))
$(call import-module,$(COMP_PATH_MobiCoreClientLib_module))
