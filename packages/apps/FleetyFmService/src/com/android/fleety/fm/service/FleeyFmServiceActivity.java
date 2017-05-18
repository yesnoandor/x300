package com.android.fleety.fm.service;

import com.android.fleety.aidl.IFmRadioService;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.text.InputType;
import android.util.Log;
import android.widget.EditText;

public class FleeyFmServiceActivity extends Activity
{
    private boolean           mIsRemoteBound    = false;
    private IFmRadioService   mRemoteService;
    private ServiceConnection mRemoteConnection = new ServiceConnection() {
                                                    public void onServiceConnected(
                                                            ComponentName className,
                                                            IBinder service)
                                                    {
                                                        mRemoteService = IFmRadioService.Stub
                                                                .asInterface(service);
                                                    }

                                                    public void onServiceDisconnected(
                                                            ComponentName className)
                                                    {
                                                        mRemoteService = null;
                                                    }
                                                };

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        EditText input = (EditText) findViewById(R.id.input_frequency);
        input.setInputType(InputType.TYPE_CLASS_PHONE);

        if (!mIsRemoteBound)
        {
            bindService(new Intent("com.android.fleety.aidl.IFmRadioService"),
                    mRemoteConnection, Context.BIND_AUTO_CREATE);
            mIsRemoteBound = true;
        }
    }
}