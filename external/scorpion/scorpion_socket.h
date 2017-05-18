#ifndef _SCORPION_SOCKET_H
#define	_SCORPION_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif



int scorpion_socket_start_server(void);
int scorpion_socket_send(unsigned char *buff,int len);

#ifdef __cplusplus
}
#endif

#endif

