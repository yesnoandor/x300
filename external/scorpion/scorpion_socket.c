#include	<stdio.h>
#include 	<stdlib.h>
#include	<errno.h>
#include 	<string.h>

#include	<sys/types.h>
#include	<netinet/in.h>
#include	<sys/socket.h>
#include	<sys/wait.h>
#include	<sys/un.h>
#include	<cutils/sockets.h>

//#include	<utils/Log.h>
//#include	<android/log.h>

#include	"scorpion_protocol.h"

#include	"scorpion_event.h"
#include	"scorpion_uart.h"
#include	"scorpion_cb.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>

#define		SCORPION_SOCKET_NAME	"scorpion"

#define		MAX_TEMP_BUFF_SIZE		256
#define		MAX_SOCKET_BUFF_SIZE	(4*1024)
#define		MAX_CONNECT_NUMBER		6


typedef struct {
	int socket_server_fd;
	int socket_client_fd;

	CircleBufferMngr *ppmngr_read;			// socket 接收的buffer
	CircleBufferMngr *ppmngr_write;			// socket 发送的buffer

	pthread_t		socket_thread_id;
	pthread_cond_t	socket_cond;
	pthread_mutex_t socket_lock;
}ScorpionSocketMngr,*p_scorpion_socket_private_t;

static ScorpionSocketMngr	scorpion_socket_mngr;


static void * scorpion_socket_recv_treat(void *dummy)
{
	p_scorpion_socket_private_t scorpion_socket;
	unsigned char buff[MAX_TEMP_BUFF_SIZE];
	int size;
	int exit = 0;
	
	fd_set readset;
  	int max_fd=0;
  	int ret;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	scorpion_socket = (p_scorpion_socket_private_t)(dummy);

	#if 0
 	FD_ZERO(&readset);
    	
	FD_SET(scorpion_uart->uart_fd, &readset);
	max_fd = scorpion_uart->uart_fd > max_fd ? scorpion_uart->uart_fd : max_fd;
		
	FD_SET(scorpion_uart->pipe_fd[0],&readset);
	max_fd = scorpion_uart->pipe_fd[0] > max_fd ? scorpion_uart->pipe_fd[0] : max_fd;
	#endif

	// 循环等待Socket 客户端发来消息
    while(1)
    {
		ALOGI("Waiting for socket receive");

		memset(buff,0,sizeof(buff));
		size = recv(scorpion_socket->socket_client_fd,buff,sizeof(buff),0);
		if(size == -1)
		{
			ALOGE("Failed to recv socket(err = %s)!",strerror(errno));
			continue;
        }

		#if 0
        ALOGI("size = %d\r\n",size);
        for(i =0;i<size;i++)
        {
        	ALOGI("buff[%d] = 0x%x\r\n",i,buff[i]);
        }
        #endif

        scorpion_protocol_ayalyze(buff,size);

    }
    
    ALOGI("%s::---------------\r\n",__FUNCTION__);

    return 0;
}


int scorpion_socket_start_server(void)
{
    //int fdListen = -1, new_fd = -1;
	
	// 用于unix domain socket   本地socket
    struct sockaddr_un client_addr;
    socklen_t client_addr_size;

    int i;
    int size ;
    int ret;
    	
    char buff[256];

    ScorpionSocketMngr * socket;

    ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

    socket = &scorpion_socket_mngr;
    
    // 获取init.rc中配置的名为 "scorpion" 的socket
    socket->socket_server_fd = android_get_control_socket(SCORPION_SOCKET_NAME);
    if (socket->socket_server_fd < 0) {
    	ALOGE("Failed to get socket %s(err = %s)!",SCORPION_SOCKET_NAME,strerror(errno));
    	
		exit(-1);
	}
    
    // 开始监听
    ret = listen(socket->socket_server_fd, MAX_CONNECT_NUMBER);   
    if (ret < 0) {
    	ALOGE("Failed to listen socket(err = %s)!",strerror(errno));
        exit(-1);
    }
    
    // 等待Socket客户端发启连接请求
	client_addr_size = sizeof(client_addr);
    socket->socket_client_fd = accept(socket->socket_server_fd, (struct sockaddr *) &client_addr, &client_addr_size); 
    if (socket->socket_client_fd < 0 ) {
    	ALOGE("Failed to accept socket (err = %s)!",strerror(errno));

        exit(-1);
    }

	ret = cb_init(&socket->ppmngr_read,MAX_SOCKET_BUFF_SIZE);
	if(!ret)
	{
		ALOGE("socket cb_init read buffer failed!");
		exit(-1);
	}
		
	ret = cb_init(&socket->ppmngr_write,MAX_SOCKET_BUFF_SIZE);
	if(!ret)
	{
		cb_deinit(&socket->ppmngr_read);
		ALOGE("socket cb_init write buffer failed!");
		return -1;
	}
	
	ret = pthread_create(&(socket->socket_thread_id),NULL,scorpion_socket_recv_treat,(void *)socket); 
    if(ret != 0)
    {
        ALOGE("Create socket recv thread failed!\r\n");
        return -1;
    }

	scorpion_event_start_server();

	scorpion_uart_start_server();

    return 0;
}

void scorpion_socket_stop_server(void)
{	
	//char c=0;
	//void * status;
	p_scorpion_socket_private_t scorpion_socket;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	scorpion_socket = &scorpion_socket_mngr;
	
	//write(scorpion_uart_private_data.pipe_fd[1],&c,sizeof(char));
	//pthread_join(scorpion_uart_private_data.recv_thread_id,&status);
	//ALOGI("status = %d\r\n",(int)status);

	//scorpion_uart_private_data.send_thread_exit = 1;
	//pthread_cond_signal(&scorpion_uart_private_data.thread_cond);
	//pthread_join(scorpion_uart_private_data.send_thread_id,&status);
	//ALOGI("status = %d\r\n",(int)status);
	
    
	//close(scorpion_uart_private_data.pipe_fd[0]);
	//close(scorpion_uart_private_data.pipe_fd[1]);
	//close(scorpion_uart_private_data.recv_thread_id);
	//close(scorpion_uart_private_data.send_thread_id);

	close(scorpion_socket->socket_thread_id);

    close(scorpion_socket->socket_client_fd);
    close(scorpion_socket->socket_server_fd);

	pthread_cond_destroy(&(scorpion_socket->socket_cond));
	pthread_mutex_destroy(&(scorpion_socket->socket_lock));

	
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}

int scorpion_socket_send(unsigned char *buff,int len)
{
	int size;
	ScorpionSocketMngr * socket;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	socket = &scorpion_socket_mngr;
	
	size = send(socket->socket_client_fd,buff,len,0);
	
	if(size == -1)
	{
		ALOGE("Failed to send socket(err = %s)!",strerror(errno));
			
		close(socket->socket_client_fd);
		exit(0);
	}

	ALOGI("%s::---------------\r\n",__FUNCTION__);
	
	return size;
}
