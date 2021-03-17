#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>

#include "tinz_common_helper.h"
#include "tinz_base_def.h"

const int auchCRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
} ;

const int auchCRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};

uint16_t CRC16_Modbus(char *databuff, int len)
{
    uint8_t uchCRCHi = 0xFF ;
    uint8_t uchCRCLo = 0xFF ;
    int uIndex ;
    while (len--)
    {
        uIndex = uchCRCLo ^ *databuff++ ;
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex] ;
        uchCRCHi = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}

//GB212 CRC校验
int CRC16_GB212(char *databuff, int len)
{
    int  wkg = 0xFFFF;
	char i;

    while( len-- )
    {
       wkg  = (wkg>>8)&0x00FF;
       wkg ^= *databuff++;

       for (i = 0 ; i < 8; i++ )
       {
          if ( wkg & 0x0001 )
          {
            wkg = ( wkg >> 1 ) ^ 0xa001;
          }
          else
          {
            wkg = wkg >> 1;
          }
       }
    }
    return( wkg );
}

//RTU CRC校验
uint16_t CRC16_RTU(uint8_t *databuff, uint16_t len)
{
    uint16_t  c,crc = 0xFFFF;
	uint16_t iLoop,jLoop;

    for(iLoop=0;iLoop<len;iLoop++)
    {
        c = *(databuff+iLoop) & 0x00ff;
        crc ^= c;

       for (jLoop = 0 ; jLoop < 8; jLoop++ )
       {
          if ( crc & 0x0001 )
          {
            crc = ( crc >> 1 ) ^ 0xa001;
          }
          else
          {
            crc = crc >> 1;
          }
       }
    }
    return( crc );
}

//异或校验
//从第index位开始进行校验
u_char XORValid(char *buffer ,int len)
{
    u_char checksum = 0;
	int i;
    //char ch1,c1; //校验位的高四位和第四位
    for (i = 0;  i<len;  i++){
    checksum ^=  buffer[i]; //进行异或交验取值
    }
    return checksum;
 }

u_char HexToAscii(u_char data_hex){
	return (data_hex < 10) ? data_hex+'0' : (data_hex - 10)+'A';
}
void DoubleToString(double value, int Decimals, char *buf){
	switch(Decimals){
		case 1:
			snprintf(buf, DOUBLE_STRING_LEN-1, "%.1f", value);
			break;
		case 2:
			snprintf(buf, DOUBLE_STRING_LEN-1, "%.2f", value);
			break;
		case 3:
			snprintf(buf, DOUBLE_STRING_LEN-1, "%.3f", value);
			break;
		case 4:
			snprintf(buf, DOUBLE_STRING_LEN-1, "%.4f", value);
			break;
		default:
			snprintf(buf, DOUBLE_STRING_LEN-1, "%.2f", value);
			break;
	}
}

inline uint8_t 
uint16_div_uint16_to_rate(uint16_t a, uint16_t b)
{
    return b ? a / (float) b * 100.00: 0;
}

inline double 
double_div_uint(double a, unsigned int b)
{
    return b ? a / b : 0;
}

inline uint32_t uint32_sub_uint32(uint32_t a, uint32_t b)
{
    return a>b ? a - b : 0;
}

inline uint32_t uint32_div_uint32(uint32_t a, uint32_t b)
{
    return b ? a / b : 0;
}

inline int32_t int32_div_uint32(int32_t a, int32_t b)
{
    return b ? a / b : 0;
}

inline uint32_t uint64_div_uint32(uint64_t a, uint32_t b)
{
    return b ? a / b : 0;
}

inline int32_t int64_div_uint32(int64_t a, int32_t b)
{
    return b ? a / b : 0;
}

inline float uint32_div_float(uint32_t a, float b)
{
    return b ? a / b : 0;
}


inline int cmpfunc_uint16(const void * a, const void * b)
{
   return ( *(uint16_t*)a - *(uint16_t*)b );
}

//返回当前季度的第一个月份
inline uint8_t month_to_qut(uint8_t month){
    if(month >0 && month <4){
        return 1;
    }else if(month >3 && month <7){
        return 4;
    }else if(month >6 && month <10){
        return 7;
    }else if(month >9 && month <=12){
        return 10;
    }
    return TINZ_ERROR;
}
inline void maxmin_uint32(uint32_t *max,uint32_t *min,uint32_t data){
    if(0 == *min){*min = data;}
    if(*min > data){*min = data;}
    if(*max < data){*max = data;}
}
inline void maxmin_int32(int32_t *max,int32_t *min,int32_t data){
    if(0 == *min){*min = data;}
    if(*min > data){*min = data;}
    if(*max < data){*max = data;}
}

inline void maxmin_uint16(uint16_t *max,uint16_t *min,uint16_t data){
    if(0 == *min){*min = data;}
    if(*min > data){*min = data;}
    if(*max < data){*max = data;}
}

inline void cou_uint32(uint32_t *start,uint32_t *end,uint32_t data){
    if(0 == *start){*start = data;}  //系统启动 取第一次采集值
    if(data > *end){*end = data;}
}

int get_system_output(char *cmd, char *output,int size){
    FILE *fp=NULL;
    fp = popen(cmd,"r");
    if(fp){
        if(fgets(output,size,fp)!=NULL){
            if(output[strlen(output)-1] == '\n'){
                output[strlen(output)-1] = '\0';
            }
        }
        pclose(fp);
    }
    return TINZ_OK;
}

int tinz_float_to_string(char *pBuf,int len,uint8_t Decimals,float data){
    switch(Decimals){
        case 0:
            return snprintf(pBuf,len,"%d",(int)data);
            break;
        case 1:     
            return snprintf(pBuf,len,"%.1f",data);
            break;
        default:
            return snprintf(pBuf,len,"%.2f",data);
    }    
}


