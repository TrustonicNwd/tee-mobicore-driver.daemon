# =============================================================================
#
# Module: libMcTeeKeymaster.so - Client library handling key operations
# with TEE Keymaster trustlet
#
# =============================================================================

LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE	:= libMcTeeKeymaster

# Add new folders with header files here
LOCAL_C_INCLUDES +=\
	$(APP_PROJECT_PATH)

# Add new source files here
LOCAL_SRC_FILES +=\
	tlcTeeKeymaster_if.c

LOCAL_C_INCLUDES +=\
    $(LOCAL_PATH)/public \
    $(LOCAL_PATH)/inc \
    $(COMP_PATH_MobiCore)/inc

# Need the MobiCore client library
LOCAL_SHARED_LIBRARIES += libMcClient liblog

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

# =============================================================================
include $(CLEAR_VARS)

# Module name (sets name of output binary / library)
LOCAL_MODULE	:= testTeeKeymaster

# Add your source files here (relative paths)
LOCAL_SRC_FILES +=\
    testTeeKeymaster.c

LOCAL_C_INCLUDES +=\
    $(LOCAL_PATH)/public \
    $(LOCAL_PATH)/inc \
    $(COMP_PATH_MobiCore)/inc \
    $(COMP_PATH_AndroidOpenSsl)/include 

LOCAL_SHARED_LIBRARIES +=\
    libMcTeeKeymaster \
    libMcClient \
    liblog \
    libcrypto

LOCAL_LDLIBS := -L$(COMP_PATH_AndroidOpenSsl)/Bin/$(APP_ABI) -llog

include $(BUILD_EXECUTABLE)

$(call import-add-path,/)
$(call import-module,$(COMP_PATH_MobiCoreDriverLib))

