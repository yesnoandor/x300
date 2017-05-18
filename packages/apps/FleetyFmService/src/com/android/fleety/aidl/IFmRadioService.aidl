package com.android.fleety.aidl;

interface IFmRadioService{

	boolean continuePlay();
	boolean startPlay();
	boolean stopPlay();
	boolean setSpeakerOn(boolean flag);
	/**
     * open fm device, call before power up
     * 
     * @return (true,success; false, failed)
     */
    boolean opendev();

    /**
     * close fm device, call after power down
     * 
     * @return (true, success; false, failed)
     */
    boolean closedev();

    /**
     * power up FM with frequency use long antenna
     * 
     * @param frequency
     *            frequency(50KHZ, 87.55; 100KHZ, 87.5)
     * @return (true, success; false, failed)
     */
    boolean powerup(float frequency);

    /**
     * power down FM
     * 
     * @param type
     *            (0, FMRadio; 1, FMTransimitter)
     * @return (true, success; false, failed)
     */
    boolean powerdown(int type);

    /**
     * tune to frequency
     * 
     * @param frequency
     *            frequency(50KHZ, 87.55; 100KHZ, 87.5)
     * @return (true, success; false, failed)
     */
    boolean tune(float frequency);

    /**
     * seek with frequency in direction
     * 
     * @param frequency
     *            frequency(50KHZ, 87.55; 100KHZ, 87.5)
     * @param isUp
     *            (true, next channel; false previous channel)
     * @return frequency(float)
     */
    float seek(float frequency, boolean isUp);

    /**
     * auto scan(from 87.50-108.00)
     * 
     * @return scan channel array(int)
     */
    int[] autoscan();

    /**
     * stop scan, also can stop seek, other native when scan should call stop scan first, else will
     * execute wait auto scan finish
     * 
     * @return (true, can stop scan process; false, can't stop scan process)
     */
    boolean stopscan();

    /**
     * get rssi from hardware(use for engineer mode)
     * 
     * @return rssi value
     */
    int readRssi();

    /**
     * open or close rds fuction
     * 
     * @param rdson
     *            rdson (true, open; false, close)
     * @return
     */
    int rdsset(boolean rdson);

    /**
     * read rds events
     * 
     * @return rds event type
     */
    int readrds();

    /**
     * get program identification
     * 
     * @return program identification
     */
    int getPI();

    /**
     * get program type
     * 
     * @return program type
     */
    byte getPTY();

    /**
     * get program service(program name)
     * 
     * @return program name
     */
    byte[] getPS();

    /**
     * get radio text, RDS standard not support Chinese character
     * 
     * @return radio text(byte)
     */
    byte[] getLRText();

    /**
     * active alternative frequencies
     * 
     * @return frequency(float)
     */
      int activeAF();

    /**
     * get alternative frequency list
     * 
     * @return alternative frequency list(array)
     */
      int[] getAFList();

    /**
     * active traffic announcement
     * 
     * @return traffic announcement channel(int)
     */
      int activeTA();

    /**
     * deactive traffic announcement
     * 
     * @return the previous channel(int)
     */
      int deactiveTA();

    /**
     * mute or unmute FM voice
     * 
     * @param mute
     *            (true, mute; false, unmute)
     * @return (true, success; false, failed)
     */
      int setmute(boolean mute);

    /**
     * get chip id
     * 
     * @return chipId(0x6620, 0x6626 ,0x6628)
     */
      int getchipid();

    /**
     * Inquiry if RDS is support in driver
     * 
     * @return (1, support; 0, NOT support; -1, error)
     */
    //WCN Should change it
      int isRDSsupport();

    /**
     * Inquiry if FM is powered up if FMRX or FMTX power up, return true
     * 
     * @return (1, Powered up; 0, Did NOT powered up)
     */
     //WCN Should change it
      int isFMPoweredUp();

    /**
     * switch antenna
     * 
     * @param antenna
     *            antenna (0, long antenna, 1 int antenna)
     * @return (0, success; 1 failed; 2 not support)
     */
      int switchAntenna(int antenna);

    /**
     * Inquiry if fm stereo mono(true, stereo; false mono)
     * 
     * @return (true, stereo; false, mono)
     */
      boolean stereoMono();

    /**
     * Force set to stero/mono mode
     * 
     * @param isMono
     *            (true, mono; false, stereo)
     * @return (true, success; false, failed)
     */
      boolean setStereoMono(boolean isMono);

    /**
     * Read cap array of int antenna
     * 
     * @return cap array value
     */
      int readCapArray();

    /**
     * read rds bler
     * 
     * @return rds bler value
     */
      int readRdsBler();

    /**
     * FM over BT
     * 
     * @param bEnable
     *            (true,To enable FM over BT; false,To disable FM over BT)
     * @return (true,success; false, failed)
     */
      boolean setFMViaBTController(boolean bEnable);

    /**
     * get hardware version
     * 
     * @return hardware version information array(0, ChipId; 1, EcoVersion; 2, PatchVersion; 3,
     *         DSPVersion)
     */
      int[] getHardwareVersion();
      int[] scannew(int upper, int lower, int space);
      int seeknew(int upper, int lower, int space, int freq, int dir, int lev);
      boolean tunenew(int upper, int lower, int space, int freq);
    /**
     * send variables to native, and get some variables return.
     * @param val send to native
     * @return get value from native
     */
    int[] emcmd(in int[] val);
    /**
     * set RSSI, desense RSSI, mute gain soft
     * @param index flag which will execute
     * (0:rssi threshold,1:desense rssi threshold,2: SGM threshold)
     * @param value send to native
     * @return execute ok or not
     */
    boolean emsetth(int index, int value);
	
}