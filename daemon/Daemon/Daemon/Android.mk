# =============================================================================
#
# Module: mcDriverDaemon
#
# =============================================================================

LOCAL_PATH	:= $(call my-dir)
MY_MCDRIVER_PATH	:= $(LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := mcDriverDaemon
LOCAL_CFLAGS += -include buildTag.h
# Add new subdirectories containing code here
include $(LOCAL_PATH)/Device/Android.mk
include $(LOCAL_PATH)/Server/Android.mk

# Includes required for the Daemon
LOCAL_C_INCLUDES +=\
	$(MY_MCDRIVER_PATH)/public \
	$(COMP_PATH_MobiCore)/inc \
	$(LOCAL_PATH)/../

# Add new source files here
LOCAL_SRC_FILES +=\
	MobiCoreDriverDaemon.cpp

# Modules this one depends on (depending ones first)
LOCAL_STATIC_LIBRARIES	+= Kernel Common McRegistryStatic PaApiStatic

# Import logwrapper
include $(COMP_PATH_Logwrapper)/Android.mk

include $(BUILD_EXECUTABLE)
