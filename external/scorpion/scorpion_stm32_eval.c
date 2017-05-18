#include	<stdbool.h>

#include	"scorpion_protocol.h"
#include	"scorpion_protocol_eval.h"

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
	unsigned char eval;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((packed)) stm32_eval_t;




unsigned char stm32_eval_treat(unsigned char *buf,int len)
{
	unsigned char eval;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	eval = *(unsigned char *)(&buf[1 + sizeof(stm32_protocol_header_t)]);

	ALOGI("eval = 0x%x\r\n",eval);

	scorpion_report_eval_in(eval);

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return STM32_PROTOCOL_RESULT_OK;
	
}




