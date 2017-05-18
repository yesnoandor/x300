#ifndef _SCORPION_PROTOCOL_VOLUMN_H
#define	_SCORPION_PROTOCOL_VOLUMN_H

#ifdef __cplusplus
extern "C" {
#endif


typedef  struct
{
	unsigned char start_magic_id;
    scorpion_protocol_header_t header;
	unsigned char volumn;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) protocol_volumn_t;


void scorpion_report_volumn_in(unsigned char volumn);

#ifdef __cplusplus
}
#endif

#endif
