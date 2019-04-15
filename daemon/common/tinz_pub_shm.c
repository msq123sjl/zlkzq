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

struct SHM_DESC shm_para={-1,0};
struct SHM_DESC shm_valve_para={-1,0};
struct SHM_DESC shm_pollutant_data={-1,0};
struct SHM_DESC shm_pollutant_para={-1,0};



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
    pstPara para_tmp = (pstPara)malloc(sizeof(stPara));
    memcpy(para_tmp,para,sizeof(stPara));
    for(iLoop = 0; iLoop < SERIAL_CNT; iLoop++){
        para_tmp->SerialPara[iLoop].Devfd=-1;
    }
    for(iLoop = 0; iLoop < SITE_CNT; iLoop++){
        para_tmp->SitePara[iLoop].isConnected = 0;
    }
    
	if(para==0)return;	
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
	/*基本设置*/
	snprintf((char*)para->GeneralPara.MN,MN_LEN,"%s","83028583206272");
    snprintf((char*)para->GeneralPara.PW,PW_LEN,"%s","123456");
	//para->GeneralPara.RtdInterval 			= 60;
	//para->GeneralPara.MinInterval 			= 5;
	//para->GeneralPara.CatchmentTime 		= 5;
	//para->GeneralPara.COD_CollectInterval 	= 3;
	para->GeneralPara.OverTime				= 60;
	para->GeneralPara.ReCount				= 3;
	para->GeneralPara.AlarmTime				= 30;
	para->GeneralPara.StType				= 32;
	para->GeneralPara.RespondOpen			= 0;
	/*因子设置*/
		//默认值是0或空
	/*串口设置*/
	for(iLoop=0; iLoop < SERIAL_CNT; iLoop++){
		snprintf((char*)para->SerialPara[iLoop].DevName, UART_DEVNAME_LEN, "/dev/ttyS%d",iLoop+1);
		para->SerialPara[iLoop].isServerOpen    = 0;
		para->SerialPara[iLoop].isRS485			= 0;
		para->SerialPara[iLoop].BaudRate		= 9600;
		para->SerialPara[iLoop].DataBits		= 8;
		para->SerialPara[iLoop].Parity			= 0;
		para->SerialPara[iLoop].StopBits		= 0;
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
    //para->IOPara.Out_catchment_open     = 8;
    //para->IOPara.Out_catchment_close    = 9;
    //para->IOPara.Out_reflux_control     = 16;
    //para->IOPara.In_drain_open          = 10;
    //para->IOPara.In_drain_close         = 11;
    //para->IOPara.In_catchment_open      = 12;
    //para->IOPara.In_catchment_close     = 13;
    //para->IOPara.In_reflux_open         = 14;
    //para->IOPara.In_reflux_close        = 15;
    para->IOPara.In_power               = 18;
	/*站点设置*/
    
    for(iLoop = 0; iLoop < SITE_CNT; iLoop++){
        para->SitePara[iLoop].ServerOpen   = 0;
        para->SitePara[iLoop].isConnected  = 0;
        para->SitePara[iLoop].SiteNum  = iLoop;
        para->SitePara[iLoop].ServerPort   = 8810;
		snprintf((char*)para->SitePara[iLoop].ServerIp,sizeof(para->SitePara[iLoop].ServerIp),"%s","192.168.1.131");
    }
	/*用户设置*/
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
    para->per_last = 0;
    para->channel = 0;
    para->OutMode = 0;
    para->OutValueAdjust[0] = 0;
    para->OutValueAdjust[1] = 50;
    para->OutValueAdjust[2] = 100;
    para->InValueAdjust[0] = 0;
    para->InValueAdjust[1] = 50;
    para->InValueAdjust[2] = 100;
}

char * getPollutantDataShm(){
	prepareShm(SHM_PATH_POLLUTANT_DATA,SHM_NAME_POLLUTANT_DATA,SHM_DATA_POLLUTANT_ID,0,sizeof(stPollutantData),&shm_pollutant_data);

	return shm_pollutant_data.shm_mem;
}

void rmPollutantDataShm(){
	shmctl(shm_pollutant_data.shm_id,IPC_RMID,NULL);
}

char * getPollutantParaShm(){
	if(prepareShm(SHM_PATH_POLLUTANT_PARA,SHM_NAME_POLLUTANT_PARA,SHM_PARA_POLLUTANT_ID,FS_NAME_POLLUTANT_PARA,sizeof(stPollutantPara),&shm_pollutant_para)==0){
		DEBUG_PRINT_INFO(5, "initPollutantParaShm[%s]\n",SHM_NAME_POLLUTANT_PARA);
		initPollutantParaShm();
		syncPollutantParaShm();
	}
	return shm_pollutant_para.shm_mem;
}

void rmPollutantParaShm(){
	shmctl(shm_pollutant_para.shm_id,IPC_RMID,NULL);
}

void syncPollutantParaShm(){
	int ret;
	FILE*  fd=0;
	pstPollutantPara para=(pstPollutantPara)shm_pollutant_para.shm_mem;
    pstPollutantPara para_tmp = (pstPollutantPara)malloc(sizeof(stPollutantPara));
    memcpy(para_tmp,para,sizeof(stPollutantPara));
    
	if(para==0)return;	
	fd=fopen(FS_NAME_POLLUTANT_PARA,"rb+");
	if(fd<=0){
		if(access(FS_PATH_POLLUTANT_PARA,0)){
			mkdir(FS_PATH_POLLUTANT_PARA,S_IRWXU);
		}
		fd=fopen(FS_NAME_POLLUTANT_PARA,"wb+");
		if(fd<=0){
			DEBUG_PRINT_ERR(5,"open fs_pollutant_para.dat file failure.\n");
			return;
		}
	}
	fseek(fd,0,SEEK_SET);
	ret=fwrite(para_tmp,sizeof(stPollutantPara),1,fd);
	fflush(fd);
	fclose(fd);
    free(para_tmp);
	DEBUG_PRINT_INFO(5,"save pollutant para %s.\n",ret==1?"succeed":"failure");
}

void initPollutantParaShm(){
    int iLoop;
	pstPollutantPara para=(pstPollutantPara)shm_pollutant_para.shm_mem;
    for(iLoop = 0;iLoop < POLLUTANT_CNT; iLoop++){
        para->Row[iLoop].MonAll = 1000;
        para->Row[iLoop].QutAll = 3000;
        para->Row[iLoop].YeaAll = 12000;
    }
}

