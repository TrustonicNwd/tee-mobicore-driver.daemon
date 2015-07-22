# =============================================================================
#
# Makefile pointing to all makefiles within the project.
#
# =============================================================================
MOBICORE_PROJECT_PATH := $(call my-dir)
# Setup common variables
LOG_WRAPPER := $(MOBICORE_PROJECT_PATH)/common/LogWrapper
COMP_PATH_MobiCore := $(MOBICORE_PROJECT_PATH)/common/MobiCore
COMP_PATH_MobiCoreDriverMod := $(MOBICORE_PROJECT_PATH)/include


# Application wide Cflags
GLOBAL_INCLUDES := bionic \
	$(COMP_PATH_MobiCore)/inc \
	$(COMP_PATH_MobiCoreDriverMod)/Public \
	$(COMP_PATH_MobiCore)/inc/TlCm

# Include the Daemon
include $(MOBICORE_PROJECT_PATH)/MobiCoreDriverLib/Android.mk

