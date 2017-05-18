#include	<stdbool.h>

#include	"scorpion_protocol.h"

#include	"scorpion_protocol_shell.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>


#define		MAX_LINE				512
#define		LINE_SIZE				80

unsigned short shell_run(char * in_buf,char * out_buf,unsigned short out_size)
{
	FILE *fp = NULL;
	char line_buf[LINE_SIZE];
	unsigned short size;
	

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	ALOGI("cmd = %s\r\n",in_buf);


	if ((fp = popen(in_buf, "r")) == NULL)
	{
		memset(out_buf,0x00,out_size);
		strcpy(out_buf,"popen failed!");
		ALOGE("%s\r\n",out_buf);

		return strlen(out_buf);
	}

	memset(out_buf,0x00,out_size);
	size = 0;
	while (fgets(line_buf, LINE_SIZE, fp) != NULL)
    {

    	if(size + (unsigned short)strlen(line_buf) >= out_size - 1)
    	{	
			strncat(out_buf,line_buf,out_size - 1 - size);
			out_buf[out_size - 1] = '\0';
			size = out_size - 1;
    	}
		else
		{
    		strncat(out_buf,line_buf,strlen(line_buf));
    		size += strlen(line_buf);
    	}
    }
    
	if (pclose(fp) == -1)
    {
    	memset(out_buf,0x00,out_size);
    	strcpy(out_buf,"pclose failed!");
		ALOGE("%s\r\n",out_buf);
		
        return strlen(out_buf);
    }

    size += 1;

    ALOGI("size = 0x%x\r\n",size);
    
	ALOGI("%s::---------------\r\n",__FUNCTION__);
	
	return size;
}

			
unsigned char scorpion_shell_respond(unsigned char *buf,unsigned short len)
{
	char * new_buf; 
	unsigned short new_len;

	unsigned char * shell_rep_buf;
	unsigned short shell_rep_len;

	scorpion_protocol_header_t * header;

	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	ALOGI("shell cmd = %s\r\n",buf);

	new_buf = (char *)malloc(MAX_LINE * LINE_SIZE);
	new_len = shell_run((char *)buf,new_buf,(MAX_LINE * LINE_SIZE));

	shell_rep_len = new_len + sizeof(scorpion_protocol_header_t) + 3;
	shell_rep_buf = (unsigned char *)malloc(shell_rep_len);

	header = (scorpion_protocol_header_t *)(&shell_rep_buf[1]);
	
	scorpion_protocol_build_header(header,SCORPION2APK_SHELL_REP,new_len);
	memcpy(&shell_rep_buf[1+sizeof(scorpion_protocol_header_t)],new_buf,new_len);
	scorpion_protocol_send_data(shell_rep_buf,shell_rep_len);

	free(new_buf);
	free(shell_rep_buf);

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	return SCORPION_PROTOCOL_RESULT_OK;
}

