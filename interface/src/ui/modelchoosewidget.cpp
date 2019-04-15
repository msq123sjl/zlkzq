#include "modelchoosewidget.h"
#include "ui_modelchoosewidget.h"

#include "analysisdata.h"
#include <QDebug>

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
}

void ModelChooseWidget::on_pbn_operation_clicked()
{
    qDebug()<<QString("operation_clicked:%1").arg(ui->pbn_operation->GetCheck());
    if (ui->pbn_operation->GetCheck())
    {
       ui->label_operation->setText("运维模式");
       //ui->pbn_operation->SetCheck(false);
    }else
    {
        ui->label_operation->setText("远程模式");
        //ui->pbn_operation->SetCheck(true);
    }
}
