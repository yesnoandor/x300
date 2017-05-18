#include	<stdbool.h>

#include	"scorpion_protocol.h"
#include	"scorpion_protocol_volumn.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>


#if 0
void fleety_report_volumn(unsigned char volumn)
{
	protocol_volumn_t protocol_volumn;
	
	DbgFuncEntry();
	
	protocol_build_header(&protocol_volumn.header,MCU2SYSTEM_EX_EVAL,sizeof(protocol_volumn.volumn));
	protocol_volumn.volumn = volumn;
	protocol_send_data((unsigned char *)&protocol_volumn,sizeof(protocol_volumn_t));

	DbgFuncExit();
}
#endif

void scorpion_report_volumn_in(unsigned char volumn)
{
	protocol_volumn_t protocol_volumn;
	
	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	scorpion_protocol_build_header(&protocol_volumn.header,SCORPION2APK_VOLUMN_IN,sizeof(protocol_volumn.volumn));
	protocol_volumn.volumn = volumn;
	scorpion_protocol_send_data((unsigned char *)&protocol_volumn,sizeof(protocol_volumn_t));

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}

