//============================================================================
/* Name        : TLC2543.c

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
//#include "tinz_base_data.h"
//#include "tinz_pub_message.h"
//#include "tinz_common_db_helper.h"


#include "ValveControl.h"
#include "TLC2543.h"


extern int gPrintLevel;

int spi_read_ad(int io_fd, int spi_fd, uint8_t channel, uint16_t *pad_value)
{
    uint16_t AdValue[AD_READ_CNT];
    uint16_t ad_value = 0;
    int iLoop;
    for(iLoop = 0; iLoop < AD_READ_CNT ; iLoop++){
        GPIO_OutClear(io_fd, SPI_TLC2543_CS);
        if(write(spi_fd, &channel, 1) != 1){
            DEBUG_PRINT_INFO(gPrintLevel, "AD Write Error\n");
            return TINZ_ERROR;
        }
        GPIO_OutSet(io_fd, SPI_TLC2543_CS);
        usleep(10);
        GPIO_OutClear(io_fd, SPI_TLC2543_CS);
        if(read(spi_fd, &AdValue[iLoop], 1) != 1){
            DEBUG_PRINT_INFO(gPrintLevel, "AD Read Error");
            return TINZ_ERROR;
        }
        GPIO_OutSet(io_fd, SPI_TLC2543_CS);
        usleep(1000);
    }
    qsort(AdValue,AD_READ_CNT,sizeof(AdValue[0]),cmpfunc_uint16);
    ad_value = AdValue[AD_READ_CNT/2 -3] + AdValue[AD_READ_CNT/2 -2 ] + AdValue[AD_READ_CNT/2 -1] + AdValue[AD_READ_CNT/2]\
             + AdValue[AD_READ_CNT/2 + 1] + AdValue[AD_READ_CNT/2 + 2] + AdValue[AD_READ_CNT/2 +3];
    *pad_value = ad_value/7;
    return TINZ_OK;

}

uint8_t AdValueToPer(uint16_t ad_value){
    return ad_value>>8;
}

float AdValueToIa(uint16_t ad_value){
    return ad_value>>8;
}


