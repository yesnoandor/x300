#include	<stdbool.h>

#include	"scorpion_stm32.h"
#include	"scorpion_stm32_delay_time.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionSTM32"
#include	<cutils/log.h>


typedef  struct
{
    unsigned char start_magic_id;
    stm32_protocol_header_t header;
	unsigned int  delays;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((packed)) stm32_delay_time_t;


void stm32_send_delay_time(unsigned int delay)
{
	stm32_delay_time_t delay_time;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	stm32_protocol_build_header(&delay_time.header,SYSTEM2MCU_DELAY_TIME,sizeof(delay));

	stm32_protocol_debug_header(&delay_time.header);

	delay_time.delays = delay;

	stm32_protocol_send_data((unsigned char *)&delay_time,sizeof(stm32_delay_time_t));
		
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}


