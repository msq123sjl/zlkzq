#ifndef __TINZ_COMMON_HELPER__
#define __TINZ_COMMON_HELPER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DOUBLE_STRING_LEN 32

#define DEBUG_LEVEL	4
#define INFO_LEVEL	3 
#define WARN_LEVEL	2
#define ERR_LEVEL	1 

#define ESM3352
//#define ESM6800
#ifdef ESM3352
    #define DEV_GPIO "/dev/em335x_gpio"
#endif 
#ifdef ESM6800
    #define DEV_GPIO "/dev/esm6800_gpio"
#endif 

#define SHOW_DEBUG 1
#if SHOW_DEBUG
    #define DEBUG_PRINT_DEBUG(Level,...) {\
		if(DEBUG_LEVEL <= Level){\
			printf(__VA_ARGS__); \
		}\
	}
	#define DEBUG_PRINT_INFO(Level,...) {\
		if(INFO_LEVEL <= Level){\
			printf(__VA_ARGS__); \
		}\
	}
	#define DEBUG_PRINT_WARN(Level,...) {\
		if(WARN_LEVEL <= Level){\
			printf(__VA_ARGS__); \
		}\
	}
#else
    #define DEBUG_PRINT_DEBUG(Level,...)
	#define DEBUG_PRINT_INFO(Level,...)
	#define DEBUG_PRINT_WARN(Level,...)
#endif

#define DEBUG_PRINT_ERR(Level,...) {\
	if(ERR_LEVEL <= Level){\
		printf(__VA_ARGS__); \
	}\
}

#define HEXTOASCII(data_hex) ((data_hex < 10) ? data_hex + '0' : (data_hex - 10) + 'A')
#define ASCIITOHEX(high,low) (((high > 'A') ? (high - 'A' + 10) : (high - '0')) << 4 | ((low > 'A') ? (low - 'A' + 10) : (low - '0')))

#define tolower(c)      (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define toupper(c)      (u_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)
#define HEXCHECK(c)      (u_char) (((c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) ? 1 : 0)

#define TINZ_MAX(a, b)       (((a) > (b) ) ? (a) : (b))
#define TINZ_MIN(a, b)       (((a) < (b) ) ? (a) : (b))
#define TINZ_MID(a,b,c)      (TINZ_MAX(a,b)>c?TINZ_MAX(TINZ_MIN(a,b),c):TINZ_MIN(TINZ_MAX(a,b),c))

int CRC16_GB212(char *databuff, int len);
uint16_t CRC16_Modbus(char *databuff, int len);
uint16_t CRC16_RTU(uint8_t *databuff, uint16_t len);
u_char XORValid(char *buffer ,int len);
u_char HexToAscii(u_char data_hex);
void DoubleToString(double value, int Decimals, char *buf);
inline double double_div_uint(double a, unsigned int b);
inline uint32_t uint32_sub_uint32(uint32_t a, uint32_t b);
inline int cmpfunc_uint16(const void * a, const void * b);
inline uint8_t month_to_qut(uint8_t month);
inline float uint32_div_float(uint32_t a, float b);
inline uint32_t uint64_div_uint32(uint64_t a, uint32_t b);
inline uint32_t uint32_div_uint32(uint32_t a, uint32_t b);
inline void maxmin_uint32(uint32_t *max,uint32_t *min,uint32_t data);
inline void maxmin_uint16(uint16_t *max,uint16_t *min,uint16_t data);
inline void cou_uint32(uint32_t *start,uint32_t *end,uint32_t data);
int get_system_output(char *cmd, char *output,int size);
inline int32_t int32_div_uint32(int32_t a, int32_t b);
inline int32_t int64_div_uint32(int64_t a, int32_t b);
inline void maxmin_int32(int32_t *max,int32_t *min,int32_t data);
inline uint8_t uint16_div_uint16_to_rate(uint16_t a, uint16_t b);
#endif