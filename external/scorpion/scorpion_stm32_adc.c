#include	<stdbool.h>

//#include	"scorpion_uart.h"
#include	"scorpion_stm32.h"
#include	"scorpion_stm32_heart_beat.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionSTM32"
#include	<cutils/log.h>

typedef  struct
{
    unsigned char start_magic_id;
    stm32_protocol_header_t header;
	unsigned char channel;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((packed)) stm32_protocol_adc_t;


void stm32_send_adc_req(unsigned char channel)
{
	stm32_protocol_adc_t adc;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	stm32_protocol_build_header(&adc.header,SYSTEM2MCU_ADC_REQ,sizeof(channel));

	stm32_protocol_debug_header(&adc.header);

	adc.channel = channel;

	stm32_protocol_send_data((unsigned char *)&adc,sizeof(stm32_protocol_adc_t));
		
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}

void stm32_get_adc_treat(unsigned char *buf,int len)
{
	unsigned short adc_in;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	adc_in = *(unsigned short *)(&buf[1 + sizeof(stm32_protocol_header_t)]);

	ALOGI("adc_in = 0x%x\r\n",adc_in);


	ALOGI("%s::---------------\r\n",__FUNCTION__);
	
}



