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

#include "up_main.h"
#include "tcp_client.h"
#include "tinz_common_helper.h"
#include "protocl_gb212.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_message.h"
#include "tinz_pub_shm.h"

extern int gPrintLevel;
extern UpMain* pserver;
extern pstMessage pgmsgbuff;
extern pstData pgData;
extern pstPara pgPara;
extern pstValveControl pgValveControl;
extern struct _msg *pmsg_upproc[SITE_SEND_CNT];
extern struct _msg *pmsg_dataproc_to_upproc;
extern uint8_t state_per;

static int tcplink_connect(TcpClientDev *pdev);



/*************平台通信**********************/
static int recv_message_from_server(TcpClientDev* pdev, char* pRecvBuf){
	int iRecvLen;
	iRecvLen = recv(pdev->dev_fd, pRecvBuf, RECVBUF_LEN, 0);

	return iRecvLen;
}

static int send_message_to_server(void *arg, char* pSendBuf){

	int iSendLen = 0;
    int iLoop;
    TcpClientDev *pdev = (TcpClientDev *)arg;
    //发送前检测TCP链接，若未连接 等待3s
    for(iLoop=0;iLoop<3;iLoop++){
        if(1 == pdev->tcplink->isConnected){
            break;
        }
        sleep(1);
    }
    if(1 == pdev->tcplink->isConnected){
	    iSendLen = send(pdev->dev_fd, pSendBuf, strlen(pSendBuf), 0);
        usleep(500000);
    }
	if(iSendLen <= 0){
		DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] retry [port:%d dstip:%s]\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp);
        return TINZ_ERROR;
	}
    DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] write Date Len to server[port:%d dstip:%s] len(%d) : %s\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp,iSendLen, pSendBuf);
    return iSendLen;

}

static void message_buf_clear_func(){
    int iLoop,jLoop,res;
    pstMessageData pmsgData = NULL;
    for(iLoop = 0;iLoop < MESSAGECNT;iLoop++){
        /*查找已发送的报文*/
        //DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] clear pgmsgbuff->Data[%d].IsUse=%d\n",iLoop,pgmsgbuff->Data[iLoop].IsUse);
        if(MSGBUF_IS_WAITING == pgmsgbuff->Data[iLoop].IsUse){
            res = 0;
            pmsgData = &pgmsgbuff->Data[iLoop];
            pmsgData->waittime += 1;
            //DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] clear1 pmsgData->waittime=%d OverTime=%d flag=%d\n",pmsgData->waittime,pgPara->GeneralPara.OverTime,pmsgData->flag);

            if(1 == (pmsgData->flag & 0x01)){//无需应答，直接清空  

                for(jLoop =0;jLoop<SITE_CNT;jLoop++){
                    /*判断服务端是否打开，并且保持连接*/
                    //if(1 == pserver->channes[jLoop].tcplink->ServerOpen && 1 == pserver->channes[jLoop].tcplink->isConnected){
                    if(1 == pserver->channes[jLoop].tcplink->ServerOpen){
                        /*判断报文是否已有应答*/
                        //DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] SITE_CNT[%d] IsRespond[%d]\n",jLoop,pmsgData->IsRespond[jLoop]);
                        if(0 == pmsgData->IsRespond[jLoop]){ 
                            /*判断报文是否发送次数超限*/
                            if(pmsgData->SendTimes[jLoop] < pgPara->GeneralPara.ReCount){
                                if(pmsgData->waittime >= pgPara->GeneralPara.OverTime){//超时未响应，重新发送报文
                                    pgmsgbuff->Data[iLoop].IsUse = MSGBUF_IS_SENDING;
                                    pmsgData->waittime = 0;
                                }
                                res = 1;
                                break;
                            }
                        }
                    }                
                }
            }
            if(0 == res){
                //pmsg_upproc[SITE_CNT]->msgbuf.mtype = MSG_SQLITE_SEND_TYTE;
                //memcpy(pmsg_upproc[SITE_CNT]->msgbuf.data,pmsgData,sizeof(stMessageData));
                //MsgSend(pmsg_upproc[SITE_CNT]);      
                #ifndef VALVE_AND_PUMP
                if(0 == pmsgData->issql){
                    MsgSend(pmsg_upproc[SITE_CNT],MSG_SQLITE_SEND_TYTE,(char*)pmsgData,(int)sizeof(stMessageData));
                }
                #endif
                memset(pmsgData,0,sizeof(stMessageData));   
                DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] clear2 pmsgData->waittime=%d IsUse=%d flag=%d issql=%d\n",pmsgData->waittime,pmsgData->IsUse,pmsgData->flag,pmsgData->issql);
            }
        }
    }    
}

/*每次发送多个报文*/
static void send_message_to_server_func(){
    char ServerOpenflag = 0;
    int iLoop,jLoop;
    pstMessageData pmsgData = NULL;
    for(iLoop = 0;iLoop < MESSAGECNT;iLoop++){
        /*查找需发送的报文*/
        if(MSGBUF_IS_SENDING == pgmsgbuff->Data[iLoop].IsUse){
            DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] send pgmsgbuff->Data[%d].IsUse=%d\n",iLoop,pgmsgbuff->Data[iLoop].IsUse);
            pmsgData = &pgmsgbuff->Data[iLoop];
            /*往服务端发送报文*/
            for(jLoop =0;jLoop<SITE_CNT;jLoop++){
                /*判断服务端是否打开，并且保持连接*/
                if(1 == pserver->channes[jLoop].tcplink->ServerOpen){
                    ServerOpenflag = 1;
                    /*判断报文是否已有应答*/
                    if(0 == pmsgData->IsRespond[jLoop]){ 
                        /*判断报文是否需要发送*/
                        if((pmsgData->flag && pmsgData->SendTimes[jLoop] < pgPara->GeneralPara.ReCount)\
                            || (0 == pmsgData->flag && 0 == pmsgData->SendTimes[jLoop])){
                            /*发送报文*/
                            pserver->channes[jLoop].packet_send_handle(&pserver->channes[jLoop],pmsgData->content);
                            pmsgData->SendTimes[jLoop]++;
                            pgmsgbuff->Data[iLoop].IsUse = MSGBUF_IS_WAITING;
                            pgmsgbuff->Data[iLoop].waittime = 0;
                        }
                    }
                } 
            }
            if(0 == ServerOpenflag){
                pgmsgbuff->Data[iLoop].IsUse = MSGBUF_IS_WAITING; //配合message_buf_clear_func
            }
            //break;
        }
    } 
    
}
static void tcpclient_thread_send()
{
    
    sleep(60);
    while(1){
        message_buf_clear_func();
        //PowerState();
        //ValvePowerState();
        if(pgValveControl->per == pgValveControl->per_last \
            && pgValveControl->per <= 100){//非控阀过程 控阀过程中不上报阀门状态
            state_per = abs(pgValveControl->per - pgValveControl->per_measure)>5 ? pgValveControl->per_measure : pgValveControl->per;
            if( abs(pgValveControl->per_alarm - state_per)>5){
                Insert_Message_Data(CN_GetValveStatus,0);
                pgValveControl->per_alarm = state_per;
                syncValveParaShm();
            }
        }
        Insert_Message_Data(CN_SendHeartbeat,0);
        send_message_to_server_func();
        sleep(pgPara->GeneralPara.RtdInterval);
    }
}

static void tcpclient_thread_recv(void * arg)
{
    int tcplink_rtycnt = 0;
    int iRecvLen,MessageLen,iLoop;
    TcpClientDev *pdev = (TcpClientDev *)arg;
    char* end = NULL;
    char* pos = NULL;
	char* pRecvBuf = (char*)malloc(RECVBUF_LEN);
    
    pdev->packet_send_handle = (packet_send_cb)send_message_to_server;
    for(;;){
        /* 客户程序开始建立 sockfd描述符 */ 
    	if((pdev->dev_fd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
    	{ 
    		DEBUG_PRINT_ERR(gPrintLevel,"[up_proc] Socket Error:\n"); 
            sleep(3);
    		continue;
    	} 
     
     	if(TINZ_ERROR == tcplink_connect(pdev)){
            sleep(30); 
            continue;
        }
    	/* 连接成功了 */ 
    	//pdev->tcplink->isConnected = 1;
    	DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] dev_fd[%d] tcp link[port:%d dstip:%s] connect success\n",pdev->dev_fd,pdev->tcplink->ServerPort,pdev->tcplink->ServerIp);

    	while(1){
    		
    		memset(pRecvBuf,0,RECVBUF_LEN);
    		iRecvLen = recv_message_from_server(pdev, pRecvBuf);
    		if(iRecvLen > 0 && iRecvLen < RECVBUF_LEN){  
                DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] receive message from server[port:%d dstip:%s] (%d) :%c:%s:\n", pdev->tcplink->ServerPort,pdev->tcplink->ServerIp, iRecvLen, pRecvBuf[0],pRecvBuf);
                pos = pRecvBuf;
                for(iLoop=0;iLoop<10;iLoop++){//最多解析10个包
                    end = strstr(pos,"\x0d\x0a");
                    if(NULL != end && end > pos) {
                        MessageLen = end - pos + 3;
                        messageProc(pos, MessageLen, NULL, pdev);
                        pos = end + 2;
                        if(pos>=pRecvBuf+iRecvLen){break;}
                	}else{
                        break;
                    }
                }
            }else{
    			DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] receive message from server[port:%d dstip:%s] error[%d] pdev->dev_fd[%d]\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp,iRecvLen,pdev->dev_fd);
                /* 客户程序重新发起连接请求 */ 
                pdev->tcplink->isConnected = 0;
                if(-1 == iRecvLen || -1 == pdev->dev_fd){
                    /*fd_flag = 0;
                    for(iLoop=0;iLoop<SITE_CNT;iLoop++){
                        if(pserver->channes[iLoop].tcplink->ServerOpen){
                            if(pserver->channes[iLoop].tcplink->SiteNum != pdev->tcplink->SiteNum){
                                if(pserver->channes[iLoop].dev_fd == pdev->dev_fd){
                                    fd_flag = 1; 
                                    break;
                                }
                            }
                        }
                    }*/
                    if(-1 != pdev->dev_fd){
                        /* 客户程序开始建立 sockfd描述符 */
                        DEBUG_PRINT_ERR(gPrintLevel,"[up_proc] [port:%d dstip:%s] Socket[%d] close start:\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp,pdev->dev_fd);
                        close(pdev->dev_fd);
                        DEBUG_PRINT_ERR(gPrintLevel,"[up_proc] [port:%d dstip:%s] Socket[%d] close end:\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp,pdev->dev_fd);

                     }
                    if((pdev->dev_fd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
                	{ 
                		DEBUG_PRINT_ERR(gPrintLevel,"[up_proc] [port:%d dstip:%s] Socket Error:\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp); 
                	}else{
                        DEBUG_PRINT_ERR(gPrintLevel,"[up_proc] [port:%d dstip:%s] Socket[%d] success:\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp,pdev->dev_fd);
                    }
                    //}
                }
                if(-1 != pdev->dev_fd){ 
                	if(TINZ_ERROR == tcplink_connect(pdev)){               
                        DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] tcp retry[%d] link[port:%d dstip:%s] connect fail\n",tcplink_rtycnt,pdev->tcplink->ServerPort,pdev->tcplink->ServerIp); 
                        sleep(60);
                    }else{
                        tcplink_rtycnt = 0;
                        DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] tcp retry link[port:%d dstip:%s] connect success\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp);
                    }        
                }
                tcplink_rtycnt++;
                //if(tcplink_rtycnt > 60){
                    //exit(0);
                //}
                sleep(1);
    		}
    	}
        close(pdev->dev_fd);
        break;
    }
	free(pRecvBuf);
}

static int tcplink_connect(TcpClientDev *pdev){
    struct sockaddr_in 	ServAddr;
    /* 客户程序填充服务端的资料 */ 
	bzero(&ServAddr,sizeof(ServAddr)); // 初始化,置0
	ServAddr.sin_family			=AF_INET;          // IPV4
	ServAddr.sin_port			=htons(pdev->tcplink->ServerPort);  // (将本机器上的short数据转化为网络上的short数据)端口号
	ServAddr.sin_addr.s_addr	=inet_addr((char*)pdev->tcplink->ServerIp); // IP地址
	/* 客户程序发起连接请求 */ 
    DEBUG_PRINT_ERR(gPrintLevel,"[up_proc] tcplink_connect start\n");
	if(connect(pdev->dev_fd,(struct sockaddr *)(&ServAddr),sizeof(struct sockaddr))==-1) 
	{ 
        DEBUG_PRINT_ERR(gPrintLevel,"[up_proc] tcp link[port:%d dstip:%s] connect Error\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp);
		close(pdev->dev_fd);
        pdev->dev_fd = -1;
        pdev->tcplink->isConnected = 0;
		return TINZ_ERROR;
	} 
    pdev->tcplink->isConnected = 1;
    DEBUG_PRINT_ERR(gPrintLevel,"[up_proc] tcplink_connect[port:%d dstip:%s] success\n",pdev->tcplink->ServerPort,pdev->tcplink->ServerIp);
    return TINZ_OK;
}

int tcpclient_open(TcpClientDev *pdev){
	
	if(!pdev->tcplink->ServerOpen){
		return TINZ_ERROR;
	}

	if(pthread_create(&pdev->thread_id,NULL,(void *)(&tcpclient_thread_recv),(void *)(pdev)) == -1)
	{
		DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] pthread_create error!\n");
	}
	return TINZ_OK;
}

int tcpclient_thread_send_create(pthread_t *thread_id){
    DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] tcpclient_thread_send create\n");

	if(pthread_create(thread_id,NULL,(void *)(&tcpclient_thread_send),NULL) == -1)
	{
		DEBUG_PRINT_INFO(gPrintLevel,"[up_proc] tcpclient_thread_send create error!\n");
	}
	return TINZ_OK;
}

