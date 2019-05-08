#ifndef __TINZ_BASE_DATA__
#define __TINZ_BASE_DATA__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tinz_base_def.h"

typedef struct _PollutantRtdDataRow{
    
    float rtd;
    float day;
    float mon;
    float qut;
    float year;
    float cou;
}stPollutantRtdDataRow,*pstPollutantRtdDataRow;

typedef struct _PollutantRtdData{
    
    stPollutantRtdDataRow   Row[POLLUTANT_CNT];
    char                    DataTime[DATATIME_LEN];
}stPollutantRtdData,*pstPollutantRtdData;

typedef struct _PollutantHistoryData{
    
    float rtd;
    float cou;
    u_char      DataTime[DATATIME_LEN];
}stPollutantHistoryData,*pstPollutantHistoryData;

typedef struct _MeterRtdData
{
	char		flag;
	float		Rtd;
	double		total;
}stMeterRtdData,*pstMeterRtdData;

typedef struct _MeterData
{
	uint16_t	CNT;
	float		RtdMax;
	float		RtdMin;
	double		RtdSum;
	double		totalStart;
	double		totalEnd;
}stMeterData,*pstMeterData;

typedef struct _Meter
{
	pstMeterPara 	para;
	stMeterRtdData	RtdData;
	stMeterData 	MinData;
	stMeterData 	HourData;
	stMeterData 	DayData;
}stMeter,*pstMeter;

typedef struct _PollutantData{
    
    stPollutantRtdData  RtdData;
    
}stPollutantData,*pstPollutantData;

typedef struct _State
{
    volatile uint8_t InPower;     //0 市电 1 无市电
    volatile uint8_t ValveState;  //0 阀门正常 1阀门异常
}stState,*pstState;
typedef struct _IOState
{
    volatile uint8_t InPower;     //0 市电 1 无市电
}stIOState,*pstIOState;

typedef struct _Data
{
    stState state;
    stIOState IOState;
    float current_Ia[AD_CNT]; //模拟通道采样电流值 单位mA
}stData,*pstData;

typedef struct _MessageData
{
    uint8_t IsUse;   // 0 空闲 1 正在写入 2 待发送
    uint8_t flag;   // 0 不应答 1 应答
    uint8_t waittime; //单位s
    char qn[QN_LEN];
    char content[MAX_TCPDATA_LEN];
    uint8_t SendTimes[SITE_CNT];
    uint8_t IsRespond[SITE_CNT];  //0 未应答 1 应答
}stMessageData,*pstMessageData;

typedef struct _Message
{
    stMessageData Data[MESSAGECNT];
}stMessage,*pstMessage;

#endif
