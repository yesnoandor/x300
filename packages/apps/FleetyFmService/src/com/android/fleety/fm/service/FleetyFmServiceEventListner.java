package com.android.fleety.fm.service;
import com.qcom.fmradio.FmReceiverJNI;
import java.util.Arrays;
import android.util.Log;


class FleetyFmSrvEvListner {

    private final int EVENT_LISTEN = 1;

    private final int STD_BUF_SIZE = 256;

    private enum FmRxEvents {
      READY_EVENT,
      TUNE_EVENT,
      SEEK_COMPLETE_EVENT,
      SCAN_NEXT_EVENT,
      RAW_RDS_EVENT,
      RT_EVENT,
      PS_EVENT,
      ERROR_EVENT,
      BELOW_TH_EVENT,
      ABOVE_TH_EVENT,
      STEREO_EVENT,
      MONO_EVENT,
      RDS_AVAL_EVENT,
      RDS_NOT_AVAL_EVENT,
      TAVARUA_EVT_NEW_SRCH_LIST,
      TAVARUA_EVT_NEW_AF_LIST
    }

    private FMThread mThread;
    private static final String TAG = "FleetyFmSrvEvListner";

    public void startListner (final int fd) {
    		this.stopListener();
        /* start a thread and listen for messages */
        mThread = new FMThread(fd);
        mThread.start();
    }

    public void stopListener(){
        //mThread.stop();
        //Thread stop is deprecate API
        //Interrupt the thread and check for the thread status
        // and return from the run() method to stop the thread
        //properly
        Log.d( TAG,  "stopping the Listener\n");
        if(mThread != null){
      		mThread.isStop = true;
      		mThread.interrupt();
      	}
    }

	private class FMThread extends Thread{
		boolean isStop = false;
		int fd = 0;
			public FMThread(int _fd){
				this.fd = _fd;
			}
            public void run(){
                byte [] buff = new byte[STD_BUF_SIZE];
                Log.d(TAG, "Starting listener .. mStationCnt = " + FleetyFmServiceUtil.mStationCnt);

                while (!isStop) {
										
                    try {
                        int index = 0;
                        int state = 0;
                        Arrays.fill(buff, (byte)0x00);
                        int freq = 0;
                        int eventCount = FmReceiverJNI.getBufferNative (fd, buff, EVENT_LISTEN);

                        if (eventCount >= 0){
                          Log.i(TAG, "Received event. Count: " + eventCount);
                        }else{
                        	this.isStop = true;
                        	FmReceiverJNI.closeFdNative(fd);
                        	continue;
                        }

                        for (  index = 0; index < eventCount; index++ ) {
                            Log.i(TAG, "Received <" +buff[index]+ ">" );

                            switch(buff[index]){
                            case 0:
                                Log.i(TAG, "Got READY_EVENT");
                                // set rds group mask
                                FmReceiverJNI.setControlNative(fd, 0x8000006, 0x40);
                                FmReceiverJNI.configurePerformanceParams(fd);
                                break;
                            case 1:
                                Log.i(TAG, "Got TUNE_EVENT");
								if(FleetyFmServiceUtil.mSrchComplete == false)
								{
									Log.i(TAG, "Got TUNE_EVENT::" + FleetyFmServiceUtil.mStationCnt);
									FleetyFmServiceUtil.mStations[FleetyFmServiceUtil.mStationCnt] = FmReceiverJNI.getFreqNative(fd);
									Log.i(TAG, "freq["+FleetyFmServiceUtil.mStationCnt+"]= " + FleetyFmServiceUtil.mStations[FleetyFmServiceUtil.mStationCnt]);
									FleetyFmServiceUtil.mStationCnt = FleetyFmServiceUtil.mStationCnt + 1;								
								}
                                break;
                            case 2:
                                Log.d(TAG, "Got SEEK_COMPLETE_EVENT");					
								Log.d(TAG, "Got SEEK_COMPLETE_EVENT------freq:" + FmReceiverJNI.getFreqNative(fd));
								FleetyFmServiceUtil.mSrchComplete = true;
									
                                break;
                            case 3:
                                Log.d(TAG, "Got SCAN_NEXT_EVENT");
                                break;
                            case 4:
                                Log.d(TAG, "Got RAW_RDS_EVENT");
                                break;
                            case 5:
                                Log.d(TAG, "Got RT_EVENT");
                                break;
                            case 6:
                                Log.d(TAG, "Got PS_EVENT");
                                break;
                            case 7:
                                Log.d(TAG, "Got ERROR_EVENT");
                                break;
                            case 8:
                                Log.d(TAG, "Got BELOW_TH_EVENT");
								Log.d(TAG, "Got BELOW_TH_EVENT------freq:" + FmReceiverJNI.getFreqNative(fd));
                                break;
                            case 9:
                                Log.d(TAG, "Got ABOVE_TH_EVENT");
                                break;
                            case 10:
                                Log.d(TAG, "Got STEREO_EVENT");
                                break;
                            case 11:
                                Log.d(TAG, "Got MONO_EVENT");
                                break;
                            case 12:
                                Log.d(TAG, "Got RDS_AVAL_EVENT");
                                break;
                            case 13:
                                Log.d(TAG, "Got RDS_NOT_AVAL_EVENT");
                                break;
                            case 14:
                                Log.d(TAG, "Got NEW_SRCH_LIST");
                                break;
                            case 15:
                                Log.d(TAG, "Got NEW_AF_LIST");
                                break;
                            case 18:
                                Log.d(TAG, "Got RADIO_DISABLED");
                                isStop = true;
                                int ret = FmReceiverJNI.closeFdNative(fd);
                                if(ret == 0)
                                {
                                    Log.i(TAG, "closeFdNative success");
                                }
                                else
                                {
                                    Log.i(TAG, "closeFdNative failed");
                                }
                                break;
                            case 19:
                                Log.d(TAG, "Got EVENT_19");
                                FmReceiverJNI.setControlNative(fd, 0x8000006, 0);
                                break;
                            case 20:
                                Log.d(TAG, "got RT plus event");
                                break;
                            case 21:
                                Log.d(TAG, "got eRT event");
                                break;
                            case 22:
                                Log.d(TAG, "got IRIS_EVT_SPUR_TBL event");
                                break;
                            default:
                                Log.d(TAG, "Unknown event");
                                break;
                            }
                        }//end of for
                    } catch ( Exception ex ) {
                        Log.d( TAG,  "RunningThread InterruptedException");
                        ex.printStackTrace();
                        Thread.currentThread().interrupt();
                    }
                }
            }
		
	}
}
