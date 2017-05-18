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
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) stm32_heart_beat_t;

void stm32_send_heart_beat(void)
{
	int len;
	stm32_heart_beat_t heart_beat;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	len = 0;
	
	stm32_protocol_build_header(&heart_beat.header,SYSTEM2MCU_HEART_BEAT,len);

	stm32_protocol_debug_header(&heart_beat.header);

	stm32_protocol_send_data((unsigned char *)&heart_beat,sizeof(stm32_heart_beat_t));
		
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}



