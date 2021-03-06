#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "em335x_gpio.h"

#include "tinz_common_helper.h"
#include "tinz_base_def.h"

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

int GetSwitchStatus(int fd,int ch)
{
    unsigned int dwPinState=0xFFFFFFFF;
    GPIO_PinState(fd, &dwPinState); //read states of all bits
    
    return ((dwPinState>>ch)&0x1);
}

void EC20_Reset(){
    int fd;
    fd=open(DEV_GPIO,O_RDWR);
    GPIO_OutEnable(fd,PCIE_ON_OFF);
    GPIO_OutClear(fd, PCIE_ON_OFF);
    sleep(5);
    GPIO_OutSet(fd, PCIE_ON_OFF);
    sleep(5);
    close(fd);
}

void beep_control(int state){
    int fd;
    fd=open(DEV_GPIO,O_RDWR);
    GPIO_OutEnable(fd,BEEP_CONTROL);
    if(state){
        GPIO_OutSet(fd, BEEP_CONTROL);
    }else{
        GPIO_OutClear(fd, BEEP_CONTROL);
    }
    close(fd);
}

/*int OpenGPIO(){
    int fd;
    fd=open(DEV_GPIO,O_RDWR);
    DEBUG_PRINT_INFO(gPrintLevel, "[%s] open fd = %d\n", fd);
    if(fd < 0){
        return TINZ_ERROR;
    }
    return fd;
}

void CloseGPIO(int fd){
    close(fd);
}*/

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

int InitGPIO()
{
    int fd,rc;
    fd=open(DEV_GPIO,O_RDWR);
    DEBUG_PRINT_INFO(gPrintLevel, "[em335x_gpio] open file = %d\n", fd);
    if(fd < 0){
        return TINZ_ERROR;
    }
    rc = GPIO_OutEnable(fd,SPI_AD7705_CS|SPI_TLC5615_CS|SWITCH_OUT1|SWITCH_OUT2|SWITCH_OUT3|SWITCH_OUT4|BEEP_CONTROL|PCIE_ON_OFF|GPIO27);//set GPIO as output
    if(rc < 0)
    {
        DEBUG_PRINT_INFO(gPrintLevel, "[em335x_gpio] GPIO_OutEnable::failed %d\n", rc);
        return TINZ_ERROR;
    }
    rc = GPIO_OutDisable(fd,AD7705_DRDY|SWITCH_IN1|SWITCH_IN2|SWITCH_IN3|SWITCH_IN4|SWITCH_IN5|SWITCH_IN6);   //set GPIO as input
    if(rc < 0)
    {
        DEBUG_PRINT_INFO(gPrintLevel, "[em335x_gpio] GPIO_OutClear::failed %d\n", rc);
        return TINZ_ERROR;
    }
    usleep(10000);
    GPIO_OutSet(fd, SPI_AD7705_CS);
    GPIO_OutSet(fd, SPI_TLC5615_CS);
    return fd;

}
