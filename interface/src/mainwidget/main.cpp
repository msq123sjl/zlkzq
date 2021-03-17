/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--主程序，加载配置信息、程序样式，设置编码方式
**************************************************/
#include <QApplication>
#include <QMetaType>

#include "widget.h"
#include "myhelper.h"
#include "myapp.h"
#include "frminput.h"


#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif

#if __ARM__
#include <QWSServer>
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_message.h"
}
pstPara pgPara;
pstValveControl pgValveControl;
pstHistoryData pgHistoryData;
pstData pgData;
pstNetPara pgNetPara;
pstCalibrationPara pgCalibrationPara;
struct _msg *pmsg_interface;
struct _msg *pmsg_interface_to_control;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if __ARM__
    QWSServer::setCursorVisible(false);
#endif
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
    Myapp::ReadConfig();                                  //读配置文件
    myHelper::setUTF8Code();                              //设置编码方式
    myHelper::setStyle(":/images/css/style.css");         //加载样式表
    myHelper::setChinese(":/images/translator/zh_CN.qm");
    myHelper::setChinese(":/images/qt_zh_CN.qm");         //加载中文字符
    //赋值当前应用程序路径和桌面宽度高度
    Myapp::AppPath=QApplication::applicationDirPath()+"/";
    Myapp::DeskWidth=qApp->desktop()->availableGeometry().width();
    Myapp::DeskHeigth=qApp->desktop()->availableGeometry().height();
    qDebug()<<QString("DeskWidth:%1 DeskHeigth:%2 AppPath:%3").arg(Myapp::DeskWidth).arg(Myapp::DeskHeigth).arg(Myapp::AppPath);
    /********共享内存******************/
    pgPara = (pstPara)getParaShm();
    pgNetPara = (pstNetPara)getNetParaShm();
    pgValveControl = (pstValveControl)getValveParaShm();
    qDebug()<<QString("per:%1 per_alarm:%2").arg(pgValveControl->per).arg(pgValveControl->per_alarm);
    pgHistoryData = (pstHistoryData)getHistoryDataShm();
    pgData = (pstData)getDataShm();
    pgCalibrationPara = (pstCalibrationPara)getCalibrationParaShm();
    /*消息队列*/
    pmsg_interface = InterfaceMessageInit(pmsg_interface);
    pmsg_interface_to_control = InterfaceToControlMessageInit(pmsg_interface_to_control);
    Widget w;
    w.show();
    frmInput::Instance()->Init("control", "black", 10, 10);
    return a.exec();
    free(pmsg_interface);
    free(pmsg_interface_to_control);
}
