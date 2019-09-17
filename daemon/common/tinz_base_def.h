#ifndef __TINZ_BASE_DEF__
#define __TINZ_BASE_DEF__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define VERSION      "1.0.0"

#define 	FS_NAME_PARADIR			"/mnt/nandflash/para"
#define 	FS_NAME_PROGDIR			"/mnt/nandflash/bin"
#define 	FS_NAME_SHMDIR			"/mnt/nandflash/shm"

#define 	WATCHDOG_CONFIG_FILE	"/mnt/nandflash/para/watchdog.conf"

#define TINZ_ERROR -1
#define TINZ_OK		1
#define TINZ_BUSY   2

#define MN_LEN 25
#define PW_LEN 7


#define METER_NAME_LEN  20
#define CODE_LEN        7
#define UNIT_LEN        6

//#define USER_NAME_LEN   10
#define USER_PWD_LEN    20

#define UART_DEVNAME_LEN 12

#define METER_CNT   32
#define SERIAL_CNT  5
#define SITE_CNT    4
#define SITE_SEND_CNT    (SITE_CNT + 1)
#define USER_CNT    3

#define QY_USER  1
#define GLY_USER 2
#define SUPER_USER 3

#define SYSTEMTIME_LEN     15
#define DATATIME_LEN     15

#define POLLUTANT_CNT     3
#define POLLUTANT_FLOW_INDEX     0
#define POLLUTANT_COD_INDEX     1
#define POLLUTANT_PH_INDEX     2

#define HISTORYCNT 180

#define AD_CNT 2
#define DA_CNT 2

#define POWERUP 0
#define POWERDOWN 1

#define 	MAX_FILENAME_SIZE 	256

#define MESSAGECNT 64

#define MSGBUF_IS_NULL 0
#define MSGBUF_IS_WRITEING 1
#define MSGBUF_IS_SENDING 2
#define MSGBUF_IS_WAITING 3

#define MAX_TCPDATA_LEN 1037
#define MIN_TCPDATA_LEN 80

#define QN_LEN	18


#define BIN4BCD(val) ((((val)/1000)<<12)+((((val)%1000)/100)<<8)+((((val)%1000)%100)/10<<4)+(val)%10)
#define BIN2BCD(val) ((((val)/10)<<4) + (val)%10)
#define BCD4BIN(val) (((val)&15)+(((val)>>4)&15)*10+(((val)>>8)&15)*100+(((val)>>12)&15)*1000)
#define BCD2BIN(val) (((val)&15) + ((val)>>4)*10)

typedef struct _PollutantParaRow{
    uint32_t MonAll;
    uint32_t QutAll;
    uint32_t YeaAll;
}stPollutantParaRow,*pstPollutantParaRow;

typedef struct _PollutantPara{
    stPollutantParaRow Row[POLLUTANT_CNT];
}stPollutantPara,*pstPollutantPara;


typedef struct _GeneralPara
{
    char    MN[MN_LEN];
	u_char    PW[PW_LEN];
    uint16_t  RtdInterval;            //实时数据间隔（s）
    //uint8_t   MinInterval;            //分钟数据间隔（min）
    //uint8_t   CatchmentTime;          //集水时间（min）
    //uint8_t   COD_CollectInterval;    //COD采集数据间隔（min）
    uint8_t   OverTime;               //超时时间（s）
    uint8_t   ReCount;                //超时重发次数
    uint16_t  AlarmTime;              //超限报警时间
    uint8_t   StType;                 //污染源类型
    uint8_t   RespondOpen;            //上位机应答
}stGeneralPara,*pstGeneralPara;

typedef struct _MeterPara
{
    uint8_t		isValid;
    u_char		Name[METER_NAME_LEN];       //因子名称  , 如电导率
    u_char  	Code[CODE_LEN];             //上报代码      w01014
    u_char  	Unit[UNIT_LEN];             //单位，如m3/h
    uint8_t   	UseChannel:6;                 //通道号
    uint8_t   	UseChannelType:2; //通道类型  1:串口 2:模拟
    uint8_t   	Address;            //设备地址
    uint8_t   	Protocol;           //协议号
    uint8_t   	Signal;             //接入信号，AD 模拟电压转实际值所用
    float   	RangeUp;            //量程上限
    float   	RangeLow;           //量程下限
    float   	AlarmUp;            //报警上限
    float   	AlarmLow;           //报警下限
    uint8_t   	MaxFlag:1;          //最大值标志
    uint8_t   	MinFlag:1;          //最小值标志
    uint8_t   	AvgFlag:1;          //平均值标志
    uint8_t   	CouFlag:1;          //累积值标志
    uint8_t   	Decimals:4;         //小数位数 
    char		flag;
    float		Rtd;
	double		total;
}stMeterPara,*pstMeterPara;

typedef struct _SerialPara
{
    uint8_t   isServerOpen;         // 0 关闭 1 打开
    uint8_t   isRS485;              // 0 232 1 485
    u_char    DevName[UART_DEVNAME_LEN];       //串口名称
    uint16_t  BaudRate;       //串口波特率
    uint8_t   DataBits;       //串口数据位
    uint8_t   Parity;         //串口校验位       0 无校验 1 奇校验 2 偶校验
    uint8_t   StopBits;       //串口停止位    
    uint8_t   FlowCtrl;       //流控制 
//    int Protocol;       //串口协议索引
//    bool use;           //串口是否使用
    int     TimeOut;        //串口读取超时(ms)
    int     Interval;       //串口通讯周期(ms)
//    int HardwareAddress;//串口硬件地址
    int     Devfd;
}stSerialPara,*pstSerialPara;

typedef struct _IOPara
{
    int    Out_drain_open;         //开排水阀
    int    Out_drain_close;        //关排水阀
    int    Out_drain_common;        //排水阀总开关
    //volatile int    Out_catchment_open;     //开集水阀
    //volatile int    Out_catchment_close;    //关集水阀
    //volatile int    Out_reflux_control;     //回流泵控制

    //volatile int     In_drain_open;         //输入检测排水阀开
    //volatile int     In_drain_close;        //输入检测排水阀关
    //volatile int     In_catchment_open;     //输入检测集水阀开
    //volatile int     In_catchment_close;    //输入检测集水阀关
    //volatile int     In_reflux_open;        //输入检测回流泵开
    //volatile int     In_reflux_close;       //输入检测回流泵关
    int     In_power;              //输入检测市电
}stIOPara,*pstIOPara;

typedef struct _SitePara
{   
    uint8_t   ServerOpen;   
    uint8_t   isConnected;  
    uint8_t   SiteNum;
    uint16_t  ServerPort; //服务器端口
    char  ServerIp[16];   //服务器IP地址
}stSitePara,*pstSitePara;

typedef struct _NetPara
{   
    uint8_t VPNOpen;            //VPN 客户端开启
    char  VPNServerIp[16];     //VPN 服务器IP
    char  VPNUserName[16];     //VPN 用户IP
    char  VPNIPIP[16];         //隧道IP
    
}stNetPara,*pstNetPara;

typedef struct _UserPara
{
    uint8_t   UserType;       //用户类型       1:企业用户      2：运维员 3：管理用户 
    int  UserPwd;
}stUserPara,*pstUserPara;

typedef struct _Para
{
    uint8_t               Mode;             //0:远程模式   1:运维模式
    stGeneralPara   GeneralPara; 
    //stMeterPara     MeterPara[METER_CNT];
    stSerialPara    SerialPara[SERIAL_CNT];
    stIOPara        IOPara;
    stSitePara      SitePara[SITE_CNT]; 
    stNetPara       NetPara;
    stUserPara      UserPara[USER_CNT];
}stPara,*pstPara;

typedef struct _ValveControl
{
    uint8_t     per;
    uint8_t     per_measure;
    uint8_t     per_last;
    uint8_t     channel;                   //AD模拟通道
    uint8_t     OutMode;                   //0:模拟量 1:开关量
    uint16_t    OutValueAdjust[3];        //0%、50%、100%  

    uint16_t    InValueAdjust[3];         //0%、50%、100%
}stValveControl,*pstValveControl;

typedef struct _CalibrationPara
{
    uint16_t AdAdjustValue[AD_CNT][3];             //AD校准值
    uint16_t DaAdjustValue[DA_CNT][3];             //DA校准值

}stCalibrationPara,*pstCalibrationPara;

typedef struct _Event
{
    char  DataTime[DATATIME_LEN];
    char  Info[MAX_TCPDATA_LEN];
}stEvent,*pstEvent;

#endif

