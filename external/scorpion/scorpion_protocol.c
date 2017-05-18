#include	<stdbool.h>

#include	"scorpion_misc.h"
#include	"scorpion_socket.h"
#include	"scorpion_protocol.h"

#include	"scorpion_protocol_heart_beat.h"
#include	"scorpion_protocol_delay_time.h"
#include	"scorpion_protocol_shell.h"
#include	"scorpion_protocol_reboot.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>


// 转义规则 ： 0x7D ==> 0x7D,0x01;	0x7E ==> 0x7D,0x02
#define		SCORPION_ESCAPE_CHAR		0x7D
#define		SCORPION_MAGIC_CHAR			0x7E

#define		MAX_PROTOCOL_BUFF_SIZE		512


/*
* 转义消息
* dest ： 存放转义以后的消息缓存
* src : 存放转义以前的消息缓存
* len : 转义以前的消息长度
* return ：转义以后的消息长度 （长度会增加）
*/
static int scorpion_protocol_escape(unsigned char *dest, unsigned char *src, int len)
{
	int i,j;
	
	if(!dest || !src || !len)
	{
		ALOGE("params error!\r\n");
		return -1;
	}
	
	for(i=0,j=0; i<len; i++)
	{
		if(src[i] == SCORPION_ESCAPE_CHAR)
		{
			dest[j++] = SCORPION_ESCAPE_CHAR;
			dest[j++] = 0x01;
		}
		else if(src[i] == SCORPION_MAGIC_CHAR)
		{
			dest[j++] = SCORPION_ESCAPE_CHAR;
			dest[j++] = 0x02;
		}
		else
		{
			dest[j++]=src[i];
		}
	}
	
	return j;
}


/*
* 解析转义消息
* 	0x7E 
* dest ： 存放解析以后的消息缓存
* src : 存放解析以前的消息缓存
* len : 存放解析以前的消息长度
* return ：存放解析以后的消息长度 （长度会减少）
*/
static int scorpion_protocol_unescape(unsigned char *dest, unsigned char *src, int len)
{
	int i,j;

	if( !dest || !src || !len )
	{
		ALOGE("params error!\r\n");
		return -1;
	}
	
	for(i=0,j=0; i<len; i++)
	{
		if(src[i]== SCORPION_ESCAPE_CHAR)
		{
			i++;
			if(i == len)
			{
				ALOGI("not unescape number error!\r\n");
				return -1;
			}
			else if(src[i] == 0x01)
			{
				dest[j++] = SCORPION_ESCAPE_CHAR;
			}
			else if(src[i]==0x02)
			{
				dest[j++] = SCORPION_MAGIC_CHAR;
			}
			else
			{
				ALOGI("unescape number error!\r\n");
				return -1;
			}
		}
		else
		{
			dest[j++]=src[i];
		}
	}
	
	return j;
}


static void scorpion_protocol_common_ack_treat(unsigned char *buf,int len)
{
	scorpion_protocol_ack_t * ack;

	ack = (scorpion_protocol_ack_t *)(&buf[1 + sizeof(scorpion_protocol_header_t)]);
	
	scorpion_protocol_debug_ack(ack);

	#if 0
	switch(ack->id)
	{
		case MESSAGE_OPERATION_REPORT:
			operation_db_update_record(ack->seq);
			break;
			
		case MESSAGE_LOCATION_REPORT:
			report_db_update_record_of_state(ack->seq);
			break;

		case MESSAGE_LOCATION_SUPPLEMENT_REPORT:
			report_db_update_record_of_state(ack->seq);
			break;
	}

	if(ack->id != MESSAGE_HEART_BEAT)
		gb905_record_cancel(MAIN_SOCKET,ack);
	#endif
}


/*
* 查找
* dest ： 存放解析以后的消息缓存
* src : 存放解析以前的消息缓存
* len : 存放解析以前的消息长度
* return ：存放解析以后的消息长度 （长度会减少）
*/
static bool scorpion_find_magic_char(unsigned char *buf, int len,unsigned int * offset)
{
	int index=0;
	bool ret = false;

	*offset = 0;
	
	while(index < len)
	{
		if(buf[index] == SCORPION_MAGIC_CHAR)
		{
			ret = true;
			*offset = index;
			break;
		}
		
		index++;
	}

	return ret;
}

/*
 * 解析出一条完整的消息
 * msg ： 存放整条消息的参数
 * header : 解析出协议的头信息
 * return ：解析出协议，需要偏移的长度
 */
static int scorpion_get_a_full_msg(unsigned char *buf,int len,buff_mgr_t *msg)
{
	bool ret;
	unsigned int head_offset = 0,tail_offset = 0;
	int offset;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);


  	ret = scorpion_find_magic_char(buf,len,&head_offset);
	if(ret && (len - head_offset >= sizeof(scorpion_protocol_header_t) + 3))		// 3 : start magic + xor  + end maigc
	{
		ret = scorpion_find_magic_char(buf + head_offset + 1,len - head_offset - 1,&tail_offset);
		tail_offset += head_offset + 1;
		if(ret)
		{
			ALOGI("head_offset = %d\r\n",head_offset);
			ALOGI("tail_offset = %d\r\n",tail_offset);
			 
			msg->buf = (buf + head_offset);
			msg->len = tail_offset - head_offset + 1;
			offset = tail_offset + 1;
		}
		else
		{
			offset = head_offset;
		}
	 }
	 else
	 {
		 offset = head_offset;
	 }
 
	 ALOGI("offset = %d\r\n",offset);
	 
	 ALOGI("%s::---------------\r\n",__FUNCTION__);

	 
	 return offset;  
 }
 
 

static bool scorpion_parse_header(buff_mgr_t * msg)
{
	bool ret = true;
	scorpion_protocol_header_t * header;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	header = (scorpion_protocol_header_t *)(msg->buf + 1);

	scorpion_protocol_debug_header(header);

	//header->msg_id = EndianReverse16(header->msg_id);
	//header->msg_len = EndianReverse16(header->msg_len);
	//header->msg_serial_number = EndianReverse16(header->msg_serial_number);

	ALOGI("msg_id = 0x%x\r\n",header->msg_id);
	ALOGI("msg_len = 0x%x\r\n",header->msg_len);
	ALOGI("msg_serial_number = 0x%x\r\n",header->msg_serial_number);
	
	if(header->msg_len != msg->len - sizeof(scorpion_protocol_header_t) - 3)
	{
		ret = false;

		ALOGE("msg len is invaild!\r\n");
		return ret;
	}
	
	ALOGI("%s::---------------\r\n",__FUNCTION__);
	
	return ret;
}

static bool scorpion_parse_protocol(buff_mgr_t * msg)
{
	scorpion_protocol_header_t * header;
	unsigned short msg_no;
	unsigned char result;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	header = (scorpion_protocol_header_t *)(msg->buf + 1);

	msg_no = header->msg_id;

	ALOGI("msg_no = 0x%x\r\n",msg_no);

	switch (msg_no)
	{
		case APK2SCORPION_GENERAL_ACK:
			scorpion_protocol_common_ack_treat(msg->buf,msg->len);
			break;
			
		case APK2SCORPION_HEART_BEAT:
			scorpion_heart_beat(msg->buf + 1 + sizeof(scorpion_protocol_header_t),header->msg_len);
			scorpion_protocol_common_ack_treat(msg->buf,msg->len);
			break;

		case APK2SCORPION_DELAY_TIME:
			scorpion_delay_time_treat(msg->buf + 1 + sizeof(scorpion_protocol_header_t),header->msg_len);
			scorpion_protocol_common_ack_treat(msg->buf,msg->len);
			break;

		case APK2SCORPION_SHELL_REQ:
			scorpion_shell_respond(msg->buf + 1 + sizeof(scorpion_protocol_header_t),header->msg_len);
			break;

		case APK2SCORPION_EX_REBOOT:
			scorpion_reboot();
			
		default:
			break;
	}

	ALOGI("%s::---------------\r\n",__FUNCTION__);
	
	return true;
}

/*
* 分析一条消息命令
* @msg： 存放完整消息命令的buf
* @header : 解析出协议的头信息
* @return：解析出协议，需要偏移的长度
*/
static int scorpion_parse_a_full_msg(buff_mgr_t * msg)
{
	int ret;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	ret = scorpion_parse_header(msg);
	if( ret < 0)
	{
		return ret;
	}

	ret = scorpion_parse_protocol(msg);

	
	ALOGI("%s::---------------\r\n",__FUNCTION__);

	return ret;
}


int scorpion_protocol_ayalyze(unsigned char * buf,int len)
{
	unsigned char * msg_buf;
	int msg_len;
	
	int offset;
	double_buff_mgr_t protocol_msg;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
 
	msg_buf = buf;
	msg_len = len;
	 
	do{
		memset(&protocol_msg,0x00,sizeof(protocol_msg));

		offset = scorpion_get_a_full_msg(msg_buf,msg_len,&protocol_msg.raw);

		#if 0
		{
			int i;
				 
			debug_printf("offset = %d\r\n",offset);
			debug_printf("raw_msg_lens = %d\r\n",protocol_msg.raw.len);
			
			for(i=0;i<protocol_msg.raw.len;i++)
			{
				debug_printf("msg_raw_buf[%d] = 0x%x\r\n",i,protocol_msg.raw.buf[i]);
			}
		}
		#endif
 
		if(offset && protocol_msg.raw.len)
		{	 
			protocol_msg.now.buf = (unsigned char *)malloc(protocol_msg.raw.len);
			if(!protocol_msg.now.buf)
			{
				ALOGE("protocol memory malloc failed!\r\n");
				return -1;
			}

 			protocol_msg.now.len = scorpion_protocol_unescape(protocol_msg.now.buf,protocol_msg.raw.buf,protocol_msg.raw.len);

 			/*
 			if(protocol_msg.now.len < 0)
 			{
 			}
 			*/
			#if 1
			{
				int i;
				 
				ALOGI("now.len = %d\r\n",protocol_msg.now.len);
				for(i=0;i<protocol_msg.now.len ;i++)
				{
					ALOGI("now.buf[%d] = 0x%x\r\n",i,protocol_msg.now.buf[i]);
				}
			}
			#endif
			 
			if(protocol_msg.now.buf[protocol_msg.now.len - 2] != xor8_computer(protocol_msg.now.buf + 1,protocol_msg.now.len - 3))
			{
				 ALOGE("xor verify failed!\r\n");
				 //free(protocol_msg.now.buf);
				 //return -2;
			}
			else
			{
				scorpion_parse_a_full_msg(&protocol_msg.now);
			}

			free(protocol_msg.now.buf);
		}
 
		msg_buf += offset;
		msg_len -= offset;

		ALOGI("offset = %d\r\n",offset);
		ALOGI("raw.len = %d\r\n",protocol_msg.raw.len);
		ALOGI("msg_len = %d\r\n",msg_len);
		
	}while(offset && protocol_msg.raw.len && msg_len > 0);

	ALOGI("%s::---------------\r\n",__FUNCTION__);
	 
	return (len - msg_len);
}
 
 
 
 
void scorpion_protocol_build_header(scorpion_protocol_header_t * header, unsigned short msg_id, unsigned short msg_len)
{
	 static  unsigned int msg_seq_number = 0;
 
	 ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
 	 
	 memset(header,0x00,sizeof(scorpion_protocol_header_t));
	 header->msg_id = msg_id;
	 header->msg_len = msg_len;

	 header->msg_serial_number = msg_seq_number++;
 
	 ALOGI("%s::---------------\r\n",__FUNCTION__);
}
 
 
void scorpion_protocol_build_ack(scorpion_protocol_ack_t * ack,scorpion_protocol_header_t * header,unsigned char result)
{
	ack->seq = header->msg_serial_number;
	ack->id = header->msg_id;
	ack->result = result;
}

void scorpion_protocol_send_ack(scorpion_protocol_header_t * header,unsigned char result)
{
	scorpion_protocol_general_ack_t ack;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);

	scorpion_protocol_build_header(&ack.header,SCORPION2APK_GENERAL_ACK,sizeof(scorpion_protocol_ack_t));
	scorpion_protocol_build_ack(&ack.ack,header,result);
	scorpion_protocol_send_data((unsigned char *)&ack,sizeof(scorpion_protocol_general_ack_t));

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}

int scorpion_protocol_send_data(unsigned char * buf, int len)
{
	int i;
	unsigned char * new_buf;
	int new_len;
	unsigned char val;
	
	buf[0] = buf[len-1] = SCORPION_MAGIC_CHAR;

	val = xor8_computer((unsigned char *)&buf[1],len - 3);
	buf[len-2] = val;

	new_buf = (unsigned char *)malloc(len * 2);
	if(!new_buf)
	{
		ALOGE("protocol memory malloc failed!\r\n");
		return -1;
	}

	new_buf[0] = buf[0];
	new_len = scorpion_protocol_escape(&new_buf[1],&buf[1],len-2);
	new_len++;
	new_buf[new_len] = buf[len-1];
	new_len++;

	ALOGI("new_len = 0x%x\r\n",new_len);
		
	for(i=0;i<new_len;i++)
	{
		ALOGI("new_buf[%d] = 0x%x\r\n",i,new_buf[i]);
	}

	scorpion_socket_send(new_buf,new_len);
		
	free(new_buf);
	
	return new_len;
}

 
//-----------------------------------------------------------
void scorpion_protocol_debug_header(scorpion_protocol_header_t * header)
{
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
 
	ALOGI("msg_id = 0x%x\r\n",header->msg_id);
	ALOGI("msg_len = 0x%x\r\n",header->msg_len);
	ALOGI("msg_serial_number = 0x%x\r\n",header->msg_serial_number);
	 
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}

void scorpion_protocol_debug_ack(scorpion_protocol_ack_t * ack)
{
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
 
	ALOGI("seq = 0x%x\r\n",ack->seq);
	ALOGI("id = 0x%x\r\n",ack->id);
	ALOGI("result = 0x%x\r\n",ack->result);
	 
	ALOGI("%s::---------------\r\n",__FUNCTION__);
}


