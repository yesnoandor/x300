#ifndef _STM32_VERSION_H
#define	_STM32_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif


unsigned char stm32_get_version_treat(unsigned char *buf,int len);
void stm32_send_version_req(void);

#ifdef __cplusplus
}
#endif

#endif