/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--PH界面功能的实现
**************************************************/
#include "phrtdwidget.h"
#include "ui_phrtdwidget.h"

#include <QDebug>
#include<QTimer>
//#include "control.h"
#include "myapp.h"

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
extern pstPara pgPara;
extern pstPollutantData pgPollutantData;
extern pstPollutantPara pgPollutantPara;

PHRtdwidget::PHRtdwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PHRtdwidget)
{
    ui->setupUi(this);

    this->initForm();
    this->init();
    this->initConnect();
}

PHRtdwidget::~PHRtdwidget()
{
    delete ui;
}

void PHRtdwidget::initForm()
{
}

void PHRtdwidget::init()
{
    ui->label_cou->setText(tr("--"));
    ui->label_cur_day->setText(tr("--"));
    ui->label_cur_mon->setText(tr("--"));
    ui->label_cur_qut->setText(tr("--"));
    ui->label_cur_yea->setText(tr("--"));
    //允许排放量设置
    ui->label_set_mon->setText(tr("--"));
    ui->label_set_qut->setText(tr("--"));
    ui->label_set_yea->setText(tr("--"));
}

//初始化信号和槽的连接
void PHRtdwidget::initConnect()
{
    this->InitShowData();
}

void PHRtdwidget::InitShowData()
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(slotShowCurrentData()));
    m_timer->start(5000);
}

void PHRtdwidget::slotShowCurrentData()
{
    ui->label_rtd->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_PH_INDEX].rtd,'f',2));
}









