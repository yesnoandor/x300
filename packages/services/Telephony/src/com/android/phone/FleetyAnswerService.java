package com.android.phone;

import android.app.ActivityManager;
import android.app.AppOpsManager;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.os.AsyncResult;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Process;
import android.os.ServiceManager;
import android.os.UserHandle;
import android.telephony.NeighboringCellInfo;
import android.telephony.CellInfo;
import android.telephony.ServiceState;
import android.text.TextUtils;
import android.util.Log;
import android.app.IntentService;

import com.android.internal.telephony.DefaultPhoneNotifier;
import com.android.internal.telephony.IccCard;
import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.CallManager;
import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.PhoneConstants;


import java.util.List;
import java.util.ArrayList;

/**
 * @author willkinson
 * @date Mar 12, 2012
 * @doc
 **/
public class FleetyAnswerService extends IntentService
{
    public FleetyAnswerService()
    {
        super("FleetyAnswerService");
    }

    protected void onHandleIntent(Intent intent)
    {
        FleetyUtils.LOGD("receive a request to answer the ringing call");
        answerRingingCallInternal();
    }


    /**
     * Make the actual telephony calls to implement answerRingingCall().
     * This should only be called from the main thread of the Phone app.
     * @see #answerRingingCall
     *
     * TODO: it would be nice to return true if we answered the call, or
     * false if there wasn't actually a ringing incoming call, or some
     * other error occurred.  (In other words, pass back the return value
     * from PhoneUtils.answerCall() or PhoneUtils.answerAndEndActive().)
     * But that would require calling this method via sendRequest() rather
     * than sendRequestAsync(), and right now we don't actually *need* that
     * return value, so let's just return void for now.
     */
    private void answerRingingCallInternal() {
        CallManager mCM=PhoneGlobals.getInstance().mCM;
        Phone mPhone=PhoneGlobals.getPhone();
        final boolean hasRingingCall = !mPhone.getRingingCall().isIdle();
        if (hasRingingCall) {
            final boolean hasActiveCall = !mPhone.getForegroundCall().isIdle();
            final boolean hasHoldingCall = !mPhone.getBackgroundCall().isIdle();
            if (hasActiveCall && hasHoldingCall) {
                // Both lines are in use!
                // TODO: provide a flag to let the caller specify what
                // policy to use if both lines are in use.  (The current
                // behavior is hardwired to "answer incoming, end ongoing",
                // which is how the CALL button is specced to behave.)
                PhoneUtils.answerAndEndActive(mCM, mCM.getFirstActiveRingingCall());
                return;
            } else {
                // answerCall() will automatically hold the current active
                // call, if there is one.
                PhoneUtils.answerCall(mCM.getFirstActiveRingingCall());
                return;
            }
        } else {
            // No call was ringing.
            return;
        }
    }




}
