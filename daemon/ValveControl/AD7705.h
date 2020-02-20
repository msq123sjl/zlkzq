#ifndef __AD7705__H
#define __AD7705__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AD_READ_CNT 1

int spi_read_ad(int io_fd, int spi_fd,uint8_t channel, uint16_t *pad_value);
uint8_t AdValueToPer(uint16_t ad_value);
float AdValueToIa(uint16_t ad_value);
uint8_t GetPerValue(int io_fd, int spi_fd,uint8_t channel);
#endif

