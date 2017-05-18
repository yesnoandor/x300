/*	chatserver_tcp.c	-	main,send_to_others	*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/types.h>
#include <dirent.h>
//#include <errno.h>/home/john/CodeBlocksWorkspace/SuAgentClient/SuAgent-Client/SuAgent-Client.cbp

//#include <unistd.h>
#include <time.h>

#include <pwd.h>

/*#include <private/android_filesystem_config.h>*/

#define START_ARG 2                // 从第几个参数开始是shell中的参数
#define MAX_ARGS_SIZE 1024        // 所有参数组成的字符串最大长度
#define SINGLE_LINE_SIZE 800    // 标准输出中单行的最大长度

#define		MAXLEN			50				//	maximun connection queue length
#define		BUFSIZE			4096
#define		DEFAULT_PORT	60001

int 		errexit(const	char *format, ...);
int			passive_tcp(int port, int maxlen);
int			send_to_others(int fd, int listen_sockfd, int maxfd, fd_set *rfds);



char * runAndGetOutPut(int, char**);
int su1main(int, char **);
int	execute_and_response_client(int, int,fd_set *);


/*--------------------------------------------------
 *main	-	start the server
 *--------------------------------------------------
*/
int main(int argc, char **argv)
{
	int		port = DEFAULT_PORT;	//	port number;
	struct	sockaddr_in	fsin;		//	the from address of a client;
	int		listen_sockfd;			//	server's listen socket
	fd_set	read_set;					//	active	descripter set
	fd_set	ready_set;					//	from	-	address length
	int 	alen;
	int		fd,maxfd;

	switch (argc) {
		case 1:
			break;
		case 2:
			port = atoi(argv[1]);	//atoi :string to int
			break;
		default:
			errexit("usage:	echoserver_tcp [port]\n");
	}

	listen_sockfd	=	passive_tcp(port,MAXLEN);
	printf("listen_sockfd:%d listening......\n",listen_sockfd);

//	maxfd	= 	getdtablesize();
	maxfd	=	listen_sockfd;

/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&ready_set); /*clear ready_set*/
	FD_SET(listen_sockfd,&ready_set);/*add listen_sockfd to ready_set*/

	while (1) {
			memcpy(&read_set, &ready_set, sizeof(read_set));//void *memcpy(void *dest, const void *src, size_t n);

			if (select(maxfd+1, &read_set, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0)
				errexit("select: %s\n",strerror(errno));
			if (FD_ISSET(listen_sockfd, &read_set)) {
                		/*  there is a client want to connect with
				 *  the server,the server receive it,and
			     *  allocate a new descripter ssock to this
			     *  connection,and add it to the select pool
			    */

					int		client_sockfd;
					alen	=	sizeof(fsin);
					client_sockfd	=	accept(listen_sockfd, (struct sockaddr *)&fsin, (socklen_t *)&alen);
					if (client_sockfd <0)
						errexit("accept: %s\n",strerror(errno));
					if (client_sockfd > maxfd)
						maxfd = client_sockfd;
					printf("maxfd is: %d\n",maxfd);
					printf("listen_sockfd accept a connection,client_sockfd: %d\n",client_sockfd);
					FD_SET(client_sockfd,&ready_set);
			}
			for (fd=0; fd<=maxfd; ++fd)
				if (fd != listen_sockfd && FD_ISSET(fd, &read_set)){

                    execute_and_response_client(fd, listen_sockfd, &ready_set);
                    printf("closed fd =%d\n",fd);
                    close(fd);
                    FD_CLR(fd,&ready_set);
//					if (send_to_others(fd, listen_sockfd, maxfd, &ready_set) == 0) {
//						(void)close(fd);
//						FD_CLR(fd,&ready_set);
//					}

				}
	}

}


int	execute_and_response_client(int fd, int listen_sockfd,fd_set *ready_set)
/*-------------------------------------------------------------------------------------
 *	Arguments:
 *		fd				-	the client fd;
 *		listen_sockfd	-	the server's listening fd;
 *		read_set		-	the set of fd that connected with the server;
*/
{
	char 		buf[BUFSIZE];
	int			cc,n=0;
	int			tmp_fd;
    memset(buf,0,sizeof(buf));
	cc	=	read(fd,buf,sizeof(buf));
	if (cc < 0)
		errexit("echo read: %s\n",strerror(errno));
	if (cc) {
		printf("server received %d bytes from client_sockfd: %d\n",cc,fd);

		//TODO split commend
		printf("server received command from client_sockfd: %d : %s \n",fd,buf);
        char ** paras=(char**)malloc(1024);
        char *str=NULL, *token=NULL;
        int j;
        for (j = 0,str=buf; ; j++, str = NULL) {
               token = strtok(str, " ");
               if (token == NULL)
                   break;
                paras[j]=token;
               printf("%d: %s\n", j, token);
           }
        char * response=runAndGetOutPut(j,paras); /*run process */
        free(paras);
        paras=NULL;

        if(FD_ISSET(listen_sockfd,ready_set)){
            if ( ( write(fd, response, strlen(response))) < 0)
					errexit("write: %s\n",strerror(errno));
        }
        printf("Alread wrote to sockfd.\n");
        free(response);
        response=NULL;

#if 0
		//for (tmp_fd = 0; tmp_fd <= maxfd; ++tmp_fd) {
			//if ((tmp_fd != listen_sockfd) && (tmp_fd != fd) && FD_ISSET(tmp_fd,ready_set)) {
			if ((tmp_fd == listen_sockfd) &&FD_ISSET(tmp_fd,ready_set)) {//also send to client itself
				char tmp_buf[20];
				char *fd_str;
				sprintf(tmp_buf,"client ");
				fd_str = gcvt((double)fd,5,tmp_buf+7);
				if ( ( write(tmp_fd, tmp_buf, strlen(tmp_buf))) < 0)
					errexit("write: %s\n",strerror(errno));
				sprintf(tmp_buf," says: ");
				if ( ( write(tmp_fd, tmp_buf, strlen(tmp_buf))) < 0)
					errexit("write: %s\n",strerror(errno));
				if ( (n = write(tmp_fd, buf, cc)) < 0)
					errexit("write: %s\n",strerror(errno));
				else
					printf("send back %d bytes to client_sockfd:%d successed!\n",n,tmp_fd);
			}
		//}
#endif
	}
	return cc;
}

/*------------------------------------------------------------------
 *	send_to_others	-	send message that recevived from fd to other connected peoples;
 *-------------------------------------------------------------------
*/
int	send_to_others(int fd, int listen_sockfd,int maxfd, fd_set *ready_set)
/*-------------------------------------------------------------------------------------
 *	Arguments:
 *		fd				-	the client which has message to others;
 *		listen_sockfd	-	the server's listening fd;
 *		maxfd			-	the maxfd of all fds in the ready_set;
 *		read_set		-	the set of fd that connected with the server;
*/
{
	char 		buf[BUFSIZE];
	int			cc,n=0;
	int			tmp_fd;

	cc	=	read(fd,buf,sizeof(buf));
	if (cc < 0)
		errexit("echo read: %s\n",strerror(errno));
	if (cc) {
		printf("server received %d bytes from client_sockfd: %d\n",cc,fd);
		for (tmp_fd = 0; tmp_fd <= maxfd; ++tmp_fd) {
			//if ((tmp_fd != listen_sockfd) && (tmp_fd != fd) && FD_ISSET(tmp_fd,ready_set)) {
			if ((tmp_fd != listen_sockfd) &&FD_ISSET(tmp_fd,ready_set)) {//also send to client itself
				char tmp_buf[20];
				char *fd_str;
				sprintf(tmp_buf,"client ");
				fd_str = gcvt((double)fd,5,tmp_buf+7);
				if ( ( write(tmp_fd, tmp_buf, strlen(tmp_buf))) < 0)
					errexit("write: %s\n",strerror(errno));
				sprintf(tmp_buf," says: ");
				if ( ( write(tmp_fd, tmp_buf, strlen(tmp_buf))) < 0)
					errexit("write: %s\n",strerror(errno));
				if ( (n = write(tmp_fd, buf, cc)) < 0)
					errexit("write: %s\n",strerror(errno));
				else
					printf("send back %d bytes to client_sockfd:%d successed!\n",n,tmp_fd);
			}
		}
	}
	return cc;
}



/*
 * SU can be given a specific command to exec. UID _must_ be
 * specified for this (ie argc => 3).
 *
 * Usage:
 * su 1000
 * su 1000 ls -l
 */
int su1main(int argc, char *argv[])
{
int i;
fprintf(stderr, "hello argc=%d \n",argc);
for(i=0;i<argc;i++)
fprintf(stderr, "1111 %s \n",argv[i]);
    struct passwd *pw;
    int uid, gid, myuid;

    if(argc < 2) {
        uid = gid = 0;
    } else {
        pw = getpwnam(argv[1]);

        if(pw == 0) {
            uid = gid = atoi(argv[1]);
        } else {
            uid = pw->pw_uid;
            gid = pw->pw_gid;
        }
    }
#if 0
    /* Until we have something better, only root and the shell can use su. */
    myuid = getuid();
    if (myuid != AID_ROOT && myuid != AID_SHELL) {
        fprintf(stderr,"su: uid %d not allowed to su\n", myuid);
        return 1;
    }

    if(setgid(gid) || setuid(uid)) {
        fprintf(stderr,"su: permission denied\n");
        return 1;
    }
#endif
    /* User specified command for exec. */
    if (argc == 3 ) {//command like "su 1000 ls"
        if (execlp(argv[2], argv[2], NULL) < 0) {//execute exe file.
            fprintf(stderr, "su: exec failed for %s Error:%s\n", argv[2],
                    strerror(errno));
            return -errno;
        }
    } else if (argc > 3) {//command like "su 1000 ls -l"
        /* Copy the rest of the args from main. */
        char *exec_args[argc - 1];
        memset(exec_args, 0, sizeof(exec_args));//clear exec_args
        memcpy(exec_args, &argv[2], sizeof(exec_args));//void *memcpy(void *dest, const void *src, size_t n);
        if (execvp(argv[2], exec_args) < 0) {
            fprintf(stderr, "su: exec failed for %s Error:%s\n", argv[2],
                    strerror(errno));
            return -errno;
        }
    }

    /* Default exec shell. */
    execlp("/system/bin/sh", "sh", NULL);

    fprintf(stderr, "su: exec failed\n");
    return 1;
}




char* combineArg(int argc, char **argv)
{
    char args[MAX_ARGS_SIZE];
    int i=START_ARG;
    memset(args, 0, MAX_ARGS_SIZE);
    for(; i<argc; ++i)
    {
        printf("argv[%d]=%s\n", i, argv[i]);
        strcat(args, argv[i]);
        strcat(args, " ");
    }
    printf("combineArg...%s\n", args);
    return args;
}

/* return char* ,must be free */
char* combineString(char *a, char *b)
{
    char *ptr=NULL;
    int lena=strlen(a), lenb=strlen(b);
    int i, l=0;

    ptr = (char *)malloc((lena+lenb+1) * sizeof(char));
    for(i=0;i<lena;i++)
        ptr[l++]=a[i];
    for(i=0;i<lenb;i++)
        ptr[l++]=b[i];
    ptr[l]='\0';
    return(ptr);
}


char * runAndGetOutPut(int argc, char**argv)
{
    //getOutput("");
    int curSize=0,bufSize=MAX_ARGS_SIZE;
    char *out=(char *)malloc(bufSize),*tmp;
    memset(out,0,bufSize);
    FILE *fp = NULL;

    if ((fp = popen(combineArg(argc, argv), "r")) == NULL)
    {
        return "popen failed!";
    }

    char buf[SINGLE_LINE_SIZE];
    while (fgets(buf, SINGLE_LINE_SIZE-1, fp) != NULL)
    {
        curSize+=strlen(buf);
        if(curSize>=bufSize){
            tmp=out;
            bufSize+=MAX_ARGS_SIZE;
            out=(char *)malloc(bufSize);
            memset(out,0,bufSize);
            strcpy(out,tmp);
            free(tmp);
            tmp=NULL;
        }
        strncat(out,buf,strlen(buf));
        //printf("%s", buf);
        //out = combineString(out, buf);
    }

    if (pclose(fp) == -1)
    {
        return "pclose failed!";
    }
    printf("...%s\n", out);

    //printf("\n%s\n", getOutput(combineArg(argc, argv)));

    return out;
}


