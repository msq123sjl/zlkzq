#include "frmconfig.h"
#include "ui_frmconfig.h"
#include <QDateTime>
#include <QDebug>

frmconfig::frmconfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmconfig)
{
    ui->setupUi(this);  
    //myHelper::FormInCenter(this,800,480);//窗体居中显示

        int frmX=this->width();
        int frmY=this->height();
        QPoint movePoint(800/2-frmX/2,480/2-frmY/2);
        this->move(movePoint);
    this->InitStyle();                          //
    this->InitForm();                         //

}

frmconfig::~frmconfig()
{
    qDebug()<<"~frmconfig ";
    delete ui;
}

void frmconfig::InitStyle()
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);//
    ui->stackedWidget_config->setAttribute(Qt::WA_TranslucentBackground, true);
}

void frmconfig::InitForm()
{
    ui->stackedWidget_config->setCurrentIndex(0);
    //加载配置文件对应界面展示信息
    //基本设置

    //用户设置



//切换到常规设置界面
}
void frmconfig::on_btn_general_clicked()
{
    //基本设置

    ui->stackedWidget_config->setCurrentIndex(0);
}

//切换到通道设置界面
void frmconfig::on_btn_parameter_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(1);
}
//切换到串口设置界面
void frmconfig::on_btn_serial_clicked()
{
    //串口设置
    ui->stackedWidget_config->setCurrentIndex(2);
}

//切换到站点设置界面
void frmconfig::on_btn_site_clicked()
{
    //站点设置

    ui->stackedWidget_config->setCurrentIndex(3);
}

//切换到用户设置界面
void frmconfig::on_btn_user_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(4);
}

//切换到系统更新界面
void frmconfig::on_btn_updateSystem_clicked()
{
    ui->stackedWidget_config->setCurrentIndex(5);
}

//切换到日期时间界面
void frmconfig::on_btn_datetime_clicked()
{
    //日期时间
    //加载年月日时分秒下拉框
    for(int i=2010;i<=2030;i++){
        ui->comboBox_year->addItem(QString::number(i));
    }
    for(int i=1;i<=12;i++){
        ui->comboBox_month->addItem(QString::number(i));
    }
    for(int i=1;i<=31;i++){
        ui->comboBox_day->addItem(QString::number(i));
    }
    for(int i=0;i<24;i++){
        ui->comboBox_hour->addItem(QString::number(i));
    }
    for(int i=0;i<60;i++){
        ui->comboBox_minute->addItem(QString::number(i));
        ui->comboBox_second->addItem(QString::number(i));
    }
    //加载当前时间
    QStringList Now =QDateTime::currentDateTime().toString("yyyy-M-d-h-m-s").split("-");
    ui->comboBox_year->setCurrentIndex(ui->comboBox_year->findText(Now[0]));
    ui->comboBox_month->setCurrentIndex(ui->comboBox_month->findText(Now[1]));
    ui->comboBox_day->setCurrentIndex(ui->comboBox_day->findText(Now[2]));
    ui->comboBox_hour->setCurrentIndex(ui->comboBox_hour->findText(Now[3]));
    ui->comboBox_minute->setCurrentIndex(ui->comboBox_minute->findText(Now[4]));
    ui->comboBox_second->setCurrentIndex(ui->comboBox_second->findText(Now[5]));
    ui->stackedWidget_config->setCurrentIndex(6);
}

//切换到本机网络界面
void frmconfig::on_btn_localNet_clicked()
{
    //本机网络设置

    ui->stackedWidget_config->setCurrentIndex(7);
}
//开关量配置
void frmconfig::on_btn_io_clicked()
{
    //加载开关量配置信息
        ui->stackedWidget_config->setCurrentIndex(8);
}


//*************************************串口设置*********************************************/
/*********************************************
 *串口号选择界面加载对应端口信息
 *********************************************/
void frmconfig::on_comboBox_serial_currentIndexChanged(int index)
{

}
//*************************************串口设置结束*****************************************/
//*************************************用户设置*********************************************/
/*********************************************
 *读取并加载用户信息
 *********************************************/

void frmconfig::ReadUserInfo()
{  
 
}

/*********************************************
 *用户名选中界面加载对应信息
 *********************************************/
void frmconfig::on_listUser_currentRowChanged(int currentRow)
{

}

/*********************************************
 *添加新用户
 *********************************************/
void frmconfig::on_btn_addUser_clicked()
{

}

/*********************************************
 *删除用户
 *********************************************/
void frmconfig::on_btn_deleteUser_clicked()
{


}

/*********************************************
 *修改用户
 *********************************************/
void frmconfig::on_btn_updateUser_clicked()
{

}

/*********************************************
 *添加用户检查合法性
 *********************************************/
bool frmconfig::AddUserIsLegal()
{

    return true;
}


//*************************************因子设置*************************************************/
/*********************************************
 *添加因子
 *********************************************/
void frmconfig::on_btn_addPara_clicked()
{

}

/*********************************************
 *删除因子
 *********************************************/
void frmconfig::on_btn_deletePara_clicked()
{

}

/*********************************************
 *添加因子检查合法性
 *********************************************/
bool frmconfig::AddParaIsLegal()
{
    return true;
}

/*********************************************
 *设置污染物信息显示表格模型
 *********************************************/
void frmconfig::InitParaInfoModel()
{

}

/*********************************************
 *读取监测污染物信息
 *********************************************/
void frmconfig::ReadParaInfo()
{


}

void frmconfig::on_comboBox_channel_currentIndexChanged(int index)
{
    if(index<=7){
        ui->comboBox_signal->setEnabled(true);
    }else{
        ui->comboBox_signal->setEnabled(false);
    }
}
//*************************************因子设置结束*********************************************/

void frmconfig::on_btn_Cancel_clicked()
{
    qDebug()<<"frmconfig close" ;
    this->close();
}

//时间日期设置
void frmconfig::on_btnChange_clicked()
{

}

void frmconfig::on_btn_SaveGeneral_clicked()
{

}

void frmconfig::on_btn_SaveSerial_clicked()
{


}

void frmconfig::on_btn_SaveSite_clicked()
{
  
}

//*************************************本机网络*************************************************/
void frmconfig::on_btn_SaveLocalNet_clicked()
{

   
}
//*************************************本机网络结束*************************************************/

void frmconfig::on_btn_Autogetip_clicked()
{
    

}

//*************************************开关量配置结束*************************************************/
void frmconfig::on_btn_SaveIo_clicked()
{

}



