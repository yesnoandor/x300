package com.android.phone;

import com.android.internal.telephony.Call;
import com.android.internal.telephony.CallManager;
import com.android.internal.telephony.Phone;
import com.android.phone.PhoneApp;
import com.android.phone.PhoneUtils;

import android.app.IntentService;
import android.content.Intent;

/**
 * <p>
 *  
 * </p>
 * @author John Fan --Created on: 2014-12-16
 */
public class FleetyRejectService extends IntentService
{
    public FleetyRejectService()
    {
        super("FleetyRejectService");
    }

    @Override
    protected void onHandleIntent(Intent intent)
    {
        FleetyUtils.LOGD("receive a request to reject the ringing call");
        CallManager mCM =  PhoneGlobals.getInstance().mCM;
        PhoneUtils.hangup(mCM);
    }

}
