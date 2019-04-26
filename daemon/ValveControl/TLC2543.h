#ifndef __TLC2543__H
#define __TLC2543__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AD_READ_CNT 41

int spi_read_ad(int io_fd, int spi_fd, uint8_t channel, uint16_t *pad_value);
uint8_t AdValueToPer(uint16_t ad_value);
float AdValueToIa(uint16_t ad_value);
#endif

