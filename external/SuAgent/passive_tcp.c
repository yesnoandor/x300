/*	passive_tcp.c - passive_tcp	*/

int passive_sock(int port,const char *transport,int maxlen);

/*	--------------------------------------------------------------------
	*passive_tcp -	creat a passive socket for use in a TCP server	
	*-------------------------------------------------------------------
*/
int
passive_tcp(int port, int maxlen)
/*
*	Argument:
*		port		- the port which connection use;
*	Return:
*		s			- a new descripter used to read/write;
*/
{
	return passive_sock(port,"tcp",maxlen);
}
