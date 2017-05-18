LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := scorpion

LOCAL_SRC_FILES := $(call all-subdir-c-files)
LOCAL_SHARED_LIBRARIES := libcutils liblog

include $(BUILD_EXECUTABLE)
