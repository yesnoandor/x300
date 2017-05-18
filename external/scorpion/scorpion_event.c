#include	<stdbool.h>
#include	<fcntl.h>
#include	<linux/input.h>

#include	<pthread.h> 

#include	"scorpion_protocol.h"
#include	"scorpion_protocol_event.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>

#define		KEYPAD_EVENT_DEV		"/dev/input/event2"
#define		KEYPAD_BASE_CODE		59

typedef struct{
	int				event_fd;
	
	pthread_t		thread_id;
	pthread_mutex_t thread_mutex;
	pthread_cond_t 	thread_cond;

	int				pipe_fd[2];
} scorpion_event_private_t,*p_scorpion_event_private_t;


static scorpion_event_private_t scorpion_event_private_data;


static void event_protocol_ayalyze(unsigned short code,unsigned int value)
{
	unsigned int index;
	unsigned int level;

	index = code - KEYPAD_BASE_CODE;
	level = value;
	scorpion_report_event_in(index,level);
}


static void * event_treat(void *dummy)
{
	p_scorpion_event_private_t scorpion_event;
	struct input_event data;
	int len;
	int exit = 0;
	
	fd_set readset;
  	int max_fd=0;
  	int ret;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
		
	scorpion_event = (p_scorpion_event_private_t)(dummy);

	if((scorpion_event->event_fd = open(KEYPAD_EVENT_DEV, O_RDONLY)) < 0)
    {
        ALOGE("can't open event dev %s\r\n", KEYPAD_EVENT_DEV);
        return NULL;
    }

	FD_ZERO(&readset);
    	
	FD_SET(scorpion_event->event_fd, &readset);
	max_fd = scorpion_event->event_fd > max_fd ? scorpion_event->event_fd : max_fd;
		
	FD_SET(scorpion_event->pipe_fd[0],&readset);
	max_fd = scorpion_event->pipe_fd[0] > max_fd ? scorpion_event->pipe_fd[0] : max_fd;
	
    while(!exit)
    {
    	ALOGI("Waiting for event select");
    	
    	ret = select(max_fd+1, &readset, NULL, NULL,NULL);

    	ALOGI("ret = 0x%x\r\n",ret);
		
		switch(ret)
    	{
			case -1:
				exit = 1;
				ALOGE("event select failed!(err = %s)\r\n",strerror(errno));
				break;
			
			case 0:
				ALOGW("event timeout!\r\n");
				break;

			default:			
				if(FD_ISSET(scorpion_event->event_fd, &readset))
				{
					len = read(scorpion_event->event_fd, &data, sizeof(data));

      				ALOGI("len = 0x%x\r\n",len);
        
					if(len == sizeof(data))
					{
						ALOGI("type = 0x%x,code = 0x%x,value = 0x%x\r\n",data.type,data.code,data.value);
        
    	    			if (data.type == EV_KEY)
        				{
        					event_protocol_ayalyze(data.code,data.value);
        				}
					}
					else
					{
						ALOGE("read failed! len = 0x%x\r\n",len);
					}
				}

				if(FD_ISSET(scorpion_event->pipe_fd[0],&readset))
				{
					exit = 1;
					ALOGI("Exit event thread!\r\n");
				}
        }
	}

    ALOGI("%s::---------------\r\n",__FUNCTION__);

    return 0;
}


int scorpion_event_start_server(void)
{
	int ret;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	ret = pipe(scorpion_event_private_data.pipe_fd);
	if(ret != 0)
	{
		ALOGE("create pipe failed!\r\n");
		return ret;
	}

    ret = pthread_cond_init(&(scorpion_event_private_data.thread_cond),NULL);
    if(ret != 0)
    {
        ALOGE("create pthread meter cond failed!\r\n");
        return ret;
    }
    
	ret = pthread_mutex_init(&(scorpion_event_private_data.thread_mutex), NULL);
    if(ret != 0)
    {
        ALOGE("Init mutex failed!\r\n");
        return -1;
    }
    
	ret = pthread_create(&(scorpion_event_private_data.thread_id),NULL,event_treat,(void *)&scorpion_event_private_data); 
    if(ret != 0)
    {
        ALOGE("Create thread failed!\r\n");
        return -1;
    }

	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return 0;
}


void stop_event_server(void)
{	
	char c=0;
	void * status;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	write(scorpion_event_private_data.pipe_fd[1],&c,sizeof(char));
	pthread_join(scorpion_event_private_data.thread_id,&status);

	ALOGI("status = %d\r\n",(int)status);

	close(scorpion_event_private_data.pipe_fd[0]);
	close(scorpion_event_private_data.pipe_fd[1]);
	close(scorpion_event_private_data.thread_id);

	pthread_cond_destroy(&(scorpion_event_private_data.thread_cond));
	pthread_mutex_destroy(&(scorpion_event_private_data.thread_mutex));
	
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}



