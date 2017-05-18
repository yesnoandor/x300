#ifndef _SCORPION_PROTOCOL_HEART_BEAT_H
#define	_SCORPION_PROTOCOL_HEART_BEAT_H

#ifdef __cplusplus
extern "C" {
#endif

void scorpion_heart_beat(unsigned char * buf,int len);
void scorpion_heart_beat_treat(void);

#ifdef __cplusplus
}
#endif

#endif
