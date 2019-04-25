#include "modelchoosewidget.h"
#include "ui_modelchoosewidget.h"
#include "myapp.h"
#include "myhelper.h"
//#include "analysisdata.h"
#include <QDebug>

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
extern pstPara pgPara;

ModelChooseWidget::ModelChooseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelChooseWidget)
{
    ui->setupUi(this);
    this->initForm();
}

ModelChooseWidget::~ModelChooseWidget()
{
    delete ui;
}

void ModelChooseWidget::initForm()
{
    if (1 == pgPara->Mode){
        ui->pbn_operation->SetCheck(true);
        ui->label_operation->setText("运维模式");
    }else{
        ui->pbn_operation->SetCheck(false);
        ui->label_operation->setText("远程模式");
    }
}

void ModelChooseWidget::on_pbn_operation_clicked()
{
    qDebug()<<QString("Myapp::UserType[%1]").arg(Myapp::UserType);
    if(Myapp::UserType > QY_USER){  //用户已登陆
        pgPara->Mode =  (true == ui->pbn_operation->GetCheck()) ? 1:0;
        qDebug()<<QString("operation_clicked:%1").arg(pgPara->Mode);
        if (1 == pgPara->Mode)
        {
           ui->label_operation->setText("运维模式");
           //ui->pbn_operation->SetCheck(false);
        }else{
            ui->label_operation->setText("远程模式");
            //ui->pbn_operation->SetCheck(true);
        }
    }else{
        initForm();
        myHelper::showMessageBoxInfo("请登陆管理员账户");
    }
}
