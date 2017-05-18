package com.android.phone;

import com.android.internal.telephony.Call;
import com.android.internal.telephony.CallManager;
import com.android.internal.telephony.Phone;
import com.android.phone.PhoneApp;
import com.android.phone.PhoneUtils;

import android.app.IntentService;
import android.content.Intent;

import android.os.Message;
import android.os.Handler;

import android.util.Slog;
import android.util.CommonFunction;


/**
 * <p>
 *  
 * </p>
 * @author Wenyu Xu --Created on: 2017-04-20
 */
public class FleetyNetworkSettingService extends IntentService
{
	private static final String TAG = "FleetyTelephonyService";
	private Phone mPhone;
	private MyHandler mHandler;
    
	
    public FleetyNetworkSettingService()
    {
        super("FleetyNetworkSettingService");
    }

    @Override
    protected void onHandleIntent(Intent intent)
    {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

		FleetyUtils.LOGD("receive a request to set network mode");

		String modemNetworkMode = intent.getStringExtra("mode");

		Slog.i(TAG,"modemNetworkMode = " + modemNetworkMode);

		mPhone = PhoneGlobals.getPhone();
        mHandler = new MyHandler();
		
		//Phone mPhone = PhoneGlobals.getInstance().phone;
		//Message message = Message.obtain();
			

		//if (PhoneGlobals.getInstance().mPhoneServiceClient != null) {
			
			Slog.i(TAG,"modemNetworkMode111 = " + modemNetworkMode);
			
			//PhoneGlobals.getInstance().phone.setPrefNetwork(0, Integer.valueOf(modemNetworkMode), message);
        //} else {
       		//Slog.i(TAG,"modemNetworkMode222 = " + modemNetworkMode);
			
        	// Set the modem network mode
			// mPhone.setPreferredNetworkType(Integer.valueOf(modemNetworkMode),message);
    	//}

		mPhone.setPreferredNetworkType(Integer.valueOf(modemNetworkMode), mHandler
                        .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
		

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
    }

	
	private class MyHandler extends Handler {
	
			static final int MESSAGE_GET_PREFERRED_NETWORK_TYPE = 0;
			static final int MESSAGE_SET_PREFERRED_NETWORK_TYPE = 1;
			static final int MESSAGE_SET_PREFERRED_NETWORK_TYPE_WITH_ACQ = 2;
			static final int MESSAGE_SET_PREFERRED_LTE = 3;
	
			@Override
			public void handleMessage(Message msg) {
				
				Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
				
				Slog.i(TAG,"msg.what = " + msg.what);
				
				switch (msg.what) {
					case MESSAGE_GET_PREFERRED_NETWORK_TYPE:
						//handleGetPreferredNetworkTypeResponse(msg);
						break;
	
					case MESSAGE_SET_PREFERRED_NETWORK_TYPE:
						//handleSetPreferredNetworkTypeResponse(msg);
						break;
					case MESSAGE_SET_PREFERRED_NETWORK_TYPE_WITH_ACQ:
						//handleSetPreferredNetworkTypeWithAcqResponse();
						break;
						
					case MESSAGE_SET_PREFERRED_LTE:
						//handleSetPreferredLTEResponse();
						break;
				}
	
				
				Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
			}
		}
}

