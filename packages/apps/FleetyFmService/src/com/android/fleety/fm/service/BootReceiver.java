package com.android.fleety.fm.service;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootReceiver extends BroadcastReceiver
{
    public static final String ACTION_BOOT     = "android.intent.action.BOOT_COMPLETED";
    public static final String ACTION_FM_CLOSE = "action.fm.radio.close";

    @Override
    public void onReceive(Context context, Intent intent)
    {
        String action = intent.getAction();

        if (action.equals(ACTION_BOOT))
        {
            Log.i("BootReceiver", "Starting FM service");
            try
            {
                context.startService(new Intent(
                        "com.android.fleety.aidl.IFmRadioService"));
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }
        else if (action.equals(ACTION_FM_CLOSE))
        {
            Log.i("BootReceiver", "Stopping FM service");
            try
            {
                context.stopService(new Intent(
                        "com.android.fleety.aidl.IFmRadioService"));
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }
    }

}
