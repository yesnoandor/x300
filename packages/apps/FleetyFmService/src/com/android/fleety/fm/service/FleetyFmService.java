package com.android.fleety.fm.service;

import java.io.IOException;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.media.AudioSystem;
import android.media.MediaPlayer;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.PowerManager;
import android.os.RemoteException;
import android.util.Log;

import com.android.fleety.aidl.IFmRadioService;

public class FleetyFmService extends Service implements
        MediaPlayer.OnErrorListener
{
    private static final String TAG             = "FleetyFmService";

    private AudioManager        mAudioManager   = null;
    private static final int    WHAT_START_PLAY = 1;
    private static final int    WHAT_STOP_PLAY  = 2;
    private static final int    WHAT_RE_PLAY    = 3;

    private Handler             handler         = new LocalHandler();

    @Override
    public IBinder onBind(Intent intent)
    {
        return mBinder;
    }

    @Override
    public void onCreate()
    {
        super.onCreate();

        Log.i(TAG, "onCreate");
        mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
	}

    public boolean continueFmPlay()
    {
        handler.sendEmptyMessage(WHAT_RE_PLAY);
        return true;
    }

    public boolean startFmPlay()
    {
        handler.sendEmptyMessage(WHAT_START_PLAY);
        return true;
    }

    public boolean stopFmPlay()
    {
        handler.sendEmptyMessage(WHAT_STOP_PLAY);
        return true;
    }

    class LocalHandler extends Handler
    {

        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case WHAT_RE_PLAY:
                {
                    Log.i(TAG, "WHAT_RE_PLAY");
                    setFmSpeakerPhoneOn(false);
                    AudioSystem.setDeviceConnectionState(AudioSystem.DEVICE_OUT_FM, AudioSystem.DEVICE_STATE_UNAVAILABLE, "");
                    break;
                }
                case WHAT_STOP_PLAY:
                {
                    Log.i(TAG, "WHAT_STOP_PLAY");
                    setFmSpeakerPhoneOn(false);
                    AudioSystem.setDeviceConnectionState(AudioSystem.DEVICE_OUT_FM, AudioSystem.DEVICE_STATE_UNAVAILABLE, "");
                    break;
                }
                case WHAT_START_PLAY:
                {
                    Log.i(TAG, "WHAT_START_PLAY");
                    try
                    {
                        setFmSpeakerPhoneOn(true);							
                        AudioSystem.setDeviceConnectionState(AudioSystem.DEVICE_OUT_FM, AudioSystem.DEVICE_STATE_AVAILABLE, "");
                    }
                    catch (Exception ex)
                    {
                        ex.printStackTrace();
                    }
                    break;
                }
            }
        }
    }

    private final IFmRadioService.Stub mBinder = new RadioServiceStub();

    class RadioServiceStub extends IFmRadioService.Stub
    {

        @Override
        public boolean continuePlay() throws RemoteException
        {
            return continueFmPlay();
        }

        @Override
        public boolean startPlay() throws RemoteException
        {
            FleetyFmServiceUtil.setmute(false);
            return startFmPlay();
        }

        @Override
        public boolean stopPlay() throws RemoteException
        {
            return stopFmPlay();
        }

        @Override
        public boolean setSpeakerOn(boolean flag) throws RemoteException
        {
            return setFmSpeakerPhoneOn(flag);
        }

        @Override
        public boolean tunenew(int upper, int lower, int space, int freq)
                throws RemoteException
        {
            return FleetyFmServiceUtil.tunenew(upper, lower, space, freq);
        }

        @Override
        public boolean tune(float frequency) throws RemoteException
        {
            Log.i(TAG, "tune " + frequency);
            return FleetyFmServiceUtil.tune(frequency);
        }

        @Override
        public int switchAntenna(int antenna) throws RemoteException
        {
            return FleetyFmServiceUtil.switchAntenna(antenna);
        }

        @Override
        public boolean stopscan() throws RemoteException
        {
            Log.i(TAG, "stopscan");
            return FleetyFmServiceUtil.stopscan();
        }

        @Override
        public boolean stereoMono() throws RemoteException
        {
            return FleetyFmServiceUtil.stereoMono();
        }

        @Override
        public int setmute(boolean mute) throws RemoteException
        {
            return FleetyFmServiceUtil.setmute(mute);
        }

        @Override
        public boolean setStereoMono(boolean isMono) throws RemoteException
        {
            return FleetyFmServiceUtil.setStereoMono(isMono);
        }

        @Override
        public boolean setFMViaBTController(boolean bEnable)
                throws RemoteException
        {
            return FleetyFmServiceUtil.setFMViaBTController(bEnable);
        }

        @Override
        public int seeknew(int upper, int lower, int space, int freq, int dir,
                int lev) throws RemoteException
        {
            return FleetyFmServiceUtil.seeknew(upper, lower, space, freq, dir,
                    lev);
        }

        @Override
        public float seek(float frequency, boolean isUp) throws RemoteException
        {
            return FleetyFmServiceUtil.seek(frequency, isUp);
        }

        @Override
        public int[] scannew(int upper, int lower, int space)
                throws RemoteException
        {
            short[] result = FleetyFmServiceUtil.scannew(upper, lower, space);
            int[] ret = new int[result.length];
            for (int i = 0; i < result.length; ++i)
            {
                ret[i] = result[i];
            }

            return ret;
        }

        @Override
        public int readrds() throws RemoteException
        {
            return FleetyFmServiceUtil.readrds();
        }

        @Override
        public int readRssi() throws RemoteException
        {
            return FleetyFmServiceUtil.readRssi();
        }

        @Override
        public int readRdsBler() throws RemoteException
        {
            return FleetyFmServiceUtil.readRdsBler();
        }

        @Override
        public int readCapArray() throws RemoteException
        {
            return FleetyFmServiceUtil.readCapArray();
        }

        @Override
        public int rdsset(boolean rdson) throws RemoteException
        {
            return FleetyFmServiceUtil.rdsset(rdson);
        }

        @Override
        public boolean powerup(float frequency) throws RemoteException
        {
            return FleetyFmServiceUtil.powerup(frequency);
        }

        @Override
        public boolean powerdown(int type) throws RemoteException
        {
            return FleetyFmServiceUtil.powerdown(type);
        }

        @Override
        public boolean opendev() throws RemoteException
        {
            Log.i(TAG, "opendev");
            return FleetyFmServiceUtil.opendev(FleetyFmService.this);
        }

        @Override
        public int isRDSsupport() throws RemoteException
        {
            return FleetyFmServiceUtil.isRDSsupport();
        }

        @Override
        public int isFMPoweredUp() throws RemoteException
        {
            return FleetyFmServiceUtil.isFMPoweredUp();
        }

        @Override
        public int getchipid() throws RemoteException
        {
            return FleetyFmServiceUtil.getchipid();
        }

        @Override
        public byte getPTY() throws RemoteException
        {
            return FleetyFmServiceUtil.getPTY();
        }

        @Override
        public byte[] getPS() throws RemoteException
        {
            return FleetyFmServiceUtil.getPS();
        }

        @Override
        public int getPI() throws RemoteException
        {
            return FleetyFmServiceUtil.getPI();
        }

        @Override
        public byte[] getLRText() throws RemoteException
        {
            return FleetyFmServiceUtil.getLRText();
        }

        @Override
        public int[] getHardwareVersion() throws RemoteException
        {
            return FleetyFmServiceUtil.getHardwareVersion();
        }

        @Override
        public int[] getAFList() throws RemoteException
        {
            short[] result = FleetyFmServiceUtil.getAFList();
            int[] ret = new int[result.length];
            for (int i = 0; i < result.length; ++i)
            {
                ret[i] = result[i];
            }

            return ret;
        }

        @Override
        public boolean emsetth(int index, int value) throws RemoteException
        {
            return FleetyFmServiceUtil.emsetth(index, value);
        }

        @Override
        public int[] emcmd(int[] val) throws RemoteException
        {
            short[] valBak = new short[val.length];
            for (int i = 0; i < valBak.length; ++i)
            {
                valBak[i] = (short) val[i];
            }
            short[] result = FleetyFmServiceUtil.emcmd(valBak);

            int[] ret = new int[result.length];
            for (int i = 0; i < result.length; ++i)
            {
                ret[i] = result[i];
            }

            return ret;
        }

        @Override
        public int deactiveTA() throws RemoteException
        {
            return FleetyFmServiceUtil.deactiveTA();
        }

        @Override
        public boolean closedev() throws RemoteException
        {
            Log.i(TAG, "closedev");
            return FleetyFmServiceUtil.closedev();
        }

        @Override
        public int[] autoscan() throws RemoteException
        {
            Log.i(TAG, "autoscan");

            return FleetyFmServiceUtil.autoscan();
        }

        @Override
        public int activeTA() throws RemoteException
        {
            return FleetyFmServiceUtil.activeTA();
        }

        @Override
        public int activeAF() throws RemoteException
        {
            return FleetyFmServiceUtil.activeAF();
        }
    };

    public boolean setFmSpeakerPhoneOn(boolean isSpeaker)
    {
        //int mForcedUseForMedia = isSpeaker ? AudioSystem.FORCE_SPEAKER:AudioSystem.FORCE_NONE;
		int mForcedUseForMedia = isSpeaker ? AudioSystem.FORCE_HEADPHONES:AudioSystem.FORCE_NONE;

		// fleety : Fixed a bug FM Stereo start 
		AudioSystem.setForceUse(AudioSystem.FOR_MEDIA, mForcedUseForMedia);
		//AudioSystem.setForceUse(AudioSystem.FOR_MEDIA, AudioSystem.FORCE_NONE);
        // fleety : Fixed a bug FM Stereo end
        
		return true;
    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra)
    {
        Log.i(TAG, "onError():+++++++++++++");
        Log.i(TAG, "what=" + what + ", extra=" + extra);
        if (MediaPlayer.MEDIA_ERROR_SERVER_DIED == what)
        {
            Log.i(TAG, "onError():MEDIA_ERROR_SERVER_DIED");
        }

        return true;
    }
    @Override
    public void onDestroy()
    {
        super.onDestroy();
    }

}
