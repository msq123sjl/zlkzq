#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "em335x_gpio.h"

#include "tinz_common_helper.h"

int gPrintLevel = 5;

int GPIO_OutEnable(int fd, unsigned int dwEnBits)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_OUTPUT_ENABLE;		// 0
    dpars.par2 = dwEnBits;

    rc = write(fd, &dpars, sizeof(struct double_pars));
    return rc;
}

int GPIO_OutDisable(int fd, unsigned int dwDisBits)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_OUTPUT_DISABLE;	// 1
    dpars.par2 = dwDisBits;

    rc = write(fd, &dpars, sizeof(struct double_pars));
    return rc;
}

int GPIO_OutSet(int fd, unsigned int dwSetBits)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_OUTPUT_SET;	// 2
    dpars.par2 = dwSetBits;

    rc = write(fd, &dpars, sizeof(struct double_pars));
    return rc;
}

int GPIO_OutClear(int fd, unsigned int dwClearBits)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_OUTPUT_CLEAR;	// 3
    dpars.par2 = dwClearBits;

    rc = write(fd, &dpars, sizeof(struct double_pars));
    return rc;
}

int GPIO_PinState(int fd, unsigned int* pPinState)
{
    int 				rc;
    struct double_pars	dpars;

    dpars.par1 = EM335X_GPIO_INPUT_STATE;	// 5
    dpars.par2 = *pPinState;

    rc = read(fd, &dpars, sizeof(struct double_pars));
    if(!rc)
    {
        *pPinState = dpars.par2;
    }
    return rc;
}

/*int InitGPIO()
{
    int fd,rc;
    fd=open("/dev/em335x_gpio",O_RDWR);
    DEBUG_PRINT_INFO(gPrintLevel, "open file = %d\n", fd);
    //printf("open file = %d\n", fd);
    if(fd < 0){
        return fd;
    }
    rc = GPIO_OutEnable(fd,GPIO0|SWITCH_OUT1|SWITCH_OUT2|SWITCH_OUT3|SWITCH_OUT4|SWITCH_OUT5|SWITCH_OUT6|SWITCH_OUT7|SWITCH_OUT8|SPI_CS);//set GPIO as output
    if(rc < 0)
    {
        DEBUG_PRINT_INFO(gPrintLevel, "GPIO_OutEnable::failed %d\n", rc);
        return -1;
    }

    rc = GPIO_OutDisable(fd,SWITCH_IN1|SWITCH_IN2|SWITCH_IN3|SWITCH_IN4|SWITCH_IN5|SWITCH_IN6|SWITCH_IN7|SWITCH_IN8|SWITCH_IN9|SWITCH_IN10|SWITCH_IN11|SWITCH_IN12);   //set GPIO as input
    if(rc < 0)
    {
        DEBUG_PRINT_INFO(gPrintLevel, "GPIO_OutClear::failed %d\n", rc);
        return -1;
    }
    usleep(10000);

    GPIO_OutDisable(fd,1<<31);

    return fd;

}*/

