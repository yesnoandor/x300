#ifndef _SCORPION_PROTOCOL_H
#define	_SCORPION_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

//#define 		__packed 		__attribute__((packed))

#define			SCORPION2APK_GENERAL_ACK	0x8001
#define			SCORPION2APK_EVENT_IN		0x8002
#define			SCORPION2APK_GPIO_IN		0x8003
#define			SCORPION2APK_ADC_REP		0x8004
#define			SCORPION2APK_EVAL_IN		0x8005
#define			SCORPION2APK_VOLUMN_IN		0x8006
#define			SCORPION2APK_SHELL_REP		0x8010

#define			APK2SCORPION_GENERAL_ACK	0x0001
#define			APK2SCORPION_HEART_BEAT		0x0002
#define			APK2SCORPION_GPIO_OUT		0x0003
#define			APK2SCORPION_ADC_REQ		0x0004
#define			APK2SCORPION_DELAY_TIME		0x0009
#define			APK2SCORPION_SHELL_REQ		0x0010
#define			APK2SCORPION_EX_VERSION_REQ	0x0040
#define			APK2SCORPION_EX_REBOOT		0x0080



enum{
	SCORPION_PROTOCOL_RESULT_OK = 0,
	SCORPION_PROTOCOL_RESULT_FAIL,
	SCORPION_PROTOCOL_RESULT_UNKNOWN,
};



/*
* SCORPION 通讯协议的头信息
*/
typedef struct{
	unsigned short msg_id;							// 消息ID
	unsigned short msg_len;							// 消息体长度
	unsigned short msg_serial_number;				// 消息体流水号 （按发送顺序从0 开始循环累加）
	unsigned short reserved;
}__attribute__((__packed__)) scorpion_protocol_header_t ,*p_scorpion_protocol_header_t;



/*
* 串口通讯协议的应答信息
*/
typedef struct{
	unsigned short seq;
	unsigned short id;
	unsigned char result;
}__attribute__((__packed__)) scorpion_protocol_ack_t;

typedef  struct
{
    unsigned char start_magic_id;
    scorpion_protocol_header_t header;
    scorpion_protocol_ack_t ack;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) scorpion_protocol_general_ack_t;

void scorpion_protocol_build_header(scorpion_protocol_header_t * header, unsigned short msg_id, unsigned short msg_len);
int scorpion_protocol_send_data(unsigned char * buf, int len);
int scorpion_protocol_ayalyze(unsigned char * buf,int len);

void scorpion_protocol_debug_header(scorpion_protocol_header_t * header);
void scorpion_protocol_debug_ack(scorpion_protocol_ack_t * ack);


#ifdef __cplusplus
}
#endif

#endif
