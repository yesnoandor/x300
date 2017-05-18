/*
 * Copyright (C) 2009 The Android Open Source Project
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
#include	"jni.h"
#include	"JNIHelp.h"
#include	"android_runtime/AndroidRuntime.h"

#include	<utils/misc.h>


#include	<hardware/hardware.h>
#include	<hardware/spider.h>

#include	<stdio.h>

// ----------------- Declaration Debug Zone ----------------
#define		LOG_TAG		"SpiderService"
#include	<utils/Log.h>



namespace android
{
	// 在硬件抽象层中定义的硬件访问结构体，参考<hardware/spider.h>
 	struct spider_device_t* spider_device = NULL;
	
	// 通过硬件抽象层定义的硬件访问接口设置
	static void spider_set_app_ready(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
		ALOGI("SpiderService JNI: set value %d to device.", val);
		
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_app_ready(spider_device, val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	}
 	
	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_heart_beat(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_heart_beat(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_relay1_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_relay1_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_relay2_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_relay2_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_relay3_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_relay3_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_relay4_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_relay4_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_amp_stb_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_amp_stb_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_amp_mute_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_amp_mute_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_otg_switch_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
		
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
		ALOGI("SpiderService JNI: set value %d to device.", val);
		
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
	
		spider_device->set_otg_switch_gpio(spider_device,val);
		
		ALOGI("%s::---------------\r\n",__FUNCTION__);
			
		return;
	}
	

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_usb2eth_rst_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_usb2eth_rst_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_usbhub_rst_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_usbhub_rst_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_stm32_rst_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_stm32_rst_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_camera_power_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

		ALOGI("SpiderService JNI: set value %d to device.", val);

		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
		
		spider_device->set_camera_power_gpio(spider_device,val);

		ALOGI("%s::---------------\r\n",__FUNCTION__);
	
		return;
	}

	
	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_tachograph_power_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
	
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
		ALOGI("SpiderService JNI: set value %d to device.", val);
	
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
			
		spider_device->set_tachograph_power_gpio(spider_device,val);
	
		ALOGI("%s::---------------\r\n",__FUNCTION__);
		
		return;
	}

	
	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_ex_power_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
		
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
		ALOGI("SpiderService JNI: set value %d to device.", val);
		
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
				
		spider_device->set_ex_power_gpio(spider_device,val);
		
		ALOGI("%s::---------------\r\n",__FUNCTION__);
			
		return;
	}
	
	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_finger_power_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
		
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
		ALOGI("SpiderService JNI: set value %d to device.", val);
		
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
				
		spider_device->set_finger_power_gpio(spider_device,val);
		
		ALOGI("%s::---------------\r\n",__FUNCTION__);
			
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_tsm_power_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
	
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
		ALOGI("SpiderService JNI: set value %d to device.", val);
	
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
			
		spider_device->set_tsm_power_gpio(spider_device,val);
	
		ALOGI("%s::---------------\r\n",__FUNCTION__);
		
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_fan_power_gpio(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
	
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
		ALOGI("SpiderService JNI: set value %d to device.", val);
	
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
			
		spider_device->set_fan_power_gpio(spider_device,val);
	
		ALOGI("%s::---------------\r\n",__FUNCTION__);
		
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_gpio_out_map(JNIEnv* env, jobject clazz, jint value) {
		int val = value;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
		ALOGI("SpiderService JNI: set value %d to device.", val);
		
		if(!spider_device) {
				ALOGI("SpiderService JNI: device is not open.");
				return;
		}
				
		spider_device->set_gpio_out_map(spider_device,val);
		
		ALOGI("%s::---------------\r\n",__FUNCTION__);
			
		return;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static jint spider_get_gpio_out_map(JNIEnv* env, jobject clazz) {
		int val;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
		
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return -1;
		}
				
		val = spider_device->get_gpio_out_map(spider_device);

		ALOGI("SpiderService JNI: get value %d to device.", val);
				
		ALOGI("%s::---------------\r\n",__FUNCTION__);
			
		return val;
	}

	
	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_gpio_in_map(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
	
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
			
		ALOGI("SpiderService JNI: set value %d to device.", val);
			
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
					
		spider_device->set_gpio_in_map(spider_device,val);
			
		ALOGI("%s::---------------\r\n",__FUNCTION__);
				
		return;
	}
	
	// 通过硬件抽象层定义的硬件访问接口
	static jint spider_get_gpio_in_map(JNIEnv* env, jobject clazz) {
		int val;
	
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return -1;
		}
					
		val = spider_device->get_gpio_in_map(spider_device);
	
		ALOGI("SpiderService JNI: get value %d to device.", val);
		
		ALOGI("%s::---------------\r\n",__FUNCTION__);
				
		return val;
	}

	// 通过硬件抽象层定义的硬件访问接口
	static void spider_set_backlight(JNIEnv* env, jobject clazz, jint value) {
		int val = value;
		
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
		ALOGI("SpiderService JNI: set value %d to device.", val);
		
		if(!spider_device) {
			ALOGI("SpiderService JNI: device is not open.");
			return;
		}
				
		spider_device->set_backlight(spider_device,val);
		
		ALOGI("%s::---------------\r\n",__FUNCTION__);
			
		return;
	}
    
	// 通过硬件抽象层定义的硬件模块打开接口打开硬件设备
	static inline int spider_device_open(const hw_module_t* module, struct spider_device_t** device) {
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		ALOGI("%s::---------------\r\n",__FUNCTION__);
		
		return module->methods->open(module, SPIDER_HARDWARE_MODULE_ID, (struct hw_device_t**)device);
	}

 	// 通过硬件模块ID 来加载指定的硬件抽象层模块并打开硬件
	static jboolean spider_init(JNIEnv* env, jclass clazz) {
		spider_module_t* module;

		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);


		if(hw_get_module(SPIDER_HARDWARE_MODULE_ID, (const struct hw_module_t**)&module) == 0) {
			ALOGI("SpiderService JNI: spider Stub found.");
			
			if(spider_device_open(&(module->common), &spider_device) == 0) {
				ALOGI("SpiderService JNI: spider device is open.");

				ALOGI("%s::---------------\r\n",__FUNCTION__);
				
				return JNI_TRUE;
			}
			
			ALOGE("SpiderService JNI: failed to open spider device.");
			ALOGI("%s::---------------\r\n",__FUNCTION__);
			return JNI_FALSE;
		}
		
		ALOGI("%s::---------------\r\n",__FUNCTION__);
		
		return JNI_FALSE;		
	}

	// JNI 方法表
	static const JNINativeMethod method_table[] = {
		{"init_native", "()Z", (void*)spider_init},
		{"set_app_ready_native", "(I)V", (void*)spider_set_app_ready},
		{"set_heart_beat_native", "(I)V", (void*)spider_set_heart_beat},
		{"set_relay1_gpio_native", "(I)V", (void*)spider_set_relay1_gpio},
		{"set_relay2_gpio_native", "(I)V", (void*)spider_set_relay2_gpio},
		{"set_relay3_gpio_native", "(I)V", (void*)spider_set_relay3_gpio},
		{"set_relay4_gpio_native", "(I)V", (void*)spider_set_relay4_gpio},
		{"set_otg_switch_gpio_native", "(I)V", (void*)spider_set_otg_switch_gpio},
		{"set_amp_stb_gpio_native", "(I)V", (void*)spider_set_amp_stb_gpio},
		{"set_amp_mute_gpio_native", "(I)V", (void*)spider_set_amp_mute_gpio},
		{"set_usb2eth_rst_gpio_native", "(I)V", (void*)spider_set_usb2eth_rst_gpio},
		{"set_usbhub_rst_gpio_native", "(I)V", (void*)spider_set_usbhub_rst_gpio},
		{"set_stm32_rst_gpio_native", "(I)V", (void*)spider_set_stm32_rst_gpio},
		{"set_camera_power_gpio_native", "(I)V", (void*)spider_set_camera_power_gpio},
		{"set_tachograph_power_gpio_native", "(I)V", (void*)spider_set_tachograph_power_gpio},
		{"set_ex_power_gpio_native", "(I)V", (void*)spider_set_ex_power_gpio},
		{"set_finger_power_gpio_native", "(I)V", (void*)spider_set_finger_power_gpio},
		{"set_tsm_power_gpio_native", "(I)V", (void*)spider_set_tsm_power_gpio},
		{"set_fan_power_gpio_native", "(I)V", (void*)spider_set_fan_power_gpio},
		{"set_gpio_out_map_native", "(I)V", (void*)spider_set_gpio_out_map},		
		{"get_gpio_out_map_native", "()I", (void*)spider_get_gpio_out_map},
		{"set_gpio_in_map_native", "(I)V", (void*)spider_set_gpio_in_map},		
		{"get_gpio_in_map_native", "()I", (void*)spider_get_gpio_in_map},
		{"set_backlight_native", "(I)V", (void*)spider_set_backlight},
	};

	// 注册JNI方法
	int register_android_server_SpiderService(JNIEnv *env) {
		ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
        ALOGI("SpiderService JNI: register......");
    	return jniRegisterNativeMethods(env, "com/android/server/SpiderService", method_table, NELEM(method_table));
	}

};

