#include	<stdbool.h>

#include	"scorpion_stm32_delay_time.h"

#include	"scorpion_protocol.h"
#include	"scorpion_protocol_delay_time.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>


			
void scorpion_delay_time_treat(unsigned char * buf,int len)
{
	unsigned int delay;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	delay = *(unsigned int *)buf;
	stm32_send_delay_time(delay);
	
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}
