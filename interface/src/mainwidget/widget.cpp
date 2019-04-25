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
#include "flowrtdwidget.h"          //流量
#include "codrtdwidget.h"          //COD
#include "phrtdwidget.h"          //PH
#include "valvewidget.h"          //厨房
#include "statisticwidget.h"        //统计界面
#include "modelchoosewidget.h"      //模式选择控制界面

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
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
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint
                         | Qt::WindowMinMaxButtonsHint);
    this->setWindowTitle(tr("总量控制器"));
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
    //底部菜单的样式
    setToolButtonStyle(ui->tbnRtd,"实时数据",E_NORMAL,
                       ":/images/bottom/control.png");
    setToolButtonStyle(ui->tbnValve,"阀门",E_NORMAL,
                       ":/images/bottom/video.png");
    setToolButtonStyle(ui->tbnStatistic,"统计",E_NORMAL,
                       ":/images/bottom/statistics.png");    
    setToolButtonStyle(ui->tbnADDA,"校准",E_NORMAL,
                       ":/images/bottom/night.png");
    setToolButtonStyle(ui->tbnModel,"模式选择",E_NORMAL,
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
    m_flowrtdWidget = new FlowRtdwidget;              //流量
    m_codrtdWidget = new CODRtdwidget;              //COD
    m_phrtdWidget = new PHRtdwidget;              //PH
    m_valveWidget = new ValveWidget;              //厨房
    //m_statisticWidget = new StatisticWidget;          //统计
    m_modelWidget = new ModelChooseWidget;            //模式控制界面

    //m_menuWidget = new MenuWidget(this);
    //ui->tbnSetting->setMenu(m_menuWidget);
    //开启线程定时保存数据
//    m_saveDataThread = new SaveDataThread;
//    if (gIsOpenPort)
//    {
//        m_saveDataThread->start();
//    }

    ui->stackedWidget->addWidget(m_flowrtdWidget);
    ui->stackedWidget->addWidget(m_codrtdWidget);
    ui->stackedWidget->addWidget(m_phrtdWidget);
    ui->stackedWidget->addWidget(m_valveWidget);
    //ui->stackedWidget->addWidget(m_statisticWidget);
    ui->stackedWidget->addWidget(m_modelWidget);
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

    ui->tbnRtd->setToolTip(tr("tip_rtd"));
    ui->tbnValve->setToolTip(tr("tip_valve"));
    ui->tbnStatistic->setToolTip(tr("tip_statistic"));
    ui->tbnADDA->setToolTip(tr("tip_adda"));
    ui->tbnModel->setToolTip(tr("tip_model"));
    ui->tbnUser->setToolTip(tr("tip_user"));
    ui->tbnNull->setToolTip(tr("tip_null"));

}

void Widget::deletWidget()
{
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
//    if (m_statisticWidget != NULL){
//        delete m_statisticWidget;
//        m_statisticWidget = NULL;
//    }

    if (m_modelWidget != NULL)
    {
        delete m_modelWidget;
        m_modelWidget = NULL;
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

    if(blk_time > 0){
        blk_time--;
    }
    if(0 == blk_time){
        system("echo 8 > /sys/class/backlight/backlight/brightness");
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
        tbn->setFont(QFont("文泉驿雅黑",16,QFont::Bold));
    }else if (textSize == E_NORMAL)
        tbn->setFont(QFont("文泉驿雅黑",12,QFont::Bold));

    tbn->setAutoRaise(true);
    //设置按钮图标
    tbn->setIcon(QPixmap(QString("%1").arg(iconName)));
    tbn->setIconSize(QPixmap(QString("%1").arg(iconName)).size());
    //设置文本在图标下边
    tbn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void Widget::startAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->stackedWidget->currentWidget(), "geometry");
    animation->setDuration(800);
    animation->setStartValue(QRect(0, 0,100, 30));
    animation->setEndValue(QRect(0, 0, 800, 360));
    animation->start();
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

void Widget::on_tbnValve_clicked()
{
    this->setCurrentWidget(E_VALVE_WIDGET);
}

void Widget::on_tbnStatistic_clicked()
{
    this->setCurrentWidget(E_RTD_WIDGET);
}

void Widget::on_tbnADDA_clicked()
{
    this->setCurrentWidget(E_RTD_WIDGET);
}

void Widget::on_tbnModel_clicked()
{
    this->setCurrentWidget(E_MODEL_WIDGET);
}

void Widget::on_tbnUser_clicked()
{
    if(Myapp::UserType>0){
        if(myHelper::showMessageBoxQusetion(QString("%1用户已登陆，是否切换\n用户").arg(Myapp::UserName))){
            Myapp::UserType = 0;
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
        myHelper::showMessageBoxInfo("请登陆管理员账户");
    }
}

void Widget::on_tbnHome_clicked()
{
    ui->stackedWidget->setCurrentIndex(E_HOME_WIDGET);
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    //qDebug()<<QString("Widget mousePressEvent:%1").arg(e->button());
    if (e->button() == Qt::LeftButton)
    {
        if(0 == blk_time){
            system("echo 0 > /sys/class/backlight/backlight/brightness");
        }
        blk_time = 120;
    }
}



