#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
  
#define		KEY_EVENT_DEV		"/dev/input/event1"



static void select_keys(int keys_fd)
{
	fd_set readset;
  	int max_fd=0;
  	int ret;

	int len;
  	struct input_event input;
		
	while(1)
    {
    	FD_ZERO(&readset);
    	
		FD_SET(keys_fd, &readset);
		max_fd = keys_fd;
			
    	ret = select(max_fd+1, &readset, NULL, NULL,NULL);
		switch(ret)
    	{
			case -1:
				printf("event select failed!(err = %s)\r\n",strerror(errno));
				break;
			
			case 0:
				printf("event timeout!\r\n");
				break;

			default:			
				if(FD_ISSET(keys_fd, &readset))
				{
					len = read(keys_fd, &input, sizeof(input));

      				printf("len = 0x%x\r\n",len);
        
					if(len == sizeof(input))
					{
						printf("type = 0x%x,code = 0x%x,value = 0x%x\r\n",input.type,input.code,input.value);
        
    	    			if (input.type == EV_KEY)
        				{
        
        				}
					}
					else
					{
						printf("read failed! len = 0x%x\r\n",len);
					}
				}

        }
	}

}



static void read_keys(int keys_fd)
{
	int len;
	struct input_event input;
		
	while(1)
	{
		len = read(keys_fd, &input, sizeof(input));
		printf("len = %d\r\n",len);
		
		if(len == sizeof(input))
		{
			printf("type = %d\r\n",input.type);
	
			if(input.type == EV_KEY)
			{	
				printf("value = %d\r\n",input.value);
	
				if(input.value == 0 || input.value == 1)
				{
					printf("key %d %s\n", input.code, (input.value) ? "Pressed" : "Released");
					if(input.code == KEY_ESC)
						break;
				}
			}
		}
    }
    
}

int main() 
{	
	int keys_fd;

	keys_fd = open(KEY_EVENT_DEV, O_RDONLY);
	if(keys_fd <= 0)
	{  
		printf("open the key device error!(%s)\n",strerror(errno));
		return -1;
	}

	//read_keys(keys_fd);

	select_keys(keys_fd);
	
    close(keys_fd);

    return 0;  
}
