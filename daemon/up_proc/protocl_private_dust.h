#ifndef __PROTOCL_PRIVATE_DUST__
#define __PROTOCL_PRIVATE_DUST__

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



#define CN_SendPowerStatus  3081

#define CN_GetRtdData       2011
#define CN_GetPerMinData    2021
#define CN_GetMinsData      2051
#define CN_GetHourData      2061
#define CN_GetDayData       2031


#define CN_SetPollutantThresholdPara    3088

#define CN_RequestRespond   9011
#define CN_ExeRespond    	9012
#define CN_DataRespond      9014


typedef struct {
	int		   	cn;
	
	ngx_str_t	qn;
	ngx_str_t	pw;
	
	int8_t     flag;    	//bit[0] 1:应答，0:不应答 
							//bit[1]：是否有数据序号；1-数据包中包含包序号和总包号两部分,0-数据包中不包含包序号和总包号两部分

    //uint8_t     per;
    //u_char      SystemTime[SYSTEMTIME_LEN];
    stPollutantPara PollutantPara;  
} ngx_ulog_url_t;

int messageProc(char *str, int iRecvLen, pstSerialPara com,TcpClientDev *tcp);
//void PowerState();
//void ValvePowerState();
int Insert_Message_Data(int cn,int flag,void* pData);


#endif
