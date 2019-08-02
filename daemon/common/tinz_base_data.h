#ifndef __TINZ_BASE_DATA__
#define __TINZ_BASE_DATA__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tinz_base_def.h"

typedef struct _PollutantRtdDataRow{

    uint8_t flag;  // 1 有效
    float   rtd;
}stPollutantRtdDataRow,*pstPollutantRtdDataRow;

typedef struct _PollutantRtdData{
    time_t                  seconds;
    stPollutantRtdDataRow   Row[POLLUTANT_CNT];
}stPollutantRtdData,*pstPollutantRtdData;

typedef struct _PollutantDataRow{

	uint16_t	CNT;
	float		Max;
	float		Min;
	double		Sum;
}stPollutantDataRow,*pstPollutantDataRow;

typedef struct _PollutantData{
    time_t              seconds;
    stPollutantDataRow  Row[POLLUTANT_CNT];
}stPollutantData,*pstPollutantData;

typedef struct _PollutantHistoryDataRow{
    
    float data;
    char  DataTime[DATATIME_LEN];
}stPollutantHistoryDataRow,*pstPollutantHistoryDataRow;

typedef struct _PollutantHistoryData{
    uint8_t flag;       //0:无动作 1：请求数据 2：数据已写入
    uint8_t DataType;  //0：无数据 1：实时数据 2：1分钟 3：5分钟 4：小时 5：天数据 
    uint8_t PollutantType; //0:流量 1:COD 2:PH
    uint8_t cnt;       //查询到的数据条数
    char  StartDataTime[DATATIME_LEN];
    char  StopDataTime[DATATIME_LEN];
    stPollutantHistoryDataRow Row[HISTORYCNT];
}stPollutantHistoryData,*pstPollutantHistoryData;

typedef struct _HistoryData{
    stPollutantHistoryData Pollutant;
}stHistoryData,*pstHistoryData;

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

typedef struct _PollutantSData{
    
    stPollutantRtdData  RtdData;
    stPollutantData     PerMinData;
    stPollutantData     MinsData;
    stPollutantData     HourData;
    stPollutantData     DayData;
}stPollutantsData,*pstPollutantsData;

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
    stPollutantsData PollutantsData;
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
