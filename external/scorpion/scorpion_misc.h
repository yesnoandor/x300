#ifndef _SCORPION_MISC_H
#define	_SCORPION_MISC_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct{
	unsigned char * buf;					
	int len;									
}buff_mgr_t;

typedef struct{
	buff_mgr_t raw;												
	buff_mgr_t now;					
}double_buff_mgr_t;

unsigned short crc16_compute(const unsigned char * p_data, unsigned int size, const unsigned short * p_crc);
unsigned char xor8_computer(unsigned char * p_data, unsigned int len);


#ifdef __cplusplus
}
#endif

#endif
