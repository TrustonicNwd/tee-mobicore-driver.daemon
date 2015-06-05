# =============================================================================
#
# MobiCore Android build components
#
# =============================================================================

LOCAL_PATH := $(call my-dir)

# Registry Shared Library
# =============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libMcRegistry
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += -DLOG_TAG=\"McRegistry\"
LOCAL_CFLAGS += -Wall -Wextra
LOCAL_CFLAGS += -DLOG_ANDROID
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := libMcClient
ifeq ($(APP_PROJECT_PATH),)
LOCAL_SHARED_LIBRARIES += liblog
else
LOCAL_LDLIBS := -llog
endif

LOCAL_SRC_FILES := \
	src/Connection.cpp \
	src/Registry.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_EXPORT_C_INCLUDES)

include $(BUILD_SHARED_LIBRARY)

# Daemon Application
# =============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := mcDriverDaemon
LOCAL_MODULE_TAGS := eng
LOCAL_CFLAGS += -DLOG_TAG=\"McDaemon\"
LOCAL_CFLAGS += -DTBASE_API_LEVEL=5
LOCAL_CFLAGS += -Wall -Wextra
LOCAL_CFLAGS += -std=c++11
LOCAL_CFLAGS += -DLOG_ANDROID
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := libMcClient libMcRegistry libstlport
ifeq ($(APP_PROJECT_PATH),)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include external/stlport/stlport
include external/stlport/libstlport.mk
LOCAL_SHARED_LIBRARIES += liblog
else
LOCAL_LDLIBS := -llog
endif

LOCAL_SRC_FILES := \
	src/Connection.cpp \
	src/CThread.cpp \
	src/MobiCoreDriverDaemon.cpp \
	src/SecureWorld.cpp \
	src/FSD.cpp \
	src/Server.cpp \
	src/PrivateRegistry.cpp

include $(BUILD_EXECUTABLE)
