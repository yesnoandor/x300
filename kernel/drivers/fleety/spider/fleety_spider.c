/*
 *  fleety leds driver
 *
 * Copyright (c) 2014-2015 Fleety Limited,
 *
 * Licensed under GPLv2 or later.
 */

// ------------------- Include Files List --------------------
#include <linux/debugfs.h>

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include <linux/semaphore.h>
#include <linux/err.h>

#include <linux/platform_device.h>
//#include <linux/gpio.h>

#include <linux/reboot.h>

#include <linux/fleety/fleety_leds.h>
#include <linux/fleety/fleety_spider.h>

// ----------------- Declaration Debug Zone ----------------
#define DEBUG_FLEETY_SPIDER			"FLEETY_SPIDER"
#define	DEBUG_LEVEL					KERN_INFO

#ifdef	DEBUG_FLEETY_SPIDER
#define dprintk(x...)	printk(x)
#else
#define dprintk(x...)
#endif


// ----------------- Macro Define -------- ----------------
#define		FLEETY_SPIDER_DRV_NAME				"fleety-spider"

#define 	FLEETY_SPIDER_DEVICE_NODE_NAME		"fleety_spider"
#define		FLEETY_SPIDER_DEVICE_CLASS_NAME		"fleety_spider_class"
#define		FLEETY_SPIDER_DEVICE_FILE_NAME		"fleety_spider"

#define		FLEETY_SPIDER_APP_READY_MIN				25
#define		HEART_BEAT_WORK_MS						1000
#define		HEART_BEAT_DEFAULT_TIMES				30




// ------------------ Typedef Declaration ------------------
struct fleety_spider_platform_data {

};


struct fleety_spider {
	int version;

	unsigned int heart_beat;
	unsigned int app_ready;
	unsigned int touch_en;
	unsigned int gpo_map;
	unsigned int gpi_map;

	unsigned int debug_on;
	 
	struct delayed_work	heart_beat_work;

	struct timer_list timer;
	
	struct semaphore sem;

	struct class* fleety_spider_class;
	struct device* fleety_spider_device;

	struct dentry* fleety_spider_debug_root;
};


// ------------------ Functions Declaration -----------------
static int  fleety_spider_probe(struct platform_device *pdev);
static int  fleety_spider_remove(struct platform_device *pdev);

static ssize_t heart_beat_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t heart_beat_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 

static ssize_t app_ready_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t app_ready_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 

static ssize_t touch_en_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t touch_en_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 

static ssize_t gpo_map_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t gpo_map_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 

static ssize_t gpi_map_show(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t gpi_map_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count); 


extern s32 gtp_touch_enalbe(int enable);

// ------------------ Static Variable -----------------------
static struct fleety_spider_platform_data fleety_spider_platform_data = {
	
};

static struct platform_device fleety_spider_platform_device = {
    .name 	=   FLEETY_SPIDER_DRV_NAME,    
    .dev    =   {
               .platform_data = &fleety_spider_platform_data,
    }
};


static struct platform_driver fleety_spider_platform_driver = {
	.driver	= {
		.name	= FLEETY_SPIDER_DRV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= fleety_spider_probe,
	.remove		= fleety_spider_remove,
};




const struct file_operations spider_debugfs_ops = {
	.owner		= THIS_MODULE,
	.read 		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};






static struct device_attribute fleety_spider_device_attributes[] = {
	__ATTR(heart_beat, S_IRUGO | S_IWUSR, heart_beat_show, heart_beat_store),
	__ATTR(app_ready, S_IRUGO | S_IWUSR, app_ready_show, app_ready_store),
	__ATTR(touch_en, S_IRUGO | S_IWUSR, touch_en_show, touch_en_store),
	__ATTR(gpo_map, S_IRUGO | S_IWUSR, gpo_map_show, gpo_map_store),
	__ATTR(gpi_map, S_IRUGO | S_IWUSR, gpi_map_show, gpi_map_store),
	__ATTR_NULL,
};


// ----------------- Function List  -------- ----------------
static void app_ready_timer_treat(unsigned long data)
{
	struct fleety_spider *fleety_spider_dev;

    dprintk(DEBUG_LEVEL "%s::+++++++++++++++\r\n",__FUNCTION__);

	fleety_spider_dev = (struct fleety_spider *)data;
	
    dprintk(DEBUG_LEVEL "app_ready = 0x%x\r\n",fleety_spider_dev->app_ready);
    
    if(!fleety_spider_dev->app_ready)
    {
        printk(KERN_ALERT "kernel restart to recovery for not app ready !!!\r\n"); 
        //mdelay(3000);
        //kernel_restart("recovery");
        //kernel_restart(NULL);

        //sys_sync();
		//kernel_restart(NULL);

		//machine_restart(NULL);

    }

	// start schedule heart beat
	schedule_delayed_work(&fleety_spider_dev->heart_beat_work,
							msecs_to_jiffies(HEART_BEAT_WORK_MS));
    
    dprintk(DEBUG_LEVEL "%s::---------------\r\n",__FUNCTION__);  
}


static void heart_beat_work_fn(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct fleety_spider *fleety_spider_dev = container_of(dwork, struct fleety_spider,
												heart_beat_work);
	
	dprintk(DEBUG_LEVEL "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);
	
	dprintk(DEBUG_LEVEL "msm-debug:heart_beat = %d\r\n",fleety_spider_dev->heart_beat);

	if(fleety_spider_dev->heart_beat == 0)
	{
		//printk(KERN_ALERT "kernel restart to recovery for no heart beat!!!\r\n"); 
        //mdelay(3000);
        //kernel_restart("recovery");
        //kernel_restart(NULL);
	}
	else
	{
		fleety_spider_dev->heart_beat--;
	}
	
	schedule_delayed_work(&fleety_spider_dev->heart_beat_work,
							msecs_to_jiffies(HEART_BEAT_WORK_MS));
	
	dprintk(DEBUG_LEVEL "msm-debug:%s::---------------\r\n",__FUNCTION__);  	  
}

static ssize_t app_ready_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

	
   	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	ret = sprintf(buf,"%d\n", fleety_spider_dev->app_ready);
	
	dprintk(KERN_INFO "msm-debug:app_ready = 0x%x\r\n",fleety_spider_dev->app_ready);
	
	
    up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return ret;
}

static ssize_t app_ready_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    unsigned long val = 0;
	
    struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

	if(strict_strtoul(buf, 0,&val) < 0){
		return -EPERM;
	}

	dprintk(KERN_INFO "msm-debug:val = 0x%x\r\n",(int)val);

	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_spider_dev->app_ready = val;
	if(fleety_spider_dev->app_ready){
		fleety_usb_switch(true);
		mdelay(200);
		fleety_usbhub_reset();
		mdelay(200);
		fleety_usb2eth_reset();
	}

	up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return count;
}



static ssize_t heart_beat_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

	
   	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	ret = sprintf(buf,"%d\n", fleety_spider_dev->heart_beat);
	
	dprintk(KERN_INFO "msm-debug:version = 0x%x\r\n",fleety_spider_dev->version);
	dprintk(KERN_INFO "msm-debug:heart_beat = 0x%x\r\n",fleety_spider_dev->heart_beat);
	
	
    up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return ret;
}

static ssize_t heart_beat_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    unsigned long val = 0;
	
    struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

	if(strict_strtoul(buf, 0,&val) < 0){
		return -EPERM;
	}

	dprintk(KERN_INFO "msm-debug:val = 0x%x\r\n",(int)val);

	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_spider_dev->heart_beat = val;

	up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return count;
}


static ssize_t touch_en_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

   	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	ret = sprintf(buf,"%d\n", fleety_spider_dev->touch_en);
	
	dprintk(KERN_INFO "msm-debug:touch_en = 0x%x\r\n",fleety_spider_dev->touch_en);
	
    up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return ret;
}

static ssize_t touch_en_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    unsigned long val = 0;
	
    struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

	if(strict_strtoul(buf, 0,&val) < 0){
		return -EPERM;
	}

	dprintk(KERN_INFO "msm-debug:val = 0x%x\r\n",(int)val);

	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_spider_dev->touch_en = val;
	gtp_touch_enalbe(val);
	
	up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return count;
}

static ssize_t gpo_map_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

   	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	ret = sprintf(buf,"%d\n", fleety_spider_dev->gpo_map);
	
	dprintk(KERN_INFO "msm-debug:gpo_map = 0x%x\r\n",fleety_spider_dev->gpo_map);
	
	
    up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return ret;
}

static ssize_t gpo_map_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    unsigned long val = 0;
	
    struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

	if(strict_strtoul(buf, 0,&val) < 0){
		return -EPERM;
	}

	dprintk(KERN_INFO "msm-debug:val = 0x%x\r\n",(int)val);

	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_spider_dev->gpo_map = val;

	up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return count;
}

static ssize_t gpi_map_show(struct device* dev, struct device_attribute* attr, char* buf) {
    ssize_t ret;
	struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);        

    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

   	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	ret = sprintf(buf,"%d\n", fleety_spider_dev->gpi_map);
	
	dprintk(KERN_INFO "msm-debug:gpi_map = 0x%x\r\n",fleety_spider_dev->gpi_map);
	
    up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return ret;
}

static ssize_t gpi_map_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) { 
    unsigned long val = 0;
	
    struct fleety_spider* fleety_spider_dev = (struct fleety_spider *)dev_get_drvdata(dev);  
    
    dprintk(KERN_INFO "msm-debug:%s::+++++++++++++++\r\n",__FUNCTION__);

	if(strict_strtoul(buf, 0,&val) < 0){
		return -EPERM;
	}

	dprintk(KERN_INFO "msm-debug:val = 0x%x\r\n",(int)val);

	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	fleety_spider_dev->gpi_map = val;

	up(&(fleety_spider_dev->sem));
    
    dprintk(KERN_INFO "msm-debug:%s::---------------\r\n",__FUNCTION__);
    
	return count;
}

static int fleety_spider_probe(struct platform_device *pdev)
{
	dev_t dev = 0;
	
	struct fleety_spider *fleety_spider_dev;
	//struct fleety_spider_platform_data  * fleety_spider_pdata = pdev->dev.platform_data;

	struct dentry *ent;
	
	//int ret;
	int err = 0;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	fleety_spider_dev = devm_kzalloc(&pdev->dev,sizeof(struct fleety_spider), GFP_KERNEL);
	if (fleety_spider_dev == NULL) {
		dev_err(&pdev->dev, "failed to alloc platform device memory\r\n");
		return -ENOMEM;
	}

	memset(fleety_spider_dev, 0, sizeof(struct fleety_spider));
	sema_init(&(fleety_spider_dev->sem),1);

	// init timer
    init_timer(&fleety_spider_dev->timer);
    fleety_spider_dev->timer.expires = jiffies + FLEETY_SPIDER_APP_READY_MIN * 60 * HZ;       // 25mins   
    fleety_spider_dev->timer.function = app_ready_timer_treat;
	fleety_spider_dev->timer.data = (unsigned long)fleety_spider_dev;
    add_timer(&fleety_spider_dev->timer);

	// init delayed work queue
	INIT_DELAYED_WORK(&fleety_spider_dev->heart_beat_work, heart_beat_work_fn);

	fleety_spider_dev->version = 0x0001;
	fleety_spider_dev->heart_beat = HEART_BEAT_DEFAULT_TIMES;
	fleety_spider_dev->touch_en = 0x01;

	// new fleety_spider_class folder in /sys/class/
	fleety_spider_dev->fleety_spider_class = class_create(THIS_MODULE, FLEETY_SPIDER_DEVICE_CLASS_NAME);
	if(IS_ERR(fleety_spider_dev->fleety_spider_class)) {
		err = PTR_ERR(fleety_spider_dev->fleety_spider_class);
		printk(KERN_ERR "Failed to create fleety class.\r\n");
	
		devm_kfree(&pdev->dev,fleety_spider_dev);
		
		return err;
	}

	fleety_spider_dev->fleety_spider_class->dev_attrs = fleety_spider_device_attributes;

	// new fleety_spider folder in /sys/class/fleety_spider_class
	fleety_spider_dev->fleety_spider_device = device_create(fleety_spider_dev->fleety_spider_class, NULL, dev, "%s", FLEETY_SPIDER_DEVICE_FILE_NAME);
	if(IS_ERR(fleety_spider_dev->fleety_spider_device)) {
		err = PTR_ERR(fleety_spider_dev->fleety_spider_device);
		printk(KERN_ERR "Failed to create fleety device.(ErrID = %d)\r\n",err);

		devm_kfree(&pdev->dev,fleety_spider_dev);
		return err;
	}

	// new debugfs/spider
	fleety_spider_dev->fleety_spider_debug_root = debugfs_create_dir("spider", NULL);
	if (!fleety_spider_dev->fleety_spider_debug_root)
	{		
		printk(KERN_ERR "Failed to create debug root dir.\r\n");

		dev_err(&pdev->dev,"Couldn't create debug root dir\n");
		devm_kfree(&pdev->dev,fleety_spider_dev);
		
		return err;
	}

	// new debugfs/spider/heart_beat
	ent = debugfs_create_u32("heart_beat", S_IFREG | S_IRUGO,
					  fleety_spider_dev->fleety_spider_debug_root,
					  &fleety_spider_dev->heart_beat);
	if (IS_ERR(ent)){
		printk(KERN_ERR "Failed to create debug heart_beat file.\r\n");
		
		dev_err(&pdev->dev,"Couldn't create debug heart_beat file.\n");
		err=PTR_ERR(ent);

		return err;
	}

	// new debugfs/spider/app_ready
	ent = debugfs_create_u32("app_ready", S_IFREG | S_IRUGO,
					  fleety_spider_dev->fleety_spider_debug_root,
					  &fleety_spider_dev->app_ready);
	if (IS_ERR(ent)){
		printk(KERN_ERR "Failed to create debug app_ready file.\r\n");
		
		dev_err(&pdev->dev,"Couldn't create debug app_ready file.\n");
		err=PTR_ERR(ent);

		return err;
	}
	
	/*
	ent = debugfs_create_file("heart_beat", S_IFREG | S_IRUGO,
					  fleety_spider_dev->fleety_spider_debug_root, fleety_spider_dev,
					  &spider_debugfs_ops);
	if (IS_ERR(ent)){
		printk(KERN_ERR "Failed to create debug heart_beat file.\r\n");
		
		dev_err(&pdev->dev,"Couldn't create debug heart_beat file.\n");
		err=PTR_ERR(ent);

		return err;
	}
	*/
	

	// new debugfs/spider/on
	ent = debugfs_create_bool("on", S_IFREG | S_IRUGO,
					  fleety_spider_dev->fleety_spider_debug_root,
					  &fleety_spider_dev->debug_on);
	if (IS_ERR(ent)){
		printk(KERN_ERR "Failed to create debug heart_beat file.\r\n");
		
		dev_err(&pdev->dev,"Couldn't create debug heart_beat file.\n");
		err=PTR_ERR(ent);

		return err;
	}

		
	// 保存成设备的私有数据
	dev_set_drvdata(fleety_spider_dev->fleety_spider_device, fleety_spider_dev);
	// 保存成平台总线设备的私有数据
	platform_set_drvdata(pdev, fleety_spider_dev);

	dprintk(KERN_INFO "%s::%s::---------------\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	return err;
}

static int fleety_spider_remove(struct platform_device *pdev)
{
	struct fleety_spider *fleety_spider_dev = (struct fleety_spider *)platform_get_drvdata(pdev);

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	debugfs_remove_recursive(fleety_spider_dev->fleety_spider_debug_root);

	devm_kfree(&pdev->dev,fleety_spider_dev);

	dprintk(KERN_INFO "%s::%s::---------------\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	return 0;
}

static int __init fleety_spider_init(void)
{
    int ret;
	 
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	ret = platform_device_register(&fleety_spider_platform_device);
    if (ret) {
		printk(KERN_INFO "platform_device_register failed!\r\n");
		return ret;
    }
    
    ret = platform_driver_register(&fleety_spider_platform_driver);
    if (ret) {
        printk(KERN_INFO "platform_driver_register failed!\r\n");
        return ret;
    }

    dprintk(KERN_INFO "%s::%s::---------------\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

    return ret;
}

static void __exit fleety_spider_exit(void)
{
    dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	platform_driver_unregister(&fleety_spider_platform_driver);
	platform_device_unregister(&fleety_spider_platform_device);

    dprintk(KERN_INFO "%s::%s::---------------\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);
}

int fleety_set_gpo_map(int index,int level)
{
	struct fleety_spider *fleety_spider_dev;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	fleety_spider_dev = (struct fleety_spider *)platform_get_drvdata(&fleety_spider_platform_device);

	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	if(level)
		fleety_spider_dev->gpo_map |= 0x01 << index;
	else
		fleety_spider_dev->gpo_map &= ~(0x01 << index);
	
    up(&(fleety_spider_dev->sem));
    
	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	return 0;
}
EXPORT_SYMBOL_GPL(fleety_set_gpo_map);

int fleety_set_gpi_map(int index,int level)
{
	struct fleety_spider *fleety_spider_dev;

	dprintk(KERN_INFO "%s:%s::+++++++++++++++\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	fleety_spider_dev = (struct fleety_spider *)platform_get_drvdata(&fleety_spider_platform_device);

	if(down_interruptible(&(fleety_spider_dev->sem))) {                
		return -ERESTARTSYS;        
	}

	if(level)
		fleety_spider_dev->gpi_map |= 0x01 << index;
	else
		fleety_spider_dev->gpi_map &= ~(0x01 << index);

	up(&(fleety_spider_dev->sem));
 
	dprintk(KERN_INFO "%s:%s::---------------\r\n",DEBUG_FLEETY_SPIDER,__FUNCTION__);

	return 0;
}
EXPORT_SYMBOL_GPL(fleety_set_gpi_map);


module_init(fleety_spider_init);
module_exit(fleety_spider_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fleety Spider Driver");
MODULE_AUTHOR("xuwy");
MODULE_VERSION("v1.0");

