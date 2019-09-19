#include "frmconfig.h"
#include "ui_frmconfig.h"
#include <QDateTime>
#include <QDebug>
#include "myhelper.h"
#include "myapp.h"

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
extern pstPara pgPara;
extern int blk_time;

frmconfig::frmconfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmconfig)
{
    ui->setupUi(this);  
    myHelper::FormInCenter(this,Myapp::DeskWidth,Myapp::DeskHeigth);//窗体居中显示
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
    //DTU功能配置
    ui->label_43->hide();
    ui->btn_com3ToServerOpen->hide();

    ui->btn_parameter->setEnabled(false);
    ui->btn_user->setEnabled(false);
    on_btn_general_clicked();
}
void frmconfig::on_btn_general_clicked()
{
    //基本设置
    //initParaShm();
    qDebug()<<QString("MN:%1 OverTime:%2 ReCount%3").arg(pgPara->GeneralPara.MN).arg(pgPara->GeneralPara.OverTime).arg(pgPara->GeneralPara.ReCount);
    qDebug()<<QString("AlarmTime:%1 StType:%2 RespondOpen:%3").arg(pgPara->GeneralPara.AlarmTime).arg(pgPara->GeneralPara.StType).arg(pgPara->GeneralPara.RespondOpen);
    ui->txtMN->setText(QString(pgPara->GeneralPara.MN));                            //MN号
    ui->txtRtdInterval->setText(QString::number(pgPara->GeneralPara.RtdInterval));             //实时数据间隔(s)
    ui->txtOverTime->setText(QString::number(pgPara->GeneralPara.OverTime));             //上传检测周期(s)
    ui->txtReCount->setText(QString::number(pgPara->GeneralPara.ReCount));                 //超时重发次数
    ui->txtAlarmTime->setText(QString::number(pgPara->GeneralPara.AlarmTime));         //超时报警时间
    ui->comboBoxStType->setCurrentIndex(ui->comboBoxStType->findText(QString("%1").arg(pgPara->GeneralPara.StType)));                         //污染源类型
    ui->btn_RespondOpen->SetCheck(bool(pgPara->GeneralPara.RespondOpen));                      //上位机应答
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
    on_comboBox_serial_currentIndexChanged(0);
    ui->stackedWidget_config->setCurrentIndex(2);
}

//切换到站点设置界面
void frmconfig::on_btn_site_clicked()
{
    //站点设置
    int iLoop = 0;
    ui->btn_server1Open->SetCheck(pgPara->SitePara[iLoop].ServerOpen>0 ? true : false);
    ui->txtIP1->setText(QString(pgPara->SitePara[iLoop].ServerIp));
    ui->txtPort1->setText(QString::number(pgPara->SitePara[iLoop].ServerPort));

    iLoop++;
    ui->btn_server2Open->SetCheck(pgPara->SitePara[iLoop].ServerOpen>0 ? true : false);
    ui->txtIP2->setText(QString(pgPara->SitePara[iLoop].ServerIp));
    ui->txtPort2->setText(QString::number(pgPara->SitePara[iLoop].ServerPort));

    iLoop++;
    ui->btn_server3Open->SetCheck(pgPara->SitePara[iLoop].ServerOpen>0 ? true : false);
    ui->txtIP3->setText(QString(pgPara->SitePara[iLoop].ServerIp));
    ui->txtPort3->setText(QString::number(pgPara->SitePara[iLoop].ServerPort));

    iLoop++;
    ui->btn_server4Open->SetCheck(pgPara->SitePara[iLoop].ServerOpen>0 ? true : false);
    ui->txtIP4->setText(QString(pgPara->SitePara[iLoop].ServerIp));
    ui->txtPort4->setText(QString::number(pgPara->SitePara[iLoop].ServerPort));

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
    Myapp::ReadLocalNet();
    ui->btn_Autogetip->SetCheck(Myapp::DHCP=="1"?true:false);
    ui->txtLocalIP->setText(Myapp::LocalIP);
    ui->txtMask->setText(Myapp::Mask);
    ui->txtGateWay->setText(Myapp::GateWay);
    if(Myapp::DHCP=="1")
    {
        ui->txtLocalIP->setEnabled(false);
        ui->txtMask->setEnabled(false);
        ui->txtGateWay->setEnabled(false);
    }
    else {
        ui->txtLocalIP->setEnabled(true);
        ui->txtMask->setEnabled(true);
        ui->txtGateWay->setEnabled(true);
    }
    //VPN设置
    ui->btn_VPN->SetCheck(pgPara->NetPara.VPNOpen==1?true:false);
    ui->txtVPNServerIP->setText(QString(pgPara->NetPara.VPNServerIp));
    ui->txtVPNIPIP->setText(QString(pgPara->NetPara.VPNIPIP));
    ui->txtVPNUser->setText(QString(pgPara->NetPara.VPNUserName));
    if(1 == pgPara->NetPara.VPNOpen){
        ui->txtVPNServerIP->setEnabled(true);
        ui->txtVPNIPIP->setEnabled(true);
        ui->txtVPNUser->setEnabled(true);
    }else{
        ui->txtVPNServerIP->setEnabled(false);
        ui->txtVPNIPIP->setEnabled(false);
        ui->txtVPNUser->setEnabled(false);
    }
    ui->stackedWidget_config->setCurrentIndex(7);
}
//开关量配置
void frmconfig::on_btn_io_clicked()
{
    //加载开关量配置信息
    int p_index=0;
    //输出
    p_index=pgPara->IOPara.Out_drain_open-6;
    ui->comboOut_drain_open->setCurrentIndex(p_index);

    p_index=pgPara->IOPara.Out_drain_close-6;
    ui->comboOut_drain_close->setCurrentIndex(p_index);

    p_index=pgPara->IOPara.Out_drain_common-6;
    ui->comboOut_drain_common->setCurrentIndex(p_index);

    p_index=pgPara->IOPara.Out_reflux_control-6;
    ui->comboOut_pump->setCurrentIndex(p_index);

    //输入
    p_index=pgPara->IOPara.In_drain_open-10;
    ui->comboIn_drain_open->setCurrentIndex(p_index);
    p_index=pgPara->IOPara.In_drain_close-10;
    ui->comboIn_drain_close->setCurrentIndex(p_index);
    p_index=pgPara->IOPara.In_reflux_open-10;
    ui->comboIn_pump->setCurrentIndex(p_index);
    p_index=pgPara->IOPara.In_power-10;
    ui->comboIn_power->setCurrentIndex(p_index);

    ui->stackedWidget_config->setCurrentIndex(8);
}


//*************************************串口设置*********************************************/
/*********************************************
 *串口号选择界面加载对应端口信息
 *********************************************/
void frmconfig::on_comboBox_serial_currentIndexChanged(int index)
{
    qDebug()<<QString("%1 %2 %3").arg(pgPara->SerialPara[index].BaudRate).arg(pgPara->SerialPara[index].DataBits).arg(pgPara->SerialPara[index].Parity);
    ui->comboBox_baudrate->setCurrentIndex(ui->comboBox_baudrate->findText(QString("%1").arg(pgPara->SerialPara[index].BaudRate)));
    ui->comboBox_databits->setCurrentIndex(ui->comboBox_databits->findText(QString("%1").arg(pgPara->SerialPara[index].DataBits)));
    ui->comboBox_parity->setCurrentIndex(pgPara->SerialPara[index].Parity);
    ui->comboBox_stopbits->setCurrentIndex(ui->comboBox_stopbits->findText(QString("%1").arg(pgPara->SerialPara[index].StopBits)));
    ui->txtuartInterval->setText(QString("%1").arg(pgPara->SerialPara[index].Interval));
    ui->txtuartTimeout->setText(QString("%1").arg(pgPara->SerialPara[index].TimeOut));
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
    if (myHelper::showMessageBoxQusetion("确定要更改日期时间吗?")){
        QString TempMsg;
        #ifdef Q_OS_LINUX
        QString str=QString("/bin/date -s '%1-%2-%3 %4:%5:%6'") //  生成时间设置命令字符串
                            .arg(ui->comboBox_year->currentText())
                            .arg(ui->comboBox_month->currentText())
                            .arg(ui->comboBox_day->currentText())
                            .arg(ui->comboBox_hour->currentText())
                            .arg(ui->comboBox_minute->currentText())
                            .arg(ui->comboBox_second->currentText());
        qDebug()<<str;
        int res1;
        int res2;
        res1=system(str.toLatin1().data());  //设置系统时间
        res2=system("/sbin/hwclock --systohc");   //将系统时间写入到RTC硬件中，以保留设置。这一操作是为了将修改好的时间写入到RTC中保存。如果不进行这一步操作，则
                                                                          //重新上电开机以后系统从RTC中读取到的仍然是原来的时间
        if(res1==0 && res2==0){
            TempMsg="本地更改日期时间成功";
        }
        else{
            TempMsg="本地更改日期时间失败";
        }
        #elif defined (Q_OS_WIN)

        #endif
        myHelper::showMessageBoxInfo(QString("%1!").arg(TempMsg));

    }
}

void frmconfig::on_btn_SaveGeneral_clicked()
{
    QString str;
    QByteArray ba;

    str = ui->txtMN->text();
    ba = str.toLatin1();
    snprintf(pgPara->GeneralPara.MN,MN_LEN,"%s",ba.data());
    pgPara->GeneralPara.RtdInterval = (uint16_t)ui->txtRtdInterval->text().toInt();
    pgPara->GeneralPara.OverTime = (uint8_t)ui->txtOverTime->text().toInt();
    pgPara->GeneralPara.ReCount = (uint8_t)ui->txtReCount->text().toInt();
    pgPara->GeneralPara.StType = (uint8_t)ui->comboBoxStType->currentText().toInt();
    pgPara->GeneralPara.AlarmTime = (uint8_t)ui->txtAlarmTime->text().toInt();
    pgPara->GeneralPara.RespondOpen = (uint8_t)ui->btn_RespondOpen->GetCheck();

    //调用保存配置文件函数
    syncParaShm();
}

void frmconfig::on_btn_SaveSerial_clicked()
{
    int index = ui->comboBox_serial->currentIndex();
    pgPara->SerialPara[index].BaudRate = (uint16_t)ui->comboBox_baudrate->currentText().toInt();
    pgPara->SerialPara[index].DataBits = (uint8_t)ui->comboBox_databits->currentText().toInt();
    pgPara->SerialPara[index].Parity = (uint8_t)ui->comboBox_parity->currentIndex();
    pgPara->SerialPara[index].StopBits = (uint8_t)ui->comboBox_stopbits->currentText().toInt();
    pgPara->SerialPara[index].TimeOut = ui->txtuartTimeout->text().toInt();
    pgPara->SerialPara[index].Interval = ui->txtuartInterval->text().toInt();
    //调用保存配置文件函数
    syncParaShm();
}

void frmconfig::on_btn_SaveSite_clicked()
{
    QByteArray ba;

    QString IP1=ui->txtIP1->text();
    if (!myHelper::isIpAddress(IP1)){
        myHelper::showMessageBoxError("IP1地址有误,请重新输入!");
        ui->txtIP1->setFocus();
        return;
    }
    ba = IP1.toLatin1();
    snprintf(pgPara->SitePara[0].ServerIp,16,"%s",ba.data());
    pgPara->SitePara[0].ServerPort = (uint16_t)ui->txtPort1->text().toInt();


    QString IP2=ui->txtIP2->text();
    if (!myHelper::isIpAddress(IP2)){
        myHelper::showMessageBoxError("IP2地址有误,请重新输入!");
        ui->txtIP2->setFocus();
        return;
    }
    ba = IP2.toLatin1();
    snprintf(pgPara->SitePara[1].ServerIp,16,"%s",ba.data());
    pgPara->SitePara[1].ServerPort = (uint16_t)ui->txtPort2->text().toInt();


    QString IP3=ui->txtIP3->text();
    if (!myHelper::isIpAddress(IP3)){
        myHelper::showMessageBoxError("IP3地址有误,请重新输入!");
        ui->txtIP3->setFocus();
        return;
    }
    ba = IP3.toLatin1();
    snprintf(pgPara->SitePara[2].ServerIp,16,"%s",ba.data());
    pgPara->SitePara[2].ServerPort = (uint16_t)ui->txtPort3->text().toInt();

    QString IP4=ui->txtIP4->text();
    if (!myHelper::isIpAddress(IP4)){
        myHelper::showMessageBoxError("IP4地址有误,请重新输入!");
        ui->txtIP4->setFocus();
        return;
    }
    ba = IP4.toLatin1();
    snprintf(pgPara->SitePara[3].ServerIp,16,"%s",ba.data());
    pgPara->SitePara[3].ServerPort = (uint16_t)ui->txtPort4->text().toInt();

    pgPara->SitePara[0].ServerOpen = ui->btn_server1Open->GetCheck()==true ? 1 : 0;
    pgPara->SitePara[1].ServerOpen = ui->btn_server2Open->GetCheck()==true ? 1 : 0;
    pgPara->SitePara[2].ServerOpen = ui->btn_server3Open->GetCheck()==true ? 1 : 0;
    pgPara->SitePara[3].ServerOpen = ui->btn_server4Open->GetCheck()==true ? 1 : 0;

    //调用保存配置文件函数
    syncParaShm();
}

//*************************************本机网络*************************************************/
void frmconfig::on_btn_SaveLocalNet_clicked()
{
    Myapp::DHCP=QString("%1").arg(ui->btn_Autogetip->GetCheck()==true?"1":"0");
    Myapp::LocalIP=ui->txtLocalIP->text();
    Myapp::Mask=ui->txtMask->text();
    Myapp::GateWay=ui->txtGateWay->text();

    myHelper::StringToChar(ui->txtVPNServerIP->text(), pgPara->NetPara.VPNServerIp,sizeof( pgPara->NetPara.VPNServerIp));
    myHelper::StringToChar(ui->txtVPNIPIP->text(), pgPara->NetPara.VPNIPIP,sizeof( pgPara->NetPara.VPNIPIP));
    myHelper::StringToChar(ui->txtVPNUser->text(), pgPara->NetPara.VPNUserName,sizeof( pgPara->NetPara.VPNUserName));
    QString str=QString("/mnt/nandflash/bin/VPN_client.sh %1 %2 %3") //  生成时间设置命令字符串
                            .arg(pgPara->NetPara.VPNServerIp)
                            .arg(pgPara->NetPara.VPNIPIP)
                            .arg(pgPara->NetPara.VPNUserName);
    qDebug()<<str;
    system(str.toLatin1().data()); 
    pgPara->NetPara.VPNOpen = ui->btn_VPN->GetCheck()==true?1:0;
    //调用保存配置文件函数
    syncParaShm();
    Myapp::WriteLocalNet();
}
//*************************************本机网络结束*************************************************/

void frmconfig::on_btn_Autogetip_clicked()
{
    if(ui->btn_Autogetip->GetCheck()==true)
    {
        ui->txtLocalIP->setEnabled(false);
        ui->txtMask->setEnabled(false);
        ui->txtGateWay->setEnabled(false);
    }
    else {
        ui->txtLocalIP->setEnabled(true);
        ui->txtMask->setEnabled(true);
        ui->txtGateWay->setEnabled(true);
    }
}
void frmconfig::on_btn_VPN_clicked()
{
    if(ui->btn_VPN->GetCheck()==true){
        ui->txtVPNServerIP->setEnabled(true);
        ui->txtVPNIPIP->setEnabled(true);
        ui->txtVPNUser->setEnabled(true);
    }else{
        ui->txtVPNServerIP->setEnabled(false);
        ui->txtVPNIPIP->setEnabled(false);
        ui->txtVPNUser->setEnabled(false);
    }
}
//*************************************开关量配置结束*************************************************/
void frmconfig::on_btn_SaveIo_clicked()
{
    //开排水口
    switch (ui->comboOut_drain_open->currentIndex()){
    case 0: pgPara->IOPara.Out_drain_open=6;
    break;
    case 1: pgPara->IOPara.Out_drain_open=7;
    break;
    case 2: pgPara->IOPara.Out_drain_open=8;
    break;
    case 3: pgPara->IOPara.Out_drain_open=9;
    break;
    default:break;
    }
    //关排水口
    switch (ui->comboOut_drain_close->currentIndex()){
    case 0: pgPara->IOPara.Out_drain_close=6;
    break;
    case 1: pgPara->IOPara.Out_drain_close=7;
    break;
    case 2: pgPara->IOPara.Out_drain_close=8;
    break;
    case 3: pgPara->IOPara.Out_drain_close=9;
    break;
    default:break;
    }
    //排水口总开关
    switch (ui->comboOut_drain_common->currentIndex()){
    case 0: pgPara->IOPara.Out_drain_common=6;
    break;
    case 1: pgPara->IOPara.Out_drain_common=7;
    break;
    case 2: pgPara->IOPara.Out_drain_common=8;
    break;
    case 3: pgPara->IOPara.Out_drain_common=9;
    break;
    default:break;
    }
    //回流泵控制
    switch (ui->comboOut_pump->currentIndex()){
    case 0: pgPara->IOPara.Out_reflux_control=6;
    break;
    case 1: pgPara->IOPara.Out_reflux_control=7;
    break;
    case 2: pgPara->IOPara.Out_reflux_control=8;
    break;
    case 3: pgPara->IOPara.Out_reflux_control=9;
    break;
    default:break;
    }
    //开阀门检测
    switch (ui->comboIn_drain_open->currentIndex()){
    case 0: pgPara->IOPara.In_drain_open=10;
    break;
    case 1: pgPara->IOPara.In_drain_open=11;
    break;
    case 2: pgPara->IOPara.In_drain_open=12;
    break;
    case 3: pgPara->IOPara.In_drain_open=13;
    break;
    case 4: pgPara->IOPara.In_drain_open=14;
    break;
    case 5: pgPara->IOPara.In_drain_open=15;
    break;
    default:break;
    }
    //关阀门检测
    switch (ui->comboIn_drain_close->currentIndex()){
    case 0: pgPara->IOPara.In_drain_close=10;
    break;
    case 1: pgPara->IOPara.In_drain_close=11;
    break;
    case 2: pgPara->IOPara.In_drain_close=12;
    break;
    case 3: pgPara->IOPara.In_drain_close=13;
    break;
    case 4: pgPara->IOPara.In_drain_close=14;
    break;
    case 5: pgPara->IOPara.In_drain_close=15;
    break;
    default:break;
    }
    //泵检测
    switch (ui->comboIn_pump->currentIndex()){
    case 0: pgPara->IOPara.In_reflux_open=10;
    break;
    case 1: pgPara->IOPara.In_reflux_open=11;
    break;
    case 2: pgPara->IOPara.In_reflux_open=12;
    break;
    case 3: pgPara->IOPara.In_reflux_open=13;
    break;
    case 4: pgPara->IOPara.In_reflux_open=14;
    break;
    case 5: pgPara->IOPara.In_reflux_open=15;
    break;
    default:break;
    }
    //市电检测
    switch (ui->comboIn_power->currentIndex()){
    case 0: pgPara->IOPara.In_power=10;
    break;
    case 1: pgPara->IOPara.In_power=11;
    break;
    case 2: pgPara->IOPara.In_power=12;
    break;
    case 3: pgPara->IOPara.In_power=13;
    break;
    case 4: pgPara->IOPara.In_power=14;
    break;
    case 5: pgPara->IOPara.In_power=15;
    break;
    default:break;
    }
    //调用保存配置文件函数
    syncParaShm();
}

void frmconfig::mouseReleaseEvent(QMouseEvent *)
{
    qDebug()<<QString("frmconfig mouseReleaseEvent");
    if(20 >= blk_time){
        system("echo 0 > /sys/class/backlight/backlight/brightness");
    }
    blk_time = 120;
}
