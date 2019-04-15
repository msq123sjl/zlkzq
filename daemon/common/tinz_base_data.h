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

#endif
