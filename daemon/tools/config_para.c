/*
 * config_para.c
 *
 *  Created on: 2019年9月5日
 *      Author: msq
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/klog.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_message.h"
#include "tinz_common_helper.h"
#include "config_file.h"


pstPara 		pgPara;

int gPrintLevel = 5;


void para_datfile_to_strigfile(pstPara para){
    
    int ret,len,iLoop;
	FILE*  fd=0;
    char *pbuf = malloc(8192);
	memset(pbuf,0,8192);

    fd=fopen(FS_NAME_PARA_CONF,"wb");
	if(fd<=0){
		DEBUG_PRINT_ERR(5,"open fs_para.conf file failure.\n");
		return;
	}
    /*GeneralPara*/
    len = snprintf(pbuf,8192,"[GeneralPara]\nMN=%s\nPW=%s\nRtdInterval=%d\nOverTime=%d\nReCount=%d\nAlarmTime=%d\nStType=%d\nRespondOpen=%d\n\n",\
        para->GeneralPara.MN,\
        para->GeneralPara.PW,\
        para->GeneralPara.RtdInterval,\
        para->GeneralPara.OverTime,\
        para->GeneralPara.ReCount,\
        para->GeneralPara.AlarmTime,\
        para->GeneralPara.StType,\
        para->GeneralPara.RespondOpen);
    if(len > 8192){
        DEBUG_PRINT_ERR(5,"[GeneralPara] len[%d] too long.\n",len);
		return;
    }
    fseek(fd,0,SEEK_SET);
	ret=fwrite(pbuf,sizeof(char),len,fd);
	fflush(fd);
    
    /*MeterPara*/
    for(iLoop=0;iLoop<METER_CNT;iLoop++){
        if(para->MeterPara[iLoop].isValid){
            len = snprintf(pbuf,8192,"[MeterPara%d]\nType=%d\nName=%s\nCode=%s\nUseChannelName=%s\nTypeName=%s\nProtocolName=%s\nUseChannel=%d\nUseChannelType=%d\nAddress=%d\nProtocol=%d\n\n",iLoop,\
                para->MeterPara[iLoop].Type,\
                para->MeterPara[iLoop].Name,\
                para->MeterPara[iLoop].Code,\
                para->MeterPara[iLoop].UseChannelName,\
                para->MeterPara[iLoop].TypeName,\
                para->MeterPara[iLoop].ProtocolName,\
                para->MeterPara[iLoop].UseChannel,\
                para->MeterPara[iLoop].UseChannelType,\
                para->MeterPara[iLoop].Address,\
                para->MeterPara[iLoop].Protocol);
            if(len > 8192){
                DEBUG_PRINT_ERR(5,"[MeterPara%d] len[%d] too long.\n",iLoop,len);
        		return;
            }
            fseek(fd,0,SEEK_END);
    	    ret=fwrite(pbuf,sizeof(char),len,fd);
    	    fflush(fd);
        }
    }

    /*SerialPara*/
    for(iLoop=0;iLoop<SERIAL_CNT;iLoop++){
    len = snprintf(pbuf,8192,"[SerialPara%d]\nisServerOpen=%d\nisRS485=%d\nDevName=%s\nBaudRate=%d\nDataBits=%d\nParity=%d\nStopBits=%d\nFlowCtrl=%d\nTimeOut=%d\nInterval=%d\n\n",iLoop+2,
        para->SerialPara[iLoop].isServerOpen,\
        para->SerialPara[iLoop].isRS485,\
        para->SerialPara[iLoop].DevName,\
        para->SerialPara[iLoop].BaudRate,\
        para->SerialPara[iLoop].DataBits,\
        para->SerialPara[iLoop].Parity,\
        para->SerialPara[iLoop].StopBits,\
        para->SerialPara[iLoop].FlowCtrl,\
        para->SerialPara[iLoop].TimeOut,\
        para->SerialPara[iLoop].Interval);
        if(len > 8192){
            DEBUG_PRINT_ERR(5,"[SerialPara%d] len[%d] too long.\n",iLoop,len);
        	return;
        }
        fseek(fd,0,SEEK_END);
    	ret=fwrite(pbuf,sizeof(char),len,fd);
    	fflush(fd);
    }
    /*IOPara*/
    len = snprintf(pbuf,8192,"[IOPara]\nIn_power=%d\nIn_drain_open=%d\nIn_drain_close=%d\nIn_reflux_open=%d\nOut_drain_open=%d\nOut_drain_close=%d\nOut_drain_common=%d\nOut_reflux_control=%d\n\n",\
        para->IOPara.In_power,\
        para->IOPara.In_drain_open,\
        para->IOPara.In_drain_close,\
        para->IOPara.In_reflux_open,\
        para->IOPara.Out_drain_open,\
        para->IOPara.Out_drain_close,\
        para->IOPara.Out_drain_common,\
        para->IOPara.Out_reflux_control);
    if(len > 8192){
        DEBUG_PRINT_ERR(5,"[IOPara] len[%d] too long.\n",len);
    	return;
    }
    fseek(fd,0,SEEK_END);
    ret=fwrite(pbuf,sizeof(char),len,fd);
    fflush(fd);
    
    /*SitePara*/
    for(iLoop=0;iLoop<SITE_CNT;iLoop++){
        len = snprintf(pbuf,8192,"[SitePara%d]\nServerOpen=%d\nServerPort=%d\nServerIp=%s\n\n",iLoop,\
            para->SitePara[iLoop].ServerOpen,\
            para->SitePara[iLoop].ServerPort,\
            para->SitePara[iLoop].ServerIp);
        if(len > 8192){
            DEBUG_PRINT_ERR(5,"[SitePara%d] len[%d] too long.\n",iLoop,len);
        	return;
        }
        fseek(fd,0,SEEK_END);
        ret=fwrite(pbuf,sizeof(char),len,fd);
        fflush(fd);
    }
    /*NetPara*/
    len = snprintf(pbuf,8192,"[NetPara]\nVPNOpen=%d\nVPNServerIp=%s\nVPNUserName=%s\nVPNIPIP=%s\n\n",\
        para->NetPara.VPNOpen,\
        para->NetPara.VPNServerIp,\
        para->NetPara.VPNUserName,\
        para->NetPara.VPNIPIP);
    if(len > 8192){
        DEBUG_PRINT_ERR(5,"[NetPara] len[%d] too long.\n",len);
    	return;
    }
    fseek(fd,0,SEEK_END);
    ret=fwrite(pbuf,sizeof(char),len,fd);
    fflush(fd);
    /*UserPara*/
    for(iLoop=0;iLoop<USER_CNT;iLoop++){
        len = snprintf(pbuf,8192,"[UserPara%d]\nUserType=%d\nUserPwd=%d\n\n",iLoop,\
            para->UserPara[iLoop].UserType,\
            para->UserPara[iLoop].UserPwd);
        if(len > 8192){
            DEBUG_PRINT_ERR(5,"[UserPara%d] len[%d] too long.\n",iLoop,len);
        	return;
        }
        fseek(fd,0,SEEK_END);
        ret=fwrite(pbuf,sizeof(char),len,fd);
        fflush(fd);
    }
    
	fclose(fd);
    free(pbuf);
}

void para_get_config(pstPara para){
    char buf[64];
    int res,val,iLoop;
    FILE*  fd=0;
    fd=fopen(FS_NAME_PARA_CONF,"rb");
	if(fd<=0){
		DEBUG_PRINT_ERR(5,"open fs_para.conf file failure.\n");
		return;
	}
    /*GeneralPara*/
    res = getconfigstring("GeneralPara","MN",para->GeneralPara.MN,sizeof(para->GeneralPara.MN) - 0,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"GeneralPara args MN need\n");return;}
    res = getconfigstring("GeneralPara","PW",(char*)para->GeneralPara.PW,sizeof(para->GeneralPara.PW) - 0,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"GeneralPara args PW need\n");return;}
    res=getconfigint("GeneralPara","RtdInterval",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"GeneralPara args RtdInterval need\n"); return;}para->GeneralPara.RtdInterval = val;
    res=getconfigint("GeneralPara","OverTime",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"GeneralPara args OverTime need\n"); return;}para->GeneralPara.OverTime = val;
    res=getconfigint("GeneralPara","ReCount",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"GeneralPara args ReCount need\n"); return;}para->GeneralPara.ReCount = val;
    res=getconfigint("GeneralPara","AlarmTime",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"GeneralPara args AlarmTime need\n"); return;}para->GeneralPara.AlarmTime = val;
    res=getconfigint("GeneralPara","StType",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"GeneralPara args StType need\n"); return;}para->GeneralPara.StType = val;
    res=getconfigint("GeneralPara","RespondOpen",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"GeneralPara args RespondOpen need\n"); return;}para->GeneralPara.RespondOpen = val;
    /*MeterPara*/
    for(iLoop=0;iLoop<METER_CNT;iLoop++){
        snprintf(buf,sizeof(buf),"MeterPara%d",iLoop);
        res=getconfigint(buf,"Type",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(0,"%s args Type need\n",buf); para->MeterPara[iLoop].isValid = 0;continue;}para->MeterPara[iLoop].Type = val;
        res=getconfigint(buf,"UseChannel",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args UseChannel need\n",buf); return;}para->MeterPara[iLoop].UseChannel = val;
        res=getconfigint(buf,"UseChannelType",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args UseChannelType need\n",buf); return;}para->MeterPara[iLoop].UseChannelType = val;
        res=getconfigint(buf,"Address",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args Address need\n",buf); return;}para->MeterPara[iLoop].Address = val;
        res=getconfigint(buf,"Protocol",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args Protocol need\n",buf); return;}para->MeterPara[iLoop].Protocol = val;   
        res = getconfigstring(buf,"Name",para->MeterPara[iLoop].Name,sizeof(para->MeterPara[iLoop].Name) - 0,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args Name need\n",buf);return;}
        res = getconfigstring(buf,"Code",para->MeterPara[iLoop].Code,sizeof(para->MeterPara[iLoop].Code) - 0,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args Code need\n",buf);return;}
        res = getconfigstring(buf,"UseChannelName",para->MeterPara[iLoop].UseChannelName,sizeof(para->MeterPara[iLoop].UseChannelName) - 0,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args UseChannelName need\n",buf);return;}
        res = getconfigstring(buf,"TypeName",para->MeterPara[iLoop].TypeName,sizeof(para->MeterPara[iLoop].TypeName) - 0,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args TypeName need\n",buf);return;}
        res = getconfigstring(buf,"ProtocolName",para->MeterPara[iLoop].ProtocolName,sizeof(para->MeterPara[iLoop].ProtocolName) - 0,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args ProtocolName need\n",buf);return;}
        para->MeterPara[iLoop].isValid = 1;
    }

    /*SerialPara*/
    for(iLoop=0;iLoop<SERIAL_CNT;iLoop++){
        snprintf(buf,sizeof(buf),"SerialPara%d",iLoop+2);
        res=getconfigint(buf,"isServerOpen",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args isServerOpen need\n",buf); return;}para->SerialPara[iLoop].isServerOpen = val;
        res=getconfigint(buf,"isRS485",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args isRS485 need\n",buf); return;}para->SerialPara[iLoop].isRS485 = val;
        res=getconfigint(buf,"BaudRate",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args BaudRate need\n",buf); return;}para->SerialPara[iLoop].BaudRate = val;
        res=getconfigint(buf,"DataBits",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args DataBits need\n",buf); return;}para->SerialPara[iLoop].DataBits = val;
        res=getconfigint(buf,"Parity",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args Parity need\n",buf); return;}para->SerialPara[iLoop].Parity = val;
        res=getconfigint(buf,"StopBits",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args StopBits need\n",buf); return;}para->SerialPara[iLoop].StopBits = val;
        res=getconfigint(buf,"FlowCtrl",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args FlowCtrl need\n",buf); return;}para->SerialPara[iLoop].FlowCtrl = val;
        res=getconfigint(buf,"TimeOut",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args TimeOut need\n",buf); return;}para->SerialPara[iLoop].TimeOut = val;
        res=getconfigint(buf,"Interval",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args Interval need\n",buf); return;}para->SerialPara[iLoop].Interval = val;
        res = getconfigstring(buf,"DevName",(char*)para->SerialPara[iLoop].DevName,sizeof(para->SerialPara[iLoop].DevName) - 0,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args DevName need\n",buf);return;}
    }

    /*IOPara*/
    res=getconfigint("IOPara","In_power",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"IOPara args In_power need\n"); return;}para->IOPara.In_power = val;
    res=getconfigint("IOPara","In_drain_open",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"IOPara args In_drain_open need\n"); return;}para->IOPara.In_drain_open = val;
    res=getconfigint("IOPara","In_drain_close",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"IOPara args In_drain_close need\n"); return;}para->IOPara.In_drain_close = val;
    res=getconfigint("IOPara","In_reflux_open",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"IOPara args In_reflux_open need\n"); return;}para->IOPara.In_reflux_open = val;
    
    res=getconfigint("IOPara","Out_drain_open",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"IOPara args Out_drain_open need\n"); return;}para->IOPara.Out_drain_open = val;
    res=getconfigint("IOPara","Out_drain_close",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"IOPara args Out_drain_close need\n"); return;}para->IOPara.Out_drain_close = val;
    res=getconfigint("IOPara","Out_drain_common",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"IOPara args Out_drain_common need\n"); return;}para->IOPara.Out_drain_common = val;
    res=getconfigint("IOPara","Out_reflux_control",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"IOPara args Out_reflux_control need\n"); return;}para->IOPara.Out_reflux_control = val;

     /*SitePara*/
    for(iLoop=0;iLoop<SITE_CNT;iLoop++){
        snprintf(buf,sizeof(buf),"SitePara%d",iLoop);
        res=getconfigint(buf,"ServerOpen",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args ServerOpen need\n",buf); return;}para->SitePara[iLoop].ServerOpen = val;
        res=getconfigint(buf,"ServerPort",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args ServerPort need\n",buf); return;}para->SitePara[iLoop].ServerPort = val;
        res = getconfigstring(buf,"ServerIp",para->SitePara[iLoop].ServerIp,sizeof(para->SitePara[iLoop].ServerIp) - 0,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args ServerIp need\n",buf);return;}
        para->SitePara[iLoop].SiteNum=iLoop;
    }

    /*NetPara*/
    res=getconfigint("NetPara","VPNOpen",&val,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args VPNOpen need\n"); return;}para->NetPara.VPNOpen = val; 
    res = getconfigstring("NetPara","VPNServerIp",para->NetPara.VPNServerIp,sizeof(para->NetPara.VPNServerIp) - 0,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args VPNServerIp need\n");return;} 
    res = getconfigstring("NetPara","VPNUserName",para->NetPara.VPNUserName,sizeof(para->NetPara.VPNUserName) - 0,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args VPNUserName need\n");return;}
    res = getconfigstring("NetPara","VPNIPIP",para->NetPara.VPNIPIP,sizeof(para->NetPara.VPNIPIP) - 0,FS_NAME_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args VPNIPIP need\n");return;}

    /*UserPara*/
    for(iLoop=0;iLoop<USER_CNT;iLoop++){
        snprintf(buf,sizeof(buf),"UserPara%d",iLoop);
        res=getconfigint(buf,"UserType",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args UserType need\n",buf); return;}para->UserPara[iLoop].UserType = val;
        res=getconfigint(buf,"UserPwd",&val,FS_NAME_PARA_CONF);
        if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"%s args UserPwd need\n",buf); return;}para->UserPara[iLoop].UserPwd = val;
    }
    syncParaShm();
}

int main(int argc, char *argv[])
{
	
    pgPara = (pstPara)getParaShm();
    if('1' == *argv[1]){
        para_datfile_to_strigfile(pgPara);
    }else if('2' == *argv[1]){
        para_get_config(pgPara);
    }
	return 0;
}







