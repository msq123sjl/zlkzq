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

#include "control.h"
#include "myapp.h"

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
    ui->label_cou->setText(tr("--"));
    ui->label_cur_day->setText(tr("--"));
    ui->label_cur_mon->setText(tr("--"));
    ui->label_cur_qut->setText(tr("--"));
    ui->label_cur_yea->setText(tr("--"));
    //允许排放量设置
    ui->label_set_mon->setText(tr("--"));
    ui->label_set_qut->setText(tr("--"));
    ui->label_set_yea->setText(tr("--"));

    ui->label_unit_2->setText(tr(""));
    ui->label_unit_3->setText(tr(""));
    ui->label_unit_4->setText(tr(""));
    ui->label_unit_5->setText(tr(""));
    ui->label_unit_6->setText(tr(""));
    ui->label_unit_7->setText(tr(""));
    ui->label_unit_8->setText(tr(""));
    ui->label_unit_9->setText(tr(""));
}

void PHRtdwidget::init()
{
}

//初始化信号和槽的连接
void PHRtdwidget::initConnect()
{
}











