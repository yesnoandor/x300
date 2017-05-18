/*	passive_sock.c	- passive_sock	*/

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef INADDR_NONE
#define INADDR_NONE		0xffffffff
#endif					/*	INADDR_NONE	,no inaddr	*/

#ifndef	DEFAULT_HOST
#define	DEFAULT_HOST	"127.0.0.1"
#endif					/*	default host	*/

int 	errexit(const char *format,...);



/*--------------------------------------------------------------
  *	passive_sock	-	allocate & bind a socket using TCP or UDP
  *--------------------------------------------------------------
*/
int
passive_sock(int port, const char *transport,int maxlen)
{
/*
*arguments:
*			port- 		the port that the connection use;
*			transport-	transport protocol("tcp" or "udp");
*			maxlen	-	the max number of clients the server can deal with;
*return:
*			s:			a new descripter used to read/write;
*/
//	struct protoent 	*ppe;		//pointer to protocol info entry;
	struct sockaddr_in	sin;		//an internet endpoint address
	int		s,type;					//	socket descripter and socket type

	memset(&sin,0,sizeof(sin));		//init 0

	sin.sin_family	=	AF_INET;
	sin.sin_port 	=	htons(port);
	sin.sin_addr.s_addr	=	inet_addr(DEFAULT_HOST);

	/*	Map transport protocol name to protocal number	*/
//	if ( (ppe = getprotobyname(transport)) == 0){
//		printf("errno=%d\n", errno);
//		errexit("can't get \"%s\" protocol entry\n",transport);
//	}
	/*	Use protocol to choose a socket type	*/
	if ( strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	/*	allocate a socket	*/
	//s = socket(PF_INET, type, ppe->p_proto);
	s = socket(PF_INET, type, 0);
	if ( s<0 )
		errexit("can't create socket: %s\n",strerror(errno));
	/*	Bind the socket	*/
	if (bind(s, (struct	sockaddr *)&sin, sizeof(sin)) <0)
		errexit("can't bind to %d port: %s\n",port,strerror(errno));
	if (type ==	SOCK_STREAM && listen(s,maxlen)<0)
		errexit("can't listen on %d port: %s\n",port,strerror(errno));
	
	return s;

}
