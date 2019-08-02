 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include "readmeter.h"
#include "tinz_common_helper.h"
#include "tinz_pub_serial.h"
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "protocol.h"
#include "modbus_Master.h"


extern int gPrintLevel;
extern pstPara pgPara;

static int find_start_and_end(u_char *readbuf,int nread,u_char start,u_char end,int *start_index,int *end_index,int datalen){
	int iLoop;
	for(iLoop = 0; iLoop < nread; iLoop++){
		if(start == readbuf[iLoop]){
			if(nread - iLoop >= datalen){
				if(end == readbuf[iLoop + datalen - 1]){
					*start_index = iLoop;
					*end_index 	= iLoop + datalen - 1;
					return 1;
				}
			}else{
				return 0;
			}
		}
	}
	return 0;
}
#if 0
//天泽VOCs
void Protocol_2(int port,int meter,int Address,int Dec,QString Name,QString Code,QString Unit)
{
    u_char sendbuf[8];
    u_char readbuf[UART_READ_LEN];
    uint16_t check;
    size_t nread;
    
    sendbuf[0] = pgPara->MeterPara[meter].Address;
    sendbuf[1] = 0x03;
    sendbuf[2] = 0x00;
    sendbuf[3] = 0x00;
    sendbuf[4] = 0x00;
    sendbuf[5] = 0x10;
    check = CRC16_Modbus(sendbuf,6);
    sendbuf[6] = (u_char)(check & 0xff);
    sendbuf[7] = (u_char)((check>>8) & 0xff);
    uart_write(&pgPara->SerialPara[port],sendbuf,sizeof(sendbuf));
    usleep(pgPara->SerialPara[port].TimeOut * 1000);

    nread=read(pgPara->SerialPara[port].Devfd,readbuf,UART_READ_LEN);
    
    double rtd=0;
    QString flag="D";
    uchar s[4];
    float f1,f2;
    QByteArray readbuf;
    QByteArray sendbuf;

    sendbuf.resize(8);
    sendbuf[0]=Address;
    sendbuf[1]=0x03;
    sendbuf[2]=0x00;
    sendbuf[3]=0x00;
    sendbuf[4]=0x00;
    sendbuf[5]=0x10;
    int check = myHelper::CRC16_Modbus(sendbuf.data(),6);
    sendbuf[6]=(char)(check);
    sendbuf[7]=(char)(check>>8);
    myCom[port]->write(sendbuf);
    usleep(COM[port].Timeout*1000);
    readbuf=myCom[port]->readAll();
    if(readbuf.length()>=37){
        qDebug()<<QString("COM%1 received:").arg(port+2)<<readbuf.toHex().toUpper();
        s[0]=readbuf[6];
        s[1]=readbuf[5];
        s[2]=readbuf[4];
        s[3]=readbuf[3];
        f1=*(float *)s;//采集浓度,低字节前

        s[0]=readbuf[34];
        s[1]=readbuf[33];
        s[2]=readbuf[32];
        s[3]=readbuf[31];
        f2=*(float *)s;//气体系数
        flag='N';
        rtd=f1*f2;
    }
    CacheDataProc(rtd,0,flag,Dec,Name,Code,Unit);
}
#endif




/*SJFC-200粉尘*/
void Protocol_1(int port,pstMeterPara pMeterPara){
    static uint8_t start_flag = 1;
    UINT16 readbuf[6];
    int result;
    if(!start_flag){        
    result = mbusMaster_ReadMultipleRegisters( pgPara->SerialPara[port].Devfd, pMeterPara->Address, 4007, readbuf, 6); 
        if (result == 0){ 
            printf("SJ [%d]: [%d]\n", readbuf[0],readbuf[1]); 
        } else{ 
            printf( "SJ ERR!\n"); 
        } 
    }else{
        if(0 == mbusMaster_WriteSingleRegister(pgPara->SerialPara[port].Devfd, pMeterPara->Address, 40034,1)){
            start_flag = 0;
            sleep(60); //等待做样完成
        }
        
    }
    start_flag = 0;
}

