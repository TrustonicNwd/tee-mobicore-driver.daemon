# =============================================================================
#
# MobiCore Android build components
#
# =============================================================================

LOCAL_PATH := $(call my-dir)

# Client Library
# =============================================================================
include $(CLEAR_VARS)
LOCAL_MODULE := libMcClient
LOCAL_MODULE_TAGS := debug eng optional

LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_CFLAGS += -DLOG_TAG=\"McClient\"
LOCAL_CFLAGS += -DTBASE_API_LEVEL=5
LOCAL_CFLAGS += -Wall -Wextra
LOCAL_CFLAGS += -std=c99

LOCAL_C_INCLUDES += $(GLOBAL_INCLUDES)
LOCAL_SHARED_LIBRARIES += $(GLOBAL_LIBRARIES)

# Add new source files here
LOCAL_SRC_FILES := \
	ClientLib/ClientLib.c \
	ClientLib/GP/tee_client_api.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/ClientLib/public \
	$(LOCAL_PATH)/ClientLib/public/GP

LOCAL_EXPORT_C_INCLUDE_DIRS +=\
	$(LOCAL_PATH)/ClientLib/public \
	$(LOCAL_PATH)/ClientLib/public/GP

# Import logwrapper
include $(LOG_WRAPPER)/Android.mk

include $(BUILD_SHARED_LIBRARY)

# Daemon Application
# =============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := mcDriverDaemon
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_CFLAGS += -DLOG_TAG=\"McDaemon\"
LOCAL_CFLAGS += -DTBASE_API_LEVEL=5
LOCAL_CFLAGS += -Wall -Wextra
LOCAL_CFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(GLOBAL_INCLUDES)
LOCAL_SHARED_LIBRARIES += $(GLOBAL_LIBRARIES) libMcClient libMcRegistry

# Common Source files required for building the daemon
LOCAL_SRC_FILES := \
	Common/Connection.cpp \
	Common/CThread.cpp \
	Daemon/MobiCoreDriverDaemon.cpp \
	Daemon/SecureWorld.cpp \
	Daemon/FSD/FSD.cpp \
	Daemon/Server/Server.cpp \
	Registry/PrivateRegistry.cpp

# Includes required for the Daemon
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/Common \
	$(LOCAL_PATH)/ClientLib/public \
	$(LOCAL_PATH)/ClientLib/public/GP \
	$(LOCAL_PATH)/Daemon/public \
	$(LOCAL_PATH)/Daemon/FSD/public \
	$(LOCAL_PATH)/Daemon/Server/public \
	$(LOCAL_PATH)/Registry/Public \
	$(LOCAL_PATH)/Registry

# Import logwrapper
include $(LOG_WRAPPER)/Android.mk

include $(BUILD_EXECUTABLE)

# Registry Shared Library
# =============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libMcRegistry
LOCAL_MODULE_TAGS := debug eng optional

LOCAL_CFLAGS += -DLOG_TAG=\"McRegistry\"
LOCAL_CFLAGS += -Wall -Wextra

LOCAL_C_INCLUDES += $(GLOBAL_INCLUDES)
LOCAL_SHARED_LIBRARIES += $(GLOBAL_LIBRARIES)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/Common \
	$(LOCAL_PATH)/Daemon/public \
	$(LOCAL_PATH)/ClientLib/public \
	$(LOCAL_PATH)/Registry/Public

# Common Source files required for building the library
LOCAL_SRC_FILES := \
	Common/Connection.cpp \
	Registry/Registry.cpp

# Import logwrapper
include $(LOG_WRAPPER)/Android.mk

include $(BUILD_SHARED_LIBRARY)
