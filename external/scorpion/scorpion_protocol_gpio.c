#include	<stdbool.h>

#include	"scorpion_protocol.h"

#include	"scorpion_protocol_event.h"

#include	"scorpion_protocol_gpio.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>

#if 0
static void protocol_build_gpio_in(msg_gpio_in_t * in,unsigned int index,unsigned int level)
{
	in->channel = 0x01 << index;
	in->level = level << index;
}

void scorpion_report_gpio_in(unsigned int index,unsigned int level)
{
	protocol_gpio_in_t protocol_gpio_in;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	scorpion_protocol_build_header(&protocol_gpio_in.header,SCORPION2APK_GPIO_IN,sizeof(msg_gpio_in_t));
	protocol_build_gpio_in(&protocol_gpio_in.in,index,level);
	scorpion_protocol_send_data((unsigned char *)&protocol_gpio_in,sizeof(protocol_gpio_in_t));

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}
#endif

void scorpion_report_gpio_in(unsigned int gpio_in)
{
	protocol_gpio_in_t protocol_gpio_in;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	scorpion_protocol_build_header(&protocol_gpio_in.header,SCORPION2APK_GPIO_IN,sizeof(msg_gpio_in_t));
	*(unsigned int *)&protocol_gpio_in.in = gpio_in;
	scorpion_protocol_send_data((unsigned char *)&protocol_gpio_in,sizeof(protocol_gpio_in_t));

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}
