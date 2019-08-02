#ifndef MYAPP_H
#define MYAPP_H
#include <QString>

class Myapp
{
public:
    static QString AppPath;             //应用程序路径
    static int DeskWidth;               //桌面宽度
    static int DeskHeigth;              //桌面高度

    static int UserType;                //用户类型       1:企业用户      2：运维员 3：管理用户
    static QString UserName;                //用户名

    static QString SoftTitle ;          //软件标题
    static QString SoftVersion;         //软件版本
    static QString CompanyName;         //开发商名称
    static QString ContactName;         //联系人
    static QString ContactTel;          //联系电话

    static QString LocalIP;                  //设备IP
    static QString Mask;                //设备子网掩码
    static QString GateWay;             //设备网关
    static QString DHCP;                   //DHCP服务

    static void ReadConfig();           //读取配置文件,在main函数最开始加载程序载入
    static void WriteConfig();          //写入配置文件,在更改配置文件、程序关闭时调用
    static void ReadLocalNet();
    static void WriteLocalNet();
};

#endif // MYAPP_H
