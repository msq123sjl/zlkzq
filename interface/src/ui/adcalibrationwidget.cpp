/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--AD校准界面功能的实现
**************************************************/
#include "adcalibrationwidget.h"
#include "ui_adcalibrationwidget.h"
#include "myapp.h"
#include "myhelper.h"
#include <QDebug>
#include <QTime>

static int set_value = 0;
static int channel = 0;

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
extern pstPara pgPara;
extern pstData pgData;

AdCalibrationWidget::AdCalibrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdCalibrationWidget)
{
    ui->setupUi(this);

    this->initForm();
    this->init();
    this->initConnect();
}

AdCalibrationWidget::~AdCalibrationWidget()
{
    delete ui;
}

void AdCalibrationWidget::initForm()
{
    setToolButtonStyle(ui->tbn_value_add,"校准值加",
                       ":/images/module/temp_add.png");
    setToolButtonStyle(ui->tbn_value_sub,"校准值减",
                       ":/images/module/temp_sub.png");
    setToolButtonStyle(ui->tbn_value_ok,"确定",
                       ":/images/module/temp_sub.png");
    ui->comboBox->setFont(QFont("文泉驿雅黑",8,QFont::Normal));
    ui->label_text->setFont(QFont("文泉驿雅黑",8,QFont::Normal));
}

void AdCalibrationWidget::init()
{
}

void AdCalibrationWidget::initConnect()
{
    InitUpdataTime();
}

void AdCalibrationWidget::InitUpdataTime()
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(slotUpdataValue()));
    m_timer->start(1000);
}



void AdCalibrationWidget::refreshValue()
{
    pgData->current_Ia[channel] = 12;
    ui->label_set_value->setText(QString::number(set_value,10) + "mA");
    ui->label_cur_value->setText(QString::number(pgData->current_Ia[channel],'f',3) + "mA");
}

void AdCalibrationWidget::setToolButtonStyle(QToolButton *tbn,
                                       const QString &text, const QString iconName)
{
    //设置显示的文本
    tbn->setText(text);
    tbn->setFont(QFont("文泉驿雅黑",6,QFont::Normal));

    tbn->setAutoRaise(true);
    //设置按钮图标
    tbn->setIcon(QPixmap(QString("%1").arg(iconName)));
    tbn->setIconSize(QSize(40,40));
    //设置文本在图标下边
    tbn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

//更新阀门开度值
void AdCalibrationWidget::slotUpdataValue()
{
    refreshValue();
}

void AdCalibrationWidget::on_tbn_value_add_clicked()
{
    if (set_value <=  16){
        set_value += 4;
    }else{
        set_value = 20;
    }
    refreshValue();
}

void AdCalibrationWidget::on_tbn_value_sub_clicked()
{
    if (set_value >=  4){
        set_value -= 4;
    }else{
        set_value = 0;
    }
    refreshValue();
}

void AdCalibrationWidget::on_tbn_value_ok_clicked()
{

}

void AdCalibrationWidget::on_comboBox_currentIndexChanged(int index)
{
    channel = index;
    set_value = 0;
}
