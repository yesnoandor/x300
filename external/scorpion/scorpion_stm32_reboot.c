#include	<stdbool.h>

#include	"scorpion_stm32.h"
#include	"scorpion_stm32_reboot.h"

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
} __attribute__((__packed__)) stm32_protocol_reboot_t;


void stm32_send_reboot(void)
{
	stm32_protocol_reboot_t reboot;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	stm32_protocol_build_header(&reboot.header,SYSTEM2MCU_REBOOT,0);

	stm32_protocol_debug_header(&reboot.header);

	stm32_protocol_send_data((unsigned char *)&reboot,sizeof(stm32_protocol_reboot_t));

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}




