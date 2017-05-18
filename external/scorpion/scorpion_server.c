#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<fcntl.h>

#include	"scorpion_stm32_heart_beat.h"
#include	"scorpion_stm32_version.h"
#include	"scorpion_protocol_heart_beat.h"

#include	"scorpion_socket.h"

#include	"scorpion_event.h"
#include	"scorpion_uart.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionSTM32"
#include	<cutils/log.h>

#define		MAX_MAIN_LOOP_TIMEOUT		30

static		pthread_cond_t		main_cond;
static		pthread_mutex_t		main_lock;

int main(int argc, char** argv)
{	
	int ret;
	int exit = 0;
	struct timeval now;
	struct timespec outtime;

	pthread_mutex_init(&main_lock, NULL);
	pthread_cond_init(&main_cond, NULL);
	
	scorpion_socket_start_server();

	stm32_send_version_req();

	while(!exit)
	{
		pthread_mutex_lock(&main_lock);

		gettimeofday(&now, NULL);
		outtime.tv_sec = now.tv_sec + MAX_MAIN_LOOP_TIMEOUT;
		outtime.tv_nsec = now.tv_usec * 1000;

		ret = pthread_cond_timedwait(&main_cond, &main_lock, &outtime);
        pthread_mutex_unlock(&main_lock);

        ALOGI("scorpion main......\r\n");

        if(ret == 0)	// ---> condition ok
		{
			ALOGE("Exit Scorpion");
			exit = 1;
			continue;
		}
		else	// ---> timeout
		{
			stm32_send_heart_beat();
			scorpion_heart_beat_treat();
		}		
	}


	return 0;
}
