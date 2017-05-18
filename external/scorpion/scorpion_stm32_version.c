#include	<stdbool.h>
#include 	<cutils/properties.h>

#include	"scorpion_stm32.h"
#include	"scorpion_stm32_version.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionSTM32"
#include	<cutils/log.h>

typedef struct
{
	unsigned char hw_ver[2];
	unsigned char sw_ver[2];
	unsigned int  uuid[3];
}__packed stm32_protocol_version_t;

typedef  struct
{
    unsigned char start_magic_id;
    stm32_protocol_header_t header;
    stm32_protocol_version_t version;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) stm32_protocol_version_rep_t;

typedef  struct
{
    unsigned char start_magic_id;
    stm32_protocol_header_t header;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) stm32_protocol_version_req_t;

void stm32_send_version_req(void)
{
	stm32_protocol_version_req_t version_req;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	stm32_protocol_build_header(&version_req.header,SYSTEM2MCU_VERSION_REQ,0);

	stm32_protocol_debug_header(&version_req.header);

	stm32_protocol_send_data((unsigned char *)&version_req,sizeof(stm32_protocol_version_req_t));

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}

unsigned char stm32_get_version_treat(unsigned char *buf,int len)
{
	stm32_protocol_version_t  * version;
	char key[50];
	char value[50];
	int ver;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	version = (stm32_protocol_version_t *)(&buf[1 + sizeof(stm32_protocol_header_t)]);

	ALOGI("hw[0] = 0x%x\r\n",version->hw_ver[0]);
	ALOGI("hw[1] = 0x%x\r\n",version->hw_ver[1]);
	ALOGI("sw[0] = 0x%x\r\n",version->sw_ver[0]);
	ALOGI("sw[1] = 0x%x\r\n",version->sw_ver[1]);
	ALOGI("uuid[0] = 0x%x\r\n",version->uuid[0]);
	ALOGI("uuid[1] = 0x%x\r\n",version->uuid[1]);
	ALOGI("uuid[2] = 0x%x\r\n",version->uuid[2]);

	ver = version->sw_ver[1] | (version->sw_ver[0] << 8);
	ALOGI("sw ver = 0x%x\r\n",ver);
	snprintf(key, sizeof(key), "sys.stm32.sw.version");
	snprintf(value, sizeof(value), "MSM742.%04d",ver);

	ALOGI("key : %s\r\n",key);
	ALOGI("value : %s\r\n",value);
	
	property_set((const char *)key, (const char *)value);

	
	ver = version->hw_ver[1] | (version->hw_ver[0] << 8);
	ALOGI("hw ver = 0x%x\r\n",ver);
	snprintf(key, sizeof(key), "sys.stm32.hw.version");
	snprintf(value, sizeof(value), "MSM742.%04d",ver);

	ALOGI("key : %s\r\n",key);
	ALOGI("value : %s\r\n",value);

	property_set((const char *)key, (const char *)value);

	//scorpion_report_volumn_in(volumn);

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return STM32_PROTOCOL_RESULT_OK;
}




