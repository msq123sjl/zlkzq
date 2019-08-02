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
//#include "tinz_base_data.h"
//#include "tinz_pub_message.h"
//#include "tinz_common_db_helper.h"

#include "ValveControl.h"
#include "TLC5615.h"

extern int gPrintLevel;

int spi_write_da(int io_fd, int spi_fd, uint16_t da_value)
{
    da_value = (da_value)<<2;
    GPIO_OutClear(io_fd, SPI_TLC5615_CS);
    if( write(spi_fd, &da_value, 1) != 1){
        DEBUG_PRINT_INFO(gPrintLevel, "[ValveControl] DA Write Error\n");
        return TINZ_ERROR;
    }
    GPIO_OutSet(io_fd, SPI_TLC5615_CS);
    return TINZ_OK;
}

uint8_t PerValueToDA(uint16_t per){
    return per;
}


