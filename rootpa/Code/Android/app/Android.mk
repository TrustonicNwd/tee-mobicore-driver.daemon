#
# build RootPA.apk
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_STATIC_JAVA_LIBRARIES := rootpa_interface
LOCAL_JNI_SHARED_LIBRARIES := commonpawrapper

LOCAL_PACKAGE_NAME := RootPA
LOCAL_MODULE_TAGS := optional

include $(BUILD_PACKAGE)

include $(CLEAR_VARS)
