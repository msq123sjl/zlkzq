#include "frmlogin.h"
#include "ui_frmlogin.h"
//#include "api/myhelper.h"
//#include "api/myapi.h"
//#include "api/myapp.h"
#include <QKeyEvent>
#include <QDebug>
#include "myhelper.h"
#include "myapp.h"

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
extern pstPara pgPara;

frmlogin::frmlogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmlogin)
{
    ui->setupUi(this);

    myHelper::FormInCenter(this,Myapp::DeskWidth,Myapp::DeskHeigth);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setGeometry(QRect(160, 100, 480,259));//可设置窗口显示的方位与大小

    myHelper::FormNotResize(this);
    this->InitStyle();
    this->InitForm();
}

frmlogin::~frmlogin()
{
    delete ui;
}

void frmlogin::InitStyle()
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
}

bool frmlogin::eventFilter(QObject *obj, QEvent *event)
{
    //用户按下回车键,触发登录信号.
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent *keyEvent=static_cast<QKeyEvent *>(event);
        if (keyEvent->key()==Qt::Key_Return || keyEvent->key()==Qt::Key_Enter) {
            this->on_btnLogin_clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj,event);
}

void frmlogin::InitForm()
{

}


void frmlogin::on_btnLogin_clicked()
{
    QString UserName=ui->comboBox_name->currentText();
    int UserIndex = ui->comboBox_name->currentIndex();
    int UserPwd=ui->txtUserPwd->text().toInt();
    if (0 == UserPwd){
        Myapp::UserType = 0;
        myHelper::showMessageBoxInfo("密码只能为非零的数字,请重新输入!");
        return;
    }
    //pgPara->UserPara[UserIndex%USER_CNT].UserPwd = 1;
    qDebug()<<QString("UserPwd[%1]:%2,%3").arg(UserIndex%USER_CNT).arg(pgPara->UserPara[UserIndex%USER_CNT].UserPwd).arg(UserPwd);
    if(pgPara->UserPara[UserIndex%USER_CNT].UserPwd == UserPwd){
        Myapp::UserType = UserIndex  + 1;
        Myapp::UserName = UserName;
    }else{
        Myapp::UserType = 0;
        myHelper::showMessageBoxInfo("密码错误,请重新输入!");
    }
    this->close();
}

void frmlogin::on_btnClose_clicked()
{
    this->close();
}
