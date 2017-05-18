#ifndef _SCORPION_PROTOCOL_EVENT_H
#define	_SCORPION_PROTOCOL_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct{
	unsigned short channel;
	unsigned short level;
}__attribute__((__packed__)) msg_gpio_in_t;


typedef  struct
{
	unsigned char start_magic_id;
    scorpion_protocol_header_t header;
	msg_gpio_in_t in;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) protocol_gpio_in_t;


void scorpion_report_event_in(unsigned int index,unsigned int level);



#ifdef __cplusplus
}
#endif

#endif
