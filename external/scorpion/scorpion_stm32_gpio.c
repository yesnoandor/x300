#include	<stdbool.h>

#include	"scorpion_protocol_gpio.h"

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
	unsigned int  out;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((packed)) stm32_gpio_out_t;

typedef  struct
{
    unsigned char start_magic_id;
    stm32_protocol_header_t header;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((packed)) stm32_protocol_gpio_in_req_t;


void stm32_send_gpio_out(unsigned int out)
{
	stm32_gpio_out_t gpio_out;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	stm32_protocol_build_header(&gpio_out.header,SYSTEM2MCU_GPIO_OUT,sizeof(out));

	stm32_protocol_debug_header(&gpio_out.header);

	gpio_out.out = out;

	stm32_protocol_send_data((unsigned char *)&gpio_out,sizeof(stm32_gpio_out_t));
		
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}


void stm32_send_gpio_in_req(void)
{
	stm32_protocol_gpio_in_req_t gpio_in_req;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	stm32_protocol_build_header(&gpio_in_req.header,SYSTEM2MCU_GPIO_IN_REQ,0);

	stm32_protocol_debug_header(&gpio_in_req.header);

	stm32_protocol_send_data((unsigned char *)&gpio_in_req,sizeof(stm32_protocol_gpio_in_req_t));
		
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}

unsigned char stm32_gpio_in_rep_treat(unsigned char *buf,int len)
{
	unsigned short gpio_in;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	gpio_in = *(unsigned short *)(&buf[1 + sizeof(stm32_protocol_header_t)]);

	ALOGI("gpio_in = 0x%x\r\n",gpio_in);

	//scorpion_report_gpio_in(gpio_in);

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return STM32_PROTOCOL_RESULT_OK;
}


unsigned char stm32_gpio_in_treat(unsigned char *buf,int len)
{
	unsigned int gpio_in;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	gpio_in = *(unsigned int *)(&buf[1 + sizeof(stm32_protocol_header_t)]);

	ALOGI("gpio_in = 0x%x\r\n",gpio_in);

	scorpion_report_gpio_in(gpio_in);

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return STM32_PROTOCOL_RESULT_OK;
}



