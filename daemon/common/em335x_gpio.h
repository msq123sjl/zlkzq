#ifndef EM335X_GPIO_H
#define EM335X_GPIO_H
#include "em335x_drivers.h"

#define    SWITCH_OUT1   GPIO6
#define    SWITCH_OUT2   GPIO7
#define    SWITCH_OUT3   GPIO8
#define    SWITCH_OUT4   GPIO9
#define    SWITCH_OUT5   GPIO16
#define    SWITCH_OUT6   GPIO17
//#define    SWITCH_OUT7   GPIO24
//#define    SWITCH_OUT8   GPIO25

#define    SWITCH_IN1      GPIO10
#define    SWITCH_IN2      GPIO11
#define    SWITCH_IN3      GPIO12
#define    SWITCH_IN4      GPIO13
#define    SWITCH_IN5      GPIO14
#define    SWITCH_IN6      GPIO15
#define    SWITCH_IN7      GPIO18
#define    SWITCH_IN8      GPIO19
#define    SWITCH_IN9      GPIO20
#define    SWITCH_IN10      GPIO21
#define    SWITCH_IN11      GPIO22
#define    SWITCH_IN12      GPIO23

#define    SPI_CS               GPIO31

#define    SPI_TLC2543_CS   GPIO24
#define    SPI_TLC5615_CS   GPIO25
#define    VALVE_OPEN       GPIO26
#define    VALVE_CLOSE      GPIO26
#define    VALVE_COMMON     GPIO26


int  GPIO_OutEnable(int fd, unsigned int dwEnBits);
int  GPIO_OutDisable(int fd, unsigned int dwDisBits);
int  GPIO_OutSet(int fd, unsigned int dwSetBits);
int  GPIO_OutClear(int fd, unsigned int dwClearBits);
int  GPIO_PinState(int fd, unsigned int* pPinState);
//int  InitGPIO();


#endif // GPIO_DRIVERS_H
