package com.android.phone;

import android.app.IntentService;
import android.content.Intent;
import android.util.Log;

/**
 * @author willkinson
 * @date Apr 12, 2012
 * @doc 
 **/
public class FleetyTestService1 extends IntentService
{

    public FleetyTestService1(String name)
    {
        super(name);
    }

    @Override
    protected void onHandleIntent(Intent arg0)
    {
        Log.v("InCallScreen", "FleetyTestService1.onHandleIntent");
    }
    
}
