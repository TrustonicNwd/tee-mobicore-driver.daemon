# =============================================================================
#
# Main build file defining the project modules and their global variables.
#
# =============================================================================

# Don't remove this - mandatory
APP_PROJECT_PATH := $(abspath $(call my-dir))

APP_STL := gnustl_static

# Proper support for C++11
NDK_TOOLCHAIN_VERSION := 4.8

# Application wide Cflags
GLOBAL_INCLUDES := \
	$(DEV_DRIVER_DIR) \
	$(DEV_MOBICORE_DIR) \
	$(DEV_MOBICORE_DIR)/McLib \
	$(DEP_LOGWRAPPER_DIR)

LOG_WRAPPER := $(DEP_LOGWRAPPER_DIR)

APP_PLATFORM := android-9

# Position Independent Executable
APP_PIE := true
