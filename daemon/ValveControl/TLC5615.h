#ifndef __TLC5615__H
#define __TLC5615__H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int spi_write_da(int io_fd, int spi_fd, uint16_t da_value);
uint8_t PerValueToDA(uint16_t per);

#endif

