#ifndef _SCORPION_PROTOCOL_EVAL_H
#define	_SCORPION_PROTOCOL_EVAL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef  struct
{
	unsigned char start_magic_id;
    scorpion_protocol_header_t header;
	unsigned char eval;
    unsigned char xor;
    unsigned char end_magic_id;
} __attribute__((__packed__)) protocol_eval_in_t;


void scorpion_report_eval_in(unsigned char eval);

#ifdef __cplusplus
}
#endif

#endif
