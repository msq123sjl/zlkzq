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
//#include "tinz_pub_message.h"
//#include "tinz_common_db_helper.h"


#include "ValveControl.h"
#include "AD7705.h"
#include "TLC5615.h"

pstValveControl pgValveControl;
pstData pgData;
pstPara pgPara;
pstCalibrationPara pgCalibrationPara;


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

static void pabort(const char *s)
{
    perror(s);
    abort();
}


static const char *device = "/dev/spidev1.0";
static u_int8_t mode = 1;
static u_int8_t bits = 8;
static u_int32_t speed = 10000;
//static u_int16_t delay;


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

static int SPI_Init()
{
    int ret=0;
    int spi_fd;
    spi_fd = open(device, O_RDWR);
    if (spi_fd < 0)
        pabort("can't open device");
    /*
     * spi mode
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz");

    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] spi mode: %d\n", mode);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] bits per word: %d\n", bits);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] max speed: %d Hz (%d KHz)\n", speed, speed/1000);
    return spi_fd;

}

static int SpiInitGPIO()
{
    int fd,rc;
    fd=open(DEV_GPIO,O_RDWR);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] open file = %d\n", fd);
    if(fd < 0){
        return TINZ_ERROR;
    }
    rc = GPIO_OutEnable(fd,SPI_AD7705_CS|SPI_TLC5615_CS|SWITCH_OUT1|SWITCH_OUT2|SWITCH_OUT3|SWITCH_OUT4);//set GPIO as output
    if(rc < 0)
    {
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] GPIO_OutEnable::failed %d\n", rc);
        return TINZ_ERROR;
    }
    rc = GPIO_OutDisable(fd,AD7705_DRDY|SWITCH_IN1|SWITCH_IN2|SWITCH_IN3|SWITCH_IN4|SWITCH_IN5|SWITCH_IN6);   //set GPIO as input
    if(rc < 0)
    {
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] GPIO_OutClear::failed %d\n", rc);
        return TINZ_ERROR;
    }
    usleep(10000);
    GPIO_OutSet(fd, SPI_AD7705_CS);
    GPIO_OutSet(fd, SPI_TLC5615_CS);
    return fd;

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
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] DA Valve Open start\n");
    spi_write_da(io_fd, spi_fd,da_value);
    while(cnt--){
        spi_read_ad(io_fd, spi_fd, pgValveControl->channel, &ad_value);
        per_current = AdValueToPer(ad_value);
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] per_current[%d]\n",per,per_current);
        if(abs(per - per_current) <= 2){
            sleep(6);
            break;
        }
        /*过零点比较*/
        if(zeroes >0 && per <= per_current){break;}
        if(zeroes == 0 && per >= per_current){break;}
        /*阀门未到位，补*/
        if(abs(per_current - per_current_last) <= 1){
            per_stop++;
            DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per_stop[%d]\n",per_stop);
            if(per_stop > 5){ //连续6秒 阀门变化不超过2度
                da_value_adjust = PerValueToDA(abs(per - per_current));
                if(per > per_current){
                    da_value = (65535 - da_value) > da_value_adjust ?  da_value + da_value_adjust : 65535;
                }else{ 
                    da_value = da_value > da_value_adjust ? da_value - da_value_adjust : 0;
                }
                DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] DA Valve adjust[%d]\n",da_value_adjust);
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
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] per_current[%d] stop\n",per,per_current);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] DA Valve Open stop\n");
    
}
//zeroes：0 关阀门     1开阀门
void Valve_control_IO_mode(uint8_t per,uint8_t zeroes){
    uint8_t  per_current = 0;
    uint16_t ad_value = 0;
    int cnt = 300;
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] IO Valve control start\n");
    zeroes >0 ? Valve_Open_Set(io_fd) : Valve_Close_Set(io_fd);
    while(cnt--){
        spi_read_ad(io_fd, spi_fd, pgValveControl->channel, &ad_value);
        per_current = AdValueToPer(ad_value);
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] per_current[%d]\n",per,per_current);
        if(abs(per - per_current) <= 5){
            sleep(6);
            break;
        }
        /*过零点比较*/
        if(zeroes >0 && per <= per_current){break;}
        if(zeroes == 0 && per >= per_current){break;}
        sleep(1);  
    }  
    Valve_Control_stop(io_fd);
    spi_read_ad(io_fd, spi_fd, pgValveControl->channel, &ad_value);
    per_current = AdValueToPer(ad_value);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] per_current[%d] stop\n",per,per_current);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] IO Valve control stop\n");

}

int Valve_and_pump_control(uint8_t open_or_close){
    uint8_t  per_current = 0;
    uint16_t ad_value = 0;
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

static void state_thread()
{
    static int ValveStateFilter = 0;
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] state_thread\n");
    sleep(5);
    while(1){
        pgData->IOState.InPower = (uint8_t)GetSwitchStatus(io_fd, pgPara->IOPara.In_power);
        pgData->IOState.In_drain_open = (uint8_t)GetSwitchStatus(io_fd, pgPara->IOPara.In_drain_open);
        pgData->IOState.In_drain_close = (uint8_t)GetSwitchStatus(io_fd, pgPara->IOPara.In_drain_close);
        pgData->IOState.In_reflux_open = (uint8_t)GetSwitchStatus(io_fd, pgPara->IOPara.In_reflux_open);
        if(0 == pgData->IOState.In_drain_open && 1 == pgData->IOState.In_drain_close){
            pgData->state.ValveState = 1;
            ValveStateFilter = 0;
        }else if(1 == pgData->IOState.In_drain_open && 0 == pgData->IOState.In_drain_close){
            pgData->state.ValveState = 0;
            ValveStateFilter = 0;
        }else{
            ValveStateFilter++;
            if(ValveStateFilter > 300){
                pgData->state.ValveState = 2;
                ValveStateFilter = 300;
            }
        } 
        pgData->state.PumpState = (1 == pgData->IOState.In_reflux_open) ? 0 : 1;
        //DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] In_drain_open[%d] In_drain_close[%d]\n",pgData->IOState.In_drain_open,pgData->IOState.In_drain_close);
        sleep(1);
    }
}

int main(int argc, char* argv[])
{    
    uint8_t  per = 0;
    //uint8_t  per_current = 0;
    uint16_t ad_value = 0;
    pthread_t   thread_id;
    
    io_fd = SpiInitGPIO();
    if(TINZ_ERROR ==  io_fd){
        return TINZ_ERROR;
    }
    spi_fd = SPI_Init();

    /*共享内存*/
	DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] getValveParaShm start\n");
	pgValveControl = (pstValveControl)getValveParaShm();
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] config file per[%d] per_last[%d]\n",pgValveControl->per,pgValveControl->per_last);
    pgData = (pstData)getDataShm();
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] State InPower[%d] ValveState[%d]\n",pgData->state.InPower,pgData->state.ValveState);
    pgPara = (pstPara)getParaShm();
    pgCalibrationPara = (pstCalibrationPara)getCalibrationParaShm();
    #if 0
    spi_read_ad(io_fd, spi_fd, pgValveControl->channel, &ad_value);
    per_current = AdValueToPer(ad_value);
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] per_last[%d] per_current[%d]\n",pgValveControl->per,pgValveControl->per_last,per_current);
    if(abs(pgValveControl->per_last - per_current) > 5){
        pgValveControl->per_last = per_current; 
    }
    #endif
    /*程序初始读取阀门状态*/
    #ifdef VALVE_AND_PUMP
    static uint16_t ValveControlFilter = 0;
    pgData->state.ValveState = 3;
    #endif
    /*创建状态监测线程*/
    if(pthread_create(&thread_id,NULL,(void *)(&state_thread),NULL) == -1)
	{
		DEBUG_PRINT_INFO(gPrintLevel,"[ValveControl] state_thread create error!\n");
	}
    /*阀门控制线程*/
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] Init per[%d] per_last[%d]\n",pgValveControl->per,pgValveControl->per_last);
    for(;;){
        #ifdef VALVE_AND_PUMP
            per = pgValveControl->per > 0 ? 1 : 0;
            DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] ValveState[%d]\n",per,pgData->state.ValveState);
            if(pgData->state.ValveState > 2){
                //continue;
            }else if(2 == pgData->state.ValveState && (0 !=ValveControlFilter || pgPara->Mode)){
                ValveControlFilter = (ValveControlFilter + 1)% 720;  //若阀门异常 每小时重新控制阀门
            }else if(pgData->state.ValveState != per){
                if(TINZ_OK == Valve_and_pump_control(per)){
                    ValveControlFilter = 0;
                    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl1] per[%d] per_last[%d]\n",per,pgValveControl->per_last);
                }
                syncValveParaShm();
            }
        #else
            /*实时采样阀门开度*/
            spi_read_ad(io_fd, spi_fd, pgValveControl->channel, &ad_value);
            pgData->current_Ia[0] = AdValueToIa(ad_value);
        
            per = pgValveControl->per;
            if(pgValveControl->per_last != per){
                DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] per[%d] per_last[%d]\n",per,pgValveControl->per_last);
                #if 0
                if(per > pgValveControl->per_last){
                    pgValveControl->OutMode ? Valve_control_IO_mode(per,1):Valve_control_DA_mode(per,1);
                }else{
                    pgValveControl->OutMode ? Valve_control_IO_mode(per,0):Valve_control_DA_mode(per,0);
                }
                #endif
                pgValveControl->per_last = per;
                syncValveParaShm();
            }
        #endif

        sleep(5);  
    }
    
    /*等待线程退出*/
	pthread_join(thread_id, NULL);
    return 0;
}


