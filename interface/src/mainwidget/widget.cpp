/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--主界面功能的实现
**************************************************/
#include "widget.h"
#include "ui_widget.h"
//#include "iconhelper.h"
#include <QKeyEvent>
#include <QToolButton>
#include <QThread>
#include <QDebug>
//#include "control.h"
#include "myhelper.h"
#include "myapp.h"

#include "frmlogin.h"
#include "frmconfig.h"
#include "rtdwidget.h"          //实时数据
#include "flowrtdwidget.h"          //流量
#include "codrtdwidget.h"          //COD
#include "phrtdwidget.h"          //PH
#include "valvewidget.h"          //阀门
#include "statisticwidget.h"        //统计界面
#include "modelchoosewidget.h"      //模式选择控制界面
#include "adcalibrationwidget.h"          //AD校准
#include "dacalibrationwidget.h"          //DA校准

QStringList weekcn;
//char *weekcn[] = {"一","一","二","三","四","五","六","日"};
extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
#include "tinz_pub_message.h"
}
extern pstPara pgPara;
extern struct _msg *pmsg_interface;

int blk_time = 120;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);  
    this->initDataTime();   
    this->initForm();    
	this->initWidget();
    this->initToolTip();
    system("echo 0 > /sys/class/backlight/backlight/brightness");
    weekcn <<"一"<<"二"<<"三"<<"四"<<"五"<<"六"<<"日";
}

Widget::~Widget()
{
    delete ui;
    this->deletWidget();
}

void Widget::initDataTime()
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(slotShowCurrentDataTime()));
    m_timer->start(1000);
}

//界面样式初始化
void Widget::initForm()
{
    //设置窗体标题栏隐藏
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint
                         | Qt::WindowMinMaxButtonsHint);
#else
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#endif
    //this->setWindowTitle(tr("总量控制器"));
    //this->setWindowState(Qt::WindowFullScreen);
    //顶部设置和首页的样式
    setToolButtonStyle(ui->tbnSetting,"设置",E_NORMAL,":/images/tool/setting_normal.png");
    setToolButtonStyle(ui->tbnHome,"首页",E_NORMAL,":/images/tool/home_normal.png");
    setStyleSheet(QLatin1String("QToolButton{border:0px;}"));
    //中间部分的样式
    setToolButtonStyle(ui->tbnFlow,"流量",E_BIG,":/images/midwidget/Bedroom.png");
    setToolButtonStyle(ui->tbnCOD,"COD",E_BIG,":/images/midwidget/Parlor.png");
    setToolButtonStyle(ui->tbnPH,"PH",E_BIG,":/images/midwidget/kitchen.png");
    setToolButtonStyle(ui->tbnSafety,"NULL",E_BIG,":/images/midwidget/Safety.png");
    setToolButtonStyle(ui->tbnAD,"AD校准",E_BIG,":/images/midwidget/Bedroom.png");
    setToolButtonStyle(ui->tbnDA,"DA校准",E_BIG,":/images/midwidget/Parlor.png");
    setToolButtonStyle(ui->tbnValveControl,"阀门",E_BIG,":/images/midwidget/kitchen.png");
    //底部菜单的样式
    setToolButtonStyle(ui->tbnRtd,"实时",E_NORMAL,
                       ":/images/bottom/control.png");
    setToolButtonStyle(ui->tbnPolluter,"排量",E_NORMAL,
                       ":/images/bottom/video.png");
    setToolButtonStyle(ui->tbnStatistic,"统计",E_NORMAL,
                       ":/images/bottom/statistics.png");    
    setToolButtonStyle(ui->tbnCalibration,"校准",E_NORMAL,
                       ":/images/bottom/night.png");
    setToolButtonStyle(ui->tbnModel,"模式",E_NORMAL,
                       ":/images/bottom/reset.png");
    setToolButtonStyle(ui->tbnUser,"用户",E_NORMAL,
                       ":/images/login.png");
    //    this->setCurrentWidget(E_HOME_WIDGET);
    ui->stackedWidget->setCurrentIndex(E_HOME_WIDGET);
    m_isPress = false;
   // location = this->location;

    /* ui->tbnRelaxation->setVisible(false);
    ui->tbnMusic->setVisible(false);
    ui->tbnCurtain->setVisible(false);
    ui->tbnNight->setVisible(false); */
    ui->tbnNull->setVisible(false);
    ui->tbnSafety->setVisible(false);
}
//界面初始化
void Widget::initWidget()
{
    m_rtdWidget = new Rtdwidget;              //实时数据
    m_flowrtdWidget = new FlowRtdwidget;              //流量
    m_codrtdWidget = new CODRtdwidget;              //COD
    m_phrtdWidget = new PHRtdwidget;              //PH
    m_valveWidget = new ValveWidget;              //阀门
    m_statisticWidget = new StatisticWidget;          //统计
    m_modelWidget = new ModelChooseWidget;            //模式控制界面
    m_adcalibrationWidget = new AdCalibrationWidget;              //AD校准
    m_dacalibrationWidget = new DaCalibrationWidget;              //DA校准
    //m_menuWidget = new MenuWidget(this);
    //ui->tbnSetting->setMenu(m_menuWidget);

    ui->stackedWidget->addWidget(m_rtdWidget);
    ui->stackedWidget->addWidget(m_flowrtdWidget);
    ui->stackedWidget->addWidget(m_codrtdWidget);
    ui->stackedWidget->addWidget(m_phrtdWidget);
    ui->stackedWidget->addWidget(m_valveWidget);
    ui->stackedWidget->addWidget(m_statisticWidget);
    ui->stackedWidget->addWidget(m_modelWidget);
    ui->stackedWidget->addWidget(m_adcalibrationWidget);
    ui->stackedWidget->addWidget(m_dacalibrationWidget);
}
void Widget::initConnect()
{

}

void Widget::initToolTip()
{
    ui->tbnHome->setToolTip(tr("tip_home"));
    ui->tbnSetting->setToolTip(tr("tip_setting"));

    ui->tbnFlow->setToolTip(tr("tip_flow"));
    ui->tbnCOD->setToolTip(tr("tip_cod"));
    ui->tbnPH->setToolTip(tr("tip_ph"));
    ui->tbnSafety->setToolTip(tr("tip_safety"));

    ui->tbnAD->setToolTip(tr("tip_ad"));
    ui->tbnDA->setToolTip(tr("tip_da"));
    ui->tbnValveControl->setToolTip(tr("tip_valvecontrol"));

    ui->tbnRtd->setToolTip(tr("tip_rtd"));
    ui->tbnPolluter->setToolTip(tr("tip_polluter"));
    ui->tbnStatistic->setToolTip(tr("tip_statistic"));
    ui->tbnCalibration->setToolTip(tr("tip_calibration"));
    ui->tbnModel->setToolTip(tr("tip_model"));
    ui->tbnUser->setToolTip(tr("tip_user"));
    ui->tbnNull->setToolTip(tr("tip_null"));

}

void Widget::deletWidget()
{
    if (m_rtdWidget != NULL){
        delete m_rtdWidget;
        m_rtdWidget = NULL;
    }
    if (m_flowrtdWidget != NULL){
        delete m_flowrtdWidget;
        m_flowrtdWidget = NULL;
    }
    if (m_codrtdWidget != NULL){
        delete m_codrtdWidget;
        m_codrtdWidget = NULL;
    }
    if (m_phrtdWidget != NULL){
        delete m_phrtdWidget;
        m_phrtdWidget = NULL;
    }

    if (m_valveWidget != NULL){
        delete m_valveWidget;
        m_valveWidget = NULL;
    }
    if (m_statisticWidget != NULL){
        delete m_statisticWidget;
        m_statisticWidget = NULL;
    }

    if (m_modelWidget != NULL)
    {
        delete m_modelWidget;
        m_modelWidget = NULL;
    }
    
    if (m_adcalibrationWidget != NULL){
        delete m_adcalibrationWidget;
        m_adcalibrationWidget = NULL;
    }
    
    if (m_dacalibrationWidget != NULL){
        delete m_dacalibrationWidget;
        m_dacalibrationWidget = NULL;
    }
}
//显示当前的日期和时间
void Widget::slotShowCurrentDataTime()
{
    ui->label_hour->setText(QTime::currentTime().toString("hh"));
    ui->label_min->setText(QTime::currentTime().toString("mm"));
    ui->label_sec->setText(QTime::currentTime().toString("ss"));
    ui->label_data->setText(QDate::currentDate().toString("yyyy年MM月dd日"));
    ui->label_week->setText(QDate::currentDate().toString("dddd"));
    ui->label_week->setText("星期" + weekcn.at(QDate::currentDate().dayOfWeek()-1));
    if(blk_time > 0){
        blk_time--;
    }
    //qDebug()<<QString("blk_time:%1").arg(blk_time);
    if(20 == blk_time){
        system("echo 7 > /sys/class/backlight/backlight/brightness");
    }

    if(1 == blk_time){
        system("echo 8 > /sys/class/backlight/backlight/brightness");
        ui->stackedWidget->setCurrentIndex(E_HOME_WIDGET);
    }
}

/**
 * @Function:       setToolButtonStyle()
 * @Description:    设置按钮样式
 * @Calls:          被本函数调用
 * @Input:          tbn:        按钮名称
 *                  text:       按钮所显示的文本
 *                  textSize:   文本大小(E_NORMAL,E_BIG)
 *                  iconName:   按钮图标路径
 * @Output:         无
 * @Return:         无
 * @Others:         其它说明
 */
void Widget::setToolButtonStyle(QToolButton *tbn, const QString &text,
                                int textSize,const QString iconName)
{
    //设置显示的文本
    tbn->setText(text);
    if (textSize == E_BIG)
    {
        tbn->setFont(QFont("楷体",16,QFont::Bold));
        //tbn->setFont(QFont("文泉驿雅黑",16,QFont::Bold));
    }else if (textSize == E_NORMAL){
        tbn->setFont(QFont("楷体",12,QFont::Bold));
        //tbn->setFont(QFont("文泉驿雅黑",12,QFont::Bold));
    }
    tbn->setAutoRaise(true);
    //设置按钮图标
    tbn->setIcon(QPixmap(QString("%1").arg(iconName)));
    tbn->setIconSize(QPixmap(QString("%1").arg(iconName)).size());
    //设置文本在图标下边
    tbn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void Widget::startAnimation()
{
    /*QPropertyAnimation *animation = new QPropertyAnimation(ui->stackedWidget->currentWidget(), "geometry");
    animation->setDuration(800);
    animation->setStartValue(QRect(0, 0,100, 30));
    animation->setEndValue(QRect(0, 0, 800, 360));
    animation->start();*/
}

void Widget::setCurrentWidget(enum_widget enum_widget_name)
{
    startAnimation();
    ui->stackedWidget->setCurrentIndex(enum_widget_name);
}

void Widget::on_tbnRtd_clicked()
{
    this->setCurrentWidget(E_RTD_WIDGET);
}

void Widget::on_tbnPolluter_clicked()
{
    this->setCurrentWidget(E_POLLUTER_WIDGET);
}

void Widget::on_tbnStatistic_clicked()
{
   //this->setCurrentWidget(E_STATISTIC_WIDGET);
    ui->stackedWidget->setCurrentIndex(E_STATISTIC_WIDGET);
}

void Widget::on_tbnCalibration_clicked()
{
    if(Myapp::UserType >= SUPER_USER && 1 == pgPara->Mode){  //运维模式
        this->setCurrentWidget(E_CALIBRATION_WIDGET);
    }else{
        myHelper::showMessageBoxInfo("请登陆管理账户\n并切换到运维模式");
    }
}

void Widget::on_tbnModel_clicked()
{
    this->setCurrentWidget(E_MODEL_WIDGET);
}

void Widget::on_tbnUser_clicked()
{
    if(Myapp::UserType>0){
        if(myHelper::showMessageBoxQusetion(QString("%1用户已登陆，是否切换\n用户").arg(Myapp::UserName))){
            myHelper::InterfaceEventMsgSend(pmsg_interface,Myapp::UserName + "用户退出",MSG_SQLITE_EVENT_USER_TYTE);
            Myapp::UserType = 0;
            //pgPara->Mode=0;  //用户退出或切换，自动将模式切换为远程模
        }
    }
    if(0 == Myapp::UserType){
        m_frmlogin = new frmlogin;
        m_frmlogin->show();
    }
}

void Widget::on_tbnNull_clicked()
{
    this->setCurrentWidget(E_RTD_WIDGET);
}

void Widget::on_tbnFlow_clicked()
{
    this->setCurrentWidget(E_FLOW_WIDGET);
}

void Widget::on_tbnCOD_clicked()
{
    this->setCurrentWidget(E_COD_WIDGET);
}

void Widget::on_tbnPH_clicked()
{
    this->setCurrentWidget(E_PH_WIDGET);
}

void Widget::on_tbnSafety_clicked()
{
    this->setCurrentWidget(E_RTD_WIDGET);
}

void Widget::on_tbnSetting_clicked()
{
    if(Myapp::UserType > QY_USER){  //用户已登陆
        m_frmconfig = new frmconfig;
        m_frmconfig->show();
    }else{
        myHelper::showMessageBoxInfo("请登陆运维人员账户");
    }
}

void Widget::on_tbnHome_clicked()
{
    ui->stackedWidget->setCurrentIndex(E_HOME_WIDGET);
}

void Widget::on_tbnDA_clicked()
{
    this->setCurrentWidget(E_DA_WIDGET);
}

void Widget::on_tbnAD_clicked()
{
    this->setCurrentWidget(E_AD_WIDGET);
}

void Widget::on_tbnValveControl_clicked()
{
    this->setCurrentWidget(E_VALVE_WIDGET);
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    //qDebug()<<QString("Widget mousePressEvent:%1").arg(e->button());
    if (e->button() == Qt::LeftButton)
    {
        //qDebug()<<QString("blk_time1:%1").arg(blk_time);
        if(blk_time <= 20){
            system("echo 0 > /sys/class/backlight/backlight/brightness");
        }
        blk_time = 120;
    }
}

/*void Widget::keyPressEvent(QKeyEvent *key)
{
    qDebug()<<QString("keyPressEvent");
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    qDebug()<<QString("mouseMoveEvent");
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    qDebug()<<QString("mouseReleaseEvent");
}

void Widget::closeEvent(QCloseEvent *event)
{
    qDebug()<<QString("closeEvent");
}*/
