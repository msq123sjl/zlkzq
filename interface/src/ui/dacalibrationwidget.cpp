/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--DA校准界面功能的实现
**************************************************/
#include "dacalibrationwidget.h"
#include "ui_dacalibrationwidget.h"
#include "myapp.h"
#include "myhelper.h"
#include <QDebug>
#include <QTime>

static int set_value = 0;
static int set_davalue=0;
static int channel = 0;

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
extern pstPara pgPara;
extern pstData pgData;
extern pstCalibrationPara pgCalibrationPara;

DaCalibrationWidget::DaCalibrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DaCalibrationWidget)
{
    ui->setupUi(this);

    this->initForm();
    this->init();
    this->initConnect();
}

DaCalibrationWidget::~DaCalibrationWidget()
{
    delete ui;
}

void DaCalibrationWidget::initForm()
{
    setToolButtonStyle(ui->tbn_value_add,"数字值加",
                       ":/images/module/temp_add.png");
    setToolButtonStyle(ui->tbn_value_sub,"数字值减",
                       ":/images/module/temp_sub.png");
    setToolButtonStyle(ui->tbn_value_ok,"确定",
                       ":/images/module/temp_sub.png");
    ui->comboBox->setFont(QFont("文泉驿雅黑",8,QFont::Normal));
}

void DaCalibrationWidget::init()
{
}

void DaCalibrationWidget::initConnect()
{
    InitUpdataTime();
}

void DaCalibrationWidget::InitUpdataTime()
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(slotUpdataValue()));
    m_timer->start(1000);
}



void DaCalibrationWidget::refreshValue()
{
    ui->label_set_value->setText(QString::number(set_value,10) + "mA");
    ui->label_cur_value->setText(QString::number(set_davalue,10));
}

void DaCalibrationWidget::setToolButtonStyle(QToolButton *tbn,
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
void DaCalibrationWidget::slotUpdataValue()
{
    refreshValue();
}

void DaCalibrationWidget::on_tbn_value_add_clicked()
{
    if (set_davalue <  4096){
        set_davalue += 1;
    }else{
        set_davalue = 4096;
    }
    refreshValue();
}

void DaCalibrationWidget::on_tbn_value_sub_clicked()
{
    if (set_davalue >  0){
        set_davalue -= 1;
    }else{
        set_davalue = 0;
    }
    refreshValue();
}

void DaCalibrationWidget::on_tbn_value_ok_clicked()
{
    if(myHelper::showMessageBoxQusetion(QString("是否校准？"))){
        switch(set_value){
            case 0:
                 pgCalibrationPara->DaAdjustValue[channel][0] = ui->label_cur_value->text().toInt();
                 break;
            case 4:
                 pgCalibrationPara->DaAdjustValue[channel][1] = ui->label_cur_value->text().toInt();
                 break;
            case 20:
                pgCalibrationPara->DaAdjustValue[channel][2] = ui->label_cur_value->text().toInt();
                syncCalibrationParaShm();
                break;
        }
    }
    //选择下一个测量值或校准值
    switch(set_value){
        case 0:
            set_value = 4;
            set_davalue = pgCalibrationPara->DaAdjustValue[channel][1];
             break;
        case 4:
            set_value = 20;
            set_davalue = pgCalibrationPara->DaAdjustValue[channel][2];
             break;
        case 20:
            break;
        default:
            set_value = 0;
            set_davalue = pgCalibrationPara->DaAdjustValue[channel][0];
            break;
    }
    refreshValue();
}

void DaCalibrationWidget::on_comboBox_currentIndexChanged(int index)
{
    channel = index%DA_CNT;
    set_value = 0;
    set_davalue = pgCalibrationPara->DaAdjustValue[channel][0];
    refreshValue();
}
