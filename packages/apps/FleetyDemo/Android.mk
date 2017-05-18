# This Android.mk is empty, and >> does not build subdirectories <<.
# Individual packages in experimental/ must be built directly with "mmm".
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Module name should match apk name to be installed.
LOCAL_MODULE := FleetyDemo
LOCAL_SRC_FILES := $(LOCAL_MODULE).apk
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)

#you can choose apk's diff location 
LOCAL_MODULE_PATH := $(TARGET_OUT_APPS)
#LOCAL_MODULE_PATH := $(TARGET_OUT_DATA_APPS)
LOCAL_CERTIFICATE := platform
LOCAL_PACKAGE_NAME := FleetyDemo

include $(BUILD_PREBUILT)
