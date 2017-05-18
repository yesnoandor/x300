#include	<stdbool.h>

#include	"scorpion_protocol.h"

#include	"scorpion_protocol_event.h"

#include	"scorpion_protocol_gpio.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>

#define		DEFAULT_HEART_BEAT_THREHOLD		100


static unsigned int scorpion_heart_beat_count = 0;
static unsigned int scorpion_heart_beat_threhold = DEFAULT_HEART_BEAT_THREHOLD; 

void scorpion_heart_beat(unsigned char * buf,int len)
{
	unsigned int threhold;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	threhold = *(unsigned int *)buf;
	ALOGI("threhold = %d\r\n",threhold);
	
	if(threhold)
	{
		scorpion_heart_beat_threhold = threhold;	
	}

	scorpion_heart_beat_count = 0;
	
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}


void scorpion_heart_beat_treat(void)
{
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	scorpion_heart_beat_count++;
	if(scorpion_heart_beat_count > scorpion_heart_beat_threhold)
	{
		scorpion_reboot();
	}
	
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}
