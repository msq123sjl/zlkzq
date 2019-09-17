#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/types.h>
#include<sys/stat.h>

#include "tinz_common_helper.h"
#include "tinz_pub_message.h"
#include "tinz_base_def.h"

//创建消息队列
//return value =-1 消息队列不成功；＝1消息队列成功
int prepareMsg(char* ftokpath,char* ftokname,int ftokid,struct _msg* msg){	
	key_t	msg_key;
	FILE*	fd=0;
	char 	tempchar[128];
	
	fd=fopen(ftokname,"rb+");
	if(fd<=0){
		if(access(ftokpath,0)){
			mkdir(ftokpath,S_IRWXU);
		}
		fd=fopen(ftokname,"wb+");
		if(fd<=0){
			sprintf(tempchar,"ftok error %s:",ftokname);
	        perror(tempchar);
			return TINZ_ERROR;
		}
	}
	fclose(fd);

	/*获取键值*/
	msg_key = ftok(ftokname,ftokid);
	if(msg_key == -1) {
   		DEBUG_PRINT_ERR(5,"ftok error %s:",ftokname);
		return TINZ_ERROR;
	}
	
	/*创建消息队列*/
	msg->msgid = msgget(msg_key,0666 | IPC_CREAT);
	if(msg->msgid==-1)
	{
		DEBUG_PRINT_ERR(5,"msgget failed with %s:",ftokname);
		return TINZ_ERROR;
    }
	return TINZ_OK;
}

int MsgSend(struct _msg* msg,long int mtype,char *data,int len){
	/*
		IPC_NOWAIT 不阻塞
		0		   阻塞
	*/
	uint8_t	CNT;
	int state = -1;
    msg->msgbuf.mtype = mtype;
    memcpy(msg->msgbuf.data,data,len);
	for(CNT=0;CNT<MSG_RTYCNT;CNT++){
		if(msgsnd(msg->msgid,(void *)&msg->msgbuf,sizeof(msg->msgbuf.data),IPC_NOWAIT)==0)
		{
			state = 0;
		    break;
		}
		sleep(1);
	}
	return state;
}

int MsgRcv(struct _msg* msg, long int mtype){
	/*读取消息*/
	memset(&msg->msgbuf,0,sizeof(struct _msgbuf));
	if(msgrcv(msg->msgid,(void *)&msg->msgbuf,sizeof(msg->msgbuf.data),mtype,MSG_NOERROR|IPC_NOWAIT)==0)
	{
		DEBUG_PRINT_ERR(5,"msgrcv[%d] failed\n",msg->msgid);
        return TINZ_ERROR;
	}
    return TINZ_OK;
}

void rmMsg(struct _msg* msg){
	msgctl(msg->msgid,IPC_RMID,NULL);
	msg->msgid = -1;
}

struct _msg* InterfaceMessageInit(struct _msg *msg){
    //DEBUG_PRINT_INFO(gPrintLevel, "pmsg_interface start\n");
    msg = (struct _msg*)malloc(sizeof(struct _msg));
    memset(msg,0,sizeof(struct _msg));
    if(TINZ_ERROR == prepareMsg(MSG_PATH_MSG,MSG_NAME_INTERFACE_TO_SQLITE, MSG_ID_INTERFACE_TO_SQLITE_ID, msg)){
        exit(0);
    }
    return msg;
}

