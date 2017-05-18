package com.fleety.android.action;

/**
 * @author JohnFan
 * 
 */
public interface R600UtilStrings
{
    String                  R600UTIL_RECEIVER_ACTION                         = "com.fleety.android.r600util.action.R600UTIL";
    String                  R600UTIL_SEND_INSTALL_ACTION                     = "com.fleety.android.ACTION_APK_INSTALL_RESULT";
    String                  R600UTIL_SEND_UNINSTALL_ACTION                   = "com.fleety.android.ACTION_PACKAGE_UNINSTALL_RESULT";
    String                  R600UTIL_KEYEVENT_ACTION                         = "com.fleety.android.ACTION_KEYEVENT";
    String                  R600UTIL_CMD                                     = "R600UTIL_CMD";
    /* control command */
    String                  R600UTIL_CMD_REBOOT1                             = "R600UTIL_CMD_REBOOT1";
    String                  R600UTIL_CMD_REBOOT2                             = "R600UTIL_CMD_REBOOT2";
    String                  R600UTIL_CMD_TIMESET                             = "R600UTIL_CMD_TIMESET";
    String                  R600UTIL_CMD_SET_PHONESPEAKER_MUTE_ON            = "R600UTIL_CMD_SET_PHONESPEAKER_MUTE_ON";
    String                  R600UTIL_CMD_SET_PHONESPEAKER_MUTE_OFF           = "R600UTIL_CMD_SET_PHONESPEAKER_MUTE_OFF";
    String                  R600UTIL_CMD_INSTALL_APK                         = "R600UTIL_CMD_INSTALL_APK";
    String                  R600UTIL_CMD_INSTALL_APK_RESPONSE                = "R600UTIL_CMD_INSTALL_APK_RESPONSE";
    String                  R600UTIL_CMD_UNINSTALL_PACKAGE                   = "R600UTIL_CMD_UNINSTALL_PACKAGE";
    String                  R600UTIL_CMD_OTAUPGRADE_ANDROID                  = "R600UTIL_CMD_OTAUPGRADE_ANDROID";
    String                  R600UTIL_CMD_CHANGE_STUTSBAR                     = "R600UTIL_CMD_CHANGE_STUTSBAR";
    String                  R600UTIL_CMD_GO2SLEEP                            = "R600UTIL_CMD_GO2SLEEP";
    String                  R600UTIL_CMD_WAKEUP                              = "R600UTIL_CMD_WAKEUP";
    /* data about */
    String                  R600UTIL_DATA_TIMESET_MILLINS                    = "R600UTIL_DATA_TIMESET_MILLINS";
    String                  R600UTIL_DATA_PACKAGE_NAME                       = "R600UTIL_DATA_PACKAGE_NAME";
    String                  R600UTIL_DATA_APK_PATH                           = "R600UTIL_DATA_APK_PATH";
    String                  R600UTIL_DATA_INSTALL_APK_RESULT                 = "R600UTIL_DATA_INSTALL_APK_RESULT";
    String                  R600UTIL_DATA_UNINSTALL_PACKAGE_RESULT           = "R600UTIL_DATA_APK_PATH";
    String                  R600UTIL_DATA_OTAUPGRADE_ANDROID_PATH            = "R600UTIL_DATA_OTAUPGRADE_ANDROID_PATH";
    String                  R600UTIL_DATA_OTAUPGRADE_ANDROID_MODE            = "R600UTIL_DATA_OTAUPGRADE_ANDROID_MODE";
    String                  R600UTIL_DATA_KEYEVENT                           = "R600UTIL_DATA_KEYEVENT";
    String                  R600UTIL_DATA_CHANGE_STUTSBAR_DISABLE            = "R600UTIL_DATA_CHANGE_STUTSBAR_DISABLE";

    /* upgrade about */
    String                  R600UTIL_DATA_OTAUPGRADE_CALLBACK_TYPE           = "R600UTIL_DATA_OTAUPGRADE_CALLBACK_TYPE";
    String                  R600UTIL_DATA_OTAUPGRADE_CALLBACK_ERRORCODE      = "R600UTIL_DATA_OTAUPGRADE_CALLBACK_ERRORCODE";
    String                  R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONCOPYFAILED   = "R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONCOPYFAILED";
    String                  R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONCOPYPROGRESS = "R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONCOPYPROGRESS";
    String                  R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONPROGRESS     = "R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONPROGRESS";
    String                  R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONSTOPPROGRESS = "R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONSTOPPROGRESS";
    String                  R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONVERIFYFAILED = "R600UTIL_DATA_OTAUPGRADE_CALLBACK_ONVERIFYFAILED";

    /* atmel ota */
    // public class com.amlogic.update.OtaUpgradeUtils

    public static final int ERROR_INVALID_UPGRADE_PACKAGE                    = 0;
    public static final int ERROR_FILE_DOES_NOT_EXIT                         = 1;
    public static final int ERROR_FILE_IO_ERROR                              = 2;
    public static final int FAIL_REASON_BATTERY                              = 1;
    public static final int UPDATE_REBOOT                                    = 0;
    public static final int UPDATE_RECOVERY                                  = 1;
    public static final int UPDATE_UPDATE                                    = 2;
    public static final int UPDATE_OTA                                       = 3;

}
