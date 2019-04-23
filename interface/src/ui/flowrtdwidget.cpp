/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--流量界面功能的实现
**************************************************/
#include "flowrtdwidget.h"
#include "ui_flowrtdwidget.h"

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

FlowRtdwidget::FlowRtdwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlowRtdwidget)
{
    ui->setupUi(this);

    this->initForm();
    this->init();
    this->initConnect();
}

FlowRtdwidget::~FlowRtdwidget()
{
    delete ui;
}

void FlowRtdwidget::initForm()
{

}

void FlowRtdwidget::init()
{
    //允许排放量设置
    slotShowCurrentData();
}

//初始化信号和槽的连接
void FlowRtdwidget::initConnect()
{
    this->InitShowData();
}

void FlowRtdwidget::InitShowData()
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(slotShowCurrentData()));
    m_timer->start(5000);
}

void FlowRtdwidget::slotShowCurrentData()
{
    //允许排放量设置
    ui->label_rtd->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_FLOW_INDEX].rtd));
    ui->label_cur_day->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_FLOW_INDEX].day));
    ui->label_cou->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_FLOW_INDEX].cou));
    ui->label_cur_mon->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_FLOW_INDEX].mon));
    ui->label_cur_qut->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_FLOW_INDEX].qut));
    ui->label_cur_yea->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_FLOW_INDEX].year));
    ui->label_set_mon->setText(QString::number(pgPollutantPara->Row[POLLUTANT_FLOW_INDEX].MonAll));
    ui->label_set_qut->setText(QString::number(pgPollutantPara->Row[POLLUTANT_FLOW_INDEX].QutAll));
    ui->label_set_yea->setText(QString::number(pgPollutantPara->Row[POLLUTANT_FLOW_INDEX].YeaAll));
}










