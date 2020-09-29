//============================================================================
/* Name        : TLC5615.c

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
#include "tinz_pub_spi.h"
//#include "tinz_base_data.h"
//#include "tinz_pub_message.h"
//#include "tinz_common_db_helper.h"

#include "ValveControl.h"
#include "TLC5615.h"

extern int gPrintLevel;
extern stSpiPara sp;

static int write_da(int io_fd, int spi_fd, uint16_t da_value){
    da_value = (da_value)<<2;
    GPIO_OutClear(io_fd, SPI_TLC5615_CS);
    usleep(100);
    if( write(spi_fd, &da_value, 2) != 2){
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] DA Write Error\n");
        return TINZ_ERROR;
    }    
    DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] DA Write [%d]\n",da_value);
    usleep(100);
    GPIO_OutSet(io_fd, SPI_TLC5615_CS);
    usleep(100);
    return TINZ_OK;
}

int spi_write_da(int io_fd, int spi_fd, uint16_t da_value)
{
    int result;
    TLC5615_SPI_PARA(sp);
    SPI_Config(spi_fd,&sp);
    result = write_da(io_fd,spi_fd,da_value);
    AD7705_SPI_PARA(sp);
    SPI_Config(spi_fd,&sp);
    return result;
}

float PerValueToIa(uint8_t per){
    return 0.16*per + 4;
}

uint16_t PerValueToDA(uint8_t per){
    float current = 0;
    uint16_t da_value;
    current = PerValueToIa(per);
    da_value = (uint16_t)(40.96*current);
    return da_value;
}

uint8_t SetPer(int io_fd, int spi_fd,uint8_t per){
    uint16_t da_value;
    da_value = PerValueToDA(per);
    return spi_write_da(io_fd, spi_fd,da_value);
}


