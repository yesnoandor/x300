#include	<stdbool.h>

#include	"scorpion_stm32_reboot.h"

#include	"scorpion_protocol.h"
#include	"scorpion_protocol_reboot.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>



void scorpion_reboot(void)
{
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	stm32_send_reboot();
	
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}
