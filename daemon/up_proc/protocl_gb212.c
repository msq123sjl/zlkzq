#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#include "tcp_client.h"
#include "tinz_common_helper.h"
#include "tinz_common_db_helper.h"
#include "nginx_helper.h"
#include "protocl_gb212.h"
#include "tinz_pub_message.h"
#include "tinz_pub_shm.h"
#include "tinz_base_data.h"
#include "tinz_base_def.h"



extern pstPara pgPara;
extern pstValveControl pgValveControl;
extern int gPrintLevel;
extern pstData pgData;
extern struct _msg *pmsg_upproc[SITE_SEND_CNT];
extern struct _msg *pmsg_upproc_to_control[SITE_CNT];
extern pstMessage pgmsgbuff;
extern uint8_t state_per;

char            code[POLLUTANT_CNT][4]={"BO1","011","001"};

ngx_ulog_url_t ngx_ulog_url_args;/* = {
    0,0,
    ngx_string("未知"),
    ngx_null_string,
    ngx_string("未知"),
    ngx_null_string,
    ngx_null_string,
};*/
	
ngx_str_t  ngx_ulog_lang_unknown = ngx_string("未知");
ngx_str_t  ngx_ulog_null = ngx_null_string;

//请求应答
void RequestRespond(int QnRtn,ngx_ulog_url_t *url_args,pstSerialPara com,TcpClientDev *tcp)
{
	char buf[MAX_TCPDATA_LEN];
	int nLen;
	int CRC16;
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000ST=91;CN=9011;PW=%-6.6s;MN=%s;Flag=0;CP=&&QN=%-17.17s;QnRtn=%d&&",\
								pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,url_args->qn.data,QnRtn);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 7 && nLen == strlen(buf)){
		nLen = nLen - 6;
		buf[2] = (nLen/1000)+'0';
    	buf[3] = (nLen%1000/100)+'0';
    	buf[4] = (nLen%100/10)+'0';
    	buf[5] = (nLen%10)+'0'; 
		CRC16 = CRC16_Modbus(&buf[6], nLen);
		snprintf(&buf[nLen + 6],7,"%.4X\r\n",CRC16);
		if(com!=NULL){
        	//com->write(str.toAscii());
        }
    	if(tcp!=NULL){
        	tcp->packet_send_handle(tcp,buf);
		}
	}else{
		DEBUG_PRINT_WARN(gPrintLevel, "RequestRespond send nLen[%d] ignore!!!", nLen);
	}								
}
//操作返回操作执行结果
void ExecuteRespond(int ExeRtn,ngx_ulog_url_t *url_args,pstSerialPara com,TcpClientDev *tcp)
{
	char buf[MAX_TCPDATA_LEN];
	int nLen;
	int CRC16;
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000ST=91;CN=9012;PW=%-6.6s;MN=%s;CP=&&QN=%-17.17s;ExeRtn=%d&&",\
								pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,url_args->qn.data,ExeRtn);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 7 && nLen == strlen(buf)){
        nLen = nLen - 6;
		buf[2] = (nLen/1000)+'0';
    	buf[3] = (nLen%1000/100)+'0';
    	buf[4] = (nLen%100/10)+'0';
    	buf[5] = (nLen%10)+'0';
		CRC16 = CRC16_Modbus(&buf[6], nLen);
		snprintf(&buf[nLen + 6],7,"%.4X\r\n",CRC16);
		
		if(com!=NULL){
        	//com->write(str.toAscii());
        }
    	if(tcp!=NULL){
        	tcp->packet_send_handle(tcp,buf);
		}
	}else{
		DEBUG_PRINT_WARN(gPrintLevel, "ExecuteRespond send nLen[%d] ignore!!!", nLen);
	}
}
//发送现场时间
static int SendCurrentTime(ngx_ulog_url_t *url_args,pstSerialPara com,TcpClientDev *tcp)
{
	char buf[MAX_TCPDATA_LEN];
	int nLen;
	int CRC16;
	time_t      now;
    struct tm   *tblock;
	now = time(NULL);
    tblock = localtime( &now );
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000ST=%02d;CN=%04d;PW=%-6.6s;MN=%s;CP=&&QN=%-17.17s;SystemTime=%4d%02d%02d%02d%02d%02d&&",\
								pgPara->GeneralPara.StType,url_args->cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,url_args->qn.data,\
								tblock->tm_year + 1900,tblock->tm_mon + 1,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 7 && nLen == strlen(buf)){
		
	
		nLen = nLen - 6;
		buf[2] = (nLen/1000)+'0';
    	buf[3] = (nLen%1000/100)+'0';
    	buf[4] = (nLen%100/10)+'0';
    	buf[5] = (nLen%10)+'0'; 
		CRC16 = CRC16_Modbus(&buf[6], nLen);
		snprintf(&buf[nLen + 6],7,"%.4X\r\n",CRC16);
		if(com!=NULL){
        	//com->write(str.toAscii());
        }
    	if(tcp!=NULL){
        	tcp->packet_send_handle(tcp,buf);
		}
	}else{
        DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] ExecuteRespond send nLen[%d] ignore!!!", nLen);
		return TINZ_ERROR;
	}
	return TINZ_OK;
}

static int TcpData_ValveStatus_Data(int cn,int flag,pstMessageData pmsgData){

    char per[4];
    int nLen;
    static uint8_t ms = 0;
    struct tm   *tblock;
    time_t      seconds;
    seconds = time(NULL);
    tblock = localtime(&seconds);
    ms = (ms+1)%256;
#if 0     
    if(1 == pgData->state.ValveState){
        snprintf(per,sizeof(per),"100");
    }else if(0 == pgData->state.ValveState){
        snprintf(per,sizeof(per),"0");
    }else{
        snprintf(per,sizeof(per),"---");
    } 
    snprintf(pmsgData->qn,sizeof(pmsgData->qn),"%4d%02d%02d%02d%02d%02d%03d",tblock->tm_year + 1900,tblock->tm_mon + 1,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec,ms);
    nLen = snprintf(pmsgData->content,sizeof(pmsgData->content) - 6,"##0000ST=%02d;CN=%04d;PW=%-6.6s;MN=%s;CP=&&QN=%-17.17s;Per=%s;Pump=%01d&&",\
                                pgPara->GeneralPara.StType,cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,pmsgData->qn,\
                                per,pgData->IOState.In_reflux_open);
#endif
    if(state_per <= 100){
        snprintf(per,sizeof(per),"%d",state_per);
    }else{
        snprintf(per,sizeof(per),"E01");
    }
    snprintf(pmsgData->qn,sizeof(pmsgData->qn),"%4d%02d%02d%02d%02d%02d%03d",tblock->tm_year + 1900,tblock->tm_mon + 1,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec,ms);
    nLen = snprintf(pmsgData->content,sizeof(pmsgData->content) - 6,"##0000ST=%02d;CN=%04d;PW=%-6.6s;MN=%s;CP=&&QN=%-17.17s;Per=%s&&",\
                                pgPara->GeneralPara.StType,cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,pmsgData->qn,per);
    return nLen;
}
//发送心跳包
static int TcpData_Heartbeat(pstMessageData pmsgData){
    int nLen;
    nLen = snprintf(pmsgData->content,sizeof(pmsgData->content) - 6,"MN=%s",pgPara->GeneralPara.MN);
    pmsgData->issql = 1;
    return nLen;
}

int Insert_Message_Data(int cn,int flag){
    int nLen,iLoop;
    int CRC16;
    pstMessageData pmsgData = NULL;
    for(iLoop = 0;iLoop < MESSAGECNT;iLoop++){
        if(MSGBUF_IS_NULL == pgmsgbuff->Data[iLoop].IsUse){
            memset(&pgmsgbuff->Data[iLoop],0,sizeof(stMessageData));
            pgmsgbuff->Data[iLoop].IsUse = MSGBUF_IS_WRITEING;
            pgmsgbuff->Data[iLoop].flag = flag;
            pmsgData = &pgmsgbuff->Data[iLoop];
            break;
        }
    }
    if(NULL == pmsgData){
        DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] cn[%d] pgmsgbuff is busy!!!\n", cn);
        return TINZ_BUSY;
    }
    /****************组装报文**************************/
    if(CN_GetValveStatus == cn){
        nLen = TcpData_ValveStatus_Data(cn,flag,pmsgData);
    }else if(CN_SendHeartbeat == cn){
        nLen = TcpData_Heartbeat(pmsgData);
    }else{
        return TINZ_ERROR;
    }
    if(CN_SendHeartbeat != cn){
        if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 7 && nLen == strlen(pmsgData->content)){ //-10 特殊处理
            CRC16 = CRC16_Modbus(&pmsgData->content[6], nLen-6);
            snprintf(&pmsgData->content[nLen],7,"%.4X\r\n",CRC16);
        
            nLen = nLen - 6;
            pmsgData->content[2] = (nLen/1000)+'0';
            pmsgData->content[3] = (nLen%1000/100)+'0';
            pmsgData->content[4] = (nLen%100/10)+'0';
            pmsgData->content[5] = (nLen%10)+'0'; 
            pmsgData->IsUse = MSGBUF_IS_SENDING;
            
        }else{
            memset(pmsgData,0,sizeof(stMessageData));
            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] Insert_Message_Count cn[%d] send nLen[%d][%s] ignore!!!\n", cn,nLen,pmsgData->content);
            return TINZ_ERROR;
        }
    }else{
        pmsgData->IsUse = MSGBUF_IS_SENDING;
    }
    return TINZ_OK;

}

static int Insert_Message_Count(int cn,int flag){
	//char buf[MAX_TCPDATA_LEN];
	static uint8_t ms = 0;
	int nLen,iLoop;
	int CRC16;
	time_t      now;
    struct tm   *tblock;
    pstMessageData pmsgData = NULL;
    for(iLoop = 0;iLoop < MESSAGECNT;iLoop++){
        if(MSGBUF_IS_NULL == pgmsgbuff->Data[iLoop].IsUse){
            memset(&pgmsgbuff->Data[iLoop],0,sizeof(stMessageData));
            pgmsgbuff->Data[iLoop].IsUse = MSGBUF_IS_WRITEING;
            pgmsgbuff->Data[iLoop].flag = flag;
            pmsgData = &pgmsgbuff->Data[iLoop];
            break;
        }
    }
    if(NULL == pmsgData){
        DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] cn[%d] pgmsgbuff is busy!!!\n", cn);
        return TINZ_ERROR;
    }
    now = time(NULL);
    tblock = localtime(&now);
    ms = (ms+1)%256;
    snprintf(pmsgData->qn,sizeof(pmsgData->qn),"%4d%02d%02d%02d%02d%02d%03d",tblock->tm_year + 1900,tblock->tm_mon + 1,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec,ms);
	nLen = snprintf(pmsgData->content,sizeof(pmsgData->content) - 6,"##0000QN=%-17.17s;ST=%02d;CN=%04d;PW=%-6.6s;MN=%s;Flag=%01d;CP=&&&&",\
								pmsgData->qn,pgPara->GeneralPara.StType,cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,flag);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 7 && nLen == strlen(pmsgData->content)){
		
	
		nLen = nLen - 6;
		pmsgData->content[2] = (nLen/1000)+'0';
    	pmsgData->content[3] = (nLen%1000/100)+'0';
    	pmsgData->content[4] = (nLen%100/10)+'0';
    	pmsgData->content[5] = (nLen%10)+'0';
        CRC16 = CRC16_Modbus(&pmsgData->content[6], nLen);
		snprintf(&pmsgData->content[nLen + 6],7,"%.4X\r\n",CRC16);
        pmsgData->IsUse = MSGBUF_IS_SENDING;
		
	}else{
        memset(pmsgData,0,sizeof(stMessageData));
		DEBUG_PRINT_WARN(gPrintLevel, "Insert_Message_Count cn[%d] send nLen[%d] ignore!!!", cn,nLen);
		return TINZ_ERROR;
	}
	return TINZ_OK;

}

//市电状态
void PowerState()
{
    int res = TINZ_ERROR;
    if(pgData->IOState.InPower != pgData->state.InPower){
        
        if(POWERUP == pgData->IOState.InPower){
            res=Insert_Message_Count(CN_SendPowerUp, 0);               
        }else{
            res=Insert_Message_Count(CN_SendPowerDown, 0);
        }
        if(TINZ_OK == res){
            pgData->state.InPower = pgData->IOState.InPower;
        }
    }
}

//电动阀供电状态
void ValvePowerState()
{
    int res = TINZ_ERROR;
    if(pgData->current_Ia[0] <= 2 && 0 == pgData->state.ValveState){
        res=Insert_Message_Count(CN_SendValvePowerDown, 0);
        if(TINZ_OK == res){
            pgData->state.ValveState = 1;
        }
    }else if(pgData->current_Ia[0] > 2 && 1 == pgData->state.ValveState){
        res=Insert_Message_Count(CN_SendValvePowerUp, 0);
        if(TINZ_OK == res){
            pgData->state.ValveState = 0;
        }
    }

}

//远程设定时间
static int messageSetTime(u_char *pSystemTime)
{
    struct tm tb;
    char tmp_buf[80];
    int res1;
    int res2;
    if (strptime((char*)pSystemTime, "%Y%m%d%H%M%S", &tb) != 0) {
        strftime(tmp_buf, 80, "/bin/date -s \"%Y-%m-%d %H:%M:%S\"", &tb);
        res1=system(tmp_buf);  //设置系统时间
        res2=system("/sbin/hwclock --systohc");   //将系统时间写入到RTC硬件中，以保留设置。这一操作是为了将修改好的时间写入到RTC中保存。如果不进行这一步操作，则
        DEBUG_PRINT_INFO(gPrintLevel, "set time[%s]\n", tmp_buf);                                                              //重新上电开机以后系统从RTC中读取到的仍然是原来的时间
        if(res1==0 && res2==0){
           return TINZ_OK; 
        }
    }
    DEBUG_PRINT_WARN(gPrintLevel, "set time[%s] err!!!\n", tmp_buf);
    return TINZ_ERROR; 
}
//发送阀门状态
static int SendValveStatus(ngx_ulog_url_t *url_args,pstSerialPara com,TcpClientDev *tcp)
{
	char buf[MAX_TCPDATA_LEN];
	int nLen;
	int CRC16;
    char per[4];
    uint8_t per_value;
#if 0
    if(1 == pgData->state.ValveState){
        snprintf(per,sizeof(per),"100");
    }else if(0 == pgData->state.ValveState){
        snprintf(per,sizeof(per),"0");
    }else{
        snprintf(per,sizeof(per),"---");
    } 
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000ST=%02d;CN=%04d;PW=%-6.6s;MN=%s;CP=&&QN=%-17.17s;Per=%s;Pump=%01d&&",\
								pgPara->GeneralPara.StType,url_args->cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,url_args->qn.data,\
								per,pgData->IOState.In_reflux_open);
#endif
    per_value = abs(pgValveControl->per - pgValveControl->per_measure)>5 ? pgValveControl->per_measure : pgValveControl->per;
    if(per_value <= 100){
        snprintf(per,sizeof(per),"%d",per_value);
    }else{
        snprintf(per,sizeof(per),"E01");
    } 
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000ST=%02d;CN=%04d;PW=%-6.6s;MN=%s;CP=&&QN=%-17.17s;Per=%s&&",\
								pgPara->GeneralPara.StType,url_args->cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,url_args->qn.data,per);

    if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 7 && nLen == strlen(buf)){

		nLen = nLen - 6;
		buf[2] = (nLen/1000)+'0';
    	buf[3] = (nLen%1000/100)+'0';
    	buf[4] = (nLen%100/10)+'0';
    	buf[5] = (nLen%10)+'0'; 
		CRC16 = CRC16_Modbus(&buf[6], nLen);
		snprintf(&buf[nLen + 6],7,"%.4X\r\n",CRC16);
	
		if(com!=NULL){
        	//com->write(str.toAscii());
        }
    	if(tcp!=NULL){
        	tcp->packet_send_handle(tcp,buf);
		}
	}else{
		DEBUG_PRINT_WARN(gPrintLevel, "ExecuteRespond send nLen[%d] ignore!!!", nLen);
		return TINZ_ERROR;
	}
	return TINZ_OK;
}

#if 0
static void rtd_data_proc(stPollutantRtdData  RtdData,TcpClientDev *tcp){
	char 	TableName[TABLE_NAME_LEN];
	char 	*sql;
	int		iLoop;
    sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
    	if(RtdData.Row[iLoop].rtd >= 0){

    		snprintf(TableName,sizeof(TableName)-1,"Rtd_%s_%-4.4s%-2.2s",code[iLoop],\
    											&RtdData.DataTime[0], &RtdData.DataTime[4]);
    		/*if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                RtdTableCreate(&scy_data,TableName);
            }*/
    		
    		snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s,%4.2f,%8.2f);",TableName,\
    									&RtdData.DataTime[0], &RtdData.DataTime[4],&RtdData.DataTime[6],\
    									&RtdData.DataTime[8], &RtdData.DataTime[10], &RtdData.DataTime[12],\
    									RtdData.Row[iLoop].rtd,
    									RtdData.Row[iLoop].cou);
            pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
    		//tinz_db_exec(&scy_data,sql);
    	} 
    }
}

static void day_data_proc(stPollutantRtdData  RtdData,TcpClientDev *tcp){
    char    TableName[TABLE_NAME_LEN];
    char    *sql;
    int     iLoop;
    sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(RtdData.Row[iLoop].day >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Day_%s",code[iLoop]);
            /*if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }*/
            snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s,%8.2f,%8.2f);",TableName,\
                                        &RtdData.DataTime[0], &RtdData.DataTime[4],&RtdData.DataTime[6],\
                                        &RtdData.DataTime[8], &RtdData.DataTime[10], &RtdData.DataTime[12],\
                                        RtdData.Row[iLoop].day,
                                        RtdData.Row[iLoop].cou);
            pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
            //tinz_db_exec(&scy_data,sql);
        } 
    }
}

static void month_data_proc(stPollutantRtdData  RtdData,TcpClientDev *tcp){
    char    TableName[TABLE_NAME_LEN];
    char    *sql;
    int     iLoop;
    sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(RtdData.Row[iLoop].day >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Mon_%s",code[iLoop]);
            /*if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }*/
            snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s,%8.2f,%8.2f);",TableName,\
                                        &RtdData.DataTime[0], &RtdData.DataTime[4],&RtdData.DataTime[6],\
                                        &RtdData.DataTime[8], &RtdData.DataTime[10], &RtdData.DataTime[12],\
                                        RtdData.Row[iLoop].mon,
                                        RtdData.Row[iLoop].cou);
            pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
            //tinz_db_exec(&scy_data,sql);
        } 
    }
}

static void qut_data_proc(stPollutantRtdData  RtdData,TcpClientDev *tcp){
    char    TableName[TABLE_NAME_LEN];
    char    *sql;
    int     iLoop;
    sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(RtdData.Row[iLoop].day >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Qut_%s",code[iLoop]);
            /*if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }*/
            snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s,%8.2f,%8.2f);",TableName,\
                                        &RtdData.DataTime[0], &RtdData.DataTime[4],&RtdData.DataTime[6],\
                                        &RtdData.DataTime[8], &RtdData.DataTime[10], &RtdData.DataTime[12],\
                                        RtdData.Row[iLoop].qut,
                                        RtdData.Row[iLoop].cou);
            pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
            //tinz_db_exec(&scy_data,sql);
        } 
    }
}

static void year_data_proc(stPollutantRtdData  RtdData,TcpClientDev *tcp){
    char    TableName[TABLE_NAME_LEN];
    char    *sql;
    int     iLoop;
    sql = pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.data;
    for(iLoop = 0; iLoop < POLLUTANT_CNT; iLoop++){
        
        if(RtdData.Row[iLoop].day >= 0){
    
            snprintf(TableName,sizeof(TableName)-1,"Year_%s",code[iLoop]);
            /*if(TINZ_OK != TableIsExist(&scy_data,TableName)){
                CountDataTableCreate(&scy_data,TableName);
            }*/
            snprintf(sql,MAX_MSG_DATA_LEN,"insert or replace into %s values (%-4.4s-%-2.2s-%-2.2s %-2.2s:%-2.2s:%-2.2s,%8.2f,%8.2f);",TableName,\
                                        &RtdData.DataTime[0], &RtdData.DataTime[4],&RtdData.DataTime[6],\
                                        &RtdData.DataTime[8], &RtdData.DataTime[10], &RtdData.DataTime[12],\
                                        RtdData.Row[iLoop].year,
                                        RtdData.Row[iLoop].cou);
            pmsg_upproc[tcp->tcplink->SiteNum]->msgbuf.mtype = MSG_SQLITE_TYTE;
            MsgSend(pmsg_upproc[tcp->tcplink->SiteNum]);
            //tinz_db_exec(&scy_data,sql);
        } 
    }
}
#endif

static inline int parse_url(char *str, int iRecvLen, ngx_ulog_url_t *url_args){
	/*
	包头 			字符 			2 			##
	数据段长度 		十进制 		4 			数据段的ASCII字符数 例如：长255，则写为“0255”
	数据段			字符 			0～1024 
	CRC校验		十六进制	 	4 			数据段的校验结果，例如4B30，如CRC校验错，即执行失败
	包尾   		字符 			2 			固定为<CR><LF>（回车、换行）
	*/
	char *pos;
	char *end = NULL;
	char flag,flag_arg;
	int  data_len;
	int  CRC16;
    int  StType;
    char tmp_buf[64];
	ngx_str_t	name;
	ngx_str_t   value;
	/*报文长度校验*/
	if(iRecvLen < 12){
		DEBUG_PRINT_WARN(gPrintLevel, "GB212 LENGTH[%d]\n",iRecvLen);
		return TINZ_ERROR;
	}
	/*查找报文头部*/
	pos = strstr(str,"##");
	if(NULL == pos){
		DEBUG_PRINT_WARN(gPrintLevel, "GB212 START ERR!!!\n");
		return TINZ_ERROR;
	}
	pos += 2;   //头
	data_len = (int)ngx_atoi((u_char*)pos, 4);
	pos += 4;    //数据段长度
	/*查找包尾，校验报文长度*/
	end = strstr(str,"\x0d\x0a");
    if(NULL == end || end - pos != data_len + 4){  //+4 CRC字节数
        DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 end ERR!!!\n");
		return TINZ_ERROR;
	}
    
	/*CRC校验*/
	if(NGX_ERROR == (CRC16 = ngx_hextoi((u_char*)(end - 4), 4))){
        DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] CRC16_Modbus CRC DATA ERR!!!\n");
		return TINZ_ERROR;
	}
	/*if(CRC16 != CRC16_Modbus(pos, data_len)){
        DEBUG_PRINT_WARN(gPrintLevel, "CRC16_Modbus CRC ERR string[%-10.10s]...[%-10.10s] data_len[%d]!!!\n",pos,pos + data_len -10,data_len);
		DEBUG_PRINT_WARN(gPrintLevel, "CRC16_Modbus CRC ERR[%04X][%04X]!!!\n",CRC16,CRC16_Modbus(pos, data_len));
		return TINZ_ERROR;
	}*/
	end = end - 4;  //CRC
	for(;pos < end;pos++){
		if(*pos == '&'){
			continue;
		}
		name.data 	= (u_char*)pos;
		name.len	= 0;
		value = ngx_ulog_null;
		flag  = 1;
		for(;pos < end;pos++){
			if(flag){*pos = ngx_tolower(*pos);}
			if (*pos == '=') {
				flag = 0;
                name.len = pos - (char*)name.data;
                
                if (pos + 1 < end) {
                    value.data = (u_char*)(pos + 1);
                }
                else {
                    break;
                }
            }

            if (*pos == ';' || *pos == ',' || *pos == '&' || pos == end) {

                if (value.data != NULL) {
                    value.len = pos - (char*)value.data;
                }
                break;
            }
		}
		//存在字段但值为空
		if(value.len > 0 && name.len > 0){
            //判断获取的字符串是哪个参数
            flag_arg = 0;
            switch (name.len) {
				case 2:
					if(ngx_str2cmp(name.data, 'm', 'n')){
                        if(strncmp((char*)value.data,(char*)pgPara->GeneralPara.MN,value.len)){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 MN [%-24.24s len:%d][%s] ERR!!!\n",value.data,value.len,pgPara->GeneralPara.MN);
							return TINZ_ERROR;
						}
						break;
					}
					if(ngx_str2cmp(name.data, 's', 't')){
						//[0-9]
                        StType = ngx_atoi(value.data, value.len);
                        if((int)pgPara->GeneralPara.StType != StType && 91 != StType){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 ST [%-7.7s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
					}
					if(ngx_str2cmp(name.data, 'c', 'n')){
						//[0-9]
						if(NGX_ERROR == (url_args->cn = ngx_atoi(value.data, value.len))){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 CN [%-7.7s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
					}
					if(ngx_str2cmp(name.data, 'q', 'n')){
						if(value.len != sizeof("YYYYMMDDHHMMSSZZZ")-1 || NGX_ERROR == ngx_isnumbers(value.data, value.len)){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 QN [%-17.17s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						url_args->qn = value;
						//[0-9A-Za-z]
						break;
					}
					if(ngx_str2cmp(name.data, 'p', 'w')){
						if(value.len != sizeof("123456")-1 || strncmp((char*)value.data,(char*)pgPara->GeneralPara.PW,value.len)){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 PW [%-6.6s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						url_args->pw = value;
						//[0-9A-Za-z]
						break;
					}
					flag_arg = 1;
					break;
				case 3:
					if(ngx_str3cmp(name.data, 'p', 'e','r')){
						if(NGX_ERROR == (url_args->per = (uint8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 per [%-5.5s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;
				case 4:
					if(ngx_str4cmp(name.data, 'f', 'l','a','g')){
						if(value.len != 1 || NGX_ERROR == (url_args->flag = (int8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 flag [%-10.10s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;			
				case 7:
                    if(ngx_str7cmp(name.data, 'b', '0','1','-','r','t','d')){
						
						if(value.len != sizeof("****.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-RTD [%-7.7s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-7.7s",value.data);
						url_args->RtdData.Row[0].rtd = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, 'b', '0','1','-','d','a','y')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-day [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[0].day = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, 'b', '0','1','-','m','o','n')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-mon [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[0].mon = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, 'b', '0','1','-','q','u','t')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-mon [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[0].qut = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, 'b', '0','1','-','y','e','a')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-yea [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[0].year = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, 'b', '0','1','-','c','o','u')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-cou [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[0].cou = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, '0', '1','1','-','r','t','d')){
						
						if(value.len != sizeof("****.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-RTD [%-7.7s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-7.7s",value.data);
						url_args->RtdData.Row[1].rtd = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, '0', '1','1','-','d','a','y')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-day [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[1].day = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, '0', '1','1','-','m','o','n')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-mon [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[1].mon = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, '0', '1','1','-','q','u','t')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-mon [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[1].qut = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, '0', '1','1','-','y','e','a')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-yea [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[1].year = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, '0', '1','1','-','c','o','u')){
						
						if(value.len != sizeof("********.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 011-cou [%-11.11s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-11.11s",value.data);
						url_args->RtdData.Row[1].cou = atof(tmp_buf);
						//[0-9]
						break;
					}
                    if(ngx_str7cmp(name.data, '0', '0','1','-','r','t','d')){
						
						if(value.len != sizeof("****.**")-1){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 001-RTD [%-7.7s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-7.7s",value.data);
						url_args->RtdData.Row[2].rtd = atof(tmp_buf);
						//[0-9]
						break;
					}
					break;						
				case 8:
                    if(ngx_str8cmp(name.data, 'd', 'a','t','a','t','i','m','e')){
						if((value.len != DATATIME_LEN -1) || NGX_ERROR == ngx_isnumbers(value.data, value.len)){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 DataTime [%-14.14s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        memcpy(url_args->RtdData.DataTime,value.data,value.len);
						break;
					}

					flag_arg = 1;
					break;
	
				case 10:
					if(ngx_str10cmp(name.data, 's', 'y','s','t','e','m','t','i','m','e')){
						if(value.len != SYSTEMTIME_LEN - 1 || NGX_ERROR == ngx_isnumbers(value.data, value.len)){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 SystemTime [%-14.14s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
                        memcpy(url_args->SystemTime,value.data,value.len);
						//[0-9]
						break;
					}
                    if(ngx_str10cmp(name.data, 'b', '0','1','-','m','o','n','a','l','l')){
						if(value.len != 8 || NGX_ERROR == (url_args->PollutantPara[0].Row.MonAll = (uint32_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 B01-MonAll [%-8.8s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
                        url_args->PollutantPara[0].isValid = 1;
						break;
					}
                    if(ngx_str10cmp(name.data, 'b', '0','1','-','q','u','t','a','l','l')){
						if(value.len != 8 || NGX_ERROR == (url_args->PollutantPara[0].Row.QutAll = (uint32_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 B01-QutAll [%-8.8s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
					}
                    if(ngx_str10cmp(name.data, 'b', '0','1','-','y','e','a','a','l','l')){
						if(value.len != 8 || NGX_ERROR == (url_args->PollutantPara[0].Row.YeaAll = (uint32_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 B01-YeaAll [%-8.8s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
					}
                    if(ngx_str10cmp(name.data, '0', '1','1','-','m','o','n','a','l','l')){
						if(value.len != 8 || NGX_ERROR == (url_args->PollutantPara[1].Row.MonAll = (uint32_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 B01-MonAll [%-8.8s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
                        url_args->PollutantPara[1].isValid = 1;
						break;
					}
                    if(ngx_str10cmp(name.data, '0', '1','1','-','q','u','t','a','l','l')){
						if(value.len != 8 || NGX_ERROR == (url_args->PollutantPara[1].Row.QutAll = (uint32_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 B01-QutAll [%-8.8s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
					}
                    if(ngx_str10cmp(name.data, '0', '1','1','-','y','e','a','a','l','l')){
						if(value.len != 8 || NGX_ERROR == (url_args->PollutantPara[1].Row.YeaAll = (uint32_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "GB212 B01-YeaAll [%-8.8s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
					}
					flag_arg = 1;
					break;	
				default:
					flag_arg = 1;
					break;
			}
			if(flag_arg){
                DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] parse [%-20.20s][%d]\n",name.data,name.len);
			}
		}
	}
	if(url_args->cn ==0 || url_args->qn.len == 0 || url_args->flag < 0){
		return TINZ_ERROR;
	}
	return TINZ_OK;
}

//收到平台信息并处理
int messageProc(char *str, int iRecvLen, pstSerialPara com,TcpClientDev *tcp)
{
    int iLoop;
    ngx_ulog_url_t url_args;
    
    memset(&url_args,0,sizeof(ngx_ulog_url_t));
    for(iLoop=0;iLoop<POLLUTANT_CNT;iLoop++){
        url_args.RtdData.Row[iLoop].rtd = -1;
    }
    url_args.flag = -1;
    url_args.per = 255;
    
	if(TINZ_ERROR == parse_url(str, iRecvLen, &url_args)){
		return TINZ_ERROR;
	}
	if(url_args.qn.len != QN_LEN-1 \
		|| url_args.cn <= 0 \
		|| url_args.flag < 0\
		|| url_args.pw.len != PW_LEN-1){
		DEBUG_PRINT_WARN(gPrintLevel, "GB212 pare ERR qn_len[%d] cn[%d] flag[%d] pw_len[%d]!!!\n",\
            url_args.qn.len,url_args.cn,url_args.flag,url_args.pw.len);
		return TINZ_ERROR;
	}

	switch(url_args.cn){
		case CN_GetTime:
			if(url_args.flag & 0x01){
				RequestRespond(REQUEST_READY,&url_args, com, tcp);
            }   
			if(TINZ_OK == SendCurrentTime(&url_args, com, tcp)){
				ExecuteRespond(RESULT_SUCCESS, &url_args,com, tcp);
			}
			else{
				ExecuteRespond(RESULT_FAILED, &url_args,com, tcp);
			}
			break;
		case CN_SetTime:
            if(pgPara->Mode){
                RequestRespond(REQUEST_REFUSED,&url_args, com, tcp);
                break;
            }
            if(url_args.flag & 0x01){
				
				RequestRespond(REQUEST_READY,&url_args, com, tcp);
            }   
			if(TINZ_OK == messageSetTime(url_args.SystemTime)){
				ExecuteRespond(RESULT_SUCCESS, &url_args,com, tcp);
			}
			else{
				ExecuteRespond(RESULT_FAILED, &url_args,com, tcp);
			}
			break;	
		case CN_RecvRtdData:
            if(url_args.flag & 0x01){
				
				RequestRespond(REQUEST_READY,&url_args, com, tcp);
            }
            pgData->PollutantsData.RtdData = url_args.RtdData;
            MsgSend(pmsg_upproc[tcp->tcplink->SiteNum],MSG_POLLUTANT_RTD_TYTE,(char*)(&pgData->PollutantsData.RtdData),(int)sizeof(stPollutantRtdData));
            ExecuteRespond(RESULT_SUCCESS, &url_args,com, tcp);
			break;	
		case CN_ValveControl:
            if(pgPara->Mode){
                RequestRespond(REQUEST_REFUSED,&url_args, com, tcp);
                DEBUG_PRINT_WARN(gPrintLevel, "GB212 pare CN_ValveControl Mode[%d] REQUEST_REFUSED!!!\n",pgPara->Mode);
                break;
            }
            if(url_args.per > 100){
                RequestRespond(REQUEST_CODE_ERR,&url_args, com, tcp);
                break;
            }
            if(pgValveControl->per != url_args.per){
                if(url_args.flag & 0x01){
                    RequestRespond(REQUEST_READY,&url_args, com, tcp);
                }
                pgValveControl->per = url_args.per;
                MsgSend(pmsg_upproc_to_control[tcp->tcplink->SiteNum],MSG_CONTROL_VALVE_TYTE,(char*)(&pgValveControl->per),(int)sizeof(pgValveControl->per));
                syncValveParaShm();
            }else{
                if(url_args.flag & 0x01){
                    RequestRespond(REQUEST_REFUSED,&url_args, com, tcp);
                }
                break;
            }
            ExecuteRespond(RESULT_SUCCESS, &url_args,com, tcp);
			break;
		case CN_GetValveStatus:
            if(url_args.flag & 0x01){
               RequestRespond(REQUEST_READY,&url_args, com, tcp); 
            }
            if(TINZ_OK == SendValveStatus(&url_args, com, tcp)){
				ExecuteRespond(RESULT_SUCCESS, &url_args,com, tcp);
			}
			else{
				ExecuteRespond(RESULT_FAILED, &url_args,com, tcp);
			}
			break;	
		case CN_SetTolerances:
            if(pgPara->Mode){
                RequestRespond(REQUEST_REFUSED,&url_args, com, tcp);
                break;
            }
            if(url_args.flag & 0x01){
               RequestRespond(REQUEST_READY,&url_args, com, tcp); 
            }
            int iLoop;
            for(iLoop=0;iLoop<POLLUTANT_CNT;iLoop++){
                pgPara->PollutantPara[iLoop] = url_args.PollutantPara[iLoop];
            }
            syncParaShm();
            ExecuteRespond(RESULT_SUCCESS, &url_args,com, tcp);
			break;
		default:
			RequestRespond(REQUEST_CODE_ERR,&url_args, com, tcp);
			break;
	}
    return TINZ_OK;
}

