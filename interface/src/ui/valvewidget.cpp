/*************************************************
Copyright:kevin
Author:Kevin LiQi
Date:2015-12-07
Email:kevinlq0912@163.com
QQ:936563422
Version:V1.0
Description:智能家居---厨房安防功能的实现，主要包括可燃性气体检测，灯光控制,
            发挥部分：添加冰箱的控制
**************************************************/
#include "valvewidget.h"
#include "ui_valvewidget.h"
#include <QToolButton>
#include "myapp.h"
#include "control.h"
#include <QDebug>
#include <QTime>

int valve_set_value = 20;
int valve_cur_value = 0;

ValveWidget::ValveWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ValveWidget)
{
    ui->setupUi(this);

    this->initForm();
    this->init();
    this->initConnect();
    this->refreshValveValue();
    this->InitUpdataTime();
}

ValveWidget::~ValveWidget()
{
    delete ui;
}

void ValveWidget::initForm()
{
    setToolButtonStyle(ui->tbn_valve_add,"开度加",
                       ":/images/module/temp_add.png");
    setToolButtonStyle(ui->tbn_valve_sub,"开度减",
                       ":/images/module/temp_sub.png");
    setToolButtonStyle(ui->tbn_valve_ok,"确定",
                       ":/images/module/temp_sub.png");
    ui->pbn_valve_contrl->styleOff = "border-image: url(:/images/switch/btncheckoff.png); border: 0px;";
    ui->pbn_valve_contrl->styleOn="border-image: url(:/images/switch/btncheckon.png); border: 0px;";

    //根据配置文件信息同步阀门控制方式
    if (true){
        ui->pbn_valve_contrl->SetCheck(true);
    }else{
        ui->pbn_valve_contrl->SetCheck(false);
    }
}

void ValveWidget::init()
{
}

void ValveWidget::initConnect()
{
}

void ValveWidget::InitUpdataTime()
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(slotUpdataValveValue()));
    m_timer->start(1000);
}

void ValveWidget::refreshValveValue()
{
    ui->label_set_value->setText(QString::number(valve_set_value,10) + '%');
    ui->label_cur_value->setText(QString::number(valve_cur_value,10) + '%');
}

void ValveWidget::setToolButtonStyle(QToolButton *tbn,
                                       const QString &text, const QString iconName)
{
    //设置显示的文本
    tbn->setText(text);
    tbn->setFont(QFont("文泉驿雅黑",10,QFont::Normal));

    tbn->setAutoRaise(true);
    //设置按钮图标
    tbn->setIcon(QPixmap(QString("%1").arg(iconName)));
    tbn->setIconSize(QSize(40,40));
    //设置文本在图标下边
    tbn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}


//更新阀门开度值
void ValveWidget::slotUpdataValveValue()
{
    valve_cur_value += 1;
    valve_cur_value %=101;
    refreshValveValue();
}

void ValveWidget::on_tbn_valve_add_clicked()
{
    if (valve_set_value <=  90){
        valve_set_value += 10;
    }else{
        valve_set_value = 100;
    }
    refreshValveValue();
}

void ValveWidget::on_tbn_valve_sub_clicked()
{
    if (valve_set_value >=  10){
        valve_set_value -= 10;
    }else{
        valve_set_value = 0;
    }
    refreshValveValue();
}

void ValveWidget::on_pbn_valve_contrl_clicked()
{
    if (ui->pbn_valve_contrl->GetCheck()){
       qDebug()<<"DA";
    }else{
        qDebug()<<"I/O";
    }
}

void ValveWidget::on_tbn_valve_ok_clicked()
{
    qDebug()<<"ok";
}
