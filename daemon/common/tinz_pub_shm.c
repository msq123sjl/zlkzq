#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/stat.h>
#include"tinz_pub_shm.h"
#include"tinz_base_def.h"
#include"tinz_base_data.h"
#include"tinz_common_helper.h"
//#include "modbus_Master.h"


struct SHM_DESC shm_para={-1,0};
struct SHM_DESC shm_valve_para={-1,0};
struct SHM_DESC shm_pollutant_data={-1,0};
struct SHM_DESC shm_pollutant_para={-1,0};
struct SHM_DESC shm_data={-1,0};
struct SHM_DESC shm_history_data={-1,0};
struct SHM_DESC shm_calibration_para={-1,0};
struct SHM_DESC shm_net_para={-1,0};
//创建参数共享内存段
//return value =-1 共享内存不成功；＝0共享内存成功，文件读写不成功，＝1内存和文件都成功
int prepareShm(char* ftokpath,char* ftokname,int ftokid,char* fsname,int shm_len,struct SHM_DESC* shm){	
	char bfirst=0;
	key_t   shm_key;       
	FILE*  fd=0;
	char tempchar[128];
	int minLen=0;
	if(shm->shm_mem)return 1;
		
	shm->shm_id=-1;
	shm->shm_mem=0;

	fd=fopen(ftokname,"rb+");
	if(fd<=0){
		if(access(ftokpath,0)){
			mkdir(ftokpath,S_IRWXU);
		}
		fd=fopen(ftokname,"wb+");
		if(fd<=0){
			sprintf(tempchar,"ftok error %s:",ftokname);
	        perror(tempchar);
			return -1;
		}
	}
	fclose(fd);
	fd=0;
	shm_key=ftok(ftokname,ftokid);
	if(shm_key == -1) {
		 sprintf(tempchar,"ftok error %s:",ftokname);
		 perror(tempchar);
   		 //perror("ftok error :");
		 return -1;
	}
	shm->shm_id= shmget(shm_key,shm_len,0);
	if(shm->shm_id==-1){
		shm->shm_id= shmget(shm_key,shm_len,IPC_CREAT|IPC_EXCL|0666);
		bfirst=1;
	}	

	if(shm->shm_id==-1){
		perror("shmget error:");
		return -1;
	}
	shm->shm_mem=(char*)shmat(shm->shm_id,NULL,0);
	if(shm->shm_mem==(char*)-1){
		perror("shmat error:");
		if(bfirst)	shmctl(shm->shm_id,IPC_RMID,NULL);
		shm->shm_mem=0;
		return -1;
	}
	if(bfirst&&fsname>0){
		fd=fopen(fsname,"rb");
		if(fd>0){
			fseek(fd,0,SEEK_END);
			minLen=ftell(fd);
			if(minLen<=0){
				fclose(fd);
				return 0;
			}
			if(minLen>shm_len)minLen=shm_len;
			fseek(fd,0,SEEK_SET);
			if(fread(shm->shm_mem,minLen,1,fd)!=1){
				fclose(fd);
				return 0;
			}
			fclose(fd);
		}
		else {
			return 0;
		}
	}
	return 1;
}

char * getNetParaShm(){
	if(prepareShm(SHM_PATH_NET_PARA,SHM_NAME_NET_PARA,SHM_PARA_NET_ID,FS_NAME_NET_PARA,sizeof(stNetPara),&shm_net_para)==0){
		DEBUG_PRINT_INFO(5, "initNetParaShm[%s]\n",SHM_NAME_NET_PARA);
		initNetParaShm();
		syncNetParaShm();
	}
	return shm_net_para.shm_mem;
}

void rmNetParaShm(){
	shmctl(shm_net_para.shm_id,IPC_RMID,NULL);
}

void syncNetParaShm(){
	int ret;
	FILE*  fd=0;
	pstNetPara para=(pstNetPara)shm_net_para.shm_mem;
    if(para==0)return;	
    pstNetPara para_tmp = (pstNetPara)malloc(sizeof(stNetPara));
    memcpy(para_tmp,para,sizeof(stNetPara));

	fd=fopen(FS_NAME_NET_PARA,"rb+");
	if(fd<=0){
		if(access(FS_PATH_NET_PARA,0)){
			mkdir(FS_PATH_NET_PARA,S_IRWXU);
		}
		fd=fopen(FS_NAME_NET_PARA,"wb+");
		if(fd<=0){
			DEBUG_PRINT_ERR(5,"open fs_net_para.dat file failure.\n");
			return;
		}
	}
	fseek(fd,0,SEEK_SET);
	ret=fwrite(para_tmp,sizeof(stNetPara),1,fd);
	fflush(fd);
	fclose(fd);
    free(para_tmp);
	DEBUG_PRINT_INFO(5,"save net para %s.\n",ret==1?"succeed":"failure");
}

void initNetParaShm(){
	pstNetPara para=(pstNetPara)shm_net_para.shm_mem;
	memset(para,0,sizeof(stNetPara));
    para->LTEOpen = 0;
    /*VPN设置*/
    para->VPNOpen = 0;
    snprintf((char*)para->VPNServerIp,sizeof(para->VPNServerIp),"%s","36.153.128.244");
    snprintf((char*)para->VPNIPIP,sizeof(para->VPNIPIP),"%s","172.16.5.1");
    snprintf((char*)para->VPNUserName,sizeof(para->VPNUserName),"%s","sc");
    
}

char * getParaShm(){
	if(prepareShm(SHM_PATH_PARA,SHM_NAME_PARA,SHM_PARA_ID,FS_NAME_PARA,sizeof(stPara),&shm_para)==0){
		DEBUG_PRINT_INFO(5, "initParaShm[%s]\n",SHM_NAME_PARA);
		initParaShm();
		syncParaShm();
	}
	return shm_para.shm_mem;
}

void rmParaShm(){
	shmctl(shm_para.shm_id,IPC_RMID,NULL);
}

void syncParaShm(){
	int ret,iLoop;
	FILE*  fd=0;
	pstPara para=(pstPara)shm_para.shm_mem;
    if(para==0)return;	
    pstPara para_tmp = (pstPara)malloc(sizeof(stPara));
    memcpy(para_tmp,para,sizeof(stPara));
    for(iLoop = 0; iLoop < SERIAL_CNT; iLoop++){
        para_tmp->SerialPara[iLoop].Devfd=-1;
    }
    for(iLoop = 0; iLoop < SITE_CNT; iLoop++){
        para_tmp->SitePara[iLoop].isConnected = 0;
    }
    para_tmp->Mode = 0;

	fd=fopen(FS_NAME_PARA,"rb+");
	if(fd<=0){
		if(access(FS_PATH_PARA,0)){
			mkdir(FS_PATH_PARA,S_IRWXU);
		}
		fd=fopen(FS_NAME_PARA,"wb+");
		if(fd<=0){
			DEBUG_PRINT_ERR(5,"open fs_para.dat file failure.\n");
			return;
		}
	}
	fseek(fd,0,SEEK_SET);
	ret=fwrite(para_tmp,sizeof(stPara),1,fd);
	fflush(fd);
	fclose(fd);
    free(para_tmp);
	DEBUG_PRINT_INFO(5,"save para %s.\n",ret==1?"succeed":"failure");
}

void initParaShm(){
	int iLoop=0;
	pstPara para=(pstPara)shm_para.shm_mem;
	memset(para,0,sizeof(stPara));	
    /*模式*/
    para->Mode  = 0;
	/*基本设置*/
	snprintf((char*)para->GeneralPara.MN,MN_LEN,"%s","YK00");
    snprintf((char*)para->GeneralPara.PW,PW_LEN,"%s","123456");
	para->GeneralPara.RtdInterval 			= 600;
	//para->GeneralPara.MinInterval 			= 5;
	//para->GeneralPara.CatchmentTime 		= 5;
	//para->GeneralPara.COD_CollectInterval 	= 3;
	para->GeneralPara.OverTime				= 6;
	para->GeneralPara.ReCount				= 3;
	para->GeneralPara.AlarmTime				= 30;
	para->GeneralPara.StType				= 32;
	para->GeneralPara.RespondOpen			= 1;
	/*因子设置*/
    for(iLoop = 0;iLoop < POLLUTANT_CNT; iLoop++){
        para->PollutantPara[iLoop].isValid = 1;
        para->PollutantPara[iLoop].Row.MonAll = 1000;
        para->PollutantPara[iLoop].Row.QutAll = 3000;
        para->PollutantPara[iLoop].Row.YeaAll = 12000;
    }
		//默认值是0或空
	/*串口设置*/
	for(iLoop=0; iLoop < SERIAL_CNT; iLoop++){
		snprintf((char*)para->SerialPara[iLoop].DevName, UART_DEVNAME_LEN, "/dev/ttyS%d",iLoop+1);
		para->SerialPara[iLoop].isServerOpen    = 0;
		para->SerialPara[iLoop].isRS485			= iLoop > 1 ? 1:0;
		para->SerialPara[iLoop].BaudRate		= 9600;
		para->SerialPara[iLoop].DataBits		= 8;
		para->SerialPara[iLoop].Parity			= 0;
		para->SerialPara[iLoop].StopBits		= 1;
		para->SerialPara[iLoop].FlowCtrl		= 0;
		para->SerialPara[iLoop].TimeOut			= 1000;
		para->SerialPara[iLoop].Interval		= 1000;
		para->SerialPara[iLoop].Devfd			= -1;
	}
	/*开关量设置 
	    Input:  10~15,18~23 
	    Output: 6~9,16,17,24,25
	*/
	para->IOPara.Out_drain_open         = 6;
    para->IOPara.Out_drain_close        = 7;
    para->IOPara.Out_drain_common        = 8;
    //para->IOPara.Out_catchment_open     = 8;
    //para->IOPara.Out_catchment_close    = 9;
    para->IOPara.Out_reflux_control     = 9;
    para->IOPara.In_drain_open          = 10;
    para->IOPara.In_drain_close         = 11;
    //para->IOPara.In_catchment_open      = 12;
    //para->IOPara.In_catchment_close     = 13;
    para->IOPara.In_reflux_open         = 12;
    //para->IOPara.In_reflux_close        = 15;
    para->IOPara.In_power               = 13;
	/*站点设置*/
    
    for(iLoop = 0; iLoop < SITE_CNT; iLoop++){
        para->SitePara[iLoop].ServerOpen   = 0;
        para->SitePara[iLoop].isConnected  = 0;
        para->SitePara[iLoop].SiteNum  = iLoop;
        para->SitePara[iLoop].ServerPort   = 28875;
		snprintf((char*)para->SitePara[iLoop].ServerIp,sizeof(para->SitePara[iLoop].ServerIp),"%s","221.131.155.106");
    }
	/*用户设置*/
    para->UserPara[0].UserType = 1;
    para->UserPara[0].UserPwd = 1;
    para->UserPara[1].UserType = 2;
    para->UserPara[1].UserPwd = 2;
    para->UserPara[2].UserType = 3;
    para->UserPara[2].UserPwd = 123456;
    
}

char * getValveParaShm(){
	if(prepareShm(SHM_PATH_VALVE_PARA,SHM_NAME_VALVE_PARA,SHM_PARA_VALVE_ID,FS_NAME_VALVE_PARA,sizeof(stValveControl),&shm_valve_para)==0){
		DEBUG_PRINT_INFO(5, "initValveParaShm[%s]\n",SHM_NAME_VALVE_PARA);
		initValveParaShm();
		syncValveParaShm();
	}
	return shm_valve_para.shm_mem;
}

void rmValveParaShm(){
	shmctl(shm_valve_para.shm_id,IPC_RMID,NULL);
}

void syncValveParaShm(){
	int ret;
	FILE*  fd=0;
	pstValveControl para=(pstValveControl)shm_valve_para.shm_mem;
    pstValveControl para_tmp = (pstValveControl)malloc(sizeof(stValveControl));
    memcpy(para_tmp,para,sizeof(stValveControl));
    
	if(para==0)return;	
	fd=fopen(FS_NAME_VALVE_PARA,"rb+");
	if(fd<=0){
		if(access(FS_PATH_VALVE_PARA,0)){
			mkdir(FS_PATH_VALVE_PARA,S_IRWXU);
		}
		fd=fopen(FS_NAME_VALVE_PARA,"wb+");
		if(fd<=0){
			DEBUG_PRINT_ERR(5,"open fs_valve_para.dat file failure.\n");
			return;
		}
	}
	fseek(fd,0,SEEK_SET);
	ret=fwrite(para_tmp,sizeof(stValveControl),1,fd);
	fflush(fd);
	fclose(fd);
    free(para_tmp);
	DEBUG_PRINT_INFO(5,"save valve para %s.\n",ret==1?"succeed":"failure");
}

void initValveParaShm(){
	pstValveControl para=(pstValveControl)shm_valve_para.shm_mem;
    para->per = 0;
    para->per_measure = 0;
    para->per_last = 0;
    para->channel = 0;
    para->OutMode = 1;  //默认开关量
    para->OutValueAdjust[0] = 0;
    para->OutValueAdjust[1] = 50;
    para->OutValueAdjust[2] = 100;
    para->InValueAdjust[0] = 0;
    para->InValueAdjust[1] = 50;
    para->InValueAdjust[2] = 100;
}

char * getDataShm(){
	if(prepareShm(SHM_PATH_DATA,SHM_NAME_DATA,SHM_DATA_ID,FS_NAME_DATA,sizeof(stData),&shm_data)==0){
        initDataShm();
        syncDataShm();
    }

	return shm_data.shm_mem;
}

void syncDataShm(){
	int ret;
	FILE*  fd=0;
	pstData pdata=(pstData)shm_data.shm_mem;
    pstData pdata_tmp = (pstData)malloc(sizeof(stData));
    memcpy(pdata_tmp,pdata,sizeof(stData));
    
	if(pdata==0)return;	
	fd=fopen(FS_NAME_DATA,"rb+");
	if(fd<=0){
		if(access(FS_PATH_DATA,0)){
			mkdir(FS_PATH_DATA,S_IRWXU);
		}
		fd=fopen(FS_NAME_DATA,"wb+");
		if(fd<=0){
			DEBUG_PRINT_ERR(5,"open fs_data.dat file failure.\n");
			return;
		}
	}
	fseek(fd,0,SEEK_SET);
	ret=fwrite(pdata_tmp,sizeof(stData),1,fd);
	fflush(fd);
	fclose(fd);
    free(pdata_tmp);
	DEBUG_PRINT_INFO(5,"save data %s.\n",ret==1?"succeed":"failure");
}

void initDataShm(){
    pstData pdata = (pstData)shm_data.shm_mem;
    DEBUG_PRINT_INFO(5,"initDataShm\n");
    memset(pdata,0,sizeof(stData));    
}

void rmDataShm(){
	shmctl(shm_data.shm_id,IPC_RMID,NULL);
}

char * getHistoryDataShm(){
    if(prepareShm(SHM_PATH_HISTORY_DATA,SHM_NAME_HISTORY_DATA,SHM_DATA_HISTORY_ID,0,sizeof(stHistoryData),&shm_history_data)==0){
        initHistoryDataShm();
    }
	return shm_history_data.shm_mem;
}

void rmHistoryDataShm(){
	shmctl(shm_history_data.shm_id,IPC_RMID,NULL);
}

void initHistoryDataShm(){
    memset(shm_history_data.shm_mem,0,sizeof(stHistoryData));
}

char * getCalibrationParaShm(){
	if(prepareShm(SHM_PATH_CALIBRATION_PARA,SHM_NAME_CALIBRATION_PARA,SHM_PARA_CALIBRATION_ID,FS_NAME_CALIBRATION_PARA,sizeof(stCalibrationPara),&shm_calibration_para)==0){
		DEBUG_PRINT_INFO(5, "initCalibrationParaShm[%s]\n",SHM_NAME_CALIBRATION_PARA);
		initCalibrationParaShm();
		syncCalibrationParaShm();
	}
	return shm_calibration_para.shm_mem;
}

void rmCalibrationParaShm(){
	shmctl(shm_calibration_para.shm_id,IPC_RMID,NULL);
}

void syncCalibrationParaShm(){
	int ret;
	FILE*  fd=0;
	pstCalibrationPara para=(pstCalibrationPara)shm_calibration_para.shm_mem;
    pstCalibrationPara para_tmp = (pstCalibrationPara)malloc(sizeof(stCalibrationPara));
    memcpy(para_tmp,para,sizeof(stCalibrationPara));
    
	if(para==0)return;	
	fd=fopen(FS_NAME_CALIBRATION_PARA,"rb+");
	if(fd<=0){
		if(access(FS_PATH_CALIBRATION_PARA,0)){
			mkdir(FS_PATH_CALIBRATION_PARA,S_IRWXU);
		}
		fd=fopen(FS_NAME_CALIBRATION_PARA,"wb+");
		if(fd<=0){
			DEBUG_PRINT_ERR(5,"open fs_calibration_para.dat file failure.\n");
			return;
		}
	}
	fseek(fd,0,SEEK_SET);
	ret=fwrite(para_tmp,sizeof(stCalibrationPara),1,fd);
	fflush(fd);
	fclose(fd);
    free(para_tmp);
	DEBUG_PRINT_INFO(5,"save calibration para %s.\n",ret==1?"succeed":"failure");
}

void initCalibrationParaShm(){
	pstCalibrationPara para=(pstCalibrationPara)shm_calibration_para.shm_mem;
    int iLoop;
    for(iLoop=0;iLoop<AD_CNT;iLoop++){
        para->AdAdjustValue[iLoop][0] = 0;
        para->AdAdjustValue[iLoop][1] = 1024;
        para->AdAdjustValue[iLoop][2] = 2048;
    }
    for(iLoop=0;iLoop<DA_CNT;iLoop++){
        para->DaAdjustValue[iLoop][0] = 0;
        para->DaAdjustValue[iLoop][1] = 1024;
        para->DaAdjustValue[iLoop][2] = 2048;
    }
}

