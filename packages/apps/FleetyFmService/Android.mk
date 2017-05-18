LOCAL_PATH:= $(call my-dir)

# Build the Phone app which includes the emergency dialer. See Contacts
# for the 'other' dialer.
include $(CLEAR_VARS)


LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += \
        src/com/android/fleety/aidl/IFmRadioService.aidl
#LOCAL_SRC_FILES += \
#        src/com/android/phone/INetworkQueryService.aidl \
#        src/org/codeaurora/btmultisim/IBluetoothDsdaService.aidl

LOCAL_PACKAGE_NAME := FleetyFMRadioService
LOCAL_JNI_SHARED_LIBRARIES := libqcomfm 
LOCAL_PROGUARD_ENABLED:=disabled

LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true


include $(BUILD_PACKAGE)
include $(LOCAL_PATH)/jni/Android.mk

# Build the test package
include $(call all-makefiles-under,$(LOCAL_PATH))
