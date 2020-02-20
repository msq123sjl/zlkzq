/*
 * test_autodial.c
 *
 *  Created on: 2019年4月18日
 *      Author: user3
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

#include "autodial.h"
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_message.h"
#include "tinz_common_helper.h"
#include "em335x_gpio.h"
#include "config_file.h"

pstPara 		pgPara;
pstNetPara pgNetPara;
int gPrintLevel = 5;
pstData pgData;
char    route_buf[64];

void para_get_NetPara_onfig(pstNetPara para){
    char buf[256];
    int res,val ;
    FILE*  fd=0;
    fd=fopen(FS_NAME_NET_PARA_CONF,"rb");
	if(fd<=0){
		DEBUG_PRINT_ERR(5,"open fs_net_para.conf file failure.\n");
		return;
	}
    /*NetPara*/    
    res=getconfigint("NetPara","LTEOpen",&val,FS_NAME_NET_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args LTEOpen need\n"); return;}para->LTEOpen = val; 
    res=getconfigint("NetPara","VPNOpen",&val,FS_NAME_NET_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args VPNOpen need\n"); return;}para->VPNOpen = val; 
    res = getconfigstring("NetPara","VPNServerIp",para->VPNServerIp,sizeof(para->VPNServerIp) - 0,FS_NAME_NET_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args VPNServerIp need\n");return;} 
    res = getconfigstring("NetPara","VPNUserName",para->VPNUserName,sizeof(para->VPNUserName) - 0,FS_NAME_NET_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args VPNUserName need\n");return;}
    res = getconfigstring("NetPara","VPNIPIP",para->VPNIPIP,sizeof(para->VPNIPIP) - 0,FS_NAME_NET_PARA_CONF);
    if(res != CFG_FILE_NOERROR) {DEBUG_PRINT_ERR(5,"NetPara args VPNIPIP need\n");return;}

    memset(buf,0,sizeof(buf));
    snprintf(buf,sizeof(buf),"/mnt/nandflash/bin/VPN_client.sh %s %s %s",para->VPNServerIp,para->VPNIPIP,para->VPNUserName);
    system(buf); 
}
void vpn_client_check_proc(){
    int iLoop;
    static int VPNdisconnect_cnt =6;
    static int VPNIPIPERRCNT=0;
    char    output[64];
    char    buf[128];
    if(pgNetPara->VPNOpen){
        get_system_output(route_buf, output, sizeof(output));
        if('2' != output[0]){
            VPNdisconnect_cnt++;
            pgData->state.VPN = 1;
            DEBUG_PRINT_INFO(gPrintLevel,"[autodial] VPNdisconnect_cnt[%d] VPNIPIPERRCNT[%d]\n",VPNdisconnect_cnt,VPNIPIPERRCNT);
            if(VPNdisconnect_cnt > 6){   
                VPNdisconnect_cnt = 0;
                DEBUG_PRINT_INFO(gPrintLevel,"[autodial] link kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`[%s]\n",output);
                system("kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`");
                sleep(1);
                get_system_output(route_buf, output, sizeof(output));
                DEBUG_PRINT_INFO(gPrintLevel,"[autodial] link pppd call pptpvpn[%s]\n",output);
                system("pppd call pptpvpn");
                for(iLoop=0;iLoop<30;iLoop++){
                    sleep(1);
                    get_system_output(route_buf, output, sizeof(output));
                    DEBUG_PRINT_INFO(gPrintLevel,"[autodial] pppd call pptpvpn[%d][%s]\n",iLoop,output);
                    if('1' == output[0]){//VPN拨号成功
                        snprintf(buf,sizeof(buf),"ifconfig ppp%01d | grep \"inet addr\" | awk '{print $2}' | awk -F: '{print $2}' |  grep -c %s",pgNetPara->LTEOpen>0 ? 1:0,pgNetPara->VPNIPIP);
                        get_system_output(buf, output, sizeof(output));            
                        DEBUG_PRINT_INFO(gPrintLevel,"[autodial] [%s] output[%s] VPNIPIPERRCNT[%d]\n",buf,output,VPNIPIPERRCNT);
                        if('1' == output[0] || VPNIPIPERRCNT > 10){//查看分配的隧道IP
                            VPNIPIPERRCNT = 0;
                            if('1' == output[0]){
                                snprintf(buf,sizeof(buf),"route add -net 172.16.0.0 netmask 255.255.0.0 gw %s",pgNetPara->VPNIPIP);
                                system(buf);
                            }else{
                                snprintf(buf,sizeof(buf),"ifconfig ppp%01d | grep \"inet addr\" | awk '{print $2}' | awk -F: '{print $2}'",pgNetPara->LTEOpen>0 ? 1:0);
                                get_system_output(buf, output, sizeof(output)); 
                                snprintf(buf,sizeof(buf),"route add -net 172.16.0.0 netmask 255.255.0.0 gw %s",output);
                                system(buf);                                        
                            }
                            DEBUG_PRINT_INFO(gPrintLevel,"[autodial] route[%s]\n",buf);
                            sleep(3);
                            get_system_output(route_buf, output, sizeof(output));
                            DEBUG_PRINT_INFO(gPrintLevel,"[autodial] link route add s dev ppp%01d[%s]\n",pgNetPara->LTEOpen>0 ? 1:0,output);
                        }else{
                            VPNIPIPERRCNT += 1;
                        }
                        break;
                    }
                }  
            }
        }else{
            pgData->state.VPN = 0;
            VPNdisconnect_cnt = 0;
        }
    }else if(0 == pgData->state.VPN){
        DEBUG_PRINT_INFO(gPrintLevel,"[autodial] unlink kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`\n");
        system("kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`");
        sleep(1);
        get_system_output(route_buf, output, sizeof(output));
        if('0' == output[0]){
            pgData->state.VPN = 1;
        }
    }
}

void pppconnt_check_proc(){
    int ret;
    static int disconnect_cnt = 0;
    ret= pppconnt_check();
	if(!ret) {
        pgData->state.LTE = 0;
        disconnect_cnt = 0;
	}else {
		//返回值为-，说明连接断开
		if(ret == -1){
			DEBUG_PRINT_INFO(gPrintLevel,"[autodial] disconnect!\n");
            pgData->state.LTE = 1;
        }
		//返回值为-2，说明检测不到信号，可能是没有检测到SIM卡，这里执行退出
		else if(ret == -2) {
			DEBUG_PRINT_INFO(gPrintLevel,"[autodial] singal is weak, please check!\n");
            pgData->state.LTE = 2;
			//break;
		}
		//返回值为-3，说明模块无响应，这里执行退出
		else if(ret == -3) {
			DEBUG_PRINT_INFO(gPrintLevel,"[autodial] there is no response, please check!\n");
            pgData->state.LTE = 3;
			//break;
		}else{
             pgData->state.LTE = 4;
             DEBUG_PRINT_INFO(gPrintLevel,"[autodial] disconnect, please check!\n");
        }
        disconnect_cnt++;
        if(disconnect_cnt > 6){
            disconnect_cnt = 0;
            DEBUG_PRINT_INFO(gPrintLevel,"[autodial] EC20_Reset!\n");
            EC20_Reset();
        }
	}
}

int main(int argc, char *argv[])
{
	int ret;  
    pgData = (pstData)getDataShm();
    pgPara = (pstPara)getParaShm();
    pgNetPara = (pstNetPara)getNetParaShm();
    //para_get_NetPara_onfig(pgNetPara);
    pgData->state.VPN = 1;
    pgData->state.LTE = 1;
    beep_control(1);
    
    system("kill `ps -w | grep pppd | grep lte-connect-script | awk '{print $1}'`");
	//先调用一次pppconnt_stop，关闭连接
	pppconnt_stop();
    
    if(pgNetPara->LTEOpen){
    	//通过代码加载驱动，测试使用移元EC20，vendor=0x2c7c product=0x0125
        system("insmod /lib/modules/4.1.6/usbserial.ko vendor=0x2c7c product=0x0125");
    	//开启拨号管理
    	ret = pppconnt_start();
    	DEBUG_PRINT_INFO(gPrintLevel,"[autodial] pppconnt_start ret = %d\n", ret);
    }
    sprintf(route_buf,"route -n | grep ppp%01d | grep -c U",pgNetPara->LTEOpen>0 ? 1:0);
	while(1)
	{
        DEBUG_PRINT_INFO(gPrintLevel,"[autodial] connected LTEOpen[%d][%d] VPNOpen[%d][%d]!\n",pgNetPara->LTEOpen,pgData->state.LTE,pgNetPara->VPNOpen,pgData->state.VPN);
        pppconnt_check_proc();
        if(0 == pgNetPara->LTEOpen || 0 == pgData->state.LTE){
            vpn_client_check_proc();
        }
        sleep(10);
	}

	//退出循环，调用pppconnt_stop，释放相关资源
	pppconnt_stop();
    system("kill `ps -w | grep pppd | grep lte-connect-script | awk '{print $1}'`");
    system("kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`");
	return 0;
}







