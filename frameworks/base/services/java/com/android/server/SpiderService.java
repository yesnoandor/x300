package com.android.server;

import android.content.Context;
import android.os.ISpiderService;
import android.util.Slog;


import com.android.server.power.PowerManagerService;
import android.os.PowerManager;
import android.os.ServiceManager;
import android.os.SystemClock;


import android.util.CommonFunction;



public class SpiderService extends ISpiderService.Stub {
	private static final String TAG = "SpiderService";

	private final PowerManager mPm;
	private Context mScorpionContext;

	private int gpo_map;
	private int gpi_map;
	
	private static final int RELAY1_GPIO_OUT_INDEX = 0;
	private static final int RELAY2_GPIO_OUT_INDEX = 1;
	private static final int RELAY3_GPIO_OUT_INDEX = 2;
	private static final int RELAY4_GPIO_OUT_INDEX = 3;
	private static final int AMP_STB_GPIO_OUT_INDEX = 4;
	private static final int AMP_MUTE_GPIO_OUT_INDEX = 5;
	private static final int OTG_SWITCH_GPIO_OUT_INDEX= 6;
	private static final int USB2ETH_RST_GPIO_OUT_INDEX = 7;
	private static final int USBHUB_RST_GPIO_OUT_INDEX = 8;
	private static final int STM32_RST_GPIO_OUT_INDEX = 9;
	private static final int CAMERA_POWER_GPIO_OUT_INDEX= 10;
	private static final int TACHOGRAPH_POWER_GPIO_OUT_INDEX = 11;
	private static final int EX_POWER_GPIO_OUT_INDEX = 12;
	private static final int FINGER_POWER_GPIO_OUT_INDEX = 13;
	private static final int TSM_POWER_GPIO_OUT_INDEX = 14;
	private static final int FAN_POWER_GPIO_OUT_INDEX = 15;


	SpiderService(Context context) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

		gpo_map = get_gpio_out_map_native();;
		gpi_map = get_gpio_in_map_native();
		
		init_native();

		mPm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	protected int common_map_treat(int map,int index,int level){		
		if(level != 0)
			map |= 0x01 << index;
		else
			map &= ~(0x01 << index);

		return map;
	}
	
	public void set_app_ready(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		set_app_ready_native(val);

		gpo_map = get_gpio_out_map();
		gpi_map = get_gpio_in_map();

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}
	
	public void set_heart_beat(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		set_heart_beat_native(val);

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_relay1_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		set_relay1_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,RELAY1_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_relay2_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
				
		set_relay2_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,RELAY2_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
		
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	
	public void set_relay3_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
			
		set_relay3_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,RELAY3_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
		
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}
	
	public void set_relay4_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		set_relay4_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,RELAY4_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_amp_stb_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		set_amp_stb_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,AMP_STB_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);	

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_amp_mute_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
				
		set_amp_mute_gpio_native(val);
		
		gpo_map = common_map_treat(gpo_map,AMP_MUTE_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);	
		
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_otg_switch_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		set_otg_switch_gpio_native(val);
		
		gpo_map = common_map_treat(gpo_map,OTG_SWITCH_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);	
			
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

		
	public void set_usb2eth_rst_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
			
		set_usb2eth_rst_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,USB2ETH_RST_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);	
	
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}
	
	public void set_usbhub_rst_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		set_usbhub_rst_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,USBHUB_RST_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
			
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_stm32_rst_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		set_stm32_rst_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,STM32_RST_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
			
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_camera_power_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		set_camera_power_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,CAMERA_POWER_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
	
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_tachograph_power_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
				
		set_tachograph_power_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,TACHOGRAPH_POWER_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
	
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_ex_power_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
			
		set_ex_power_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,EX_POWER_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
	
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}
	
	public void set_finger_power_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		set_finger_power_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,FINGER_POWER_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
	
			
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}
	
	public void set_tsm_power_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
			
		set_tsm_power_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,TSM_POWER_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
	
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}
	
	public void set_fan_power_gpio(int val) {
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		set_fan_power_gpio_native(val);

		gpo_map = common_map_treat(gpo_map,FAN_POWER_GPIO_OUT_INDEX,val);
		set_gpio_out_map(gpo_map);
	
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void set_gpio_out_map(int val){
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		set_gpio_out_map_native(val);
			
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public int get_gpio_out_map(){
		int val;
		
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		val = get_gpio_out_map_native();
			
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");

		return val;
	}

	public void set_gpio_in_map(int val){
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		set_gpio_in_map_native(val);
			
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public int get_gpio_in_map(){
		int val;
		
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
					
		val = get_gpio_in_map_native();
			
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");

		return val;
	}


	public void reboot(String reason){
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		try {
            mPm.reboot(reason);
        } catch (Exception e) {
        	Slog.e(TAG, "reboot exception!",e);
        }

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void suspend(){

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

		PowerManagerService pms = (PowerManagerService) ServiceManager.getService("power");
		
		try {
            pms.shutdown(true,true);
        } catch (Exception e) {
        	Slog.e(TAG, "suspend exception!" );
        }

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}

	public void sleep(long time){
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		try {
            mPm.goToSleep(time);
        } catch (Exception e) {
        	Slog.e(TAG, "sleep exception!",e);
        }

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	
	}

	public void fsleep(int val){
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

		set_backlight_native(val);

		/*
		try {
            mPm.setBacklightBrightness(0);
        } catch (Exception e) {
        	Slog.e(TAG, "fsleep exception!" );
        }
        */

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	
	}

	public void wakeup(){
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		try {
            mPm.userActivity(SystemClock.uptimeMillis(),true);
        } catch (Exception e) {
        	Slog.e(TAG, "sleep exception!" );
        }

		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
	}
	
	public String getVersion(int index){
		String strVersion = "1.0.0.0";
		
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
		Slog.i(TAG,CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");

		return strVersion;
	}
	
	private static native boolean init_native();
	
	private static native void set_app_ready_native(int val);
	private static native void set_heart_beat_native(int val);

	private static native void set_relay1_gpio_native(int val);
	private static native void set_relay2_gpio_native(int val);
	private static native void set_relay3_gpio_native(int val);
	private static native void set_relay4_gpio_native(int val);
	private static native void set_amp_stb_gpio_native(int val);
	private static native void set_amp_mute_gpio_native(int val);
	private static native void set_otg_switch_gpio_native(int val);
	private static native void set_usb2eth_rst_gpio_native(int val);
	private static native void set_usbhub_rst_gpio_native(int val);
	private static native void set_stm32_rst_gpio_native(int val);
	private static native void set_camera_power_gpio_native(int val);
	private static native void set_tachograph_power_gpio_native(int val);
	private static native void set_ex_power_gpio_native(int val);
	private static native void set_finger_power_gpio_native(int val);
	private static native void set_tsm_power_gpio_native(int val);
	private static native void set_fan_power_gpio_native(int val);

	private static native void set_gpio_out_map_native(int val);
	private static native int get_gpio_out_map_native();	
	private static native void set_gpio_in_map_native(int val);
	private static native int get_gpio_in_map_native();

	private static native void set_backlight_native(int val);
};

