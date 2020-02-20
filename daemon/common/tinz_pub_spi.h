
#ifndef __TINZ_PUB_I2C_H__
#define __TINZ_PUB_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "tinz_base_def.h"

#define TLC5615_SPI_PARA(sp) {sp.mode=0;sp.bits=16;sp.speed=200000;}
#define AD7705_SPI_PARA(sp)  {sp.mode=3;sp.bits=8;sp.speed=200000;}

typedef struct _SpiPara
{   
    u_int8_t mode;   //bit[1] CPOL  0:SCL空闲为低电平          1:SCL空闲为高电平
                         //bit[0] CPHA  0:SCL前沿数据被采样         1:SCL后沿数据被采样
    u_int8_t bits;
    u_int32_t speed;

    //u_int8_t mode = 0;
    //u_int8_t bits = 16;
    //u_int32_t speed = 200000;
}stSpiPara,*pstSpiPara;

int SPI_Init(pstSpiPara psp);

#ifdef __cplusplus
}
#endif

#endif

