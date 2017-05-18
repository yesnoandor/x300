#include	<stdbool.h>

#include	"scorpion_protocol.h"
#include	"scorpion_protocol_eval.h"

// ----------------- Declaration Debug Zone ----------------
#define		LOG_NDEBUG 				1
#define		LOG_TAG 				"ScorpionService"
#include	<cutils/log.h>

#if 0
void fleety_report_eval(unsigned char eval)
{
	protocol_eval_t protocol_eval;
	
	DbgFuncEntry();
	
	protocol_build_header(&protocol_eval.header,MCU2SYSTEM_EX_EVAL,sizeof(protocol_eval.eval));
	protocol_eval.eval = eval;
	protocol_send_data((unsigned char *)&protocol_eval,sizeof(protocol_eval_t));

	DbgFuncExit();
}
#endif

void scorpion_report_eval_in(unsigned char eval)
{
	protocol_eval_in_t protocol_eval_in;

	ALOGI("%s::+++++++++++++++\r\n",__FUNCTION__);
	
	scorpion_protocol_build_header(&protocol_eval_in.header,SCORPION2APK_EVAL_IN,sizeof(protocol_eval_in.eval));
	protocol_eval_in.eval = eval;
	scorpion_protocol_send_data((unsigned char *)&protocol_eval_in,sizeof(protocol_eval_in_t));

	ALOGI("%s::---------------\r\n",__FUNCTION__);
}
