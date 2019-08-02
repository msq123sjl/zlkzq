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

pstPara 		pgPara;
pstData pgData;
pstHistoryData pgHistoryData;
struct _msg *pmsg_upproc[SITE_SEND_CNT];
struct _msg *pmsg_interface;
struct _msg *pmsg_dataproc_to_upproc;
char         code[POLLUTANT_CNT][7]={"a01001","a01002","a01006","a01007","a01008","a34001","a50001"};


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

static void pollutant_mins_data_insert(pstPollutantData pData){
	char 	TableName[TABLE_NAME_LEN];
	char 	sql[SQL_LEN];
	int		iLoop;
    struct tm *info;
    info = localtime(&pData->seconds);
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){

        snprintf(TableName,sizeof(TableName)-1,"Mins_%s_%04d%02d",code[iLoop],\
        									info->tm_year + 1900, info->tm_mon + 1);
        if(TINZ_OK != TableIsExist(&scy_data,TableName)){
            DustDataTableCreate(&scy_data,TableName);
        }
        
        snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%04d%02d%02d%02d%02d\',%4.2f);",TableName,\
        							info->tm_year+1900, info->tm_mon+1,info->tm_mday,info->tm_hour,info->tm_min,\
        							(float)double_div_uint(pData->Row[iLoop].Sum,pData->Row[iLoop].CNT));
        tinz_db_exec(&scy_data,sql);
        DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] sql:%s\n",sql);
    }
}

static void pollutant_hour_data_insert(pstPollutantData pData){
	char 	TableName[TABLE_NAME_LEN];
	char 	sql[SQL_LEN];
	int		iLoop;
    struct tm *info;
    info = localtime(&pData->seconds);
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){

        snprintf(TableName,sizeof(TableName)-1,"Hour_%s",code[iLoop]);
        if(TINZ_OK != TableIsExist(&scy_data,TableName)){
            DustDataTableCreate(&scy_data,TableName);
        }
        
        snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%04d%02d%02d%02d\',%4.2f);",TableName,\
        							info->tm_year+1900, info->tm_mon+1,info->tm_mday,info->tm_hour,\
        							(float)double_div_uint(pData->Row[iLoop].Sum,pData->Row[iLoop].CNT));
        tinz_db_exec(&scy_data,sql);
        DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] sql:%s\n",sql);
    }
}

static void pollutant_day_data_insert(pstPollutantData  pData){
	char 	TableName[TABLE_NAME_LEN];
	char 	sql[SQL_LEN];
	int		iLoop;
    struct tm *info;
    info = localtime(&pData->seconds);
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){

        snprintf(TableName,sizeof(TableName)-1,"Day_%s",code[iLoop]);
        if(TINZ_OK != TableIsExist(&scy_data,TableName)){
            DustDataTableCreate(&scy_data,TableName);
        }
        
        snprintf(sql,sizeof(sql),"insert or replace into %s values (\'%04d%02d%02d\',%4.2f);",TableName,\
        							info->tm_year+1900, info->tm_mon+1,info->tm_mday,\
        							(float)double_div_uint(pData->Row[iLoop].Sum,pData->Row[iLoop].CNT));
        tinz_db_exec(&scy_data,sql);
        DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] sql:%s\n",sql);
    }
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
    //pstPollutantRtdData  pRtdData;
    pstMessageData pmsgData;
    pstEvent pEvent;
    if(msg->msgbuf.mtype > 0){
        DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] msg recvtype[%ld]\n",msg->msgbuf.mtype);
        switch(msg->msgbuf.mtype){
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
        if(pgPara->SitePara[iLoop].ServerOpen){
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
            case 3: //5分钟
                snprintf(TableName,sizeof(TableName),"Mins_%s_%-4.4s%-2.2s",code[PollutantType],\
                             &pgHistoryData->Pollutant.StartDataTime[0],&pgHistoryData->Pollutant.StartDataTime[4]);
                break;
            case 4: //小时
                snprintf(TableName,sizeof(TableName),"Hour_%s",code[PollutantType]);
                break;
            case 5: //天 
                snprintf(TableName,sizeof(TableName),"Day_%s",code[PollutantType]);
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

static void pollutant_data_handle(pstPollutantData data,pstPollutantRtdData rtddata){
    int iLoop;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        if(1 == rtddata->Row[iLoop].flag){
            data->Row[iLoop].Max = (data->Row[iLoop].Max > rtddata->Row[iLoop].rtd) ? data->Row[iLoop].Max : rtddata->Row[iLoop].rtd;
            data->Row[iLoop].Min = (data->Row[iLoop].Min < rtddata->Row[iLoop].rtd) ? data->Row[iLoop].Min : rtddata->Row[iLoop].rtd;
            data->Row[iLoop].Sum += rtddata->Row[iLoop].rtd;
            data->Row[iLoop].CNT++;
        }
    }
}
static void pollutant_rtd_calc_proc(time_t second,pstPollutantRtdData rtddata,long int mtype){
    rtddata->seconds = second;
    pollutant_data_handle(&pgData->PollutantsData.PerMinData,rtddata);
    pollutant_data_handle(&pgData->PollutantsData.MinsData,rtddata);
    pollutant_data_handle(&pgData->PollutantsData.HourData,rtddata);
    pollutant_data_handle(&pgData->PollutantsData.DayData,rtddata); 
    MsgSend(pmsg_dataproc_to_upproc,mtype,(char*)&pgData->PollutantsData.RtdData,(int)sizeof(stPollutantRtdData));
    memset(&pgData->PollutantsData.RtdData,0,sizeof(stPollutantRtdData));
}

static void pollutant_data_calc_proc(time_t second,pstPollutantData data,long int mtype){
    data->seconds = second;
    MsgSend(pmsg_dataproc_to_upproc,mtype,(char*)data,(int)sizeof(stPollutantData));
    //5分钟 小时 天数据入库
    switch(mtype){
        case MSG_POLLUTANT_MINS_TYTE:
            pollutant_mins_data_insert(data);
            break;
        case MSG_POLLUTANT_HOUR_TYTE:
            pollutant_hour_data_insert(data);
            break;
        case MSG_POLLUTANT_DAY_TYTE:
            pollutant_day_data_insert(data);
            break;
        default:
            break;
    }
    memset(data,0,sizeof(stPollutantData));   
}


static void dataproc_thread(){
    time_t seconds,rtd_second,permin_second,mins_second,hour_second,day_second;
    do{
        seconds = time(NULL);
        usleep(100000);
    }while(seconds%60);   //整分钟开始

    rtd_second      = seconds;
    permin_second   = seconds - seconds%60;
    mins_second     = seconds - seconds%300;
    hour_second     = seconds - seconds%3600;
    day_second      = seconds - seconds%86400;

    while(1){
        if(rtd_second/30 < seconds/30){
            rtd_second = seconds - seconds%30;
            pollutant_rtd_calc_proc(rtd_second,&pgData->PollutantsData.RtdData,MSG_POLLUTANT_RTD_TYTE);
        }
        if(permin_second/60 < seconds/60){
            pollutant_data_calc_proc(permin_second,&pgData->PollutantsData.PerMinData,MSG_POLLUTANT_PERMIN_TYTE);
            permin_second = seconds - seconds%60;
        }
        if(mins_second/300 < seconds/300){
            pollutant_data_calc_proc(mins_second,&pgData->PollutantsData.MinsData,MSG_POLLUTANT_MINS_TYTE);
            mins_second = seconds - seconds%300;
        }
        if(hour_second/3600 < seconds/3600){
            pollutant_data_calc_proc(hour_second,&pgData->PollutantsData.HourData,MSG_POLLUTANT_HOUR_TYTE);
            hour_second = seconds - seconds%3600;
        }
        if(day_second/86400 < seconds/86400){
            pollutant_data_calc_proc(day_second,&pgData->PollutantsData.DayData,MSG_POLLUTANT_DAY_TYTE);
            day_second = seconds - seconds%86400;
        }
        sleep(1);
        seconds = time(NULL);
    }
    
}

int main(int argc, char* argv[])
{	
    pthread_t   thread_id;
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
	DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] open [%s]\n",SCY_DATA);
	snprintf(scy_data.name,sizeof(scy_data.name)-1,SCY_DATA);
	if(TINZ_ERROR == tinz_db_open(&scy_data)){
		exit(0);	
	}
	/*数据初始化*/
	DEBUG_PRINT_INFO(gPrintLevel, "[dataproc] data init\n");
	//meter_data_init();
	
	/*创建数据处理线程*/
    if(pthread_create(&thread_id,NULL,(void *)(&dataproc_thread),NULL) == -1)
	{
		DEBUG_PRINT_INFO(gPrintLevel,"[dataproc] dataproc_thread create error!\n");
	}
   	while(1){
        
		/**/
		MessageRecv();
        history_data_query();
		sleep(1);
	}
	tinz_db_close(&scy_data);
	return 0;
}


