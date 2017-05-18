TARGET_USES_QCOM_BSP := true
ifneq ($(TARGET_PRODUCT),msm8909_512)
TARGET_USES_QCA_NFC := false
endif
ifeq ($(TARGET_USES_QCOM_BSP), true)
# Add QC Video Enhancements flag
TARGET_ENABLE_QC_AV_ENHANCEMENTS := true
endif #TARGET_USES_QCOM_BSP


#QTIC flag
-include $(QCPATH)/common/config/qtic-config.mk

$(call inherit-product, device/qcom/common/common.mk)

#Set BT_DEFAULT_NAME to PRODUCT
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/qcom/X300

# font rendering engine feature switch
-include $(QCPATH)/common/config/rendering-engine.mk
ifneq (,$(strip $(wildcard $(PRODUCT_RENDERING_ENGINE_REVLIB))))
    MULTI_LANG_ENGINE := REVERIE
endif

#Android EGL implementation
PRODUCT_PACKAGES += libGLES_android

# Audio configuration file
PRODUCT_COPY_FILES += \
    device/qcom/X300/audio/audio_policy.conf:system/etc/audio_policy.conf \
    device/qcom/X300/audio/audio_effects.conf:system/vendor/etc/audio_effects.conf \
    device/qcom/X300/audio/mixer_paths_qrd_skuh.xml:system/etc/mixer_paths_qrd_skuh.xml \
    device/qcom/X300/audio/mixer_paths_qrd_skui.xml:system/etc/mixer_paths_qrd_skui.xml \
    device/qcom/X300/audio/mixer_paths.xml:system/etc/mixer_paths.xml \
    device/qcom/X300/audio/mixer_paths_msm8909_pm8916.xml:system/etc/mixer_paths_msm8909_pm8916.xml \
    device/qcom/X300/audio/mixer_paths_skua.xml:system/etc/mixer_paths_skua.xml \
    device/qcom/X300/audio/mixer_paths_skuc.xml:system/etc/mixer_paths_skuc.xml \
    device/qcom/X300/audio/sound_trigger_mixer_paths.xml:system/etc/sound_trigger_mixer_paths.xml \
    device/qcom/X300/audio/sound_trigger_platform_info.xml:system/etc/sound_trigger_platform_info.xml

# Audio cal data files
OVERRIDE_ACDB_MODULES := true
PRODUCT_COPY_FILES += \
	device/qcom/X300/acdbdata/MTP/MTP_Bluetooth_cal.acdb:system/etc/acdbdata/MTP/MTP_Bluetooth_cal.acdb \
	device/qcom/X300/acdbdata/MTP/MTP_General_cal.acdb:system/etc/acdbdata/MTP/MTP_General_cal.acdb \
	device/qcom/X300/acdbdata/MTP/MTP_Global_cal.acdb:system/etc/acdbdata/MTP/MTP_Global_cal.acdb \
	device/qcom/X300/acdbdata/MTP/MTP_Handset_cal.acdb:system/etc/acdbdata/MTP/MTP_Handset_cal.acdb \
	device/qcom/X300/acdbdata/MTP/MTP_Hdmi_cal.acdb:system/etc/acdbdata/MTP/MTP_Hdmi_cal.acdb \
	device/qcom/X300/acdbdata/MTP/MTP_Headset_cal.acdb:system/etc/acdbdata/MTP/MTP_Headset_cal.acdb \
	device/qcom/X300/acdbdata/MTP/MTP_Speaker_cal.acdb:system/etc/acdbdata/MTP/MTP_Speaker_cal.acdb


# NFC packages
ifeq ($(TARGET_USES_QCA_NFC),true)
NFC_D := true

ifeq ($(NFC_D), true)
    PRODUCT_PACKAGES += \
        libnfcD-nci \
        libnfcD_nci_jni \
        nfc_nci.msm8909 \
        NfcDNci \
        QNfc \
        Tag \
        GsmaNfcService \
        com.gsma.services.nfc\
        com.gsma.services.utils \
        com.gsma.services.nfc.xml \
        com.android.nfc_extras \
        com.android.qcom.nfc_extras \
        com.android.qcom.nfc_extras.xml \
        com.android.nfc.helper \
        SmartcardService \
        org.simalliance.openmobileapi \
        org.simalliance.openmobileapi.xml \
        com.vzw.nfc \
        com.vzw.nfc.xml \
        libassd
else
    PRODUCT_PACKAGES += \
    libnfc-nci \
    libnfc_nci_jni \
    nfc_nci.msm8909 \
    NfcNci \
    Tag \
    com.android.nfc_extras
endif

# file that declares the MIFARE NFC constant
# Commands to migrate prefs from com.android.nfc3 to com.android.nfc
# NFC access control + feature files + configuration
PRODUCT_COPY_FILES += \
        packages/apps/Nfc/migrate_nfc.txt:system/etc/updatecmds/migrate_nfc.txt \
        frameworks/native/data/etc/com.nxp.mifare.xml:system/etc/permissions/com.nxp.mifare.xml \
        frameworks/native/data/etc/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml \
        frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
        frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml
# Enable NFC Forum testing by temporarily changing the PRODUCT_BOOT_JARS
# line has to be in sync with build/target/product/core_base.mk

PRODUCT_BOOT_JARS := core:conscrypt:okhttp:core-junit:bouncycastle:ext:com.android.nfc.helper:framework:framework2:telephony-common:voip-common:mms-common:android.policy:services:apache-xml:webviewchromium:telephony-msim

ifeq ($(NFC_D), true)
PRODUCT_BOOT_JARS += org.simalliance.openmobileapi:com.android.qcom.nfc_extras:com.gsma.services.nfc
# SmartcardService, SIM1,SIM2,eSE1 not including eSE2,SD1 as default
ADDITIONAL_BUILD_PROPERTIES += persist.nfc.smartcard.config=SIM1,SIM2,eSE1
endif

endif # TARGET_USES_QCA_NFC

PRODUCT_BOOT_JARS += qcmediaplayer \
                     oem-services \
                     qcom.fmradio \
                     org.codeaurora.Performance \
                     vcard
# Listen configuration file
PRODUCT_COPY_FILES += \
    device/qcom/msm8909/listen_platform_info.xml:system/etc/listen_platform_info.xml

#fstab.qcom
PRODUCT_PACKAGES += fstab.qcom

#spec service
PRODUCT_PACKAGES += init.qti.carrier.rc

PRODUCT_PACKAGES += \
    libqcomvisualizer \
    libqcompostprocbundle \
    libqcomvoiceprocessing \
    Browser

ifeq ($BOARD_HAVE_QCOM_FM),true)
    PRODUCT_PACKAGES += \
    FM2 \
    qcom.fmradio \
    libqcomfm_jni \
    FMRecord   
endif

#OEM Services library
PRODUCT_PACKAGES += oem-services
PRODUCT_PACKAGES += libsubsystem_control
PRODUCT_PACKAGES += libSubSystemShutdown libGpioSys


#ANT+ stack
PRODUCT_PACKAGES += \
AntHalService \
libantradio \
antradio_app

# zhangzhaolei add overlay
PRODUCT_PACKAGE_OVERLAYS += device/qcom/X300/overlay

# Defined the locales
PRODUCT_LOCALES += th_TH vi_VN tl_PH hi_IN ar_EG ru_RU tr_TR pt_BR bn_IN mr_IN ta_IN te_IN zh_HK in_ID

# Add the overlay path
PRODUCT_PACKAGE_OVERLAYS := $(QCPATH)/qrdplus/Extension/res-overlay \
        $(QCPATH)/qrdplus/globalization/multi-language/res-overlay \
        $(PRODUCT_PACKAGE_OVERLAYS)
#include device/qcom/msm8909/msm8909.mk

PRODUCT_NAME := X300
PRODUCT_DEVICE := X300
PRODUCT_MODEL := MSM722
PRODUCT_BRAND := X300
PRODUCT_MANUFACTURER := Robbing
PRODUCT_LOCALES := zh_CN en_US xhdpi hdpi mdpi

# zhangzhaolei add for software && hardware version 20150603 start
BUILD_DISPLAY_SW_VERSION := MSM742.005
BUILD_DISPLAY_HW_VERSION := MSM742.001
# zhangzhaolei add for software && hardware version 20150603 end

# set default Carrier: ChinaTelecom, ChinaUnicom, ChinaMobile
# used in vendor/qcom/proprietary/qrdplus/Extension/config/Android.mk
CURRENT_CARRIER := ChinaTelecom

# Camera raw sensor
CAM_RAW_BACK := OV8858_HIS_QTECH_BJ
CAM_RAW_BACKUP := 
CAM_RAW_FRONT := OV2680_HIS_SHINE40492

PRODUCT_COPY_FILES += \
    device/qcom/X300/media/media_profiles.xml:system/etc/media_profiles.xml \
    device/qcom/X300/media/media_codecs.xml:system/etc/media_codecs.xml

# boot animation
PRODUCT_BOOT_ANIMATION := true
PRODUCT_COPY_FILES += \
	device/qcom/X300/bootanimation/boot.wav:system/media/boot.wav \
	device/qcom/X300/bootanimation/shutdown.wav:system/media/shutdown.wav

# audio config
PRODUCT_COPY_FILES += \
    device/qcom/X300/audio/mixer_paths.xml:system/etc/mixer_paths.xml

PRODUCT_PACKAGES += wcnss_service

# wlan driver
PRODUCT_COPY_FILES += \
    device/qcom/X300/wlan/WCNSS_qcom_cfg.ini:system/etc/wifi/WCNSS_qcom_cfg.ini \
    device/qcom/X300/wlan/WCNSS_qcom_cfg_test.ini:system/etc/wifi/WCNSS_qcom_cfg_test.ini \
    device/qcom/X300/wlan/WCNSS_wlan_dictionary.dat:persist/WCNSS_wlan_dictionary.dat \
    device/qcom/X300/wlan/WCNSS_qcom_wlan_nv.bin:persist/WCNSS_qcom_wlan_nv.bin

PRODUCT_PACKAGES += \
    wpa_supplicant_overlay.conf \
    p2p_supplicant_overlay.conf

# Feature define
PRODUCT_COPY_FILES += \
  frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
  frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
  frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
  frameworks/native/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
  frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
  frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
  frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
  frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
  frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
  device/qcom/X300/overlay/frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
  frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
  frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml

include vendor/qcom/prebuild/prebuild_modules.mk
#include vendor/qcom/prebuild/prebuild_copy_files.mk

PRODUCT_PACKAGES += \
    pronto_wlan.ko \
    gps.conf \
    libaudioroute \
    libconnectivitymanager \
    libmedialogservice \
    libminui \
    libhdmi

# Fleety : ++++++++++
# 	add release image&logo
PRODUCT_COPY_FILES += \
	release/emmc_appsboot.mbn:emmc_appsboot.mbn \
	release/fleety_logo.raw:logo.raw
#	add tool busybox
PRODUCT_COPY_FILES += \
	device/qcom/X300/fleety/tools/busybox:system/bin/busybox
#	add script 
PRODUCT_COPY_FILES += \
	device/qcom/X300/fleety/scripts/init.shell.sh:system/scripts/init.shell.sh \
	device/qcom/X300/fleety/scripts/init.iptables.sh:system/scripts/init.iptables.sh \
	device/qcom/X300/fleety/scripts/init.event.sh:system/scripts/init.event.sh \
	device/qcom/X300/fleety/scripts/recovery.sh:system/scripts/recovery.sh
# 	add boot animation
PRODUCT_COPY_FILES += \
	device/qcom/X300/fleety/bootanimationfleety.zip:/system/media/bootanimationfleety.zip

# add so from hardware/jni folder
PRODUCT_PACKAGES += \
	spider.default \
	libqcomfm 

# add exe from external folder
PRODUCT_PACKAGES += \
	scorpion \
	SuAgentServer \
	SuAgentClient

# add apk from packages\apps folder
PRODUCT_PACKAGES += \
	FleetyDemo \
	FleetyFMRadioService \
	r600util
# Fleety : ----------

