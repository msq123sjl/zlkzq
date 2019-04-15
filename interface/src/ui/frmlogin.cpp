#include "frmlogin.h"
#include "ui_frmlogin.h"
//#include "api/myhelper.h"
//#include "api/myapi.h"
//#include "api/myapp.h"
#include <QKeyEvent>
#include <QDebug>

frmlogin::frmlogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmlogin)
{
    ui->setupUi(this);
    qDebug()<<"1";
            int frmX=this->width();
        int frmY=this->height();
        QPoint movePoint(800/2-frmX/2,480/2-frmY/2);
        this->move(movePoint);
        qDebug()<<"2";
    //myHelper::FormInCenter(this,myApp::DeskWidth,myApp::DeskHeigth);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setGeometry(QRect(160, 100, 480,259));//可设置窗口显示的方位与大小
qDebug()<<"3";
    //myHelper::FormNotResize(this);
    this->InitStyle();
qDebug()<<"4";
    this->InitForm();
qDebug()<<"5";

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
    QString UserName=ui->txtUserName->text();
    QString UserPwd=ui->txtUserPwd->text();
    if (UserName==""||UserPwd==""){
        //myHelper::ShowMessageBoxError("用户名和密码不能为空,请重新输入!");
        return;
    }

//    QString sql;
//    QSqlQuery query;
//    sql = "select [UserPwd],[UserType] from [UserInfo]  where [UserName]='"+UserName+"'";
//    query.exec(sql);
//    query.next();
//    if (UserPwd.toUpper()==query.value(0).toString().toUpper()){
//        //记录当前用户,写入到配置文件,下次启动时显示最后一次登录用户名.
//        this->close();
//    }else{
//        //myHelper::ShowMessageBoxError("用户名或密码错误,请重新输入!");
//    }
    this->close();
}

void frmlogin::on_btnClose_clicked()
{
    this->close();
}
