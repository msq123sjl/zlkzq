//============================================================================
/* Name        : test_spi.cpp

*/
//============================================================================
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>

#include "em335x_gpio.h"
#include "em335x_drivers.h"
#include "tinz_common_helper.h"
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_spi.h"
#include "tinz_pub_message.h"
//#include "tinz_common_db_helper.h"


#include "ValveControl.h"
#include "AD7705.h"
#include "TLC5615.h"

pstValveControl pgValveControl;
pstData pgData;
pstPara pgPara;
pstCalibrationPara pgCalibrationPara;
struct _msg *pmsg_upproc_to_control[SITE_CNT];
struct _msg *pmsg_interface_to_control;

stSpiPara sp;

int gPrintLevel = 5;
int io_fd,spi_fd;

void _proj_init(void)__attribute__((constructor));
void _proj_uninit(void)__attribute__((destructor));

void _proj_init(void){
	DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] adda start!!!\n");
}
void _proj_uninit(void)
{
	DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] adda stop!!!\n");
}

//static u_int8_t mode = 0;
//static u_int8_t bits = 16;
//static u_int32_t speed = 200000;


//阀门关闭输出清除
void Valve_Control_stop(int fd)
{
    GPIO_OutSet(fd, 1 << pgPara->IOPara.Out_drain_close);
    GPIO_OutSet(fd, 1 << pgPara->IOPara.Out_drain_open);
    GPIO_OutSet(fd, 1 << pgPara->IOPara.Out_drain_common);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve Control stop\n");
}


//排水阀门开启输出使能
void Valve_Open_Set(int fd)
{
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve Open start\n");

    GPIO_OutSet(fd, 1 << pgPara->IOPara.Out_drain_close);
    GPIO_OutClear(fd, 1 << pgPara->IOPara.Out_drain_open);
    GPIO_OutClear(fd, 1 << pgPara->IOPara.Out_drain_common);
}

//排水阀门关闭输出使能
void Valve_Close_Set(int fd)
{
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve Close start\n");

    GPIO_OutSet(fd, 1 << pgPara->IOPara.Out_drain_open);
    GPIO_OutClear(fd, 1 << pgPara->IOPara.Out_drain_close);
    GPIO_OutClear(fd, 1 << pgPara->IOPara.Out_drain_common);
}

//泵开启输出使能
void Pump_Open_Set(int fd)
{
    DEBUG_PRINT_INFO(gPrintLevel, "[PumpControl] Pump Open start\n");

    GPIO_OutClear(fd, 1 << pgPara->IOPara.Out_reflux_control);
}

//泵关闭输出使能
void Pump_Close_Set(int fd)
{
    DEBUG_PRINT_INFO(gPrintLevel, "[PumpControl] Pump Close start\n");

    GPIO_OutSet(fd, 1 << pgPara->IOPara.Out_reflux_control);
}

/*void per_value_state(uint8_t       per_current,uint8_t per_array[],uint8_t *big_cnt,uint8_t *small_cnt){
    int iLoop;
    *big_cnt = 0;
    *small_cnt = 0;
    for(iLoop = 0; iLoop < FILTER_CNT; iLoop++){
        if(per_current > per_array[iLoop]){
            small_cnt++;
        }else if(per_current < per_array[iLoop]){
            big_cnt++;
        }
    }
    for(iLoop = 0; iLoop < FILTER_CNT - 1; iLoop++){
        per_array[iLoop + 1] = per_array[iLoop];
    }
    per_array[0] = per_current;
}*/

void Valve_control_DA_mode(uint8_t per,uint8_t zeroes){
    uint8_t  per_stop = 0;
    uint8_t  per_current = 0;
    uint8_t  per_current_last = 0;
    uint16_t ad_value = 0,da_value = 0,da_value_adjust = 0;
    int cnt = 300;
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_DA_mode per[%d] start \n",per);
    da_value = PerValueToDA(per);
    spi_write_da(io_fd, spi_fd,da_value);
    while(cnt--){
        spi_read_ad(io_fd, spi_fd, pgValveControl->channel, &ad_value);
        per_current = AdValueToPer(ad_value);
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_DA_mode per[%d] per_current[%d]\n",per,per_current);
        if(per_current > 100){//阀门检测异常
            DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_DA_mode AD Value err!!!");
            return;
        }
        if(abs(per - per_current) <= 5){//阀门到位 正负偏差5%
            sleep(1);
            break;
        }
        /*过零点比较*/
        if(zeroes >0 && per <= per_current){break;}  //关阀门发到位
        if(zeroes == 0 && per >= per_current){break;}//开阀门到位
        /*阀门未到位，补*/
        if(abs(per_current - per_current_last) <= 1){
            per_stop++;
            DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_DA_mode per_stop[%d]\n",per_stop);
            if(per_stop > 5){ //连续6秒 阀门变化不超过2度
                da_value_adjust = PerValueToDA(abs(per - per_current));
                if(per > per_current){
                    da_value = (65535 - da_value) > da_value_adjust ?  da_value + da_value_adjust : 65535;
                }else{ 
                    da_value = da_value > da_value_adjust ? da_value - da_value_adjust : 0;
                }
                DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_DA_mode DA Valve adjust[%d]\n",da_value_adjust);
                spi_write_da(io_fd, spi_fd,da_value);
                sleep(60);
                break;
            }
        }else{
            per_stop = 0;
            per_current_last = per_current;
        }
        sleep(1);  
    }
    spi_write_da(io_fd, spi_fd,0);
    spi_read_ad(io_fd, spi_fd, pgValveControl->channel, &ad_value);
    per_current = AdValueToPer(ad_value);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_DA_mode per[%d] per_current[%d] stop\n",per,per_current);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_DA_mode DA Valve Open stop\n");
    
}
//zeroes：0 关阀门     1开阀门
void Valve_control_IO_mode(uint8_t per,uint8_t zeroes){
    uint8_t  per_current = 0;
    int cnt = 300;
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_IO_mode start\n");
    zeroes >0 ? Valve_Open_Set(io_fd) : Valve_Close_Set(io_fd);
    while(cnt--){
        per_current = GetPerValue(io_fd, spi_fd, pgValveControl->channel);
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_IO_mode per[%d] per_current[%d]\n",per,per_current);
        if(2 == pgData->state.ValveState){//阀门检测异常
            DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_IO_mode AD Value err!!!");
            return;
        }
        if(abs(per - per_current) <= 5){//阀门到位 正负偏差5%
            if((0 == zeroes && per < per_current) || (zeroes>0 && per > per_current)){
                //适当延迟 减小误差
                sleep(5);
            }
            break;
        }
        /*过零点比较*/
        if(zeroes >0 && per <= per_current){break;}
        if(zeroes == 0 && per >= per_current){break;}
        sleep(1);  
    }  
    Valve_Control_stop(io_fd);
    per_current = GetPerValue(io_fd, spi_fd, pgValveControl->channel);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_IO_mode per[%d] per_current[%d] stop\n",per,per_current);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_control_IO_mode stop\n");

}

int Valve_and_pump_control(uint8_t open_or_close){
    int cnt = 300;
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] IO Valve_and_pump_control [%s] start\n",open_or_close == 1 ? "open":"close");
    /*泵控制*/
    #if 1
    if(0 == open_or_close){
        Pump_Close_Set(io_fd);
        while(cnt--){
            DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Pump close In_reflux_open[%d]\n",GetSwitchStatus(io_fd, pgPara->IOPara.In_reflux_open));         
            if(1 == GetSwitchStatus(io_fd, pgPara->IOPara.In_reflux_open)){
                cnt = 300;
                break;
            }
            sleep(1);  
        }  
    }
    #endif
    /*阀门控制*/
    if(cnt == 300){
        open_or_close == 1 ? Valve_Open_Set(io_fd) : Valve_Close_Set(io_fd);
        while(cnt--){
            if(1 == open_or_close){      
                DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve [%s] In_drain_open[%d]\n",open_or_close == 1 ? "open":"close",GetSwitchStatus(io_fd, pgPara->IOPara.In_drain_open));
                if(0 == GetSwitchStatus(io_fd, pgPara->IOPara.In_drain_open)){
                    cnt = 300;
                    break;
                }
            }else{
                DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve [%s] In_drain_close[%d]\n",open_or_close == 1 ? "open":"close",GetSwitchStatus(io_fd, pgPara->IOPara.In_drain_close));
                if(0 == GetSwitchStatus(io_fd, pgPara->IOPara.In_drain_close)){
                    cnt = 300;
                    break;
                }
            }
            sleep(1);  
        }
    }  
    Valve_Control_stop(io_fd);
    #if 1
    /*泵控制*/
    if(cnt == 300){
        if(1 == open_or_close){
            Pump_Open_Set(io_fd);
            while(cnt--){
                DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Pump open In_reflux_open[%d]\n",GetSwitchStatus(io_fd, pgPara->IOPara.In_reflux_open));         
                if(0 == GetSwitchStatus(io_fd, pgPara->IOPara.In_reflux_open)){
                    cnt = 300;
                    break;
                }
                sleep(1);  
            }  
        }
    }
    #endif
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Valve_and_pump_control stop[%d]\n",cnt);
    if(300 == cnt){
        return TINZ_OK;
    }else{
        return TINZ_ERROR;
    }
}

void Valve_control(){
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] per_measure[%d]\n",pgValveControl->per,pgValveControl->per_measure);
    if(pgValveControl->per > pgValveControl->per_measure){
        pgValveControl->OutMode ? Valve_control_IO_mode(pgValveControl->per,1):Valve_control_DA_mode(pgValveControl->per,1);
    }else{
        pgValveControl->OutMode ? Valve_control_IO_mode(pgValveControl->per,0):Valve_control_DA_mode(pgValveControl->per,0);
    }
    pgValveControl->per_last = pgValveControl->per;
    syncValveParaShm();
}

static void MessageInit(){
    int iLoop;
    for(iLoop=0;iLoop<SITE_CNT;iLoop++){
        if(pgPara->SitePara[iLoop].ServerOpen){
        	DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] pmsg_upproc_to_control_%d start\n",iLoop);
        	pmsg_upproc_to_control[iLoop] = (struct _msg*)malloc(sizeof(struct _msg));
        	memset(pmsg_upproc_to_control[iLoop],0,sizeof(struct _msg));
        	if(TINZ_ERROR == prepareMsg(MSG_PATH_MSG,MSG_NAME_UPPROC_TO_CONTROL, iLoop+1, pmsg_upproc_to_control[iLoop])){
        		exit(0);
        	}
        }
    }
    
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] pmsg_interface_to_control start\n");
    pmsg_interface_to_control = InterfaceToControlMessageInit(pmsg_interface_to_control);
}

static void MessageRecvProc(struct _msg* msg){
    if(msg->msgbuf.mtype > 0){
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] msg recvtype[%ld]\n",msg->msgbuf.mtype);
        switch(msg->msgbuf.mtype){
            case MSG_CONTROL_VALVE_TYTE:
                Valve_control();
                break;
            default:
                DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] msg recvtype[%ld] not recognize [%-20.20s]\n",msg->msgbuf.mtype,msg->msgbuf.data);
        }
    }

}

static void MessageRecv(){
    int iLoop;
    /*接收上行消息队列*/
    for(iLoop=0;iLoop<SITE_CNT;iLoop++){
        if(pgPara->SitePara[iLoop].ServerOpen){
            MsgRcv(pmsg_upproc_to_control[iLoop], 0); 
            MessageRecvProc(pmsg_upproc_to_control[iLoop]);
        }
    }
    /*接收前端消息队列*/
    MsgRcv(pmsg_interface_to_control, 0); 
    MessageRecvProc(pmsg_interface_to_control);
}

static void state_thread()
{
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] state_thread\n");
    sleep(5);
    while(1){
        //DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] In_drain_open[%d] In_drain_close[%d]\n",pgData->IOState.In_drain_open,pgData->IOState.In_drain_close);
        sleep(1);
    }
}

int main(int argc, char* argv[])
{    
    pthread_t   thread_id;
    io_fd = InitGPIO();
    if(TINZ_ERROR ==  io_fd){
        return TINZ_ERROR;
    }
    AD7705_SPI_PARA(sp);
    spi_fd = SPI_Init(&sp);

    /*共享内存*/
	DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] getValveParaShm start\n");
	pgValveControl = (pstValveControl)getValveParaShm();
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] config file per[%d] per_last[%d]\n",pgValveControl->per,pgValveControl->per_last);
    pgData = (pstData)getDataShm();
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] State InPower[%d] ValveState[%d]\n",pgData->state.InPower,pgData->state.ValveState);
    pgPara = (pstPara)getParaShm();
    pgCalibrationPara = (pstCalibrationPara)getCalibrationParaShm();
    /*消息队列*/
    MessageInit();
    
    /*程序初始读取阀门状态*/
    /*创建状态监测线程*/
    if(pthread_create(&thread_id,NULL,(void *)(&state_thread),NULL) == -1)
	{
		DEBUG_PRINT_INFO(gPrintLevel,"[ValveControl] state_thread create error!\n");
	}
    /*阀门控制线程*/
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Init per[%d] per_last[%d]\n",pgValveControl->per,pgValveControl->per_last);    
    for(;;){
        /*实时采样阀门开度*/
        pgValveControl->per_measure = GetPerValue(io_fd, spi_fd, pgValveControl->channel);
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] per_last[%d] per_measure[%d]\n",pgValveControl->per,pgValveControl->per_last,pgValveControl->per_measure);
        /*da_value = 512;
        spi_write_da(io_fd, spi_fd,da_value);*/
        MessageRecv();
        sleep(5);  
    }
    
    /*等待线程退出*/
	pthread_join(thread_id, NULL);
    int iLoop;
    for(iLoop=0;iLoop<SITE_CNT;iLoop++){
        if(NULL != pmsg_upproc_to_control[iLoop]){
            free(pmsg_upproc_to_control[iLoop]);
        }
    }
    return 0;
}


