/*************************************************
Copyright:kevin
Author:Kevin LiQi
Date:2015-12-04
Email:kevinlq0912@163.com
QQ:936563422
Version:V1.0
Description:智能家居--客厅界面的实现，包括客厅的灯，温湿度
**************************************************/
#include "flowrtdwidget.h"
#include "ui_flowrtdwidget.h"

#include <QDebug>

#include "control.h"
#include "myapp.h"

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

    //允许排放量设置
    ui->label_set_mon->setText(QString::number(123456.23));
    ui->label_set_qut->setText(QString::number(123456.23));
    ui->label_set_yea->setText(QString::number(123456.23));
}

void FlowRtdwidget::init()
{
}

//初始化信号和槽的连接
void FlowRtdwidget::initConnect()
{
}











