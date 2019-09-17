#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "up_main.h"
#include "tcp_client.h"
#include "tinz_common_helper.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_shm.h"
#include "tinz_pub_message.h"



pstPara pgPara;
pstValveControl pgValveControl;
pstPollutantData pgPollutantData;
pstPollutantPara pgPollutantPara;
pstData pgData;

struct _msg *pmsg_upproc[SITE_SEND_CNT];
pstMessage pgmsgbuff = NULL;
struct _msg *pmsg_dataproc_to_upproc;


int gPrintLevel = 5;
UpMain* pserver = NULL;

static void MessageInit(){
    int iLoop;
    for(iLoop=0;iLoop<SITE_SEND_CNT;iLoop++){
        if(iLoop == SITE_CNT || pgPara->SitePara[iLoop].ServerOpen){
        	DEBUG_PRINT_INFO(gPrintLevel, "[up_proc] pmsg_upproc_%d start\n",iLoop);
        	pmsg_upproc[iLoop] = (struct _msg*)malloc(sizeof(struct _msg));
        	memset(pmsg_upproc[iLoop],0,sizeof(struct _msg));
        	if(TINZ_ERROR == prepareMsg(MSG_PATH_MSG,MSG_NAME_UPPROC_TO_SQLITE, iLoop+1, pmsg_upproc[iLoop])){
        		exit(0);
        	}
        }
    }

    DEBUG_PRINT_INFO(gPrintLevel, "[up_proc] pmsg_dataproc_to_upproc start\n");
    pmsg_dataproc_to_upproc = (struct _msg*)malloc(sizeof(struct _msg));
    memset(pmsg_dataproc_to_upproc,0,sizeof(struct _msg));
    if(TINZ_ERROR == prepareMsg(MSG_PATH_MSG,MSG_NAME_DATAPROC_TO_UPPROC, MSG_ID_DATAPROC_TO_UPPROC, pmsg_dataproc_to_upproc)){
    	exit(0);
    }
}
static void wait_for_serveropen_set(){
    int iLoop;
    for(;;){
        for(iLoop=0;iLoop<SITE_CNT;iLoop++){
            if(pgPara->SitePara[iLoop].ServerOpen){
                return;
            }
        }
        sleep(5);
    }

}
int main(int argc, char *argv[])
{
	int iLoop;
    pthread_t thread_id_send = 0;
	//DEBUG_PRINT_INFO(gPrintLevel,"== port = %d\n", atoi(argv[1]));
    pgPara = (pstPara)getParaShm();
	DEBUG_PRINT_INFO(gPrintLevel,"== AlarmTime = %d\n", pgPara->GeneralPara.AlarmTime);
    DEBUG_PRINT_INFO(gPrintLevel, "up_proc getValveParaShm start\n");
    pgValveControl = (pstValveControl)getValveParaShm();
    DEBUG_PRINT_INFO(gPrintLevel, "up_proc getPollutantDataShm start\n");
    pgPollutantData = (pstPollutantData)getPollutantDataShm();
    DEBUG_PRINT_INFO(gPrintLevel, "up_proc getPollutantDataShm end\n");
    DEBUG_PRINT_INFO(gPrintLevel, "up_proc getPollutantParaShm start\n");
    pgPollutantPara = (pstPollutantPara)getPollutantParaShm();
    DEBUG_PRINT_INFO(gPrintLevel, "up_proc getPollutantParaShm end\n");
    DEBUG_PRINT_INFO(gPrintLevel, "up_proc getDataShm start\n");
    pgData = (pstData)getDataShm();
    DEBUG_PRINT_INFO(gPrintLevel, "up_proc getDataShm end\n");

    /*测试临时用*/
    //pgPara->SitePara[0].ServerOpen   = 1;
    //snprintf((char*)pgPara->SitePara[0].ServerIp,sizeof(pgPara->SitePara[0].ServerIp),"%s","192.168.137.1");

    wait_for_serveropen_set(); //无上行服务器 停留在此处             但消息队列不能够有效接收 导致消息队列溢出 需做进一步处理
    /*消息队列*/
    MessageInit();
    DEBUG_PRINT_INFO(gPrintLevel, "[up_proc] MessageInit end\n");

	pserver = (UpMain*)malloc(sizeof(UpMain));
	if(NULL == pgPara || NULL == pserver){
		DEBUG_PRINT_ERR(gPrintLevel, "[up_proc] getParaShm or malloc fail!!!\n")
		free(pserver);
		return 0;
	}
	memset(pserver,0,sizeof(UpMain));
    
    pgmsgbuff = (pstMessage)malloc(sizeof(stMessage));
    memset(pgmsgbuff,0,sizeof(stMessage));    
	signal(SIGPIPE, SIG_IGN);
    //pgPara->SitePara[0].ServerOpen   = 1;
    //pgPara->SitePara[0].isConnected  = 0;
    //pgPara->SitePara[0].ServerPort   = atoi(argv[1]);
    //snprintf((char*)pgPara->SitePara[0].ServerIp,sizeof(pgPara->SitePara[0].ServerIp)-1,"%s","192.168.1.131");

    for(iLoop=0; iLoop < SITE_CNT; iLoop++){
		
		pserver->channes[iLoop].tcplink = &pgPara->SitePara[iLoop];
		tcpclient_open(&pserver->channes[iLoop]);
	}
    tcpclient_thread_send_create(&thread_id_send);
    /*等待socket 发送线程退出*/
    pthread_join(thread_id_send, NULL);
	/*等待socket 平台通道接收线程退出*/
	for(iLoop = 0; iLoop < SITE_CNT; iLoop++){
		if(!pserver->channes[iLoop].tcplink->ServerOpen || !pserver->channes[iLoop].tcplink->isConnected){continue;}
		pthread_join(pserver->channes[iLoop].thread_id, NULL);
		pserver->channes[iLoop].tcplink->isConnected = 0;
		close(pserver->channes[iLoop].dev_fd);
	}
	free(pserver);
	
	return 0;
}
