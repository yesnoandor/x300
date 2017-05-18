/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// ------------------- Include Files List --------------------
#include <stdio.h>

#include <fcntl.h>
#include <errno.h>
#include <cutils/atomic.h>
#include <linux/ioctl.h>
#include <sys/types.h>

#include <linux/input.h>

#include <pthread.h> 
#include <termios.h>

#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/stat.h> 

#include <hardware/hardware.h>
#include <hardware/spider.h>

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"SpiderStub"
#include	<cutils/log.h>



// ----------------- Macro Define -------- ----------------
#define		SPIDER_PATH				"/sys/class/fleety_spider_class/fleety_spider"
#define		APP_READY_NODE			"app_ready"
#define		HEART_BEAT_NODE			"heart_beat"
#define		TOUCH_EN_NODE			"touch_en"
#define		GPO_MAP_NODE			"gpo_map"
#define		GPI_MAP_NODE			"gpi_map"

#define		GPIO_PATH				"/sys/class/fleety_leds_class/fleety_leds"
#define		RELAY1_NODE				"output1_relay"
#define		RELAY2_NODE				"output2_relay"
#define		RELAY3_NODE				"output3_relay"
#define		RELAY4_NODE				"output4_relay"
#define		AMP_STB_NODE			"amp_stb"
#define		AMP_MUTE_NODE			"amp_mute"
#define		OTG_SWITCH_NODE			"otg_switch"
#define		USB2ETH_RST_NODE		"usb2eth_rst"
#define		USBHUB_RST_NODE			"usbhub_rst"
#define		STM32_RST_NODE			"stm32_rst"
#define		CAMERA_POWER_NODE		"camera_power"
#define		TACHOGRAPH_POWER_NODE	"tachograph_power"
#define		EX_POWER_NODE			"ex_power"
#define		FINGER_POWER_NODE		"finger_power"
#define		TSM_POWER_NODE			"tsm_power"
#define		FAN_POWER_NODE			"fan_power"

#define		LIGHT_PATH				"/sys/class/leds/lcd-backlight"
#define		BL_NODE					"brightness"	

#define     MODULE_NAME				"SPIDER"
#define     MODULE_AUTHOR			"wenyu_xu"

// ----------------- Functions Declaration -----------------
static int spider_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device);
static int spider_device_close(struct hw_device_t* device);

static int spider_set_app_ready(struct spider_device_t* dev, int val);
static int spider_set_heart_beat(struct spider_device_t* dev, int val);

static int spider_set_relay1_gpio(struct spider_device_t* dev, int val);
static int spider_set_relay2_gpio(struct spider_device_t* dev, int val);
static int spider_set_relay3_gpio(struct spider_device_t* dev, int val);
static int spider_set_relay4_gpio(struct spider_device_t* dev, int val);
static int spider_set_amp_stb_gpio(struct spider_device_t* dev, int val);
static int spider_set_amp_mute_gpio(struct spider_device_t* dev, int val);
static int spider_set_otg_switch_gpio(struct spider_device_t* dev, int val);
static int spider_set_usb2eth_rst_gpio(struct spider_device_t* dev, int val);
static int spider_set_usbhub_rst_gpio(struct spider_device_t* dev, int val);
static int spider_set_stm32_rst_gpio(struct spider_device_t* dev, int val);
static int spider_set_camera_power_gpio(struct spider_device_t* dev, int val);
static int spider_set_tachograph_power_gpio(struct spider_device_t* dev, int val);
static int spider_set_ex_power_gpio(struct spider_device_t* dev, int val);
static int spider_set_finger_power_gpio(struct spider_device_t* dev, int val);
static int spider_set_tsm_power_gpio(struct spider_device_t* dev, int val);
static int spider_set_fan_power_gpio(struct spider_device_t* dev, int val);

static int spider_set_gpio_out_map(struct spider_device_t* dev, int val);
static int spider_get_gpio_out_map(struct spider_device_t* dev);
static int spider_set_gpio_in_map(struct spider_device_t* dev, int val);
static int spider_get_gpio_in_map(struct spider_device_t* dev);

static int spider_set_backlight(struct spider_device_t* dev, int val);

// ----------------- Module Define -------- ---------------
//	模块方法表
static struct hw_module_methods_t spider_module_methods = {
	open: spider_device_open
};


//	模块实例变量
struct spider_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: SPIDER_HARDWARE_MODULE_ID,
		name: MODULE_NAME,
		author: MODULE_AUTHOR,
		methods: &spider_module_methods,
	}
};




// ----------------- Function List -------- ----------------
static int spider_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device) {
	struct spider_device_t* dev;
	
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    dev = (struct spider_device_t*)malloc(sizeof(struct spider_device_t));    
	if(!dev) {
		ALOGE("Spider Stub: failed to alloc space");
		return -EFAULT;
	}

	memset(dev, 0, sizeof(struct spider_device_t));
	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (hw_module_t*)module;
	dev->common.close = spider_device_close;

	dev->set_app_ready = spider_set_app_ready;
	dev->set_heart_beat = spider_set_heart_beat;

	dev->set_relay1_gpio = spider_set_relay1_gpio;
	dev->set_relay2_gpio = spider_set_relay2_gpio;
	dev->set_relay3_gpio = spider_set_relay3_gpio;
	dev->set_relay4_gpio = spider_set_relay4_gpio;
	
	dev->set_amp_stb_gpio = spider_set_amp_stb_gpio;
	dev->set_amp_mute_gpio = spider_set_amp_mute_gpio;

	dev->set_otg_switch_gpio = spider_set_otg_switch_gpio;

	dev->set_usb2eth_rst_gpio = spider_set_usb2eth_rst_gpio;
	dev->set_usbhub_rst_gpio = spider_set_usbhub_rst_gpio;
	dev->set_stm32_rst_gpio = spider_set_stm32_rst_gpio;

	dev->set_camera_power_gpio = spider_set_camera_power_gpio;
	dev->set_tachograph_power_gpio = spider_set_tachograph_power_gpio;
	dev->set_ex_power_gpio = spider_set_ex_power_gpio;
	dev->set_finger_power_gpio = spider_set_finger_power_gpio;
	dev->set_tsm_power_gpio = spider_set_tsm_power_gpio;
	dev->set_fan_power_gpio = spider_set_fan_power_gpio;

	dev->set_gpio_out_map = spider_set_gpio_out_map;
	dev->get_gpio_out_map = spider_get_gpio_out_map;
	dev->set_gpio_in_map = spider_set_gpio_in_map;
	dev->get_gpio_in_map = spider_get_gpio_in_map;
	
	dev->set_backlight = spider_set_backlight;

	*device = &(dev->common);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_device_close(struct hw_device_t* device) {
	struct spider_device_t* spider_device = (struct spider_device_t*)device;

    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
    
	if(spider_device) {
		//close(hello_device->fd);
		free(spider_device);
	}

    ALOGI("%s::---------------\r\n",__FUNCTION__);
    
	return 0;
}

static int spider_set_common_treat(char * path,char * node,int val)
{
	int fd;
	char buffer[100];
	int bytes;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	memset(buffer,0,sizeof(buffer));
	snprintf(buffer, sizeof(buffer), "%s/%s",path,node);

	ALOGI("node = %s\r\n",buffer);
	
	fd = open(buffer, O_RDWR);
	
	if (fd < 0) {
		ALOGE("open heart beat node error!(%s)\r\n",buffer);
		return -EFAULT;
	}

	memset(buffer,0,sizeof(buffer));
	bytes = snprintf(buffer, sizeof(buffer), "%d\n",val);
	
	write(fd, buffer, bytes);
	close(fd);

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_get_common_treat(char * path,char * node)
{
	int fd;
	char buffer[100];
	int bytes;
	int val;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	memset(buffer,0,sizeof(buffer));
	snprintf(buffer, sizeof(buffer), "%s/%s",path,node);

	ALOGI("node = %s\r\n",buffer);
	
	fd = open(buffer, O_RDWR);
	
	if (fd < 0) {
		ALOGE("open heart beat node error!(%s)\r\n",buffer);
		return -EFAULT;
	}

	memset(buffer,0,sizeof(buffer));
	bytes = read(fd,buffer,100);

	val = atoi(buffer);
	ALOGI("val = %d\r\n",val);
	
	close(fd);

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return val;
}

static int spider_set_app_ready(struct spider_device_t* dev, int val) {
	int fd;
	
	char buffer[100];
	int bytes;
	int ret;

    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	ALOGI("Spider Stub: set value %d to device.", val);

	ret = spider_set_common_treat(SPIDER_PATH,APP_READY_NODE,val);
	
    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return ret;
}

static int spider_set_heart_beat(struct spider_device_t* dev, int val) {
	int ret;
	
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

    ret = spider_set_common_treat(SPIDER_PATH,HEART_BEAT_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return ret;
}

static int spider_set_relay1_gpio(struct spider_device_t* dev, int val) {
	int ret;
	
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	ret = spider_set_common_treat(GPIO_PATH,RELAY1_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return ret;
}

static int spider_set_relay2_gpio(struct spider_device_t* dev, int val) {
	int ret;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	ret = spider_set_common_treat(GPIO_PATH,RELAY2_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return ret;
}

static int spider_set_relay3_gpio(struct spider_device_t* dev, int val) {
	int ret;
	
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	ret = spider_set_common_treat(GPIO_PATH,RELAY3_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return ret;
}

static int spider_set_relay4_gpio(struct spider_device_t* dev, int val) {
	int ret;
	
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	ret = spider_set_common_treat(GPIO_PATH,RELAY4_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return ret;
}

static int spider_set_amp_stb_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,AMP_STB_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}


static int spider_set_amp_mute_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,AMP_MUTE_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_set_otg_switch_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,OTG_SWITCH_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}


static int spider_set_usb2eth_rst_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,USB2ETH_RST_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}


static int spider_set_usbhub_rst_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,USBHUB_RST_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_set_stm32_rst_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,STM32_RST_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_set_camera_power_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,CAMERA_POWER_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_set_tachograph_power_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,TACHOGRAPH_POWER_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_set_ex_power_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,EX_POWER_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_set_finger_power_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,FINGER_POWER_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}



static int spider_set_tsm_power_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,TSM_POWER_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_set_fan_power_gpio(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,FAN_POWER_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_set_gpio_out_map(struct spider_device_t* dev,int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(SPIDER_PATH,GPO_MAP_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_get_gpio_out_map(struct spider_device_t* dev) {
	int val;
	
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	val = spider_get_common_treat(SPIDER_PATH,GPO_MAP_NODE);

	ALOGI("val = 0x%x\r\n",val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return val;
}

static int spider_set_gpio_in_map(struct spider_device_t* dev, int val) {
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(SPIDER_PATH,GPI_MAP_NODE,val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

static int spider_get_gpio_in_map(struct spider_device_t* dev) {
	int val;
	
    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	val = spider_get_common_treat(SPIDER_PATH,GPI_MAP_NODE);

	ALOGI("val = 0x%x\r\n",val);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return val;
}

static int spider_set_backlight(struct spider_device_t* dev, int val){
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    ALOGI("Spider Stub: set value %d to device.", val);

    ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(LIGHT_PATH,BL_NODE,val);
	spider_set_common_treat(SPIDER_PATH,TOUCH_EN_NODE,val?1:0);

    ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}

#if 0
static int spider_set_gpio_map(struct spider_device_t* dev, unsigned int gpio){
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	ALOGI("val = 0x%x\r\n",val);

	spider_set_common_treat(GPIO_PATH,FAN_POWER_NODE,val);

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}
#endif
