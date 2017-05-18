package com.fleety.android.r600util.receiver;

import java.lang.reflect.Method;
import java.text.SimpleDateFormat;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.PowerManager;
import android.os.SystemClock;
import android.util.Log;

import com.fleety.android.action.R600UtilStrings;
import com.fleety.android.r600util.service.PackageInstaller;
import com.fleety.android.r600util.service.PackageUninstaller;
import com.fleety.android.util.CommonUtil;

/**
 * @author JohnFan
 * @date Mar 8, 2013
 * @doc
 */
public class R600Receiver extends BroadcastReceiver
{
    private static final String TAG = "R600Util -> ";

    @Override
    public void onReceive(Context context, Intent intent)
    {
        System.out.println("r600util onReceive");
        Log.i("test1","r600util onReceive");
        String cmd = intent.getStringExtra(R600UtilStrings.R600UTIL_CMD);
        if (CommonUtil.isBlankString(cmd))
        {
            System.err.println(TAG + "cmd is null");
            return;
        }
        if (R600UtilStrings.R600UTIL_CMD_REBOOT1.equals(cmd))
        {
            reboot1(context);
        }
        else if (R600UtilStrings.R600UTIL_CMD_REBOOT2.equals(cmd))
        {
            reboot2(context);
        }
        else if (R600UtilStrings.R600UTIL_CMD_TIMESET.equals(cmd)) // set system time
        {
            long millis = intent.getLongExtra(
                    R600UtilStrings.R600UTIL_DATA_TIMESET_MILLINS, -1);
            if (millis != -1)
            {
                SystemClock.setCurrentTimeMillis(millis);
                System.out.println(TAG
                        + "r600util set time to "
                        + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss")
                                .format(millis));
            }
        }
        else if (R600UtilStrings.R600UTIL_CMD_SET_PHONESPEAKER_MUTE_ON
                .equals(cmd))
        {
            setSpeakerMute(true);
        }
        else if (R600UtilStrings.R600UTIL_CMD_SET_PHONESPEAKER_MUTE_OFF
                .equals(cmd))
        {
            setSpeakerMute(false);
        }
        else if (R600UtilStrings.R600UTIL_CMD_INSTALL_APK.equals(cmd))
        {
            Intent i = new Intent(context, PackageInstaller.class);
            i.putExtras(intent.getExtras());
            context.startService(i);
        }
        else if (R600UtilStrings.R600UTIL_CMD_UNINSTALL_PACKAGE.equals(cmd))
        {
            Intent i = new Intent(context, PackageUninstaller.class);
            i.putExtras(intent.getExtras());
            context.startService(i);
        }
    }

    private void reboot1(Context context)
    {
        // reboot method 1
        Intent reboot = new Intent(Intent.ACTION_REBOOT);
        reboot.putExtra("nowait", 1);
        reboot.putExtra("interval", 1);
        reboot.putExtra("window", 0);
        context.sendBroadcast(reboot);

        System.out.println(TAG + "execute cmd--> reboot\n" + "reboot method 1");

    }

    private void reboot2(Context context)
    {
        // reboot method 2
        PowerManager pManager = (PowerManager) context
                .getSystemService(Context.POWER_SERVICE);
        pManager.reboot("reboot");
        System.out.println(TAG + "execute cmd--> reboot\n" + "reboot method 2");

    }

    private void setSpeakerMute(boolean needed)
    {
        try
        {
            Class clazz = Class.forName("android.os.SystemProperties");
            Method m = clazz.getMethod("set", new Class[] { String.class,
                    String.class });
            String value = needed ? 1 + "" : 0 + "";
            m.invoke(clazz, new Object[] { "r600.phone.speaker.mute", value });
            System.out.println(TAG + "SetSpeakerMute to " + value);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

}
