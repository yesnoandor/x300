#include	<stdbool.h>
#include	<termios.h>			// 终端控制定义
#include	<fcntl.h>
#include	<netinet/in.h>
#include	<linux/input.h>

#include	<pthread.h>

#include	"scorpion_cb.h"
#include	"scorpion_stm32.h"
#include	"scorpion_uart.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionSTM32"
#include	<cutils/log.h>


#define		STM32_UART_DEV			"/dev/ttyACM4"
#define		MAX_TEMP_BUFF_SIZE		256
#define		MAX_UART_BUFF_SIZE		2048	

#define		MAX_SOCKET_SEND_TIMEOUT	30

typedef struct{
	int				uart_fd;

	CircleBufferMngr *ppmngr_read;			// UART 接收的buffer
	CircleBufferMngr *ppmngr_write;			// UART 接收的buffer
	
	pthread_t		recv_thread_id;
	pthread_t		send_thread_id;
	pthread_mutex_t thread_mutex;
	pthread_cond_t 	thread_cond;
	int				send_thread_exit;

	int				pipe_fd[2];
} scorpion_uart_private_t,*p_scorpion_uart_private_t;


typedef struct{
	int bps;
	int databits;
	int stopbits;
	int parity;

	int timeout;
}scorpion_uart_params_t;

static scorpion_uart_private_t scorpion_uart_private_data;

static scorpion_uart_params_t termios_params =
{
	B115200,8,1,'N',0,		// STM32 bps
};

static int scorpion_uart_create(char *device)
{
	int fd;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	fd = open(device, O_RDWR);                         
	if (fd < 0)                                          
	{
		ALOGE("open device error(err = %s)\r\n",strerror(errno));
		return -1;
	}

	ALOGI("%s::---------------\r\n",__FUNCTION__);
	
	return fd;
}

static bool scorpion_uart_set_params(int fd,scorpion_uart_params_t * params)
{
	struct termios options;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	if(tcgetattr(fd,&options) != 0)
	{
		ALOGE("get termios failed!\r\n");
		return false;
	}

	// default reset
	options.c_cflag = 0;
	options.c_lflag = 0;
	options.c_iflag = 0;
	options.c_oflag = 0;
	

	// 设置数据位数
	options.c_cflag &= ~CSIZE;
	switch (params->databits)
	{
		case 7:
			options.c_cflag |= CS7;
			break;

		case 8:
			options.c_cflag |= CS8;
			break;

		default:      
			ALOGE("don't supported data bits\r\n");
			return false;
	}
	
	// 设置停止位
	switch (params->stopbits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;    
			break;
			
		case 2:      
			options.c_cflag |= CSTOPB;    
			break;
			
		default:
			ALOGE("don't supported stop bits\r\n");
			return false;
	}
	
	// 设置奇偶校验位
	switch (params->parity)
	{
		// 无校验
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;		/* Clear parity enable */
			options.c_iflag &= ~INPCK;		/* Enable parity checking */   
			break;
		
		// 奇校验
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/    
			options.c_iflag |= INPCK;             /* Disnable parity checking */   
			break;

		// 偶校验
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;		/* Enable parity */      
			options.c_cflag &= ~PARODD;		/* 转换为偶效验*/       
			options.c_iflag |= INPCK;       /* Disnable parity checking */  
			break; 

		default: 
			ALOGE("don't supported parity\r\n");      
			return false;
	}
	
	// 设置波特率
	cfsetospeed(&options,params->bps);	
	cfsetispeed(&options,params->bps);

	// Set the timeout options
    options.c_cc[VMIN]  = 0;        		// read blocked except 1bytes data ready
    options.c_cc[VTIME] = params->timeout;	// interval between two chars (unit : 100ms)
    options.c_cc[VINTR] = 0;
    options.c_cc[VQUIT] = 0;
    options.c_cc[VSTART] = 0;
    options.c_cc[VSTOP] = 0;
    options.c_cc[VSUSP] = 0;

	// 设置新属性
	tcflush(fd, TCIFLUSH);				// 溢出数据可以接收，但不读  
	tcsetattr(fd, TCSANOW, &options);	// 设置终端参数生效，TCANOW = 修改立即发生

	ALOGI("timeout = %d\r\n",params->timeout);
	ALOGI("%s::---------------\r\n",__FUNCTION__);
	
    return true;  
}


void scorpion_uart_treat_gateway(void)
{
	int len =0,size=0;
	unsigned char * buff;
	p_scorpion_uart_private_t scorpion_uart_mgr;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	scorpion_uart_mgr = &scorpion_uart_private_data;
	
	len = cb_datalen(scorpion_uart_mgr->ppmngr_read);

	ALOGI("len = %d\r\n",len);
	
	if(len)
	{
		buff = (unsigned char *)malloc(len);
		
		size = cb_read_no_offset(scorpion_uart_mgr->ppmngr_read,buff,len);
		if(size != len)
		{
			ALOGE("socket send size error!(size = %d,len = %d)\r\n",size,len);
		}
		
		size = stm32_protocol_ayalyze(buff,len);

		cb_read_move_offset(scorpion_uart_mgr->ppmngr_read,size);

		free(buff);
	}

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}



static void * scorpion_uart_recv_treat(void *dummy)
{
	p_scorpion_uart_private_t scorpion_uart;
	unsigned char temp_buff[MAX_TEMP_BUFF_SIZE];
	int size;
	int exit = 0;
	
	fd_set readset;
  	int max_fd=0;
  	int ret;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	scorpion_uart = (p_scorpion_uart_private_t)(dummy);

 	FD_ZERO(&readset);
    	
	FD_SET(scorpion_uart->uart_fd, &readset);
	max_fd = scorpion_uart->uart_fd > max_fd ? scorpion_uart->uart_fd : max_fd;
		
	FD_SET(scorpion_uart->pipe_fd[0],&readset);
	max_fd = scorpion_uart->pipe_fd[0] > max_fd ? scorpion_uart->pipe_fd[0] : max_fd;

    while(!exit)
    {   
		ALOGI("Waiting for uart select");
		
    	ret = select(max_fd+1, &readset, NULL, NULL,NULL);

    	ALOGI("ret = %d",ret);
    	
		switch(ret)
    	{
			case -1:
				exit = 1;
				ALOGE("uart select failed!(err = %s)\r\n",strerror(errno));
				break;
			
			case 0:
				ALOGW("uart timeout!\r\n");
				break;

			default:			
				if(FD_ISSET(scorpion_uart->uart_fd, &readset))
				{
					size = read(scorpion_uart->uart_fd, temp_buff, MAX_TEMP_BUFF_SIZE);
      
					if(size <= 0)
					{
						exit = 1;
						ALOGE("uart read failed!(size = %d)\r\n",size);
						break;
					}
					else
					{
						ALOGI("size = %d",size);
						{
							int i;

							for(i=0;i<size;i++)
							{
								ALOGI("buf[%d] = 0x%x",i,temp_buff[i]);
							}
						}

						if (cb_write(scorpion_uart->ppmngr_read,temp_buff,size) != size)
						{
							ALOGW("socket cb read buff discard!\r\n");
						}
						
						scorpion_uart_treat_gateway();
					}

				}

				if(FD_ISSET(scorpion_uart->pipe_fd[0],&readset))
				{
					exit = 1;
					ALOGI("Exit uart thread!\r\n");
				}
        }
	}

    ALOGI("%s::---------------\r\n",__FUNCTION__);

    return 0;
}

static void scorpion_uart_send_treat(scorpion_uart_private_t * scorpion_uart)
{
	int len;
	int size;

	unsigned char temp_buff[MAX_TEMP_BUFF_SIZE];

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	if(cb_empty(scorpion_uart->ppmngr_write))
	{
		ALOGI("send uart buff empty!\r\n");
		return;
	}

	len = cb_read(scorpion_uart->ppmngr_write,temp_buff,MAX_TEMP_BUFF_SIZE);

	#if 1
	{
		int i;

		ALOGI("len = 0x%x\r\n",len);
			
		for(i=0;i<len;i++)
		{
			ALOGI("socket_send_buff[%02d] = 0x%02x\r\n",i,temp_buff[i]);
		}
	}
	#endif

	size = write(scorpion_uart_private_data.uart_fd,temp_buff,len);
	if(size < 0)
	{
		ALOGE("scorpion uart write failed!\r\n");
	}

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}



static void * scorpion_uart_send_thread_proc(void *dummy)
{
	int ret;
	int exit = 0;

	struct timeval now;
	struct timespec outtime;

	p_scorpion_uart_private_t scorpion_uart;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	scorpion_uart = (p_scorpion_uart_private_t)(dummy);
	scorpion_uart->send_thread_exit = 0;
	
	while(!scorpion_uart->send_thread_exit)
	{
		pthread_mutex_lock(&scorpion_uart->thread_mutex);

		scorpion_uart_send_treat(scorpion_uart);
		
		gettimeofday(&now, NULL);
		outtime.tv_sec = now.tv_sec + MAX_SOCKET_SEND_TIMEOUT;
		outtime.tv_nsec = now.tv_usec * 1000;
 
		ret = pthread_cond_timedwait(&(scorpion_uart->thread_cond), &scorpion_uart->thread_mutex, &outtime);
        
		pthread_mutex_unlock(&scorpion_uart->thread_mutex);

		if(ret == 0)	// ---> condition ok
		{
			ALOGI("uart cond ok!\r\n");
		}
		else	// ---> timeout
		{
			ALOGI("uart timeout!\r\n");
		}
	}

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return ((void *)0);
}

int scorpion_uart_start_server(void)
{
	int ret;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	ret = pipe(scorpion_uart_private_data.pipe_fd);
	if(ret != 0)
	{
		ALOGE("create pipe failed!\r\n");
		return ret;
	}

    ret = pthread_cond_init(&(scorpion_uart_private_data.thread_cond),NULL);
    if(ret != 0)
    {
        ALOGE("init uart cond failed!\r\n");
        return ret;
    }
    
	ret = pthread_mutex_init(&(scorpion_uart_private_data.thread_mutex), NULL);
    if(ret != 0)
    {
        ALOGE("init uart mutex failed!\r\n");
        return -1;
    }

    scorpion_uart_private_data.uart_fd = scorpion_uart_create(STM32_UART_DEV);
	scorpion_uart_set_params(scorpion_uart_private_data.uart_fd,&termios_params);

	ret = cb_init(&scorpion_uart_private_data.ppmngr_read,MAX_UART_BUFF_SIZE);
	if(!ret)
	{
		ALOGE("uart cb_init read buffer failed!\r\n");
	}
		
	ret = cb_init(&scorpion_uart_private_data.ppmngr_write,MAX_UART_BUFF_SIZE);
	if(!ret)
	{
		cb_deinit(&scorpion_uart_private_data.ppmngr_read);
		ALOGE("uart cb_init write buffer failed!");
	}


	ret = pthread_create(&(scorpion_uart_private_data.recv_thread_id),NULL,scorpion_uart_recv_treat,(void *)&scorpion_uart_private_data); 
    if(ret != 0)
    {
        ALOGE("Create uart recv thread failed!\r\n");
        return -1;
    }

    ret = pthread_create(&(scorpion_uart_private_data.send_thread_id),NULL,scorpion_uart_send_thread_proc,(void *)&scorpion_uart_private_data); 
    if(ret != 0)
    {
        ALOGE("Create uart send thread failed!\r\n");
        return -1;
    }

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}


void scorpion_uart_stop_server(void)
{	
	char c=0;
	void * status;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	write(scorpion_uart_private_data.pipe_fd[1],&c,sizeof(char));
	pthread_join(scorpion_uart_private_data.recv_thread_id,&status);
	ALOGI("status = %d\r\n",(int)status);

	scorpion_uart_private_data.send_thread_exit = 1;
	pthread_cond_signal(&scorpion_uart_private_data.thread_cond);
	pthread_join(scorpion_uart_private_data.send_thread_id,&status);
	ALOGI("status = %d\r\n",(int)status);

	close(scorpion_uart_private_data.pipe_fd[0]);
	close(scorpion_uart_private_data.pipe_fd[1]);
	close(scorpion_uart_private_data.recv_thread_id);
	close(scorpion_uart_private_data.send_thread_id);

	pthread_cond_destroy(&(scorpion_uart_private_data.thread_cond));
	pthread_mutex_destroy(&(scorpion_uart_private_data.thread_mutex));
	
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}

int scorpion_uart_send(unsigned char *buff,int len)
{
	int size = 0;
	p_scorpion_uart_private_t scorpion_uart;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
	scorpion_uart = &scorpion_uart_private_data;

	ALOGI("len = 0x%x\r\n",len);

	if(scorpion_uart->uart_fd < 0)
	{
		ALOGE("uart send size error!(size = %d,len = %d)\r\n",size,len);
		
		return 0;
	}
	
	if(len <= cb_freelen(scorpion_uart->ppmngr_write))
	{
		size = cb_write(scorpion_uart->ppmngr_write,buff,len);
		if(size != len)
		{
			ALOGE("uart send size error!(size = %d,len = %d)\r\n",size,len);
		}

		pthread_cond_signal(&scorpion_uart->thread_cond); 
	}

	ALOGI("%s::---------------\r\n",__FUNCTION__);
	
	return size;

}




