/*
 *  fleety leds driver
 *
 * Copyright (c) 2014-2015 Fleety Limited,
 *
 * Licensed under GPLv2 or later.
 */

// ------------------- Include Files List --------------------
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include <linux/semaphore.h>
#include <linux/err.h>

#include <linux/platform_device.h>
#include <linux/gpio.h>

#include <linux/leds.h>			
#include <linux/of_gpio.h>		

#include <linux/fleety/fleety_spider.h>
#include <linux/fleety/fleety_leds.h>

// ----------------- Declaration Debug Zone ----------------
#define DEBUG_FLEETY_LEDS	"FLEETY_LEDS"

#ifdef	DEBUG_FLEETY_LEDS
#define dprintk(x...)	printk(x)
#else
#define dprintk(x...)
#endif


// ----------------- Macro Define -------- ----------------
#define		FLEETY_LEDS_DRV_NAME				"fleety-leds"

#define 	FLEETY_LEDS_DEVICE_NODE_NAME		"fleety_leds"
#define		FLEETY_LEDS_DEVICE_CLASS_NAME		"fleety_leds_class"
#define		FLEETY_LEDS_DEVICE_FILE_NAME		"fleety_leds"
#define		FLEETY_AMPS_DEVICE_FILE_NAME		"fleety_amps"

#define		FLEETY_OUTPUT1_LED_GPIO			1002		// 911 + 91
#define		FLEETY_OUTPUT2_LED_GPIO			1003		// 911 + 92
#define		FLEETY_OUTPUT3_LED_GPIO			1008		// 911 + 97
#define		FLEETY_OUTPUT4_LED_GPIO			942			// 911 + 31

#define		FLEETY_AMP_STB_GPIO				937			// 911 + 26
#define		FLEETY_AMP_MUTE_GPIO			913			// 911 + 2

#define		FLEETY_OTG_ID_GPIO				914			// 911 + 3
#define		FLEETY_OTG_SWITCH_GPIO			943			// 911 + 32

#define		FLEETY_USB2ETH_RST_GPIO			933			// 911 + 22
#define		FLEETY_USBHUB_RST_GPIO			947			// 911 + 36
#define		FLEETY_STM32_RST_GPIO			1004		// 911 + 93

#define		FLEETY_CAMERA_POWER_GPIO		980			// 911 + 69
#define		FLEETY_TACHOGRAPH_POWER_GPIO	1007		// 911 + 96
#define		FLEETY_EX_POWER_GPIO			1010		// 911 + 99
#define		FLEETY_FINGER_POWER_GPIO		1023		// 911 + 112
#define		FLEETY_TSM_POWER_GPIO			934			// 911 + 23
#define		FLEETY_FAN_POWER_GPIO			919			// 911 + 8



enum {
	OUTPUT1_RELAY_GPIO_INDEX = 0,
	OUTPUT2_RELAY_GPIO_INDEX,
	OUTPUT3_RELAY_GPIO_INDEX,
	OUTPUT4_RELAY_GPIO_INDEX,
	
	AMP_STB_GPIO_INDEX,
	AMP_MUTE_GPIO_INDEX,

	OTG_ID_GPIO_INDEX,
	OTG_SWITCH_GPIO_INDEX,

	USB2ETH_RST_GPIO_INDEX,
	USBHUB_RST_GPIO_INDEX,
	STM32_RST_GPIO_INDEX,

	CAMERA_POWER_GPIO_INDEX,
	TACHOGRAPH_POWER_GPIO_INDEX,
	EX_POWER_GPIO_INDEX,
	FINGER_POWER_GPIO_INDEX,
	TSM_POWER_GPIO_INDEX,
	FAN_POWER_GPIO_INDEX,
	
	FLEETY_GPIO_MAX,
};

// ------------------ Typedef Declaration ------------------
struct fleety_leds_platform_data {

};

struct fleety_gpio {
    int dir;        // 0 or 1 : =1, output ; = 0,input
    int index;      // 
    int level;      // 0 or 1
    int state;      // 0 or 1
    char * name;
};


struct fleety_leds {
	int version;

	struct fleety_gpio led_gpio[FLEETY_GPIO_MAX];
	 
	struct delayed_work	work;
	
	struct semaphore sem;

	struct class* fleety_leds_class;
	struct device* fleety_leds_device;
	struct device* fleety_amps_device;
};


// ------------------ Functions Declaration -----------------
static int  fleety_leds_probe(struct platform_device *pdev);
static int  fleety_leds_remove(struct platform_device *pdev);

static ssize_t output1_relay_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t output1_relay_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t output2_relay_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t output2_relay_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t output3_relay_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t output3_relay_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t output4_relay_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t output4_relay_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 

static ssize_t amp_stb_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t amp_stb_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t amp_mute_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t amp_mute_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 

static ssize_t otg_switch_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t otg_switch_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 

static ssize_t usb2eth_rst_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t usb2eth_rst_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t usbhub_rst_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t usbhub_rst_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t stm32_rst_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t stm32_rst_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 

static ssize_t camera_power_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t camera_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t tachograph_power_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t tachograph_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t ex_power_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t ex_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t finger_power_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t finger_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t tsm_power_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t tsm_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 
static ssize_t fan_power_on_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t fan_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 



// ------------------ Static Variable -----------------------
#ifdef CONFIG_OF_GPIO
static const struct of_device_id of_fleety_leds_match[] = {
	{ .compatible = "fleety-leds", },
	{},
};
#endif

static struct fleety_leds_platform_data fleety_leds_platform_data = {
	
};

static struct platform_device fleety_leds_platform_device = {
    .name 	=   FLEETY_LEDS_DRV_NAME,    
    .dev    =   {
               .platform_data = &fleety_leds_platform_data,
    }
};


static struct platform_driver fleety_leds_platform_driver = {
	.driver	= {
		.name	= FLEETY_LEDS_DRV_NAME,
		.owner	= THIS_MODULE,
#ifdef CONFIG_OF_GPIO		
		.of_match_table = of_match_ptr(of_fleety_leds_match),
#endif
	},
	.probe		= fleety_leds_probe,
	.remove		= fleety_leds_remove,
};


static struct device_attribute fleety_leds_device_attributes[] = {
	__ATTR(output1_relay, S_IRWXUGO , output1_relay_on_show, output1_relay_on_store),
	__ATTR(output2_relay, S_IRWXUGO, output2_relay_on_show, output2_relay_on_store),
	__ATTR(output3_relay, S_IRWXUGO, output3_relay_on_show, output3_relay_on_store),
	__ATTR(output4_relay, S_IRWXUGO, output4_relay_on_show, output4_relay_on_store),

	__ATTR(amp_stb, S_IRWXUGO, amp_stb_on_show, amp_stb_on_store),
	__ATTR(amp_mute, S_IRWXUGO, amp_mute_on_show, amp_mute_on_store),

	__ATTR(otg_switch, S_IRWXUGO, otg_switch_on_show, otg_switch_on_store),

	__ATTR(usb2eth_rst, S_IRWXUGO, usb2eth_rst_on_show, usb2eth_rst_on_store),
	__ATTR(usbhub_rst, S_IRWXUGO, usbhub_rst_on_show, usbhub_rst_on_store),
	__ATTR(stm32_rst, S_IRWXUGO, stm32_rst_on_show, stm32_rst_on_store),

	__ATTR(camera_power, S_IRWXUGO, camera_power_on_show, camera_power_on_store),
	__ATTR(tachograph_power, S_IRWXUGO, tachograph_power_on_show, tachograph_power_on_store),
	__ATTR(ex_power, S_IRWXUGO, ex_power_on_show, ex_power_on_store),
	__ATTR(finger_power, S_IRWXUGO, finger_power_on_show, finger_power_on_store),
	__ATTR(tsm_power, S_IRWXUGO, tsm_power_on_show, tsm_power_on_store),
	__ATTR(fan_power, S_IRWXUGO, fan_power_on_show, fan_power_on_store),
	
	
	__ATTR_NULL,
};


// ----------------- Function List  -------- ----------------
static ssize_t fleety_leds_gpio_show_treat(struct fleety_leds * fleety_leds_dev,char* buf,unsigned char index)
{
	ssize_t ret;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

   	if(down_interruptible(&(fleety_leds_dev->sem))) {                
		return -ERESTARTSYS;        
	}    

    ret = sprintf(buf,"%d\n", fleety_leds_dev->led_gpio[index].level);

    up(&(fleety_leds_dev->sem));

    dprintk(KERN_INFO "%s:version = 0x%x\r\n",DEBUG_FLEETY_LEDS,fleety_leds_dev->version);
	dprintk(KERN_INFO "%s:name = %s\r\n",DEBUG_FLEETY_LEDS,fleety_leds_dev->led_gpio[index].name);
	dprintk(KERN_INFO "%s:level = %d\r\n",DEBUG_FLEETY_LEDS,fleety_leds_dev->led_gpio[index].level);

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

    return ret;
}

static ssize_t fleety_leds_gpio_store_treat(struct fleety_leds * fleety_leds_dev,const char* buf,unsigned char index)
{
	unsigned long val = 0;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	if(strict_strtoul(buf, 0,&val) < 0){
		return -EPERM;
	}

	dprintk(KERN_INFO "%s:index = %d\r\n",DEBUG_FLEETY_LEDS,index);
	dprintk(KERN_INFO "%s:val = 0x%x\r\n",DEBUG_FLEETY_LEDS,(int)val);

	if(down_interruptible(&(fleety_leds_dev->sem))) {                
		return -ERESTARTSYS;        
	}        

    fleety_leds_dev->led_gpio[index].level = val;
	gpio_set_value(fleety_leds_dev->led_gpio[index].index,val);
	
	up(&(fleety_leds_dev->sem));
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

    return 0;
}

static void fleety_leds_gpio_configure(struct fleety_leds* fleety_leds_dev)
{
	int i;
	int gpio;
	int level;
	char *name;
	
	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	for(i=0;i<FLEETY_GPIO_MAX;i++){
		gpio = fleety_leds_dev->led_gpio[i].index;
		name = fleety_leds_dev->led_gpio[i].name;
		level = fleety_leds_dev->led_gpio[i].level;

		dprintk(KERN_INFO "%s:name = %s\r\n",DEBUG_FLEETY_LEDS,name);
		dprintk(KERN_INFO "%s:gpio = %d\r\n",DEBUG_FLEETY_LEDS,gpio);
		dprintk(KERN_INFO "%s:level = %d\r\n",DEBUG_FLEETY_LEDS,level);
	
		if (gpio_is_valid(gpio)){
			
			if(gpio_request(gpio,name))
				BUG();

			if(gpio_direction_output(gpio,level))
				BUG();

			fleety_set_gpo_map(i,level);
		}
	
	}

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);
}


static ssize_t output1_relay_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

  	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,OUTPUT1_RELAY_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t output1_relay_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,OUTPUT1_RELAY_GPIO_INDEX);
	
	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t output2_relay_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,OUTPUT2_RELAY_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t output2_relay_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 	
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,OUTPUT2_RELAY_GPIO_INDEX);
 
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}


static ssize_t output3_relay_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,OUTPUT3_RELAY_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t output3_relay_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 	
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,OUTPUT3_RELAY_GPIO_INDEX);
 
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t output4_relay_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,OUTPUT4_RELAY_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t output4_relay_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 	
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,OUTPUT4_RELAY_GPIO_INDEX);
 
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t amp_stb_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,AMP_STB_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t amp_stb_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,AMP_STB_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}


static ssize_t amp_mute_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,AMP_MUTE_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}


static ssize_t amp_mute_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,AMP_MUTE_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t otg_switch_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,OTG_SWITCH_GPIO_INDEX);

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);
 
	return ret;
}

static ssize_t otg_switch_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,OTG_SWITCH_GPIO_INDEX);
    fleety_leds_gpio_store_treat(fleety_leds_dev,buf,OTG_ID_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}


static ssize_t usb2eth_rst_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,USB2ETH_RST_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t usb2eth_rst_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,USB2ETH_RST_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t usbhub_rst_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,USBHUB_RST_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t usbhub_rst_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,USBHUB_RST_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t stm32_rst_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,STM32_RST_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t stm32_rst_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,STM32_RST_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t camera_power_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,CAMERA_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t camera_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,CAMERA_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);
 
	return count;
}

static ssize_t tachograph_power_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,TACHOGRAPH_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t tachograph_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,TACHOGRAPH_POWER_GPIO_INDEX);
    
	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t ex_power_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,EX_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t ex_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,EX_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t finger_power_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,FINGER_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t finger_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,FINGER_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t tsm_power_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,TSM_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t tsm_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  
    
   	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,TSM_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

static ssize_t fan_power_on_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);        

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	ret = fleety_leds_gpio_show_treat(fleety_leds_dev,buf,FAN_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return ret;
}

static ssize_t fan_power_on_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
	struct fleety_leds* fleety_leds_dev = (struct fleety_leds *)dev_get_drvdata(dev);  

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_gpio_store_treat(fleety_leds_dev,buf,FAN_POWER_GPIO_INDEX);
    
    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return count;
}

#ifdef CONFIG_OF_GPIO
static int fleety_leds_create_of(struct platform_device *pdev,struct fleety_leds *fleety_leds_dev)
{
	struct device_node *np = pdev->dev.of_node, *child;
	int count;
	int index;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	dprintk(KERN_INFO "%s:name = %s\r\n",DEBUG_FLEETY_LEDS,np->name);
	//dprintk(KERN_INFO "%s:type = %s\r\n",DEBUG_FLEETY_LEDS,np->type);
	
	count = of_get_child_count(np);
	dprintk(KERN_INFO "%s:count = %d\r\n",DEBUG_FLEETY_LEDS,count);
	
	if (!count || count > FLEETY_GPIO_MAX)
	{
		dprintk(KERN_ERR "gpis count exceed expired!\r\n");
	
		return (-ENODEV);
	}
	
	for_each_child_of_node(np, child)
		if (of_get_gpio(child, 0) == -EPROBE_DEFER)
			return (-EPROBE_DEFER);

	#if 0
	priv = devm_kzalloc(&pdev->dev, sizeof_gpio_leds_priv(count),
			GFP_KERNEL);
	if (!priv)
		return ERR_PTR(-ENOMEM);
	#endif	

	index = 0;
	for_each_child_of_node(np, child) {
		struct gpio_led led = {};
		enum of_gpio_flags flags;
		const char *state;

		led.gpio = of_get_gpio_flags(child, 0, &flags);
		led.active_low = flags & OF_GPIO_ACTIVE_LOW;
		led.name = of_get_property(child, "label", NULL) ? : child->name;
		led.default_trigger =
			of_get_property(child, "linux,default-trigger", NULL);
		state = of_get_property(child, "default-state", NULL);
		if (state) {
			if (!strcmp(state, "on"))
				led.default_state = LEDS_GPIO_DEFSTATE_ON;
			else
				led.default_state = LEDS_GPIO_DEFSTATE_OFF;
		}

		dprintk(KERN_INFO "%s:led-name = %s\r\n",DEBUG_FLEETY_LEDS,led.name);
		dprintk(KERN_INFO "%s:led-gpio = %d\r\n",DEBUG_FLEETY_LEDS,led.gpio);
		dprintk(KERN_INFO "%s:led-state = %d\r\n",DEBUG_FLEETY_LEDS,led.default_state);
		dprintk(KERN_INFO "%s:led-active_low = %d\r\n",DEBUG_FLEETY_LEDS,led.active_low);
		//dprintk(KERN_INFO "%s:led-default_trigger = %s\r\n",DEBUG_FLEETY_LEDS,led.default_trigger);

		#if 0
		led.retain_state_suspended =
			(unsigned)of_property_read_bool(child,
				"retain-state-suspended");

		
		ret = create_gpio_led(&led, &priv->leds[priv->num_leds++],
				      &pdev->dev, NULL);
		if (ret < 0) {
			of_node_put(child);
			goto err;
		}
		#endif

		dprintk(KERN_INFO "%s:index = %d\r\n",DEBUG_FLEETY_LEDS,index);

		dprintk(KERN_INFO "%s:=========================\r\n",DEBUG_FLEETY_LEDS);

		
		fleety_leds_dev->led_gpio[index].index = led.gpio + 911;
		dprintk(KERN_INFO "%s:gpio = %d\r\n",DEBUG_FLEETY_LEDS,fleety_leds_dev->led_gpio[index].index);
		
		fleety_leds_dev->led_gpio[index].level = led.default_state;
		dprintk(KERN_INFO "%s:level = %d\r\n",DEBUG_FLEETY_LEDS,fleety_leds_dev->led_gpio[index].level);
	
		fleety_leds_dev->led_gpio[index].dir = 1;
		dprintk(KERN_INFO "%s:dir = %d\r\n",DEBUG_FLEETY_LEDS,fleety_leds_dev->led_gpio[index].dir);
	
		//strcpy(fleety_leds_dev->led_gpio[index].name,led.name);
		fleety_leds_dev->led_gpio[index].name = (char *)led.name;
		dprintk(KERN_INFO "%s:name = %s\r\n",DEBUG_FLEETY_LEDS,fleety_leds_dev->led_gpio[index].name);
		

		index++;
	}



	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return 0;
}
#endif

static int fleety_leds_probe(struct platform_device *pdev)
{
	dev_t dev = 0;
	
	struct fleety_leds *fleety_leds_dev;
	//struct zivoo_leds_platform_data *pdata = pdev->dev.platform_data;
	
	//int ret;
	int err;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_dev = devm_kzalloc(&pdev->dev,sizeof(struct fleety_leds), GFP_KERNEL);
	if (fleety_leds_dev == NULL) {
		dev_err(&pdev->dev, "failed to alloc platform device memory\r\n");
		return -ENOMEM;
	}

	memset(fleety_leds_dev, 0, sizeof(struct fleety_leds));
	sema_init(&(fleety_leds_dev->sem),1);

#ifdef CONFIG_OF_GPIO
	fleety_leds_create_of(pdev,fleety_leds_dev);
#endif

	// 
	fleety_leds_dev->led_gpio[OUTPUT1_RELAY_GPIO_INDEX].index = FLEETY_OUTPUT1_LED_GPIO;
	fleety_leds_dev->led_gpio[OUTPUT1_RELAY_GPIO_INDEX].name = "relay1-gpio";
	fleety_leds_dev->led_gpio[OUTPUT1_RELAY_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[OUTPUT1_RELAY_GPIO_INDEX].dir = 1;
	
	fleety_leds_dev->led_gpio[OUTPUT2_RELAY_GPIO_INDEX].index = FLEETY_OUTPUT2_LED_GPIO;
	fleety_leds_dev->led_gpio[OUTPUT2_RELAY_GPIO_INDEX].name = "relay2-gpio";
	fleety_leds_dev->led_gpio[OUTPUT2_RELAY_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[OUTPUT2_RELAY_GPIO_INDEX].dir = 1;

	fleety_leds_dev->led_gpio[OUTPUT3_RELAY_GPIO_INDEX].index = FLEETY_OUTPUT3_LED_GPIO;
	fleety_leds_dev->led_gpio[OUTPUT3_RELAY_GPIO_INDEX].name = "relay3-gpio";
	fleety_leds_dev->led_gpio[OUTPUT3_RELAY_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[OUTPUT3_RELAY_GPIO_INDEX].dir = 1;
	
	fleety_leds_dev->led_gpio[OUTPUT4_RELAY_GPIO_INDEX].index = FLEETY_OUTPUT4_LED_GPIO;
	fleety_leds_dev->led_gpio[OUTPUT4_RELAY_GPIO_INDEX].name = "relay4-gpio";
	fleety_leds_dev->led_gpio[OUTPUT4_RELAY_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[OUTPUT4_RELAY_GPIO_INDEX].dir = 1;
	

	fleety_leds_dev->led_gpio[AMP_STB_GPIO_INDEX].index = FLEETY_AMP_STB_GPIO;
	fleety_leds_dev->led_gpio[AMP_STB_GPIO_INDEX].name = "amp-stb";
	fleety_leds_dev->led_gpio[AMP_STB_GPIO_INDEX].level = 1;
	fleety_leds_dev->led_gpio[AMP_STB_GPIO_INDEX].dir = 1;
	
	fleety_leds_dev->led_gpio[AMP_MUTE_GPIO_INDEX].index = FLEETY_AMP_MUTE_GPIO;
	fleety_leds_dev->led_gpio[AMP_MUTE_GPIO_INDEX].name = "amp-mute";
	fleety_leds_dev->led_gpio[AMP_MUTE_GPIO_INDEX].level = 1;
	fleety_leds_dev->led_gpio[AMP_MUTE_GPIO_INDEX].dir = 1;

	fleety_leds_dev->led_gpio[OTG_ID_GPIO_INDEX].index = FLEETY_OTG_ID_GPIO;
	fleety_leds_dev->led_gpio[OTG_ID_GPIO_INDEX].name = "otg-id";
	fleety_leds_dev->led_gpio[OTG_ID_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[OTG_ID_GPIO_INDEX].dir = 1;

	fleety_leds_dev->led_gpio[OTG_SWITCH_GPIO_INDEX].index = FLEETY_OTG_SWITCH_GPIO;
	fleety_leds_dev->led_gpio[OTG_SWITCH_GPIO_INDEX].name = "otg-switch";
	fleety_leds_dev->led_gpio[OTG_SWITCH_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[OTG_SWITCH_GPIO_INDEX].dir = 1;

	fleety_leds_dev->led_gpio[USB2ETH_RST_GPIO_INDEX].index = FLEETY_USB2ETH_RST_GPIO;
	fleety_leds_dev->led_gpio[USB2ETH_RST_GPIO_INDEX].name = "usb2eth-rst";
	fleety_leds_dev->led_gpio[USB2ETH_RST_GPIO_INDEX].level = 1;
	fleety_leds_dev->led_gpio[USB2ETH_RST_GPIO_INDEX].dir = 1;
	
	fleety_leds_dev->led_gpio[USBHUB_RST_GPIO_INDEX].index = FLEETY_USBHUB_RST_GPIO;
	fleety_leds_dev->led_gpio[USBHUB_RST_GPIO_INDEX].name = "usbhub-rst";
	fleety_leds_dev->led_gpio[USBHUB_RST_GPIO_INDEX].level = 1;
	fleety_leds_dev->led_gpio[USBHUB_RST_GPIO_INDEX].dir = 1;

	fleety_leds_dev->led_gpio[STM32_RST_GPIO_INDEX].index = FLEETY_STM32_RST_GPIO;
	fleety_leds_dev->led_gpio[STM32_RST_GPIO_INDEX].name = "stm32-rst";
	fleety_leds_dev->led_gpio[STM32_RST_GPIO_INDEX].level = 1;
	fleety_leds_dev->led_gpio[STM32_RST_GPIO_INDEX].dir = 1;
	
	fleety_leds_dev->led_gpio[CAMERA_POWER_GPIO_INDEX].index = FLEETY_CAMERA_POWER_GPIO;
	fleety_leds_dev->led_gpio[CAMERA_POWER_GPIO_INDEX].name = "camera-power";
	fleety_leds_dev->led_gpio[CAMERA_POWER_GPIO_INDEX].level = 1;
	fleety_leds_dev->led_gpio[CAMERA_POWER_GPIO_INDEX].dir = 1;
	
	fleety_leds_dev->led_gpio[TACHOGRAPH_POWER_GPIO_INDEX].index = FLEETY_TACHOGRAPH_POWER_GPIO;
	fleety_leds_dev->led_gpio[TACHOGRAPH_POWER_GPIO_INDEX].name = "tachograph-power";
	fleety_leds_dev->led_gpio[TACHOGRAPH_POWER_GPIO_INDEX].level = 1;
	fleety_leds_dev->led_gpio[TACHOGRAPH_POWER_GPIO_INDEX].dir = 1;

	fleety_leds_dev->led_gpio[EX_POWER_GPIO_INDEX].index = FLEETY_EX_POWER_GPIO;
	fleety_leds_dev->led_gpio[EX_POWER_GPIO_INDEX].name = "ex-power";
	fleety_leds_dev->led_gpio[EX_POWER_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[EX_POWER_GPIO_INDEX].dir = 1;

	fleety_leds_dev->led_gpio[FINGER_POWER_GPIO_INDEX].index = FLEETY_FINGER_POWER_GPIO;
	fleety_leds_dev->led_gpio[FINGER_POWER_GPIO_INDEX].name = "finger-power";
	fleety_leds_dev->led_gpio[FINGER_POWER_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[FINGER_POWER_GPIO_INDEX].dir = 1;

	
	fleety_leds_dev->led_gpio[TSM_POWER_GPIO_INDEX].index = FLEETY_TSM_POWER_GPIO;
	fleety_leds_dev->led_gpio[TSM_POWER_GPIO_INDEX].name = "tsm-power";
	fleety_leds_dev->led_gpio[TSM_POWER_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[TSM_POWER_GPIO_INDEX].dir = 1;

	fleety_leds_dev->led_gpio[FAN_POWER_GPIO_INDEX].index = FLEETY_FAN_POWER_GPIO;
	fleety_leds_dev->led_gpio[FAN_POWER_GPIO_INDEX].name = "fan-power";
	fleety_leds_dev->led_gpio[FAN_POWER_GPIO_INDEX].level = 0;
	fleety_leds_dev->led_gpio[FAN_POWER_GPIO_INDEX].dir = 1;
	
	
	fleety_leds_dev->version = 0x0001;

	fleety_leds_gpio_configure(fleety_leds_dev);
	
	// new fleety_leds_class folder in /sys/class/
	fleety_leds_dev->fleety_leds_class = class_create(THIS_MODULE, FLEETY_LEDS_DEVICE_CLASS_NAME);
	if(IS_ERR(fleety_leds_dev->fleety_leds_class)) {
		err = PTR_ERR(fleety_leds_dev->fleety_leds_class);
		printk(KERN_ERR "Failed to create fleety class.\r\n");
	
		devm_kfree(&pdev->dev,fleety_leds_dev);
		
		return err;
	}		 
	fleety_leds_dev->fleety_leds_class->dev_attrs = fleety_leds_device_attributes;

	// new fleety_leds folder in /sys/class/fleety_leds_class
	fleety_leds_dev->fleety_leds_device = device_create(fleety_leds_dev->fleety_leds_class, NULL, dev, "%s", FLEETY_LEDS_DEVICE_FILE_NAME);
	if(IS_ERR(fleety_leds_dev->fleety_leds_device)) {
		err = PTR_ERR(fleety_leds_dev->fleety_leds_device);
		printk(KERN_ALERT "Failed to create fleety device.(ErrID = %d)\r\n",err);

		devm_kfree(&pdev->dev,fleety_leds_dev);
		return err;
	}

	dev_set_drvdata(fleety_leds_dev->fleety_leds_device, fleety_leds_dev);
	platform_set_drvdata(pdev, fleety_leds_dev);
	platform_set_drvdata(&fleety_leds_platform_device,fleety_leds_dev);

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return 0;

//err1:
//	input_unregister_device(input);
//	input = NULL;

//err:
	//input_free_device(input);
	
	//return ret;
}

static int fleety_leds_remove(struct platform_device *pdev)
{
	struct fleety_leds *fleety_leds_dev = (struct fleety_leds *)platform_get_drvdata(pdev);

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	devm_kfree(&pdev->dev,fleety_leds_dev);

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return 0;
}

static int __init fleety_leds_init(void)
{
    int ret;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);
	
	#if 0
	ret = platform_device_register(&fleety_leds_platform_device);
    if (ret) {
		printk(KERN_INFO "platform_device_register failed!\r\n");
		return ret;
    }
	#endif
    
    ret = platform_driver_register(&fleety_leds_platform_driver);
    if (ret) {
        printk(KERN_INFO "platform_driver_register failed!\r\n");
        return ret;
    }

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

    return ret;
}

static void __exit fleety_leds_exit(void)
{
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	platform_driver_unregister(&fleety_leds_platform_driver);
	//platform_device_unregister(&fleety_leds_platform_device);

    dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);
}


int fleety_usb_switch(bool host)
{
	struct fleety_leds *fleety_leds_dev;
	int level;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_dev = (struct fleety_leds *)platform_get_drvdata(&fleety_leds_platform_device);

	if(down_interruptible(&(fleety_leds_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	level = host;

	fleety_leds_dev->led_gpio[OTG_ID_GPIO_INDEX].level = level;
	fleety_leds_dev->led_gpio[OTG_SWITCH_GPIO_INDEX].level = level;

    gpio_set_value(fleety_leds_dev->led_gpio[OTG_ID_GPIO_INDEX].index,level);
	gpio_set_value(fleety_leds_dev->led_gpio[OTG_SWITCH_GPIO_INDEX].index,level);

	up(&(fleety_leds_dev->sem));

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return 0;
}
EXPORT_SYMBOL_GPL(fleety_usb_switch);


int fleety_usbhub_reset(void)
{
	struct fleety_leds *fleety_leds_dev;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_dev = (struct fleety_leds *)platform_get_drvdata(&fleety_leds_platform_device);

	if(down_interruptible(&(fleety_leds_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_leds_dev->led_gpio[USBHUB_RST_GPIO_INDEX].level = 0;
	gpio_set_value(fleety_leds_dev->led_gpio[USBHUB_RST_GPIO_INDEX].index,0);
	
	mdelay(200);

	fleety_leds_dev->led_gpio[USBHUB_RST_GPIO_INDEX].level = 1;
    gpio_set_value(fleety_leds_dev->led_gpio[USBHUB_RST_GPIO_INDEX].index,1);

	up(&(fleety_leds_dev->sem));

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return 0;
}
EXPORT_SYMBOL_GPL(fleety_usbhub_reset);


int fleety_usb2eth_reset(void)
{
	struct fleety_leds *fleety_leds_dev;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	fleety_leds_dev = (struct fleety_leds *)platform_get_drvdata(&fleety_leds_platform_device);

	if(down_interruptible(&(fleety_leds_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_leds_dev->led_gpio[USB2ETH_RST_GPIO_INDEX].level = 0;
	gpio_set_value(fleety_leds_dev->led_gpio[USB2ETH_RST_GPIO_INDEX].index,0);
	
	mdelay(200);

	fleety_leds_dev->led_gpio[USB2ETH_RST_GPIO_INDEX].level = 1;
    gpio_set_value(fleety_leds_dev->led_gpio[USB2ETH_RST_GPIO_INDEX].index,1);

	up(&(fleety_leds_dev->sem));

	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return 0;
}
EXPORT_SYMBOL_GPL(fleety_usb2eth_reset);

int fleety_amp_standby(int on)
{
	struct fleety_leds *fleety_leds_dev;

	dprintk(KERN_ERR "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	dprintk(KERN_ERR "on = %d\r\n",on);

	fleety_leds_dev = (struct fleety_leds *)platform_get_drvdata(&fleety_leds_platform_device);

	if(down_interruptible(&(fleety_leds_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_leds_dev->led_gpio[AMP_STB_GPIO_INDEX].level = on;
	gpio_set_value(fleety_leds_dev->led_gpio[AMP_STB_GPIO_INDEX].index,on);
	
	up(&(fleety_leds_dev->sem));

	dprintk(KERN_ERR "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return 0;
}
EXPORT_SYMBOL_GPL(fleety_amp_standby);


int fleety_amp_mute(int on)
{
	struct fleety_leds *fleety_leds_dev;

	dprintk(KERN_ERR "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	dprintk(KERN_ERR "on = %d\r\n",on);

	fleety_leds_dev = (struct fleety_leds *)platform_get_drvdata(&fleety_leds_platform_device);

	if(down_interruptible(&(fleety_leds_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_leds_dev->led_gpio[AMP_MUTE_GPIO_INDEX].level = on;
	gpio_set_value(fleety_leds_dev->led_gpio[AMP_MUTE_GPIO_INDEX].index,on);
	
	up(&(fleety_leds_dev->sem));

	dprintk(KERN_ERR "%s:%s::---------------\r\n",DEBUG_FLEETY_LEDS,__FUNCTION__);

	return 0;
}
EXPORT_SYMBOL_GPL(fleety_amp_mute);

late_initcall(fleety_leds_init);
module_exit(fleety_leds_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fleety Led Driver");
MODULE_AUTHOR("xuwy");
MODULE_VERSION("v1.1");

