#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "sqlite3.h"


#include "tinz_common_helper.h"
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_message.h"
#include "tinz_common_db_helper.h"

#include "dataproc.h"

//stPollutantRtdData  RtdData;
pstPara 		pgPara;
//pstPollutantData pgPollutantData;
pstData pgData;
struct _msg *pmsg_upproc[SITE_CNT];
char            code[POLLUTANT_CNT][4]={"BO1","011","001"};


int 			gPrintLevel = 5;
tinz_db_ctx_t 	scy_data;

void _proj_init(void)__attribute__((constructor));
void _proj_uninit(void)__attribute__((destructor));

void _proj_init(void){
	DEBUG_PRINT_INFO(gPrintLevel, "start!!!\n");
}
void _proj_uninit(void)
{
	DEBUG_PRINT_INFO(gPrintLevel, "stop!!!\n");
}

/*static void rtd_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].RtdData.flag 	= 0;
		Meter[iLoop].RtdData.Rtd 	= 0;
		Meter[iLoop].RtdData.total 	= 0;
	}
}

static void min_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].MinData.CNT	= 0;
		Meter[iLoop].MinData.RtdMax	= 0;
		Meter[iLoop].MinData.RtdMin = -1;
		Meter[iLoop].MinData.RtdSum = 0;
		Meter[iLoop].MinData.totalStart = Meter[iLoop].MinData.totalEnd;
	}
}

static void hour_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].HourData.CNT		= 0;
		Meter[iLoop].HourData.RtdMax	= 0;
		Meter[iLoop].HourData.RtdMin 	= -1;
		Meter[iLoop].HourData.RtdSum 	= 0;
		Meter[iLoop].HourData.totalStart = Meter[iLoop].HourData.totalEnd;
	}
}

static void day_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].DayData.CNT	= 0;
		Meter[iLoop].DayData.RtdMax	= 0;
		Meter[iLoop].DayData.RtdMin = -1;
		Meter[iLoop].DayData.RtdSum = 0;
		Meter[iLoop].DayData.totalStart = Meter[iLoop].DayData.totalEnd;
	}
}

static void meter_data_init(){
	int iLoop;
	for(iLoop = 0; iLoop < METER_CNT; iLoop++){
		Meter[iLoop].para = (pstMeterPara)&pgPara->MeterPara[iLoop];
	}
	rtd_data_init();
	min_data_init();
	hour_data_init();
	day_data_init();
}*/

static void MessageInit(){
    int iLoop;
    for(iLoop=0;iLoop<SITE_CNT;iLoop++){
        if(pgPara->SitePara[iLoop].ServerOpen){
        	DEBUG_PRINT_INFO(gPrintLevel, "pmsg_upproc_%d start\n",iLoop);
        	pmsg_upproc[iLoop] = (struct _msg*)malloc(sizeof(struct _msg));
        	memset(pmsg_upproc[iLoop],0,sizeof(struct _msg));
        	if(TINZ_ERROR == prepareMsg(MSG_PATH_UPPROC_TO_SQLITE,MSG_NAME_UPPROC_TO_SQLITE, iLoop+1, pmsg_upproc[iLoop])){
        		exit(0);
        	}
        }
    }
}

static void pollutant_data_proc_rtd(pstPollutantRtdData  pRtdData){
	char 	TableName[TABLE_NAME_LEN];
	char 	sql[SQL_LEN];
	int		iLoop;
    //sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
    	if(pRtdData->Row[iLoop].rtd >= 0){

    		snprintf(TableName,sizeof(TableName)-1,"Rtd_%s_%-4.4s%-2.2s",code[iLoop],\
    											&pRtdData->DataTime[0], &pRtdData->DataTime[4]);
    		if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                RtdTableCreate(&scy_data,TableName);
            }
    		
    		snprintf(sql,SQL_LEN,"insert or replace into %s values (\'%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s\',%4.2f,%8.2f);",TableName,\
    									&pRtdData->DataTime[0], &pRtdData->DataTime[4],&pRtdData->DataTime[6],\
    									&pRtdData->DataTime[8], &pRtdData->DataTime[10], &pRtdData->DataTime[12],\
    									pRtdData->Row[iLoop].rtd,
    									pRtdData->Row[iLoop].cou);
            //pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            //MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
    		tinz_db_exec(&scy_data,sql);
    		DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
    	} 
    }
}

static void pollutant_data_proc_day(pstPollutantRtdData  pRtdData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];
    int     iLoop;
    //sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(pRtdData->Row[iLoop].day >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Day_%s",code[iLoop]);
            if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }
            snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (\'%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s\',%8.2f,%8.2f);",TableName,\
                                        &pRtdData->DataTime[0], &pRtdData->DataTime[4],&pRtdData->DataTime[6],\
                                        &pRtdData->DataTime[8], &pRtdData->DataTime[10], &pRtdData->DataTime[12],\
                                        pRtdData->Row[iLoop].day,
                                        pRtdData->Row[iLoop].cou);
            //pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            //MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
            tinz_db_exec(&scy_data,sql);
            DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
        } 
    }
}

static void pollutant_data_proc_month(pstPollutantRtdData  pRtdData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];
    int     iLoop;
    //sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(pRtdData->Row[iLoop].mon >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Mon_%s",code[iLoop]);
            if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }
            snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (\'%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s\',%8.2f,%8.2f);",TableName,\
                                        &pRtdData->DataTime[0], &pRtdData->DataTime[4],&pRtdData->DataTime[6],\
                                        &pRtdData->DataTime[8], &pRtdData->DataTime[10], &pRtdData->DataTime[12],\
                                        pRtdData->Row[iLoop].mon,
                                        pRtdData->Row[iLoop].cou);
            //pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            //MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
            tinz_db_exec(&scy_data,sql);
            DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
        } 
    }
}

static void pollutant_data_proc_qut(pstPollutantRtdData  pRtdData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];
    int     iLoop;
    //sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(pRtdData->Row[iLoop].qut >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Qut_%s",code[iLoop]);
            if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }
            snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (\'%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s\',%8.2f,%8.2f);",TableName,\
                                        &pRtdData->DataTime[0], &pRtdData->DataTime[4],&pRtdData->DataTime[6],\
                                        &pRtdData->DataTime[8], &pRtdData->DataTime[10], &pRtdData->DataTime[12],\
                                        pRtdData->Row[iLoop].qut,
                                        pRtdData->Row[iLoop].cou);
            //pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            //MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
            tinz_db_exec(&scy_data,sql);
            DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
        } 
    }
}

static void pollutant_data_proc_year(pstPollutantRtdData  pRtdData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];
    int     iLoop;
    //sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(pRtdData->Row[iLoop].year >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Year_%s",code[iLoop]);
            if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }
            snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (\'%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s\',%8.2f,%8.2f);",TableName,\
                                        &pRtdData->DataTime[0], &pRtdData->DataTime[4],&pRtdData->DataTime[6],\
                                        &pRtdData->DataTime[8], &pRtdData->DataTime[10], &pRtdData->DataTime[12],\
                                        pRtdData->Row[iLoop].year,
                                        pRtdData->Row[iLoop].cou);
            //pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            //MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
            tinz_db_exec(&scy_data,sql);
            DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
        } 
    }
}
static void pollutant_data_proc(pstPollutantRtdData  pRtdData){
    pollutant_data_proc_rtd(pRtdData);
    pollutant_data_proc_day(pRtdData);
    pollutant_data_proc_month(pRtdData);
    pollutant_data_proc_qut(pRtdData);
    pollutant_data_proc_year(pRtdData);
}
static void MessageRecv(){
    int iLoop;
    pstPollutantRtdData  pRtdData;
    for(iLoop=0;iLoop<SITE_CNT;iLoop++){
        if(pgPara->SitePara[iLoop].ServerOpen){
            MsgRcv(pmsg_upproc[iLoop], MSG_SQLITE_TYTE); 
            if(pmsg_upproc[iLoop]->msgbuf.mtype > 0){
                DEBUG_PRINT_INFO(gPrintLevel, "pmsg_upproc_%d recvtype[%ld][%-10.10s]\n",iLoop,pmsg_upproc[iLoop]->msgbuf.mtype,pmsg_upproc[iLoop]->msgbuf.data);
                switch(pmsg_upproc[iLoop]->msgbuf.mtype){
                    case MSG_SQLITE_RTD_TYTE:
                        pRtdData = (pstPollutantRtdData)pmsg_upproc[iLoop]->msgbuf.data;
                        pollutant_data_proc(pRtdData);
                        break;
                    default:
                        DEBUG_PRINT_INFO(gPrintLevel, "pmsg_upproc_%d recvtype[%ld] not recognize [%-20.20s]\n",iLoop,pmsg_upproc[iLoop]->msgbuf.mtype,pmsg_upproc[iLoop]->msgbuf.data);
                }
            }
    		/*if(MSG_SQLITE_RTD_TYTE == pmsg_upproc[iLoop]->msgbuf.mtype){
    			tinz_db_exec(&scy_data, pmsg_upproc[iLoop]->msgbuf.data);
                DEBUG_PRINT_INFO(gPrintLevel, "pmsg_upproc_%d recv:%s\n",iLoop,pmsg_upproc[iLoop]->msgbuf.data);
    		}*/
        }
    }
}

//数据表查询
void sqlite3_select(tinz_db_ctx_t* ctx, char *tableName)
{
    const unsigned char *gettime;
    float max,min,avg,cou;
	char sql[SQL_LEN];
	snprintf(sql,sizeof(sql)-1,"select * from %s;",tableName);
    DEBUG_PRINT_ERR(5, "sql: %s\n",sql);
	if(SQLITE_OK != sqlite3_prepare(ctx->db, sql, -1, &ctx->stat, 0)){
        return;
    }
    while(SQLITE_ROW == sqlite3_step(ctx->stat)){
        gettime = sqlite3_column_text(ctx->stat, 0);
        max = sqlite3_column_double(ctx->stat, 1);
        min = sqlite3_column_double(ctx->stat, 2);
        avg = sqlite3_column_double(ctx->stat, 3);
        cou = sqlite3_column_double(ctx->stat, 4);
        DEBUG_PRINT_ERR(5, "gettime: %s,max:%f,min:%f,avg:%f,cou:%f\n",gettime,max,min,avg,cou);
    }
    sqlite3_finalize(ctx->stat);
}

int main(int argc, char* argv[])
{	
	/*共享内存*/
	DEBUG_PRINT_INFO(gPrintLevel, "getParaShm start\n");
	pgPara = (pstPara)getParaShm();
    pgPara->GeneralPara.AlarmTime = 60;
    pgData = (pstData)getDataShm();
    //DEBUG_PRINT_INFO(gPrintLevel, "getPollutantDataShm start\n");
    //pgPollutantData = (pstPollutantData)getPollutantDataShm();
	/*消息队列*/
    MessageInit();
	/*数据库*/
	DEBUG_PRINT_INFO(gPrintLevel, "open [%s]\n",SCY_DATA);
	snprintf(scy_data.name,sizeof(scy_data.name)-1,SCY_DATA);
	if(TINZ_ERROR == tinz_db_open(&scy_data)){
		exit(0);	
	}
    sqlite3_select(&scy_data, "Mins_w09008");
	/*数据初始化*/
	DEBUG_PRINT_INFO(gPrintLevel, "data init\n");
	//meter_data_init();
   	while(1){
        
		/**/
		MessageRecv();
		sleep(1);
	}
	tinz_db_close(&scy_data);
	return 0;
}


