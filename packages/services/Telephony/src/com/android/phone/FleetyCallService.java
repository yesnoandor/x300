package com.android.phone;

import android.app.IntentService;
import android.content.Intent;
import android.net.Uri;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.CallManager;

import com.android.phone.OutgoingCallBroadcaster;
import com.android.phone.PhoneUtils;
import com.android.phone.Constants;

/**
 * <p>
 *  
 * </p>
 * @author John Fan --Created on: 2014-12-16
 */
public class FleetyCallService extends IntentService
{
    public FleetyCallService()
    {
        super("FleetyCallService");
    }

    @Override
    protected void onHandleIntent(Intent intent)
    {
        FleetyUtils.LOGD("receive a request to make phone call");
        String phoneNumber = intent
                .getStringExtra(FleetyConstants.PHONE_NUMBER);
        boolean mute = intent.getBooleanExtra(FleetyConstants.MUTE, false);
        Intent newIntent = new Intent(Intent.ACTION_CALL, Uri.parse("tel:"
                + phoneNumber));
        PhoneGlobals.getInstance().callController.placeCall(newIntent);
        FleetyUtils.LOGD("finish to call " + phoneNumber + ",mute:" + mute);
    }

}

