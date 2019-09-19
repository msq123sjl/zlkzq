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

typedef struct _PollutantHistoryDataRow{
    
    float data;
    char  DataTime[DATATIME_LEN];
}stPollutantHistoryDataRow,*pstPollutantHistoryDataRow;

typedef struct _PollutantHistoryData{
    uint8_t flag;       //0:无动作 1：请求数据 2：数据已写入
    uint8_t DataType;  //0：无数据 1：实时数据 2：天数据 3：月数据 4：季度数据 5 年数据 
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

typedef struct _PollutantData{
    
    stPollutantRtdData  RtdData;
    
}stPollutantData,*pstPollutantData;

typedef struct _State
{
    volatile uint8_t InPower;     //0 市电 1 无市电
    volatile uint8_t LTE;     //0 链接 1 链接断开 2 SIM卡无 3 模块无响应
	volatile uint8_t VPN;     //0 链接 1 链接断开
    volatile uint8_t ValveState;  //0 阀门正常 1阀门异常           泵阀联动：阀门状态 0 关闭   1 开 2异常 
    volatile uint8_t PumpState;   //泵阀联动：泵 0 关闭   1 开 
}stState,*pstState;
typedef struct _IOState
{
    volatile uint8_t InPower;     //0 市电 1 无市电
    volatile uint8_t In_drain_open;      //0 开 1 关        输入检测排水阀开
    volatile uint8_t In_drain_close;     //0 开 1 关       输入检测排水阀关
    volatile uint8_t In_reflux_open;     //0 开 1 关       输入检测回流泵开
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
