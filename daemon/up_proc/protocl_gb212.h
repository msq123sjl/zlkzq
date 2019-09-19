#ifndef __PROTOCL_GB212__
#define __PROTOCL_GB212__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "nginx_helper.h"

#define RESULT_SUCCESS 1
#define RESULT_FAILED  2
#define RESULT_CONDITION_ERR  3
#define RESULT_NODATA  100

#define REQUEST_READY    1
#define REQUEST_REFUSED  2
#define REQUEST_CODE_ERR 3


#define CN_SendTime         1011
#define CN_GetTime          1011
#define CN_SetTime          1012
#define CN_RecvRtdData      2081
#define CN_ValveControl     3051     //阀门控制
#define CN_GetValveStatus   3071
#define CN_SendPowerDown    3081
#define CN_SendPowerUp      3082
#define CN_SetTolerances    3093
#define CN_SendValvePowerDown    3083
#define CN_SendValvePowerUp      3084

#define CN_RequestRespond   9011
#define CN_ExeRespond    	9012



typedef struct {
	int		   	cn;
	
	ngx_str_t	qn;
	ngx_str_t	pw;
	
	int8_t     flag;    	//bit[0] 1:应答，0:不应答 
							//bit[1]：是否有数据序号；1-数据包中包含包序号和总包号两部分,0-数据包中不包含包序号和总包号两部分

    uint8_t     per;
    u_char      SystemTime[SYSTEMTIME_LEN];
    stPollutantRtdData  RtdData;
    stPollutantPara PollutantPara;  
} ngx_ulog_url_t;

int messageProc(char *str, int iRecvLen, pstSerialPara com,TcpClientDev *tcp);
void PowerState();
void ValvePowerState();
int Insert_Message_Data(int cn,int flag,void* pData);


#endif
