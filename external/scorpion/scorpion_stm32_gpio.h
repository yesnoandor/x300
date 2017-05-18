#ifndef _STM32_GPIO_H
#define	_STM32_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif


void stm32_send_gpio_out(unsigned int out);
unsigned char stm32_gpio_in_treat(unsigned char *buf,int len);


#ifdef __cplusplus
}
#endif

#endif

