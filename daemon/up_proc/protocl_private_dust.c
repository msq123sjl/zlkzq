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
#include "protocl_private_dust.h"
#include "tinz_pub_message.h"
#include "tinz_pub_shm.h"
#include "tinz_base_data.h"
#include "tinz_base_def.h"



extern pstPara pgPara;
extern int gPrintLevel;
extern pstPollutantPara pgPollutantPara;
extern pstData pgData;
extern struct _msg *pmsg_upproc[SITE_SEND_CNT];
extern pstMessage pgmsgbuff;

char         code[POLLUTANT_CNT][7]={"a01001","a01002","a01006","a01007","a01008","a34001","a50001"};

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
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000ST=91;CN=9011;PW=%-6.6s;MN=%-14.14s;Flag=0;CP=&&QN=%-17.17s;QnRtn=%d&&",\
								pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,url_args->qn.data,QnRtn);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 6 && nLen == strlen(buf)){
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
		DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] RequestRespond send nLen[%d] ignore!!!", nLen);
	}								
}
//操作返回操作执行结果
void ExecuteRespond(int ExeRtn,ngx_ulog_url_t *url_args,pstSerialPara com,TcpClientDev *tcp)
{
	char buf[MAX_TCPDATA_LEN];
	int nLen;
	int CRC16;
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000ST=91;CN=9012;PW=%-6.6s;MN=%-14.14s;CP=&&QN=%-17.17s;ExeRtn=%d&&",\
								pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,url_args->qn.data,ExeRtn);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 6 && nLen == strlen(buf)){
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
	}
}
#if 0
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
	nLen = snprintf(buf,sizeof(buf) - 6,"##0000ST=%02d;CN=%04d;PW=%-6.6s;MN=%-14.14s;CP=&&QN=%-17.17s;SystemTime=%4d%02d%02d%02d%02d%02d&&",\
								pgPara->GeneralPara.StType,url_args->cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,url_args->qn.data,\
								tblock->tm_year + 1900,tblock->tm_mon + 1,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 6 && nLen == strlen(buf)){
		CRC16 = CRC16_Modbus(&buf[6], nLen);
		snprintf(&buf[nLen + 6],7,"%.4X\r\n",CRC16);
	
		nLen = nLen - 6;
		buf[2] = (nLen/1000)+'0';
    	buf[3] = (nLen%1000/100)+'0';
    	buf[4] = (nLen%100/10)+'0';
    	buf[5] = (nLen%10)+'0'; 
		
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
	nLen = snprintf(pmsgData->content,sizeof(pmsgData->content) - 6,"##0000QN=%-17.17s;ST=%02d;CN=%04d;PW=%-6.6s;MN=%-14.14s;Flag=%01d;CP=&&&&",\
								pmsgData->qn,pgPara->GeneralPara.StType,cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,flag);
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 6 && nLen == strlen(pmsgData->content)){
		CRC16 = CRC16_Modbus(&pmsgData->content[6], nLen);
		snprintf(&pmsgData->content[nLen + 6],7,"%.4X\r\n",CRC16);
	
		nLen = nLen - 6;
		pmsgData->content[2] = (nLen/1000)+'0';
    	pmsgData->content[3] = (nLen%1000/100)+'0';
    	pmsgData->content[4] = (nLen%100/10)+'0';
    	pmsgData->content[5] = (nLen%10)+'0'; 
        pmsgData->IsUse = MSGBUF_IS_SENDING;
		
	}else{
        memset(pmsgData,0,sizeof(stMessageData));
		DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] Insert_Message_Count cn[%d] send nLen[%d] ignore!!!", cn,nLen);
		return TINZ_ERROR;
	}
	return TINZ_OK;

}
#endif
static int TcpData_Pollutant_RtdData(int cn,int flag,pstPollutantRtdData pRtdData,pstMessageData pmsgData){
	static uint8_t ms = 0;
    int nLen;
    struct tm   *tblock;
    tblock = localtime(&pRtdData->seconds);
    ms = (ms+1)%256;
    snprintf(pmsgData->qn,sizeof(pmsgData->qn),"%4d%02d%02d%02d%02d%02d%03d",tblock->tm_year + 1900,tblock->tm_mon + 1,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec,ms);
	nLen = snprintf(pmsgData->content,sizeof(pmsgData->content) - 6,"##0000QN=%-17.17s;ST=%02d;CN=%04d;PW=%-6.6s;MN=%-14.14s;Flag=%01d;CP=&&DataTime=%-14.14s;\
        a01001-Rtd=%4.2f,a01001-Flag=%c;\
        a01002-Rtd=%4.2f,a01002-Flag=%c;\
        a01006-Rtd=%4.2f,a01006-Flag=%c;\
        a01007-Rtd=%4.2f,a01007-Flag=%c;\
        a01008-Rtd=%4.2f,a01008-Flag=%c;\
        a34001-Rtd=%4.2f,a34001-Flag=%c;\
        a50001-Rtd=%4.2f,a50001-Flag=%c;&&",pmsgData->qn,pgPara->GeneralPara.StType,cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,flag,pmsgData->qn,\
        pRtdData->Row[0].rtd,pRtdData->Row[0].flag ? 'N' : 'D',\
        pRtdData->Row[1].rtd,pRtdData->Row[1].flag ? 'N' : 'D',\
        pRtdData->Row[2].rtd,pRtdData->Row[2].flag ? 'N' : 'D',\
        pRtdData->Row[3].rtd,pRtdData->Row[3].flag ? 'N' : 'D',\
        pRtdData->Row[4].rtd,pRtdData->Row[4].flag ? 'N' : 'D',\
        pRtdData->Row[5].rtd,pRtdData->Row[5].flag ? 'N' : 'D',\
        pRtdData->Row[6].rtd,pRtdData->Row[6].flag ? 'N' : 'D');
    return nLen;
}

static int TcpData_Pollutant_Data(int cn,int flag,pstPollutantData pData,pstMessageData pmsgData){
	static uint8_t ms = 0;
    int nLen;
    struct tm   *tblock;
    tblock = localtime(&pData->seconds);
    ms = (ms+1)%256;
    snprintf(pmsgData->qn,sizeof(pmsgData->qn),"%4d%02d%02d%02d%02d%02d%03d",tblock->tm_year + 1900,tblock->tm_mon + 1,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec,ms);
	nLen = snprintf(pmsgData->content,sizeof(pmsgData->content) - 6,"##0000QN=%-17.17s;ST=%02d;CN=%04d;PW=%-6.6s;MN=%-14.14s;Flag=%01d;CP=&&DataTime=%-14.14s;\
        a01001-Min=%4.2f,a01001-Avg=%4.2f,a01001-Max=%4.2f;\
        a01002-Min=%4.2f,a01002-Avg=%4.2f,a01002-Max=%4.2f;\
        a01006-Min=%4.2f,a01006-Avg=%4.2f,a01006-Max=%4.2f;\
        a01007-Min=%4.2f,a01007-Avg=%4.2f,a01007-Max=%4.2f;\
        a01008-Min=%4.2f,a01008-Avg=%4.2f,a01008-Max=%4.2f;\
        a34001-Min=%4.2f,a34001-Avg=%4.2f,a34001-Max=%4.2f;\
        a50001-Min=%4.2f,a50001-Avg=%4.2f,a50001-Max=%4.2f;&&",pmsgData->qn,pgPara->GeneralPara.StType,cn,pgPara->GeneralPara.PW,pgPara->GeneralPara.MN,flag,pmsgData->qn,\
        pData->Row[0].Min,(float)double_div_uint(pData->Row[0].Sum,pData->Row[0].CNT),pData->Row[0].Max,\
        pData->Row[1].Min,(float)double_div_uint(pData->Row[1].Sum,pData->Row[1].CNT),pData->Row[1].Max,\
        pData->Row[2].Min,(float)double_div_uint(pData->Row[2].Sum,pData->Row[2].CNT),pData->Row[2].Max,\
        pData->Row[3].Min,(float)double_div_uint(pData->Row[3].Sum,pData->Row[3].CNT),pData->Row[3].Max,\
        pData->Row[4].Min,(float)double_div_uint(pData->Row[4].Sum,pData->Row[4].CNT),pData->Row[4].Max,\
        pData->Row[5].Min,(float)double_div_uint(pData->Row[5].Sum,pData->Row[5].CNT),pData->Row[5].Max,\
        pData->Row[6].Min,(float)double_div_uint(pData->Row[6].Sum,pData->Row[6].CNT),pData->Row[6].Max);
    return nLen;
}

int Insert_Message_Data(int cn,int flag,void* pData){
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
    if(CN_GetPerMinData == cn || CN_GetMinsData == cn ||CN_GetHourData == cn || CN_GetDayData == cn){
        nLen = TcpData_Pollutant_Data(cn,flag,(pstPollutantData)pData,pmsgData);
    }else if(CN_GetRtdData == cn){
        nLen = TcpData_Pollutant_RtdData(cn,flag,(pstPollutantRtdData)pData,pmsgData);
    }else{
        return TINZ_ERROR;
    }
    
	if(nLen >= MIN_TCPDATA_LEN && nLen < MAX_TCPDATA_LEN - 6 && nLen == strlen(pmsgData->content)){
		CRC16 = CRC16_GB212(&pmsgData->content[6], nLen);
		snprintf(&pmsgData->content[nLen + 6],7,"%.4X\r\n",CRC16);
	
		nLen = nLen - 6;
		pmsgData->content[2] = (nLen/1000)+'0';
    	pmsgData->content[3] = (nLen%1000/100)+'0';
    	pmsgData->content[4] = (nLen%100/10)+'0';
    	pmsgData->content[5] = (nLen%10)+'0'; 
        pmsgData->IsUse = MSGBUF_IS_SENDING;
		
	}else{
        memset(pmsgData,0,sizeof(stMessageData));
		DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] Insert_Message_Count cn[%d] send nLen[%d] ignore!!!", cn,nLen);
		return TINZ_ERROR;
	}
	return TINZ_OK;

}
#if 0
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
    if (strptime(pSystemTime, "%Y%m%d%H%M%S", &tb) != 0) {
        strftime(tmp_buf, 80, "/bin/date -s \"%Y-%m-%d %H:%M:%S\"", &tb);
        res1=system(tmp_buf);  //设置系统时间
        res2=system("/sbin/hwclock --systohc");   //将系统时间写入到RTC硬件中，以保留设置。这一操作是为了将修改好的时间写入到RTC中保存。如果不进行这一步操作，则
        DEBUG_PRINT_INFO(gPrintLevel, "[up_proc] set time[%s]\n", tmp_buf);                                                              //重新上电开机以后系统从RTC中读取到的仍然是原来的时间
        if(res1==0 && res2==0){
           return TINZ_OK; 
        }
    }
    DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] set time[%s] err!!!\n", tmp_buf);
    return TINZ_ERROR; 
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
    char tmp_buf[64];
	ngx_str_t	name;
	ngx_str_t   value;
	/*报文长度校验*/
	if(iRecvLen < 12){
		DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 LENGTH[%d]\n",iRecvLen);
		return TINZ_ERROR;
	}
	/*查找报文头部*/
	pos = strstr(str,"##");
	if(NULL == pos){
		DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 START ERR!!!\n");
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
	if(CRC16 != CRC16_GB212(pos, data_len)){
        DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] CRC16_GB212 CRC ERR string[%-10.10s]...[%-10.10s] data_len[%d]!!!\n",pos,pos + data_len -10,data_len);
		DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] CRC16_GB212 CRC ERR[%04X][%04X]!!!\n",CRC16,CRC16_GB212(pos, data_len));
		return TINZ_ERROR;
	}
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
						if(value.len != MN_LEN - 1 || strncmp((char*)value.data,(char*)pgPara->GeneralPara.MN,value.len)){
							DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 MN [%-14.14s len:%d][%s] ERR!!!\n",value.data,value.len,pgPara->GeneralPara.MN);
							return TINZ_ERROR;
						}
						break;
					}
					if(ngx_str2cmp(name.data, 's', 't')){
						//[0-9]
						if((int)pgPara->GeneralPara.StType != (ngx_atoi(value.data, value.len))){
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
                    
				case 4:
					if(ngx_str4cmp(name.data, 'f', 'l','a','g')){
						if(value.len != 1 || NGX_ERROR == (url_args->flag = (int8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 flag [%-10.10s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						//[0-9]
						break;
					}
					flag_arg = 1;
					break;									
	
				case 10:
                    if(ngx_str10cmp(name.data, 'a', '3','4','0','0','1','-','t','h','d')){
                        if(value.len != sizeof("****.**")-1){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 a34001-Thd [%-7.7s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-7.7s",value.data);
						url_args->PollutantPara.Row[5%POLLUTANT_CNT].Thd[0] = atof(tmp_buf);
                        break;
                    }
                    if(ngx_str10cmp(name.data, 'a', '5','0','0','0','1','-','t','h','d')){
                        if(value.len != sizeof("****.**")-1){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 a50001-Thd [%-7.7s] ERR!!!\n",value.data);
							return TINZ_ERROR;
						}
                        sprintf(tmp_buf,"%-7.7s",value.data);
						url_args->PollutantPara.Row[6%POLLUTANT_CNT].Thd[0] = atof(tmp_buf);
                        break;
                    }
					flag_arg = 1;
					break;
                case 14:
                    if(ngx_str14cmp(name.data, 'a', '3','4','0','0','1','-','e','n','d','t','i','m','e')){
                        if(value.len != 4 || NGX_ERROR == (url_args->PollutantPara.Row[5%POLLUTANT_CNT].EndTime = (uint8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 a34001-EndTime [%-4.4s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
                    }
                    if(ngx_str14cmp(name.data, 'a', '5','0','0','0','1','-','e','n','d','t','i','m','e')){
                        if(value.len != 4 || NGX_ERROR == (url_args->PollutantPara.Row[6%POLLUTANT_CNT].EndTime = (uint8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 a50001-EndTime [%-4.4s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
                    }
                    flag_arg = 1;
					break;
                case 15:
                    if(ngx_str15cmp(name.data, 'a', '3','4','0','0','1','-','d','a','t','e','t','y','p','e')){
                        if(value.len != 1 || NGX_ERROR == (url_args->PollutantPara.Row[5%POLLUTANT_CNT].DataType = (uint8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 a34001-DateType [%-1.1s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
                    }
                    if(ngx_str15cmp(name.data, 'a', '5','0','0','0','1','-','d','a','t','e','t','y','p','e')){
                        if(value.len != 1 || NGX_ERROR == (url_args->PollutantPara.Row[6%POLLUTANT_CNT].DataType = (uint8_t)ngx_atoi(value.data, value.len))){
							DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 a50001-DateType [%-1.1s] LEN[%d] ERR!!!\n",value.data,value.len);
							return TINZ_ERROR;
						}
						break;
                    }
                    flag_arg = 1;
					break;
                case 16:
                    if(ngx_str16cmp(name.data, 'a', '3','4','0','0','1','-','b','e','g','i','n','t','i','m','e')){
                        if(value.len != 4 || NGX_ERROR == (url_args->PollutantPara.Row[5%POLLUTANT_CNT].BeginTime = (uint8_t)ngx_atoi(value.data, value.len))){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 a34001-beginTime [%-4.4s] LEN[%d] ERR!!!\n",value.data,value.len);
                            return TINZ_ERROR;
                        }
                        break;
                    }
                    if(ngx_str16cmp(name.data, 'a', '5','0','0','0','1','-','b','e','g','i','n','t','i','m','e')){
                        if(value.len != 4 || NGX_ERROR == (url_args->PollutantPara.Row[6%POLLUTANT_CNT].BeginTime = (uint8_t)ngx_atoi(value.data, value.len))){
                            DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 a50001-beginTime [%-4.4s] LEN[%d] ERR!!!\n",value.data,value.len);
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
    int iLoop,jLoop;
    ngx_ulog_url_t url_args;
    
    memset(&url_args,0,sizeof(ngx_ulog_url_t));
    memset(&url_args.PollutantPara,-1,sizeof(stPollutantPara));
    url_args.flag = -1;
    
	if(TINZ_ERROR == parse_url(str, iRecvLen, &url_args)){
		return TINZ_ERROR;
	}
	if(url_args.qn.len != QN_LEN-1 \
		|| url_args.cn <= 0 \
		|| url_args.flag < 0\
		|| url_args.pw.len != PW_LEN-1){
		DEBUG_PRINT_WARN(gPrintLevel, "[up_proc] GB212 pare ERR qn_len[%d] cn[%d] flag[%d] pw_len[%d]!!!\n",\
            url_args.qn.len,url_args.cn,url_args.flag,url_args.pw.len);
		return TINZ_ERROR;
	}

	switch(url_args.cn){
        case CN_SetPollutantThresholdPara:
            if(url_args.flag & 0x01){
				RequestRespond(REQUEST_READY,&url_args, com, tcp);
            }  
            for(iLoop=0;iLoop<POLLUTANT_CNT;iLoop++){
                for(jLoop=0;jLoop<2;jLoop++){
                    if(url_args.PollutantPara.Row[iLoop].DataType == jLoop + 1){
                        if(url_args.PollutantPara.Row[iLoop].BeginTime >=0 \
                           && url_args.PollutantPara.Row[iLoop].EndTime >= 0 \
                           && url_args.PollutantPara.Row[iLoop].Thd[jLoop] > 0){

                            pgPollutantPara->Row[iLoop].BeginTime   = jLoop ? url_args.PollutantPara.Row[iLoop].EndTime : url_args.PollutantPara.Row[iLoop].BeginTime;
                            pgPollutantPara->Row[iLoop].EndTime     = jLoop ? url_args.PollutantPara.Row[iLoop].BeginTime : url_args.PollutantPara.Row[iLoop].EndTime;
                            pgPollutantPara->Row[iLoop].DataType    = 1;
                            pgPollutantPara->Row[iLoop].DataFlag[jLoop] = 1;
                            pgPollutantPara->Row[iLoop].Thd[jLoop]      = url_args.PollutantPara.Row[iLoop].Thd[0];
                        }else{
                            ExecuteRespond(RESULT_FAILED, &url_args,com, tcp);
                        }
                    }
                }
            }
		    ExecuteRespond(RESULT_SUCCESS, &url_args,com, tcp);
            break;
		default:
			RequestRespond(REQUEST_CODE_ERR,&url_args, com, tcp);
			break;
	}
    return TINZ_OK;
}

