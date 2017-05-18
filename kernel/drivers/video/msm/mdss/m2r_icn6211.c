#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <linux/qpnp/pin.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/leds.h>
#include <linux/proc_fs.h>
#include <linux/qpnp/pwm.h>
#include <linux/err.h>
#include <linux/clk.h>
#include "mdss_dsi.h"

struct device_init_sig_cmd
{
	u8 address;
	u8 data;
};

struct icn6211_platform_data {
	int icn6211_reset_gpio;
	int icn6211_power_en;
	int icn6211_power_enp;
	u32 i2c_tx_len;
};

//int g_m2r_icn6211_flag = 0;
u32 g_rgb_continue_splash_display = 0;
static int icn6211_cmd_num = 0;
static struct class *icn6211_class = NULL;
struct i2c_client *icn6211_i2c_client = NULL;
struct icn6211_platform_data *icn6211_pdata = NULL;
static struct device_init_sig_cmd *icn6211_init_setting = NULL;

static int icn_i2c_write(u8 addr, u8 val, int data_size)
{
	int ret = -1;
	char buf[2] = {0,0};
	
//	char bufTest[2] = {0,0};
	
	buf[0] = addr & 0xff;
	buf[1] = val & 0xff;

	//printk(KERN_EMERG	"%s: i2c_master_send buf_0(0x%x) buf_1(0x%x)!\n", __func__, buf[0], buf[1]);
	
	if ((ret = i2c_master_send( icn6211_i2c_client, (char*)(&buf[0]),	data_size )) != data_size) {
		printk(KERN_ERR	"%s: i2c_master_send failed 1 (%d)!\n", __func__, ret);
	}

#if 0	
	printk(KERN_EMERG	"addr:%x\n", addr);
	//folllow debug purpose.....
	bufTest[0] = addr & 0xff;
	printk(KERN_EMERG "bufTest[0]:%x,%x\n",bufTest[0],bufTest[1]);
	if ((ret = i2c_master_send( icn6211_i2c_client, (char*)(&bufTest[0]),	1 )) != 1) {
		printk(KERN_ERR	"%s: i2c_master_send failed 2 (%d)!\n", __func__, ret);
	}	
	
	if((ret = i2c_master_recv( icn6211_i2c_client, (char*)(&bufTest[1]), 1)) != 1) {
		printk(KERN_ERR	"%s: i2c_master_recv failed 1 (%d)!\n", __func__, ret);	
	}
	printk(KERN_EMERG	"%s: i2c_master_recv buf_0(0x%x) buf_1(0x%x)!\n", __func__, bufTest[0], bufTest[1]);
	//debug ends..
#endif
	return 0;
}

void icn_init_table(struct device_init_sig_cmd *table, int count)
{
	int i = 0;
	u8 addr = 0;
	u8 data = 0;

	for(i = 0; i < count; i++) {
		addr = table->address;
		data = table->data;
//		printk(KERN_EMERG "===>addr[%d]= 0x%x   data[%d]= 0x%x<===\n",i,table->address, i,table->data);
		icn_i2c_write(addr, data, icn6211_pdata->i2c_tx_len);
		usleep(100);
		table++;
	}
}

/* icn6211 init */
void icn6211_bridge_init(void)
{
	icn_init_table(icn6211_init_setting, icn6211_cmd_num);
	return;
}

static void icn6211_reset(struct icn6211_platform_data *icn_info)
{
	int rc = 0;

	rc = gpio_direction_output(icn_info->icn6211_reset_gpio, 1);
	if (rc)
		pr_err("Failed to set icn6211_reset_gpio output\n");

	gpio_set_value(icn_info->icn6211_reset_gpio, 1);
	msleep(50);
	gpio_set_value(icn_info->icn6211_reset_gpio, 0);
	msleep(50);
	gpio_set_value(icn_info->icn6211_reset_gpio, 1);
	msleep(50);
    printk(KERN_EMERG "icn6211_reset bx OK\n");

	//follow debug purpose..
	//n6211_bridge_init();
	//debug ends..
}

static int icn6211_poweron(struct icn6211_platform_data *icn_info)
{
	int rc = 0;

	rc = gpio_direction_output(icn_info->icn6211_power_en, 1);
	if (rc)
		pr_err("Failed to set icn6211_poweron output\n");
	
	rc = gpio_direction_output(icn_info->icn6211_power_enp, 1);
	if (rc)
		pr_err("Failed to set icn6211_poweron ppppp output\n");
	
	printk(KERN_EMERG "%s\n",__FUNCTION__);

	return 0;
}

static int icn6211_power_off(struct icn6211_platform_data *icn_info)
{
	int rc = 0;

	rc = gpio_direction_output(icn_info->icn6211_power_en, 0);
	if (rc)
		pr_err("Failed to set vx5b3d_RGB_LDO_en_gpio output\n");

	rc = gpio_direction_output(icn_info->icn6211_power_enp, 0);
	if (rc)
		pr_err("Failed to set vx5b3d_RGB_LDO_en_gpio output\n");

	printk(KERN_EMERG "%s\n",__FUNCTION__);
	return 0;
}

//add by hubingbing@erobbing.com
static void prase_one_cmd_one_data(const char **buf)
{
	int len = 0;
	u8 val = 0;
	char *p = (char *)*buf;
	struct device_init_sig_cmd *temp = NULL;

	if(NULL == buf || NULL == *buf)
		return;
	//get len of cmd
	for(;(p=strstr(p,"0x"))!=0;p++)
		len++;
	icn6211_cmd_num = len/2;
	icn6211_init_setting = kzalloc(icn6211_cmd_num * sizeof(struct device_init_sig_cmd), GFP_KERNEL);
	//printk(KERN_EMERG "len/2 = %d\n", len);
	temp = icn6211_init_setting;
	len = 0;
	while(NULL != (p = strsep((char **)buf, ",")))
	{
		//buf[i] = simple_strtoul(p, NULL, 16);
		if(strstr(p,"0x")){
			//printk(KERN_EMERG "P: %s\n", p);
			val = simple_strtoul(p, NULL, 16);
			if(len%2 == 0)
				temp->address = val;
			if(len%2 == 1){
				temp->data = val;
				//printk(KERN_EMERG "addr= 0x%x  data= 0x%x\n",temp->address, temp->data);
				temp++;
			}
			len++;
		}
	}
}

static int icn6211_parse_dt(struct device *dev)
{
	int rc = 0;
	u32 temp_val = 0;
	const char *data = NULL;
	struct device_node *np = dev->of_node;

	/* reset, power en info */
	icn6211_pdata->icn6211_reset_gpio = of_get_named_gpio(np, "icn,reset-gpio", 0);
	icn6211_pdata->icn6211_power_en = of_get_named_gpio(np, "icn,pw-en-gpio", 0);
	icn6211_pdata->icn6211_power_enp = of_get_named_gpio(np, "icn,pw-enp-gpio", 0);
	g_rgb_continue_splash_display = of_property_read_bool(np, "rgb,cont-splash-enabled");
	rc = of_property_read_u32(np, "icn,i2c-tx-len", &temp_val);
	if (!rc)
		icn6211_pdata->i2c_tx_len = temp_val;
	else
		return rc;

	rc = gpio_request(icn6211_pdata->icn6211_power_en,"m2r_LDO_en_gpio");
	if(rc<0)
		printk("%s:%d m2r_LDO_en_gpio faild\n",__FUNCTION__,__LINE__);

	rc = gpio_request(icn6211_pdata->icn6211_power_enp,"m2r_LDO_enp_gpio");
	if(rc<0)
		printk("%s:%d m2r_LDO_enp_gpio faild\n",__FUNCTION__,__LINE__);
	
	rc = gpio_request(icn6211_pdata->icn6211_reset_gpio,"m2r_reset_gpio");
	if(rc<0)
		printk("%s:%d m2r_reset_gpio faild\n",__FUNCTION__,__LINE__);

	rc = of_property_read_string(np,"icn,m2r-init-cmd", &data);
	if (!rc){
		//printk(KERN_EMERG "%s:%d =====%s======\n",__FUNCTION__,__LINE__,data);
		prase_one_cmd_one_data(&data);
	}else{
		printk(KERN_ERR "%s:%d prase icn,m2r-init-cmd ERROR!\n",__FUNCTION__,__LINE__);
	}

	return rc;
}

static ssize_t reset_bridge_store(struct class *class, struct class_attribute *attr, const char *buf, size_t n)
{
	gpio_set_value(icn6211_pdata->icn6211_reset_gpio, 1);
	msleep(5);
	gpio_set_value(icn6211_pdata->icn6211_reset_gpio, 0);
	msleep(50);
	gpio_set_value(icn6211_pdata->icn6211_reset_gpio, 1);
	msleep(1);
	return n;
}

static ssize_t en_bridge_store(struct class *class, struct class_attribute *attr, const char *buf, size_t n)
{
	if(n >= strlen("off") && strncmp(buf, "off", strlen("off")) == 0)
	{
		gpio_direction_output(icn6211_pdata->icn6211_power_en, 0);
		printk(KERN_EMERG "reset_lcd_gpio_state: off \n");
	}

	if(n >= strlen("on") && strncmp(buf, "on", strlen("on")) == 0)
	{
		gpio_direction_output(icn6211_pdata->icn6211_power_en, 1);
		printk(KERN_EMERG "reset_lcd_gpio_state: on \n");
	}

	return n;
}

static struct class_attribute icn6211_class_attrs[] = {
	__ATTR(icn_en_bridge, 0666, NULL, en_bridge_store),
	__ATTR(icn_reset_bridge, 0666, NULL, reset_bridge_store),
	__ATTR_NULL
};

static void icn6211_class_release(struct class *cls)
{
	kfree(cls);
}

static int icn6211_release(struct i2c_client *client)
{
	kfree(icn6211_init_setting);
	kfree(icn6211_class);
	kfree(icn6211_pdata);
	return 0;
}

int icn6211_suspend(void)   //bx5b3a_suspend
{
	int  rc = 0;
	rc = icn6211_power_off(icn6211_pdata);
	if (rc < 0){
		pr_err("Failed to power off vx3b3b\n");
		return rc;
	}

	return rc;
}
EXPORT_SYMBOL_GPL(icn6211_suspend);

int icn6211_resume(void) //bx5b3a_resume
{
	icn6211_poweron(icn6211_pdata);
//	msleep(50);
	icn6211_reset(icn6211_pdata);
	icn6211_bridge_init();
	return 0;
}
EXPORT_SYMBOL_GPL(icn6211_resume);

static int icn6211_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc = 0;
	printk(KERN_EMERG "=============hbb==============>icn6211_probe: on \n");
	icn6211_i2c_client = client;
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA)){
		pr_err("icn6211_probe: i2c_check_functionality() fail!\n");
		return -EIO;
	}

	/*init icn6211_pdata struct*/
	icn6211_pdata = kzalloc(sizeof(*icn6211_pdata), GFP_KERNEL);
	if(client->dev.of_node){
		rc=icn6211_parse_dt(&client->dev);
		if(rc){
			pr_err("quickvx: icn6211_parse_dt() fail!\n");
			return -EIO;
		}
	}

	/*init icn6211_class struct for create sysfs*/
	icn6211_class = kzalloc(sizeof(*icn6211_class), GFP_KERNEL);
	icn6211_class->name = "icn6211";
	icn6211_class->owner = THIS_MODULE;
	icn6211_class->class_release = icn6211_class_release;
	icn6211_class->class_attrs = icn6211_class_attrs;
	rc = class_register(icn6211_class);
	if (rc) {
		kfree(icn6211_class);
		icn6211_class = NULL;
		printk("%s: failed to create class se955_class\n",__FUNCTION__);
		return rc;
	}

	if (!g_rgb_continue_splash_display){
		rc = icn6211_poweron(icn6211_pdata);
		if (rc < 0){
			pr_err("Failed to power on icn6211\n");
			return rc;
		}
		icn6211_reset(icn6211_pdata);
		icn6211_bridge_init();
	}
	return 0;
}

static struct of_device_id icn6211_match_table[] = {
	{ .compatible = "icn,icn6211",},
	{ },
};

static struct i2c_device_id i2c_icn6211_idtable[] = {
	{ "icn6211", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, i2c_icn6211_idtable);

static struct i2c_driver icn6211_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name  = "icn6211",
		.of_match_table = icn6211_match_table,
	},
	.id_table 	= i2c_icn6211_idtable,
	.probe    	= icn6211_probe,
	.remove		= icn6211_release,
};

static int __init icn6211_init(void)
{
	int ret;

	ret = i2c_add_driver(&icn6211_driver);
	if (ret < 0)
		pr_err("icn6211_init: i2c_add_driver failed !\n");

	return ret;
}

static void __exit icn6211_exit(void)
{
	i2c_del_driver(&icn6211_driver);
}

module_init(icn6211_init);
module_exit(icn6211_exit);
