#ifndef _STM32_PROTOCOL_H
#define	_STM32_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

//#define 		__packed 		__attribute__((packed))

#define			MCU2SYSTEM_GENERAL_ACK		0x8001
#define			MCU2SYSTEM_GPIO_IN			0x8003
#define			MCU2SYSTEM_ADC_REP			0x8004
#define			MCU2SYSTEM_EVAL_IN			0x8005
#define			MCU2SYSTEM_VOLUMN_IN		0x8006
#define			MCU2SYSTEM_GPIO_IN_REP		0x8007
#define			MCU2SYSTEM_VERSION_REP		0x8040

#define			SYSTEM2MCU_GENERAL_ACK		0x0001
#define			SYSTEM2MCU_HEART_BEAT		0x0002
#define			SYSTEM2MCU_GPIO_OUT			0x0003
#define			SYSTEM2MCU_ADC_REQ			0x0004
#define			SYSTEM2MCU_GPIO_IN_REQ		0x0007
#define			SYSTEM2MCU_DELAY_TIME		0x0009
#define			SYSTEM2MCU_VERSION_REQ		0x0040

#define			SYSTEM2MCU_REBOOT			0x0080




enum{
	STM32_PROTOCOL_RESULT_OK = 0,
	STM32_PROTOCOL_RESULT_FAIL,
	STM32_PROTOCOL_RESULT_UNKNOWN,
};



/*
* STM32 串口通讯协议的头信息
*/
typedef struct{
	unsigned short msg_id;							// 消息ID
	unsigned short msg_len;							// 消息体长度
	unsigned short msg_serial_number;				// 消息体流水号 （按发送顺序从0 开始循环累加）
	unsigned short reserved;
}__attribute__((__packed__)) stm32_protocol_header_t ,*p_stm32_protocol_header_t;



/*
* 串口通讯协议的应答信息
*/
typedef struct{
	unsigned short seq;
	unsigned short id;
	unsigned char result;
}__attribute__((__packed__)) stm32_protocol_ack_t;

typedef  struct
{
    unsigned char start_magic_id;
    stm32_protocol_header_t header;
    stm32_protocol_ack_t ack;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) stm32_protocol_general_ack_t;

bool stm32_protocol_init(void);

void stm32_protocol_recv_treat(unsigned char * buf,int len);
void stm32_protocol_event_treat(void * param);

int stm32_protocol_ayalyze(unsigned char * buf,int len);

void stm32_protocol_send_ack(stm32_protocol_header_t * header,unsigned char result);
int stm32_protocol_send_data(unsigned char * buf, int len);

void stm32_protocol_build_header(stm32_protocol_header_t * header, unsigned short msg_id, unsigned short msg_len);

unsigned char stm32_gpio_in_rep_treat(unsigned char *buf,int len);

void stm32_protocol_debug_header(stm32_protocol_header_t * header);
void stm32_protocol_debug_ack(stm32_protocol_ack_t * ack);


#ifdef __cplusplus
}
#endif

#endif
