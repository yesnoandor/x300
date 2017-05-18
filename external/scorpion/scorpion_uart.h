#ifndef _SCORPION_UART_H
#define	_SCORPION_UART_H

#ifdef __cplusplus
extern "C" {
#endif


int scorpion_uart_start_server(void);
void scorpion_uart_stop_server(void);

int scorpion_uart_send(unsigned char *buff,int len);

#ifdef __cplusplus
}
#endif

#endif