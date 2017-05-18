#include		"scorpion_misc.h"


// ----------------- Declaration Debug Zone ----------------
#define		LOG_TAG				"ScorpionService"
//#define		LOG_NDEBUG 				1		
#include	<utils/Log.h>

unsigned short crc16_compute(const unsigned char * p_data, unsigned int size, const unsigned short * p_crc)
{
    unsigned int i;
    unsigned short crc = (p_crc == NULL) ? 0xffff : *p_crc;

    for (i = 0; i < size; i++)
    {
        crc = (unsigned char)(crc >> 8) | (crc << 8);
        crc ^= p_data[i];
        crc ^= (unsigned char)(crc & 0xff) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xff) << 4) << 1;
    }
    
    return crc;
}


unsigned char xor8_computer(unsigned char * p_data, unsigned int len)
{
	unsigned char xor = 0;
	unsigned int i;

	for(i=0;i<len;i++)
	{
		xor ^= p_data[i];
	}
		
	return xor;
}

unsigned int power(int base, int times) 
{
	int i;
	unsigned int rusult = 1;
	
	for(i=0;i < times; i++)
		rusult *= base;

	return rusult;
}

unsigned int bcd2decimal(unsigned char *buf, int len)
{
	int	i;
	unsigned int tmp;
	unsigned int dec = 0;
	
	for(i=0;i<len;i++)
	{
		tmp = ((buf[i] >> 4) & 0x0F) * 10 + (buf[i]&0x0F);
		dec += tmp * power(100, len-1-i);    
	}

	return dec;
}

