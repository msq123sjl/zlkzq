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

int gPrintLevel = 5;
pstData pgData;

void para_get_NetPara_onfig(pstPara para){
    char buf[256];
    int res,val ;
    FILE*  fd=0;
    fd=fopen(FS_NAME_PARA_CONF,"rb");
	if(fd<=0){
		DEBUG_PRINT_ERR(5,"open fs_para.conf file failure.\n");
		return;
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

    memset(buf,0,sizeof(buf));
    snprintf(buf,sizeof(buf),"/mnt/nandflash/bin/VPN_client.sh %s %s %s",pgPara->NetPara.VPNServerIp,pgPara->NetPara.VPNIPIP,pgPara->NetPara.VPNUserName);
    system(buf); 
}

int main(int argc, char *argv[])
{
	int 	i1,iLoop;
    int     disconnect_cnt = 0;
    int     VPNdisconnect_cnt =6;
    int     VPNIPIPERRCNT=0;
    char    output[64];
    char    buf[128];
    pgData = (pstData)getDataShm();
    pgPara = (pstPara)getParaShm();
    para_get_NetPara_onfig(pgPara);
    system("kill `ps -w | grep pppd | grep lte-connect-script | awk '{print $1}'`");
    //EC20_Reset();
    beep_control(0);
	//先调用一次pppconnt_stop，关闭连接
	pppconnt_stop();

	//通过代码加载驱动，测试使用华为ME909s，VID=0x12d1，PID=15c1
	//system("insmod /lib/modules/4.1.16/usbserial.ko vendor=0x12d1 product=0x15c1");
	//通过代码加载驱动，测试使用移元EC20，vendor=0x2c7c product=0x0125
    system("insmod /lib/modules/4.1.6/usbserial.ko vendor=0x2c7c product=0x0125");
	//开启拨号管理
	i1 = pppconnt_start();
	DEBUG_PRINT_INFO(gPrintLevel,"i1 = %d\n", i1);
    pgData->state.VPN = 1;
	while(1)
	{
		i1 = pppconnt_check();
		if(!i1) {
			//main processing
			//...
			DEBUG_PRINT_INFO(gPrintLevel,"connected!\n");
            pgData->state.LTE = 0;
            disconnect_cnt = 0;
            if(pgPara->NetPara.VPNOpen){
                get_system_output("route -n | grep ppp1 | grep -c U", output, sizeof(output));
                if('2' != output[0]){
                    VPNdisconnect_cnt++;
                    pgData->state.VPN = 1;
                    if(VPNdisconnect_cnt > 6){
                        DEBUG_PRINT_INFO(gPrintLevel,"link kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`[%s]\n",output);
                        system("kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`");
                        sleep(1);
                        get_system_output("route -n | grep ppp1 | grep -c U", output, sizeof(output));
                        DEBUG_PRINT_INFO(gPrintLevel,"link pppd call pptpvpn[%s]\n",output);
                        system("pppd call pptpvpn");
                        for(iLoop=0;iLoop<30;iLoop++){
                            sleep(1);
                            get_system_output("route -n | grep ppp1 | grep -c U", output, sizeof(output));
                            DEBUG_PRINT_INFO(gPrintLevel,"pppd call pptpvpn[%d][%s]\n",iLoop,output);
                            if('1' == output[0]){
                                snprintf(buf,sizeof(buf),"ifconfig ppp1 | grep \"inet addr\" | awk '{print $2}' | awk -F: '{print $2}' |  grep -c %s",pgPara->NetPara.VPNIPIP);
                                get_system_output(buf, output, sizeof(output));            
                                DEBUG_PRINT_INFO(gPrintLevel,"[%s] output[%s] VPNIPIPERRCNT[%d]\n",buf,output,VPNIPIPERRCNT);
                                if('1' == output[0] || VPNIPIPERRCNT > 10){
                                    VPNIPIPERRCNT = 0;
                                    if('1' == output[0]){
                                        snprintf(buf,sizeof(buf),"route add -net 172.16.0.0 netmask 255.255.0.0 gw %s",pgPara->NetPara.VPNIPIP);
                                        system(buf);
                                    }else{
                                        snprintf(buf,sizeof(buf),"ifconfig ppp1 | grep \"inet addr\" | awk '{print $2}' | awk -F: '{print $2}'");
                                        get_system_output(buf, output, sizeof(output)); 
                                        snprintf(buf,sizeof(buf),"route add -net 172.16.0.0 netmask 255.255.0.0 gw %s",output);
                                        system(buf);                                        
                                    }
                                    DEBUG_PRINT_INFO(gPrintLevel,"route[%s]\n",buf);
                                    sleep(3);
                                    get_system_output("route -n | grep ppp1 | grep -c U", output, sizeof(output));
                                    DEBUG_PRINT_INFO(gPrintLevel,"link route add s dev ppp1[%s]\n",output);
                                }else{
                                    VPNIPIPERRCNT += 1;
                                }
                                break;
                            }
                        }  
                        VPNdisconnect_cnt = 0;
                    }
                }else{
                    pgData->state.VPN = 0;
                    VPNdisconnect_cnt = 0;
                }
            }else if(0 == pgData->state.VPN){
                DEBUG_PRINT_INFO(gPrintLevel,"unlink kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`\n");
                system("kill `ps -w | grep pppd | grep pptpvpn | awk '{print $1}'`");
                sleep(1);
                get_system_output("route -n | grep ppp1 | grep -c U", output, sizeof(output));
                if('0' == output[0]){
                    pgData->state.VPN = 1;
                }
            }
		}
		else {
			//返回值为-，说明连接断开
			if(i1 == -1){
				DEBUG_PRINT_INFO(gPrintLevel,"disconnect!\n");
                pgData->state.LTE = 1;
            }
			//返回值为-2，说明检测不到信号，可能是没有检测到SIM卡，这里执行退出
			else if(i1 == -2) {
				DEBUG_PRINT_INFO(gPrintLevel,"singal is weak, please check!\n");
                pgData->state.LTE = 2;
				//break;
			}
			//返回值为-3，说明模块无响应，这里执行退出
			else if(i1 == -3) {
				DEBUG_PRINT_INFO(gPrintLevel,"there is no response, please check!\n");
                pgData->state.LTE = 3;
				//break;
			}else{
                 pgData->state.LTE = 4;
            }
            disconnect_cnt++;
            if(disconnect_cnt > 6){
                disconnect_cnt = 0;
                DEBUG_PRINT_INFO(gPrintLevel,"EC20_Reset!\n");
                EC20_Reset();
            }
		}
		sleep(10);
	}

	//退出循环，调用pppconnt_stop，释放相关资源
	pppconnt_stop();
    system("kill `ps -w | grep pppd | grep lte-connect-script | awk '{print $1}'`");
	return 0;
}







