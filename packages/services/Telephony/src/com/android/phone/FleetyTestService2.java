package com.android.phone;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

/**
 * @author willkinson
 * @date Apr 12, 2012
 * @doc 
 **/
public class FleetyTestService2 extends Service
{

    @Override
    public IBinder onBind(Intent arg0)
    {
        return null;
    }
    
    @Override
    public void onCreate()
    {
        super.onCreate();
        Log.v("InCallScreen", "FleetyTestService2.onCreate");
        stopSelf();
    }

}
