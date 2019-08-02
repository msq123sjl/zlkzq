/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--COD界面功能的实现
**************************************************/
#include "codrtdwidget.h"
#include "ui_codrtdwidget.h"

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
CODRtdwidget::CODRtdwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CODRtdwidget)
{
    ui->setupUi(this);

    this->initForm();
    this->init();
    this->initConnect();
}

CODRtdwidget::~CODRtdwidget()
{
    delete ui;
}

void CODRtdwidget::initForm()
{

}

void CODRtdwidget::init()
{
    //允许排放量设置
    slotShowCurrentData();
}

//初始化信号和槽的连接
void CODRtdwidget::initConnect()
{
    this->InitShowData();
}

void CODRtdwidget::InitShowData()
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(slotShowCurrentData()));
    m_timer->start(5000);
}

void CODRtdwidget::slotShowCurrentData()
{
    //允许排放量设置
    ui->label_rtd->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_COD_INDEX].rtd,'f',0)+"mg/m³");
    ui->label_cur_day->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_COD_INDEX].day,'f',0)+"mg");
    ui->label_cou->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_COD_INDEX].cou,'f',0)+"mg");
    ui->label_cur_mon->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_COD_INDEX].mon,'f',0)+"mg");
    ui->label_cur_qut->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_COD_INDEX].qut,'f',0)+"mg");
    ui->label_cur_yea->setText(QString::number(pgPollutantData->RtdData.Row[POLLUTANT_COD_INDEX].year,'f',0)+"mg");
    ui->label_set_mon->setText(QString::number(pgPollutantPara->Row[POLLUTANT_COD_INDEX].MonAll)+"mg");
    ui->label_set_qut->setText(QString::number(pgPollutantPara->Row[POLLUTANT_COD_INDEX].QutAll)+"mg");
    ui->label_set_yea->setText(QString::number(pgPollutantPara->Row[POLLUTANT_COD_INDEX].YeaAll)+"mg");
}
