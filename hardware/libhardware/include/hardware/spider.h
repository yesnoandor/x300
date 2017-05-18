#ifndef ANDROID_SPIDER_INTERFACE_H
#define ANDROID_SPIDER_INTERFACE_H

#include	<hardware/hardware.h>

__BEGIN_DECLS

// 定义模块ID
#define		SPIDER_HARDWARE_MODULE_ID 		"spider"

// 硬件模块结构体
struct spider_module_t {
	struct hw_module_t common;
};

// 硬件接口结构体
struct spider_device_t {
	struct hw_device_t common;
	int fd;
	
	int (*set_app_ready)(struct spider_device_t* dev, int val);
	int (*set_heart_beat)(struct spider_device_t* dev, int val);

	int (*set_relay1_gpio)(struct spider_device_t* dev, int val);
	int (*set_relay2_gpio)(struct spider_device_t* dev, int val);
	int (*set_relay3_gpio)(struct spider_device_t* dev, int val);
	int (*set_relay4_gpio)(struct spider_device_t* dev, int val);

	int (*set_amp_stb_gpio)(struct spider_device_t* dev, int val);
	int (*set_amp_mute_gpio)(struct spider_device_t* dev, int val);

	int (*set_otg_switch_gpio)(struct spider_device_t* dev, int val);
	
	int (*set_usb2eth_rst_gpio)(struct spider_device_t* dev, int val);
	int (*set_usbhub_rst_gpio)(struct spider_device_t* dev, int val);
	int (*set_stm32_rst_gpio)(struct spider_device_t* dev, int val);
	
	int (*set_camera_power_gpio)(struct spider_device_t* dev, int val);
	int (*set_tachograph_power_gpio)(struct spider_device_t* dev, int val);
	int (*set_ex_power_gpio)(struct spider_device_t* dev, int val);
	int (*set_finger_power_gpio)(struct spider_device_t* dev, int val);
	int (*set_tsm_power_gpio)(struct spider_device_t* dev, int val);
	int (*set_fan_power_gpio)(struct spider_device_t* dev, int val);

	int (*set_gpio_out_map)(struct spider_device_t* dev, int val);
	int (*get_gpio_out_map)(struct spider_device_t* dev);
	int (*set_gpio_in_map)(struct spider_device_t* dev, int val);
	int (*get_gpio_in_map)(struct spider_device_t* dev);
	
	int (*set_backlight)(struct spider_device_t* dev, int val);
};

__END_DECLS

#endif

