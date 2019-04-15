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

#include "control.h"
#include "myapp.h"

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

    //允许排放量设置
    ui->label_set_mon->setText(QString::number(123456.23));
    ui->label_set_qut->setText(QString::number(123456.23));
    ui->label_set_yea->setText(QString::number(123456.23));
}

void CODRtdwidget::init()
{
}

//初始化信号和槽的连接
void CODRtdwidget::initConnect()
{
}











