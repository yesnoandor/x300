#include	<stdbool.h>

#include	"scorpion_protocol.h"
#include	"scorpion_protocol_volumn.h"

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
	unsigned char volumn;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) stm32_volumn_t;


unsigned char stm32_volumn_treat(unsigned char *buf,int len)
{
	unsigned char volumn;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	volumn = *(unsigned char *)(&buf[1 + sizeof(stm32_protocol_header_t)]);

	ALOGI("volumn = 0x%x\r\n",volumn);

	scorpion_report_volumn_in(volumn);

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return STM32_PROTOCOL_RESULT_OK;
}




