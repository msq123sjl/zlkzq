//============================================================================
/* Name        : AD7705.c

*/
//============================================================================
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "em335x_gpio.h"
#include "em335x_drivers.h"
#include "tinz_common_helper.h"
//#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
//#include "tinz_pub_message.h"
//#include "tinz_common_db_helper.h"

#include "ValveControl.h"
#include "AD7705.h"

extern pstData pgData;
extern int gPrintLevel;

static void AD7705_write(int io_fd, int spi_fd,uint8_t data){
    GPIO_OutClear(io_fd, SPI_AD7705_CS);
    usleep(100);
    write(spi_fd, &data, 1);
    usleep(100);
    GPIO_OutSet(io_fd, SPI_AD7705_CS);
    usleep(100);
}

static void AD7705_read(int io_fd, int spi_fd,uint16_t *pad_value){
    uint8_t data;
    GPIO_OutClear(io_fd, SPI_AD7705_CS);
    usleep(100);
    read(spi_fd, &data, 1);
    *pad_value = (data << 8) & 0xff00 ;    
    read(spi_fd, &data, 1);
    *pad_value = (*pad_value) | (data  & 0x00ff);
    //DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] AD read[%d]\n",*pad_value);
    usleep(100);
    GPIO_OutSet(io_fd, SPI_AD7705_CS);
    usleep(100);
}
/******************
channel: 0x00 第一通道  0x01 第二通道
******************/
int spi_read_ad(int io_fd, int spi_fd,uint8_t channel, uint16_t *pad_value){
    int iLoop;
    AD7705_write(io_fd,spi_fd,0x20|channel);    //Active Channel is Ain1(+)/Ain1(−), next operation as write to the clock register
    AD7705_write(io_fd,spi_fd,0x0c);            //master clock enabled, 4.9512MHz Clock, set output rate to 50Hz
    AD7705_write(io_fd,spi_fd,0x10|channel);    //Active Channel is Ain1(+)/Ain1(−), next operation as write to the setup register
    AD7705_write(io_fd,spi_fd,0x44);            //gain = 1, bipolar mode, buffer off, clear FSYNC and perform a Self Calibration
    for(iLoop=0;iLoop < 30;iLoop++){
        if(GetSwitchStatus(io_fd,AD7705_DRDY_CH) == 0x0){
            //DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] AD DRDY[%d][%d]\n",iLoop,channel);
            break;
        }
        if(29 == iLoop){
            DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] AD read timeout\n");
            return TINZ_ERROR;
        }        
        usleep(10000);
    }
    AD7705_write(io_fd,spi_fd,0x38|channel);
    AD7705_read(io_fd,spi_fd,pad_value);
    return TINZ_OK;
}

uint8_t AdValueToPer(uint16_t ad_value){
    uint8_t per;
    float current;
    current = AdValueToIa(ad_value);
    if(current > 22 || current < 3){
        pgData->state.ValveState = 1;
        return 255;
    }
    current = current < 4 ? 4 : current;
    current = current > 20 ? 20 : current;
    per = (uint8_t)(6.25 * current - 25);
    return per;
}

float AdValueToIa(uint16_t ad_value){
    float Voltage,Current;
    Voltage = 2.5*ad_value/65536.0;
    Current = Voltage*10;
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] AdValueToIa ad[%d] Voltage[%.3f] Current[%.3f]\n",ad_value,Voltage,Current);
    return Current;
}

uint8_t GetPerValue(int io_fd, int spi_fd,uint8_t channel){
    uint16_t ad_value[3] = {0,0,0};
    int iLoop;
    for(iLoop=0;iLoop<3;iLoop++){
        spi_read_ad(io_fd, spi_fd, channel, &ad_value[iLoop]);
    }
    return AdValueToPer(TINZ_MID(ad_value[0], ad_value[1], ad_value[2]));
}


