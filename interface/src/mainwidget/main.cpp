/*************************************************
Copyright:kevin
Author:Kevin LiQi
Date:2015-12-01
Email:kevinlq0912@163.com
QQ:936563422
Version:V1.0
Description:智能家居---主程序，加载配置信息、程序样式，设置编码方式
**************************************************/
#include <QApplication>
#include <QMetaType>
//#include <QSharedMemory>
//#include <QDir>
//#include <QTextCodec>

#include "widget.h"
#include "myhelper.h"
#include "myapp.h"
//#include "database.h"
//#include "control.h"
//#include "logindialog.h"
//#include "systemsettingdialog.h"
//#include "analysisdata.h"
#include "frminput.h"
#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif

#if __ARM__
#include <QWSServer>
#endif

#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
pstPara pgPara;
pstValveControl pgValveControl;
pstPollutantData pgPollutantData;
pstPollutantPara pgPollutantPara;
pstHistoryData pgHistoryData;
pstData pgData;
pstCalibrationPara pgCalibrationPara;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if __ARM__
    QWSServer::setCursorVisible(false);
#endif

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    Myapp::ReadConfig();                                  //读配置文件
    myHelper::setUTF8Code();                              //设置编码方式
    myHelper::setStyle(":/images/css/style.css");         //加载样式表
    myHelper::setChinese(":/images/translator/zh_CN.qm");
    myHelper::setChinese(":/images/qt_zh_CN.qm");         //加载中文字符

    //赋值当前应用程序路径和桌面宽度高度
    Myapp::AppPath=QApplication::applicationDirPath()+"/";
    qDebug()<<"AppPath1:"<<Myapp::AppPath;
    Myapp::DeskWidth=qApp->desktop()->availableGeometry().width();
    Myapp::DeskHeigth=qApp->desktop()->availableGeometry().height();
    qDebug()<<QString("DeskWidth:%1 DeskHeigth:%2").arg(Myapp::DeskWidth).arg(Myapp::DeskHeigth);

    /********共享内存******************/
    pgPara = (pstPara)getParaShm();
    qDebug()<<QString("AlarmTime:%1").arg(pgPara->GeneralPara.AlarmTime);
    pgValveControl = (pstValveControl)getValveParaShm();
    qDebug()<<QString("per:%1 per_last:%2").arg(pgValveControl->per).arg(pgValveControl->per_last);
    pgPollutantData = (pstPollutantData)getPollutantDataShm();
    pgPollutantPara = (pstPollutantPara)getPollutantParaShm();
    pgHistoryData = (pstHistoryData)getHistoryDataShm();
    pgData = (pstData)getDataShm();
    pgCalibrationPara = (pstCalibrationPara)getCalibrationParaShm();
    /**/

    Widget w;
    w.show();
    frmInput::Instance()->Init("control", "black", 10, 10);
    return a.exec();
}
