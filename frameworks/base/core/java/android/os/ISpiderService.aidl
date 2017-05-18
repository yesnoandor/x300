package android.os;

interface ISpiderService {
    void set_app_ready(int val);
    void set_heart_beat(int val);

    void set_relay1_gpio(int val);
    void set_relay2_gpio(int val);
    void set_relay3_gpio(int val);
    void set_relay4_gpio(int val);

    void set_amp_stb_gpio(int val);
    void set_amp_mute_gpio(int val);

    void set_otg_switch_gpio(int val);

    void set_usb2eth_rst_gpio(int val);
    void set_usbhub_rst_gpio(int val);
    void set_stm32_rst_gpio(int val);

    void set_camera_power_gpio(int val);
    void set_tachograph_power_gpio(int val);
    void set_ex_power_gpio(int val);
    void set_finger_power_gpio(int val);
    void set_tsm_power_gpio(int val);
    void set_fan_power_gpio(int val);

	void set_gpio_out_map(int val);
	int get_gpio_out_map();	
	void set_gpio_in_map(int val);
	int get_gpio_in_map();

	void reboot(String reason);
	void suspend();
	void sleep(long time);
	void fsleep(int val);
	void wakeup();

	String getVersion(int index);
}
