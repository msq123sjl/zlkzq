#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#include "tinz_pub_spi.h"
#include "tinz_base_def.h"
#include "tinz_common_helper.h"

static const char *device = "/dev/spidev1.0";

static void pabort(const char *s)
{
    perror(s);
    abort();
}

void SPI_Config(int spi_fd,pstSpiPara psp){
    
    int ret=0;
    /*
     * spi mode
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &psp->mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &psp->mode);
    if (ret == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &psp->bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &psp->bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &psp->speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &psp->speed);
    if (ret == -1)
        pabort("can't get max speed hz");

    DEBUG_PRINT_INFO(5, "[ValveControl] spi mode: %d\n", psp->mode);
    DEBUG_PRINT_INFO(5, "[ValveControl] bits per word: %d\n", psp->bits);
    DEBUG_PRINT_INFO(5, "[ValveControl] max speed: %d Hz (%d KHz)\n", psp->speed, psp->speed/1000);

}

int SPI_Init(pstSpiPara psp)
{
    int spi_fd;
    spi_fd = open(device, O_RDWR);
    if (spi_fd < 0)
        pabort("can't open device");
    
    SPI_Config(spi_fd,psp);
    return spi_fd;

}




