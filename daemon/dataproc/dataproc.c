#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "sqlite3.h"

#include "nginx_helper.h"
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
pstHistoryData pgHistoryData;
struct _msg *pmsg_upproc[SITE_SEND_CNT];
struct _msg *pmsg_interface;
struct _msg *pmsg_dataproc_to_upproc;
char         code[POLLUTANT_CNT][4]={"BO1","011","001"};


int 			gPrintLevel = 5;
tinz_db_ctx_t 	scy_data;

void _proj_init(void)__attribute__((constructor));
void _proj_uninit(void)__attribute__((destructor));

void _proj_init(void){
	DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] start!!!\n");
}
void _proj_uninit(void)
{
	DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] stop!!!\n");
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
    for(iLoop=0;iLoop<SITE_SEND_CNT;iLoop++){
        if(iLoop == SITE_CNT || pgPara->SitePara[iLoop].ServerOpen){
        	DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] pmsg_upproc_%d start\n",iLoop);
        	pmsg_upproc[iLoop] = (struct _msg*)malloc(sizeof(struct _msg));
        	memset(pmsg_upproc[iLoop],0,sizeof(struct _msg));
        	if(TINZ_ERROR == prepareMsg(MSG_PATH_MSG,MSG_NAME_UPPROC_TO_SQLITE, iLoop+1, pmsg_upproc[iLoop])){
        		exit(0);
        	}
        }
    }
    
    DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] pmsg_dataproc_to_upproc start\n");
    pmsg_dataproc_to_upproc = (struct _msg*)malloc(sizeof(struct _msg));
    memset(pmsg_dataproc_to_upproc,0,sizeof(struct _msg));
    if(TINZ_ERROR == prepareMsg(MSG_PATH_MSG,MSG_NAME_DATAPROC_TO_UPPROC, MSG_ID_DATAPROC_TO_UPPROC, pmsg_dataproc_to_upproc)){
    	exit(0);
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
    		
    		snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%-4.4s%-2.2s%-2.2s%-2.2s%-2.2s%-2.2s\',%4.2f,%8.2f);",TableName,\
    									&pRtdData->DataTime[0], &pRtdData->DataTime[4],&pRtdData->DataTime[6],\
    									&pRtdData->DataTime[8], &pRtdData->DataTime[10], &pRtdData->DataTime[12],\
    									pRtdData->Row[iLoop].rtd,
    									pRtdData->Row[iLoop].cou);
    		tinz_db_exec(&scy_data,sql);
    		DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
    	} 
    }
}

static void pollutant_data_proc_day(pstPollutantRtdData  pRtdData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];
    int     iLoop;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(pRtdData->Row[iLoop].day >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Day_%s",code[iLoop]);
            if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }
            snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%-4.4s%-2.2s%-2.2s000000\',%8.2f,%8.2f);",TableName,\
                                        &pRtdData->DataTime[0], &pRtdData->DataTime[4],&pRtdData->DataTime[6],\
                                        pRtdData->Row[iLoop].day,
                                        pRtdData->Row[iLoop].cou);
            tinz_db_exec(&scy_data,sql);
            DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
        } 
    }
}

static void pollutant_data_proc_month(pstPollutantRtdData  pRtdData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];
    int     iLoop;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(pRtdData->Row[iLoop].mon >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Mon_%s",code[iLoop]);
            if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }
            snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%-4.4s%-2.2s01000000\',%8.2f,%8.2f);",TableName,\
                                        &pRtdData->DataTime[0], &pRtdData->DataTime[4],\
                                        pRtdData->Row[iLoop].mon,
                                        pRtdData->Row[iLoop].cou);
            tinz_db_exec(&scy_data,sql);
            DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
        } 
    }
}

static void pollutant_data_proc_qut(pstPollutantRtdData  pRtdData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];
    int     iLoop;
    uint8_t res = 0;
    uint8_t month = 0;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(pRtdData->Row[iLoop].qut >= 0){
            res = (uint8_t)ngx_atoi((u_char*)&pRtdData->DataTime[4], 2);
            if(TINZ_ERROR != (month = month_to_qut(res))){
               
                snprintf(TableName,sizeof(TableName)-1,"Qut_%s",code[iLoop]);
                if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                    CountDataTableCreate(&scy_data,TableName);
                }
                snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%-4.4s%.2d01000000\',%8.2f,%8.2f);",TableName,\
                                            &pRtdData->DataTime[0], month,\
                                            pRtdData->Row[iLoop].qut,
                                            pRtdData->Row[iLoop].cou);
                tinz_db_exec(&scy_data,sql);
                DEBUG_PRINT_INFO(gPrintLevel, "sql:%s\n",sql);
            }else{
                DEBUG_PRINT_INFO(gPrintLevel, "month_to_qut[%s] err\n",pRtdData->DataTime);
            }
        } 
    }
}

static void pollutant_data_proc_year(pstPollutantRtdData  pRtdData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];
    int     iLoop;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(pRtdData->Row[iLoop].year >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Year_%s",code[iLoop]);
            if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }
            snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%-4.4s0101000000\',%8.2f,%8.2f);",TableName,\
                                        &pRtdData->DataTime[0],\
                                        pRtdData->Row[iLoop].year,
                                        pRtdData->Row[iLoop].cou);
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

static void UpmainMessageSend(pstMessageData pmsgData){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[MAX_MSG_DATA_LEN];

    snprintf(TableName,sizeof(TableName)-1,"MessageSend");
    if(TINZ_OK != TableIsExist(&scy_data,TableName)){
        MessageSendTableCreate(&scy_data,TableName);
    }
    snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%s\',\'%s\',%d,%d,%d,%d,%d,%d,%d,%d);",TableName,\
                                pmsgData->qn,\
                                pmsgData->content,\
                                pmsgData->SendTimes[0],\
                                pmsgData->SendTimes[1],\
                                pmsgData->SendTimes[2],\
                                pmsgData->SendTimes[3],\
                                pmsgData->IsRespond[0],\
                                pmsgData->IsRespond[1],\
                                pmsgData->IsRespond[2],\
                                pmsgData->IsRespond[3]);
    tinz_db_exec(&scy_data,sql);
    DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] sql:%s\n",sql);
}

static void InsertEventData(pstEvent pEvent){
    char    TableName[TABLE_NAME_LEN];
    char 	sql[SQL_LEN];

    snprintf(TableName,sizeof(TableName)-1,"Event");
    if(TINZ_OK != TableIsExist(&scy_data,TableName)){
        EventTableCreate(&scy_data,TableName);
    }
    snprintf(sql,sizeof(sql),"insert into %s values (\'%s\',\'%s\');",TableName,\
                                pEvent->DataTime,\
                                pEvent->Info);
    tinz_db_exec(&scy_data,sql);
    DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] sql:%s\n",sql);
}


static void MessageRecvProc(struct _msg* msg){
    pstPollutantRtdData  pRtdData;
    pstMessageData pmsgData;
    pstEvent pEvent;
    if(msg->msgbuf.mtype > 0){
        DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] msg recvtype[%ld]\n",msg->msgbuf.mtype);
        switch(msg->msgbuf.mtype){
            case MSG_POLLUTANT_RTD_TYTE:
                pRtdData = (pstPollutantRtdData)msg->msgbuf.data;
                pollutant_data_proc(pRtdData);
                break;
            case MSG_SQLITE_SEND_TYTE:
                pmsgData = (pstMessageData)msg->msgbuf.data;
                UpmainMessageSend(pmsgData);
                break;
            case MSG_SQLITE_EVENT_USER_TYTE:
                pEvent = (pstEvent)msg->msgbuf.data;
                InsertEventData(pEvent);
                break;
            default:
                DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] msg recvtype[%ld] not recognize [%-20.20s]\n",msg->msgbuf.mtype,msg->msgbuf.data);
        }
    }

}

static void MessageRecv(){
    int iLoop;
    /*接收上行消息队列*/
    for(iLoop=0;iLoop<SITE_SEND_CNT;iLoop++){
        if(SITE_CNT == iLoop || pgPara->SitePara[iLoop].ServerOpen){
            MsgRcv(pmsg_upproc[iLoop], 0); 
            MessageRecvProc(pmsg_upproc[iLoop]);
        }
    }
    /*接收前端消息队列*/
    MsgRcv(pmsg_interface, 0); 
    MessageRecvProc(pmsg_interface);
}

void tinz_select_historydata_db_table_cb(tinz_db_ctx_t* ctx){
    const unsigned char *gettime;
    pgHistoryData->Pollutant.cnt = 0;
    while(SQLITE_ROW == sqlite3_step(ctx->stat)){
        gettime = sqlite3_column_text(ctx->stat, 0);
        pgHistoryData->Pollutant.Row[pgHistoryData->Pollutant.cnt].data = sqlite3_column_double(ctx->stat, 1);
        snprintf(pgHistoryData->Pollutant.Row[pgHistoryData->Pollutant.cnt].DataTime,DATATIME_LEN,"%s",gettime);
        DEBUG_PRINT_INFO(5, "[dataproc] gettime[%s],data[%f]\n",gettime,pgHistoryData->Pollutant.Row[pgHistoryData->Pollutant.cnt].data);
        pgHistoryData->Pollutant.cnt++;
    }
}

//数据表查询
void sqlite3_select(tinz_db_ctx_t* ctx, char *sql, void(*cb)(tinz_db_ctx_t* ctx))
{
    DEBUG_PRINT_ERR(5, "[dataproc] sql: %s\n",sql);
	if(SQLITE_OK != sqlite3_prepare(ctx->db, sql, -1, &ctx->stat, 0)){
        return;
    }
    cb(ctx);
    sqlite3_finalize(ctx->stat);
}
/*void sqlite3_select(tinz_db_ctx_t* ctx, char *tableName)
{
    const unsigned char *gettime;
    float max,min,avg,cou;
	char sql[SQL_LEN];
	snprintf(sql,sizeof(sql)-1,"select * from %s;",tableName);
    DEBUG_PRINT_ERR(5, "[dataproc] sql: %s\n",sql);
	if(SQLITE_OK != sqlite3_prepare(ctx->db, sql, -1, &ctx->stat, 0)){
        return;
    }
    while(SQLITE_ROW == sqlite3_step(ctx->stat)){
        gettime = sqlite3_column_text(ctx->stat, 0);
        max = sqlite3_column_double(ctx->stat, 1);
        min = sqlite3_column_double(ctx->stat, 2);
        avg = sqlite3_column_double(ctx->stat, 3);
        cou = sqlite3_column_double(ctx->stat, 4);
        DEBUG_PRINT_ERR(5, "[dataproc] gettime: %s,max:%f,min:%f,avg:%f,cou:%f\n",gettime,max,min,avg,cou);
    }
    sqlite3_finalize(ctx->stat);
}*/

void history_data_query(){
    if(1 == pgHistoryData->Pollutant.flag && pgHistoryData->Pollutant.DataType > 0){
        char    TableName[TABLE_NAME_LEN];
        uint8_t PollutantType = pgHistoryData->Pollutant.PollutantType%POLLUTANT_CNT;
        switch (pgHistoryData->Pollutant.DataType)
        {
            case 1: //实时
                snprintf(TableName,sizeof(TableName),"Rtd_%-3.3s_%-4.4s%-2.2s",code[PollutantType],\
                             &pgHistoryData->Pollutant.StartDataTime[0],&pgHistoryData->Pollutant.StartDataTime[4]);
                break;
            case 2: //天
                snprintf(TableName,sizeof(TableName),"Day_%-3.3s",code[PollutantType]);
                break;
            case 3: //月 
                snprintf(TableName,sizeof(TableName),"Mon_%-3.3s",code[PollutantType]);
                break;
            case 4: //季度
                snprintf(TableName,sizeof(TableName),"Qut_%-3.3s",code[PollutantType]);
                break;
            case 5: //年
                snprintf(TableName,sizeof(TableName),"Year_%-3.3s",code[PollutantType]);
                break;
            default:   
                DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] PollutantType wrong\n");
                pgHistoryData->Pollutant.flag = 2;
                return;
        }
    
        char sql[SQL_LEN];
        snprintf(sql,sizeof(sql),"select * from %s where GetTime >= '%-14.14s' and GetTime <= '%-14.14s' order by GetTime desc limit %d;",\
                            TableName,pgHistoryData->Pollutant.StartDataTime,pgHistoryData->Pollutant.StopDataTime,HISTORYCNT);
        sqlite3_select(&scy_data, sql,tinz_select_historydata_db_table_cb);
        pgHistoryData->Pollutant.flag = 2;
    }
}

int main(int argc, char* argv[])
{	
    //pthread_t   thread_id;
    /*共享内存*/
	DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] getParaShm start\n");
	pgPara = (pstPara)getParaShm();
    pgData = (pstData)getDataShm();
    pgHistoryData = (pstHistoryData)getHistoryDataShm();
    initHistoryDataShm();

	/*消息队列*/
    MessageInit();
    pmsg_interface = InterfaceMessageInit(pmsg_interface);
	/*数据库*/
	DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] open [%s]\n",ZLKZQ_DATA);
	snprintf(scy_data.name,sizeof(scy_data.name)-1,ZLKZQ_DATA);
	if(TINZ_ERROR == tinz_db_open(&scy_data)){
		exit(0);	
	}

   	while(1){
        
		/**/
		MessageRecv();
        history_data_query();
		sleep(1);
	}
	tinz_db_close(&scy_data);
    int iLoop;
    for(iLoop=0;iLoop<SITE_SEND_CNT;iLoop++){
        if(NULL != pmsg_upproc[iLoop]){
            free(pmsg_upproc[iLoop]);
        }
    }
    free(pmsg_dataproc_to_upproc);
    free(pmsg_interface);
	return 0;
}


