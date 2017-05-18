#ifndef _STM32_ADC_H
#define	_STM32_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

void stm32_send_adc_req(unsigned char channel);

void stm32_get_adc_treat(unsigned char *buf,int len);

#ifdef __cplusplus
}
#endif

#endif

