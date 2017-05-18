package com.android.fleety.fm.service;

import android.content.Context;
import android.content.Intent;
import com.qcom.fmradio.FmReceiverJNI;
import android.util.Log;
import android.media.AudioSystem;

public class FleetyFmServiceUtil
{
	private static final String TAG = "FleetyFmServiceUtil";
	private static final String V4L2_DEVICE = "/dev/radio0";
	private static int fd;
	public static FleetyFmSrvEvListner mEvents = null;
	public static int [] mStations = new int[100];
	public static int mStationCnt = 0;
	public static boolean mSrchComplete = false;
	private static int mState = 0;

	private static final int low_band = 87500; 		// 87.5MHz
	private static final int high_band = 108000;    // 108MHz
	/* V4l2 Controls */
	private static final int V4L2_CID_PRIVATE_BASE						   = 0x8000000;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SRCHMODE			   = V4L2_CID_PRIVATE_BASE + 1;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SCANDWELL			   = V4L2_CID_PRIVATE_BASE + 2;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SRCHON 			   = V4L2_CID_PRIVATE_BASE + 3;
	private static final int V4L2_CID_PRIVATE_TAVARUA_STATE				   = V4L2_CID_PRIVATE_BASE + 4;
	private static final int V4L2_CID_PRIVATE_TAVARUA_TRANSMIT_MODE		   = V4L2_CID_PRIVATE_BASE + 5;
	private static final int V4L2_CID_PRIVATE_TAVARUA_RDSGROUP_MASK		   = V4L2_CID_PRIVATE_BASE + 6;
	private static final int V4L2_CID_PRIVATE_TAVARUA_REGION 			   = V4L2_CID_PRIVATE_BASE + 7;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SIGNAL_TH			   = V4L2_CID_PRIVATE_BASE + 8;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SRCH_PTY			   = V4L2_CID_PRIVATE_BASE + 9;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SRCH_PI			   = V4L2_CID_PRIVATE_BASE + 10;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SRCH_CNT			   = V4L2_CID_PRIVATE_BASE + 11;
	private static final int V4L2_CID_PRIVATE_TAVARUA_EMPHASIS			   = V4L2_CID_PRIVATE_BASE + 12;
	private static final int V4L2_CID_PRIVATE_TAVARUA_RDS_STD			   = V4L2_CID_PRIVATE_BASE + 13;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SPACING			   = V4L2_CID_PRIVATE_BASE + 14;
	private static final int V4L2_CID_PRIVATE_TAVARUA_RDSON				   = V4L2_CID_PRIVATE_BASE + 15;
	private static final int V4L2_CID_PRIVATE_TAVARUA_RDSGROUP_PROC		   = V4L2_CID_PRIVATE_BASE + 16;
	private static final int V4L2_CID_PRIVATE_TAVARUA_LP_MODE			   = V4L2_CID_PRIVATE_BASE + 17;
	private static final int V4L2_CID_PRIVATE_TAVARUA_RDSD_BUF			   = V4L2_CID_PRIVATE_BASE + 19;
	private static final int V4L2_CID_PRIVATE_TAVARUA_IOVERC 			   = V4L2_CID_PRIVATE_BASE + 24;
	private static final int V4L2_CID_PRIVATE_TAVARUA_INTDET 			   = V4L2_CID_PRIVATE_BASE + 25;
	private static final int V4L2_CID_PRIVATE_TAVARUA_MPX_DCC			   = V4L2_CID_PRIVATE_BASE + 26;
	private static final int V4L2_CID_PRIVATE_TAVARUA_AF_JUMP			   = V4L2_CID_PRIVATE_BASE + 27;
	private static final int V4L2_CID_PRIVATE_TAVARUA_RSSI_DELTA 		   = V4L2_CID_PRIVATE_BASE + 28;
	private static final int V4L2_CID_PRIVATE_TAVARUA_HLSI				   = V4L2_CID_PRIVATE_BASE + 29;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SET_AUDIO_PATH 	   = V4L2_CID_PRIVATE_BASE + 41;
	private static final int V4L2_CID_PRIVATE_SINR						   = V4L2_CID_PRIVATE_BASE + 44;
	private static final int V4L2_CID_PRIVATE_TAVARUA_ON_CHANNEL_THRESHOLD  = V4L2_CID_PRIVATE_BASE + 0x2D;
	private static final int V4L2_CID_PRIVATE_TAVARUA_OFF_CHANNEL_THRESHOLD = V4L2_CID_PRIVATE_BASE + 0x2E;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SINR_THRESHOLD 	   = V4L2_CID_PRIVATE_BASE + 0x2F;
	private static final int V4L2_CID_PRIVATE_TAVARUA_SINR_SAMPLES		   = V4L2_CID_PRIVATE_BASE + 0x30;
	private static final int V4L2_CID_PRIVATE_SPUR_FREQ					   = V4L2_CID_PRIVATE_BASE + 0x31;
	private static final int V4L2_CID_PRIVATE_SPUR_FREQ_RMSSI			   = V4L2_CID_PRIVATE_BASE + 0x32;
	private static final int V4L2_CID_PRIVATE_SPUR_SELECTION 			   = V4L2_CID_PRIVATE_BASE + 0x33;
	private static final int V4L2_CID_PRIVATE_AF_RMSSI_TH				   = V4L2_CID_PRIVATE_BASE + 0x36;
	private static final int V4L2_CID_PRIVATE_AF_RMSSI_SAMPLES			   = V4L2_CID_PRIVATE_BASE + 0x37;
	private static final int V4L2_CID_PRIVATE_GOOD_CH_RMSSI_TH			   = V4L2_CID_PRIVATE_BASE + 0x38;
	private static final int V4L2_CID_PRIVATE_SRCHALGOTYPE				   = V4L2_CID_PRIVATE_BASE + 0x39;
	private static final int V4L2_CID_PRIVATE_CF0TH12					   = V4L2_CID_PRIVATE_BASE + 0x3A;
	private static final int V4L2_CID_PRIVATE_SINRFIRSTSTAGE 			   = V4L2_CID_PRIVATE_BASE + 0x3B;
	private static final int V4L2_CID_PRIVATE_RMSSIFIRSTSTAGE			   = V4L2_CID_PRIVATE_BASE + 0x3C;
	private static final int V4L2_CID_PRIVATE_RXREPEATCOUNT				   = V4L2_CID_PRIVATE_BASE + 0x3D;
	private static final int V4L2_CID_PRIVATE_RSSI_TH					   = V4L2_CID_PRIVATE_BASE + 0x3E;
	private static final int V4L2_CID_PRIVATE_AF_JUMP_RSSI_TH			   = V4L2_CID_PRIVATE_BASE + 0x3F;
	private static final int V4L2_CID_PRIVATE_BLEND_SINRHI				   = V4L2_CID_PRIVATE_BASE + 0x40;
	private static final int V4L2_CID_PRIVATE_BLEND_RMSSIHI				   = V4L2_CID_PRIVATE_BASE + 0x41;

    /**
     * open fm device, call before power up
     * 
     * @return (true,success; false, failed)
     */
    public static boolean opendev(Context context)
    {
        //return FMRadioNative.opendev();
        
        fd = FmReceiverJNI.acquireFdNative(V4L2_DEVICE);
		Log.i(TAG, "opendev :" + fd);
		if(fd <= 0)
		{
			Log.i(TAG, "opendev-----acquireFdNative failed");
			return false;
		}
		Log.i(TAG, "opendev-----open file success");
		
		// start fm event listner
		if(mEvents == null)
    	{
			mEvents = new FleetyFmSrvEvListner();
			mEvents.startListner(fd);
		}
		else
		{
			mEvents.startListner(fd);
		}
		
		// qcom fm setup
		// set fm state  --> on   [1:on   0:off]
		FmReceiverJNI.setControlNative(fd, 0x8000004, 1);
        
		// set the audio path as digital [1:analog  0:digital]
        FmReceiverJNI.setControlNative(fd, 0x8000029, 0);

		FmReceiverJNI.SetCalibrationNative(fd);

		// set FM pre-emphasis/de-emphasis [0:FM_DE_EMP75 1:FM_DE_EMP50 ]
        FmReceiverJNI.setControlNative(fd, 0x800000c, 1);

		// set RDS standard type [ 0:RBDS (North America)  1:RDS (Rest of the world)]
        FmReceiverJNI.setControlNative(fd, 0x800000d, 1);

		// set FM channel spacing[0:FM_CHSPACE_200_KHZ 1:FM_CHSPACE_100_KHZ 2:FM_CHSPACE_50_KHZ]
        FmReceiverJNI.setControlNative(fd, 0x800000e, 1);

		// set FM search algorithm [0:old 1:new]
        FmReceiverJNI.setControlNative(fd, 0x800002b, 0);

        // set search band range
        FmReceiverJNI.setBandNative(fd, low_band, high_band);

        FmReceiverJNI.setControlNative(fd, 0x8000007, 4);

        // set fm working mode[0:normal mode 1:low power mode]
        FmReceiverJNI.setControlNative(fd, 0x8000011, 0);

		// turn on/off RDS processing [0:off 1:on]
		FmReceiverJNI.setControlNative(fd, 0x800000f, 1);

        FmReceiverJNI.setControlNative(fd, 0x8000010, 0xfffffff8);

        FmReceiverJNI.setControlNative(fd, 0x8000014, 1);

		// turn on/off RDS processing [0:off 1:on]
        FmReceiverJNI.setControlNative(fd, 0x800000f, 1);

        FmReceiverJNI.setControlNative(fd, 0x800001b, 1);

        FmReceiverJNI.setControlNative(fd, 0x8000012, 0);

		mState = 1;
		Log.i(TAG, "opendev-----acquireFdNative success");
		
		return true;
    }

    /**
     * close fm device, call after power down
     * 
     * @return (true, success; false, failed)
     */
    public static boolean closedev()
    {
        //return FMRadioNative.closedev();
		Log.i(TAG, "closedev");

		// set fm state  --> off   [1:on   0:off]
        FmReceiverJNI.setControlNative(fd, 0x8000004, 0);
        Log.i(TAG, "closedev-----set fm state:off");
		mState = 0;
				
        return true;
    }

    /**
     * power up FM with frequency use long antenna
     * 
     * @param frequency
     *            frequency(50KHZ, 87.55; 100KHZ, 87.5)
     * @return (true, success; false, failed)
     */
    public static boolean powerup(float frequency)
    {
        //return FMRadioNative.powerup(frequency);	

		Log.i(TAG, "powerup-----frequency:" + frequency);
		
		// add by fleety jie.liu 20121202 start
        FmReceiverJNI.setFreqNative(fd, (int)(frequency*1000));
		// add by fleety jie.liu 20121202 end
		
		return true;
    }

    /**
     * power down FM
     * 
     * @param type
     *            (0, FMRadio; 1, FMTransimitter)
     * @return (true, success; false, failed)
     */
    public static boolean powerdown(int type)
    {
        //return FMRadioNative.powerdown(type);
		Log.i(TAG, "powerdown");
        return true;
    }

    /**
     * tune to frequency
     * 
     * @param frequency
     *            frequency(50KHZ, 87.55; 100KHZ, 87.5)
     * @return (true, success; false, failed)
     */
    public static boolean tune(float frequency)
    {
        //return FMRadioNative.tune(frequency);
		int freq_hz;
		int ret;

		Log.i(TAG, "tune-----frequency:" + frequency + "MHz");

		freq_hz = (int)(frequency*1000);
		
		Log.i(TAG, "tune-----setFreqNative:" + freq_hz + "KHz");
        ret = FmReceiverJNI.setFreqNative(fd, freq_hz);
		if(ret == 0)
		{
			Log.i(TAG, "tune-----setFreqNative success");
			return true;
		}
		else
		{
			Log.i(TAG, "tune-----setFreqNative failed");
			return false;
		}
    }

    /**
     * seek with frequency in direction
     * 
     * @param frequency
     *            frequency(50KHZ, 87.55; 100KHZ, 87.5)
     * @param isUp
     *            (true, next channel; false previous
     *            channel)
     * @return frequency(float)
     */
    public static float seek(float frequency, boolean isUp)
    {
        //return FMRadioNative.seek(frequency, isUp);
		return 0;
    }

    /**
     * auto scan(from 87.50-108.00)
     * 
     * @return scan channel array(short)
     */
    public static int[] autoscan()
    {
	   /*
	        FMRadioNative.powerup((float) 100.0);
	        FMRadioNative.rdsset(false);
	        FMRadioNative.setmute(true);
	        short[] result = FMRadioNative.autoscan();

	        if (result == null)
	        {
	            return new int[0];
	        }

	        int[] ret = new int[result.length];
	        for (int i = 0; i < result.length; ++i)
	        {
	            ret[i] = result[i];
	        }
	        FMRadioNative.rdsset(true);
	        FMRadioNative.setmute(false);

	        return ret;
	   */
		int freq_hz;
		int jni_ret;

		Log.i(TAG, "autoscan");

		freq_hz = FmReceiverJNI.getFreqNative(fd);
		Log.i(TAG, "getFreqNative:" + freq_hz);

		jni_ret = FmReceiverJNI.setMonoStereoNative(fd,1);
		Log.d(TAG,"setMonoStereoNative:"+jni_ret);

		jni_ret = FmReceiverJNI.setFreqNative(fd, low_band);
		Log.i(TAG, "setFreqNative_ret:" + jni_ret);

		FmReceiverJNI.setControlNative(fd, 0x8000001, 1);
		FmReceiverJNI.setControlNative(fd, 0x8000002, 2);
		FmReceiverJNI.startSearchNative(fd, 1);

		mStationCnt = 0;
		mSrchComplete = false;
		Log.i(TAG, "wait search.. mStationCnt=" + mStationCnt);
		while(!mSrchComplete)
		{
			// wait search complete
		}
		Log.i(TAG, "search complete: mStationCnt=" + mStationCnt);
		if(mStationCnt == 0)
		{
			Log.i(TAG, "search complete: no station found..");
			return new int[0];
		}
		int[] ret = new int[mStationCnt];
		for (int i = 0; i < mStationCnt; ++i)
		{
			ret[i] = mStations[i];
			Log.i(TAG, "search complete: ret[i]=" + ret[i]);
		}

		return ret;

	}

    /**
     * stop scan, also can stop seek, other native when scan
     * should call stop scan first, else will execute wait
     * auto scan finish
     * 
     * @return (true, can stop scan process; false, can't
     *         stop scan process)
     */
    public static boolean stopscan()
    {
        //return FMRadioNative.stopscan();
		int ret;
		
		Log.i(TAG, "stopscan");
		ret = FmReceiverJNI.cancelSearchNative(fd);
		if(ret == 0)
		{
			Log.i(TAG, "stopscan-----cancelSearchNative success");
			return true;
		}
		else
		{
			Log.i(TAG, "stopscan-----cancelSearchNative failed");
			return false;
		}
    }

    /**
     * get rssi from hardware(use for engineer mode)
     * 
     * @return rssi value
     */
    public static int readRssi()
    {
        //return FMRadioNative.readRssi();
		return 1;
    }

    /**
     * open or close rds fuction
     * 
     * @param rdson
     *            rdson (true, open; false, close)
     * @return
     */
    public static int rdsset(boolean rdson)
    {
        //return FMRadioNative.rdsset(rdson);
		return 1;
    }

    /**
     * read rds events
     * 
     * @return rds event type
     */
    public static short readrds()
    {
        //return FMRadioNative.readrds();
		return 1;
    }

    /**
     * get program identification
     * 
     * @return program identification
     */
    public static short getPI()
    {
        //return FMRadioNative.getPI();
		return 1;
    }

    /**
     * get program type
     * 
     * @return program type
     */
    public static byte getPTY()
    {
        //return FMRadioNative.getPTY();
		return 1;
    }

    /**
     * get program service(program name)
     * 
     * @return program name
     */
    public static byte[] getPS()
    {
        //return FMRadioNative.getPS();
		return new byte[0];
    }

    /**
     * get radio text, RDS standard not support Chinese
     * character
     * 
     * @return radio text(byte)
     */
    public static byte[] getLRText()
    {
        //return FMRadioNative.getLRText();
		return new byte[0];
    }

    /**
     * active alternative frequencies
     * 
     * @return frequency(float)
     */
    public static short activeAF()
    {
        //return FMRadioNative.activeAF();
		return 1;
    }

    /**
     * get alternative frequency list
     * 
     * @return alternative frequency list(array)
     */
    public static short[] getAFList()
    {
        //return FMRadioNative.getAFList();
		return new short[0];	
    }

    /**
     * active traffic announcement
     * 
     * @return traffic announcement channel(short)
     */
    public static short activeTA()
    {
        //return FMRadioNative.activeTA();
		return 1;
    }

    /**
     * deactive traffic announcement
     * 
     * @return the previous channel(short)
     */
    public static short deactiveTA()
    {
        //return FMRadioNative.deactiveTA();
		return 1;
    }

    /**
     * mute or unmute FM voice
     * 
     * @param mute
     *            (true, mute; false, unmute)
     * @return (true, success; false, failed)
     */
    public static int setmute(boolean mute)
    {
        //return FMRadioNative.setmute(mute);
		return 1;
    }

    /**
     * get chip id
     * 
     * @return chipId(0x6620, 0x6626 ,0x6628)
     */
    public static int getchipid()
    {
        //return FMRadioNative.getchipid();
		return 0x6620;
    }

    /**
     * Inquiry if RDS is support in driver
     * 
     * @return (1, support; 0, NOT support; -1, error)
     */
    // WCN Should change it
    public static int isRDSsupport()
    {
        //return FMRadioNative.isRDSsupport();
		return 1;
    }

    /**
     * Inquiry if FM is powered up if FMRX or FMTX power up,
     * return true
     * 
     * @return (1, Powered up; 0, Did NOT powered up)
     */
    // WCN Should change it
    public static int isFMPoweredUp()
    {
        //return FMRadioNative.isFMPoweredUp();
		return mState;
    }

    /**
     * switch antenna
     * 
     * @param antenna
     *            antenna (0, long antenna, 1 short antenna)
     * @return (0, success; 1 failed; 2 not support)
     */
    public static int switchAntenna(int antenna)
    {
        //return FMRadioNative.switchAntenna(antenna);
		return 1;
    }

    /**
     * Inquiry if fm stereo mono(true, stereo; false mono)
     * 
     * @return (true, stereo; false, mono)
     */
    public static boolean stereoMono()
    {
        //return FMRadioNative.stereoMono();
		return true;
    }

    /**
     * Force set to stero/mono mode
     * 
     * @param isMono
     *            (true, mono; false, stereo)
     * @return (true, success; false, failed)
     */
    public static boolean setStereoMono(boolean isMono)
    {
        //return FMRadioNative.setStereoMono(isMono);
		return true;
    }

    /**
     * Read cap array of short antenna
     * 
     * @return cap array value
     */
    public static short readCapArray()
    {
        //return FMRadioNative.readCapArray();
		return 1;
    }

    /**
     * read rds bler
     * 
     * @return rds bler value
     */
    public static short readRdsBler()
    {
        //return FMRadioNative.readRdsBler();
		return 1;
    }

    /**
     * FM over BT
     * 
     * @param bEnable
     *            (true,To enable FM over BT; false,To
     *            disable FM over BT)
     * @return (true,success; false, failed)
     */
    public static boolean setFMViaBTController(boolean bEnable)
    {
        //return FMRadioNative.setFMViaBTController(bEnable);
		return true;
    }

    /**
     * get hardware version
     * 
     * @return hardware version information array(0, ChipId;
     *         1, EcoVersion; 2, PatchVersion; 3,
     *         DSPVersion)
     */
    public static int[] getHardwareVersion()
    {
        //return FMRadioNative.getHardwareVersion();
		return new int[0];
    }

    public static short[] scannew(int upper, int lower, int space)
    {
        //return FMRadioNative.scannew(upper, lower, space);
		return new short[0];
    }

    public static int seeknew(int upper, int lower, int space, int freq,
            int dir, int lev)
    {
        //return FMRadioNative.seeknew(upper, lower, space, freq, dir, lev);
		return 1;
    }

    public static boolean tunenew(int upper, int lower, int space, int freq)
    {
        //return FMRadioNative.tunenew(upper, lower, space, freq);
		return true;
    }

    /**
     * send variables to native, and get some variables
     * return.
     * 
     * @param val
     *            send to native
     * @return get value from native
     */
    public static short[] emcmd(short[] val)
    {
        //return FMRadioNative.emcmd(val);
		return new short[0];
    }

    /**
     * set RSSI, desense RSSI, mute gain soft
     * 
     * @param index
     *            flag which will execute (0:rssi
     *            threshold,1:desense rssi threshold,2: SGM
     *            threshold)
     * @param value
     *            send to native
     * @return execute ok or not
     */
    public static boolean emsetth(int index, int value)
    {
        //return FMRadioNative.emsetth(index, value);
		return true;
    }
}
