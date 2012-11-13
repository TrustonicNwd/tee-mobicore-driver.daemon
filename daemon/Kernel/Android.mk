# =============================================================================
#
# Module: Kernel Module access
# All paths are relative to application root!
#
# =============================================================================


include $(LOCAL_PATH)/Kernel/Platforms/Generic/Android.mk

# Add new source files here
LOCAL_SRC_FILES += Kernel/CKMod.cpp

# Header files for components including this module
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Kernel
