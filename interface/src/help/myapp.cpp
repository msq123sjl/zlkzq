#include "myapp.h"
#include "myhelper.h"
#include <QSettings>

//设置程序进行过程中需要的全局变量
QString Myapp::AppPath = "";

int Myapp::UserType = 0;
QString Myapp::UserName = "";

int Myapp::DeskWidth = 1024;
int Myapp::DeskHeigth = 768;
//QString Myapp::winPortName = "COM5";                   //Win默认串口号
//QString Myapp::linuxPortName = "ttyUSB0";               //linux默认串口号
//int Myapp::baudRate = 9600;
//bool Myapp::AutoRun = true;


////默认所有的灯都关闭
//bool Myapp::bed_room_led_left   = false;
//bool Myapp::bed_room_led_right  = false;
//bool Myapp::parlour_led_main    = false;
//bool Myapp::parlour_led_top     = false;
//bool Myapp::parlour_led_help    = false;
//bool Myapp::kitchen_led         = false;
//bool Myapp::curtain_status      = false;

//int Myapp::setTempValue = 30;
//int Myapp::setHumValue = 30;
//int Myapp::setSmokeValue = 20;

//存储全局配置信息,程序启动时加载,关闭时保存
QString Myapp::SoftTitle = QString::fromLocal8Bit("总量控制器");
QString Myapp::SoftVersion = "V1.0";
QString Myapp::CompanyName = QString::fromLocal8Bit("江苏尚维斯环境科技有限公司");
QString Myapp::ContactName = QString::fromLocal8Bit("admin");
QString Myapp::ContactTel = "1111111111";

QString Myapp::LocalIP="192.168.1.112";
QString Myapp::Mask="255.255.255.0";
QString Myapp::GateWay="192.168.1.1";
QString Myapp::DHCP="0";

void Myapp::ReadConfig()
{
    QString fileName = Myapp::AppPath + "config.ini";
    //如果配置文件不存在,则以初始值继续运行
    //没有这个判断的话,配置赋值都为空
    if (!myHelper::FileIsExist(fileName)) {
        //对应中文转成正确的编码
        Myapp::SoftTitle = Myapp::SoftTitle;
        Myapp::CompanyName = Myapp::CompanyName;
        Myapp::ContactName = Myapp::ContactName;
        return;
    }

    QSettings *set=new QSettings(fileName,QSettings::IniFormat);
    set->setIniCodec("UTF-8");

    set->beginGroup("AppConfig");

    Myapp::SoftTitle = set->value("SoftTitle").toString();
    Myapp::SoftVersion = set->value("SoftVersion").toString();
    Myapp::CompanyName = set->value("CompanyName").toString();
    Myapp::ContactName = set->value("ContactName").toString();
    Myapp::ContactTel = set->value("ContactTel").toString();

//    Myapp::winPortName = set->value("winPortName").toString();
//    Myapp::linuxPortName = set->value("linuxPortName").toString();
//    Myapp::baudRate = set->value("baudRate").toInt();

//    Myapp::bed_room_led_left = set->value("bed_room_led_left").toBool();
//    Myapp::bed_room_led_right = set->value("bed_room_led_right").toBool();
//    Myapp::parlour_led_main = set->value("parlour_led_main").toBool();
//    Myapp::parlour_led_top = set->value("parlour_led_top").toBool();
//    Myapp::parlour_led_help = set->value("parlour_led_help").toBool();
//    Myapp::kitchen_led = set->value("kitchen_led").toBool();
//    Myapp::curtain_status = set->value("curtain_status").toBool();
//    Myapp::setHumValue = set->value("setHumValue").toInt();
//    Myapp::setTempValue = set->value("setTempValue").toInt();
//    Myapp::setSmokeValue = set->value("setSmokeValue").toInt();
    set->endGroup();
}

void Myapp::WriteConfig()
{
    QString fileName = Myapp::AppPath + "config.ini";
    QSettings *set = new QSettings(fileName,QSettings::IniFormat);
    set->setIniCodec("UTF-8");
    set->beginGroup("AppConfig");

    set->setValue("SoftTitle",Myapp::SoftTitle);
    set->setValue("SoftVersion",Myapp::SoftVersion);
    set->setValue("CompanyName",Myapp::CompanyName);
    set->setValue("ContactName",Myapp::ContactName);
    set->setValue("ContactTel",Myapp::ContactTel);

//    set->setValue("winPortName",Myapp::winPortName);
//    set->setValue("linuxPortName",Myapp::linuxPortName);
//    set->setValue("baudRate",Myapp::baudRate);

//    set->setValue("bed_room_led_left",Myapp::bed_room_led_left);
//    set->setValue("bed_room_led_right",Myapp::bed_room_led_right);
//    set->setValue("parlour_led_main",Myapp::parlour_led_main);
//    set->setValue("parlour_led_top",Myapp::parlour_led_top);
//    set->setValue("parlour_led_help",Myapp::parlour_led_help);
//    set->setValue("kitchen_led",Myapp::kitchen_led);
//    set->setValue("curtain_status",Myapp::curtain_status);

//    set->setValue("setHumValue",Myapp::setHumValue);
//    set->setValue("setTempValue",Myapp::setTempValue);
//    set->setValue("setSmokeValue",Myapp::setSmokeValue);

    set->endGroup();
}

void Myapp::ReadLocalNet()
{
    QString fileName=Myapp::AppPath+"../userinfo.txt";
    QSettings *set=new QSettings(fileName,QSettings::IniFormat);

    set->beginGroup("LOCAL_MACHINE");
    Myapp::DHCP=set->value("DHCP").toString();
    Myapp::GateWay=set->value("DefaultGateway").toString();
    Myapp::LocalIP=set->value("IPAddress").toString();
    Myapp::Mask=set->value("SubnetMask").toString();

    set->endGroup();
    delete set;

}

void Myapp::WriteLocalNet()
{
    QString strAll;
    QFile readfile(Myapp::AppPath+"../userinfo.txt");
    if(readfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&readfile);
            strAll=stream.readAll();
    }
 readfile.close();
  QStringList strList;
 QFile writeFile(Myapp::AppPath+"userinfo.txt");
 if(writeFile.open(QIODevice::WriteOnly|QIODevice::Text))
 {
         QTextStream stream(&writeFile);
         strList=strAll.split("\n");
         for(int i=0;i<strList.count();i++)
         {
             if(i==strList.count()-1)
             {
                 stream<<strList.at(i);
             }
             else
             {
                 stream<<strList.at(i)<<'\n';
             }

             if(strList.at(i).contains("[LOCAL_MACHINE]"))
             {

                 QString tempStr=strList.at(i+1);
                 tempStr.replace(0,tempStr.length(),"DHCP=\""+Myapp::DHCP+'\"');
                 stream<<tempStr<<'\n';
                 tempStr=strList.at(i+2);
                 tempStr.replace(0,tempStr.length(),"DefaultGateway=\""+Myapp::GateWay+'\"');
                 stream<<tempStr<<'\n';
                 tempStr=strList.at(i+3);
                 tempStr.replace(0,tempStr.length(),"IPAddress=\""+Myapp::LocalIP+'\"');
                 stream<<tempStr<<'\n';
                 tempStr=strList.at(i+4);
                 tempStr.replace(0,tempStr.length(),"SubnetMask=\""+Myapp::Mask+'\"');
                 stream<<tempStr<<'\n';
                 i+=4;
             }
         }
 }
 writeFile.close();
}

