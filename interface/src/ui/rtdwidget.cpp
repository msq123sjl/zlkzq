/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--流量界面功能的实现
**************************************************/
#include "rtdwidget.h"
#include "ui_rtdwidget.h"

#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "myapp.h"
#include <QStandardItemModel>

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
extern pstPara pgPara;
extern pstData pgData;
extern pstNetPara pgNetPara;
extern pstValveControl pgValveControl;

QStandardItemModel  *model_rtd;

Rtdwidget::Rtdwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Rtdwidget)
{
    ui->setupUi(this);

    this->initForm();
    this->InitRtdTable();
    this->init();
    this->initConnect();
}

Rtdwidget::~Rtdwidget()
{
    delete ui;
}

void Rtdwidget::initForm()
{

}

void Rtdwidget::InitRtdTable()
{
    int iLoop,jLoop;
    model_rtd = new QStandardItemModel(this);
    model_rtd->setColumnCount(3);
    model_rtd->setHeaderData( 0,Qt::Horizontal,QString::fromLocal8Bit("因子"));
    model_rtd->setHeaderData( 1,Qt::Horizontal,QString::fromLocal8Bit("实时值"));
    model_rtd->setHeaderData( 2,Qt::Horizontal,QString::fromLocal8Bit("累计值"));
    columnWidths[0]=100;
    columnWidths[1]=160;
    columnWidths[2]=180;
    ui->tableView->setModel(model_rtd);
    //表头信息显示居中
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    //设置列宽根据内容变化
    for(iLoop=0;iLoop<model_rtd->columnCount();iLoop++){
        //ui->tableView->horizontalHeader()->setResizeMode(iLoop,QHeaderView::Interactive);
        ui->tableView->setColumnWidth(iLoop,columnWidths[iLoop]);
    }
    
    //点击表头时不对表头光亮
    ui->tableView->horizontalHeader()->setHighlightSections(false);
    ui->tableView->verticalHeader()->hide();
    //ui->tableView->horizontalHeader()->setTextColor(QColor(255,255,255)); //设置文字颜色
    //选中模式为单行选中
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    //选中整行
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setStyleSheet( "QTableView::item:hover{background-color:rgb(0,200,255,255)}"
                                                             "QTableView::item:selected{background-color:#0000AA}");
    //设置非编辑状态
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    //设置表头背景色
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");
    ui->tableView->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue}");

    //ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);//垂直滚动条按项移动
    
    model_rtd->setItem(0,0,new QStandardItem(QString("污水")));
    model_rtd->setItem(0,1,new QStandardItem(QString("--")));
    model_rtd->setItem(0,2,new QStandardItem(QString("--")));
    model_rtd->setItem(1,0,new QStandardItem(QString("COD")));
    model_rtd->setItem(1,1,new QStandardItem(QString("--")));
    model_rtd->setItem(1,2,new QStandardItem(QString("--")));
    model_rtd->setItem(2,0,new QStandardItem(QString("PH")));
    model_rtd->setItem(2,1,new QStandardItem(QString("--")));
    model_rtd->setItem(2,2,new QStandardItem(QString("--")));
    for(iLoop=0;iLoop<model_rtd->rowCount();iLoop++){
        for(jLoop=0;jLoop<model_rtd->columnCount();jLoop++){
            model_rtd->item(iLoop,jLoop)->setTextAlignment(Qt::AlignCenter);
            model_rtd->item(iLoop,jLoop)->setForeground(QBrush(QColor(255, 255, 255)));
        }    
    }
    
}

void Rtdwidget::init()
{
    //允许排放量设置
    slotShowCurrentData();
}

//初始化信号和槽的连接
void Rtdwidget::initConnect()
{
    this->InitShowData();
}

void Rtdwidget::InitShowData()
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),
            this,SLOT(slotShowCurrentData()));
    m_timer->start(10000);
}

void Rtdwidget::slotShowCurrentData()
{
    static int lab_index =0;
    static int site_number = -1;
    #ifndef VALVE_AND_PUMP    
    if('2' == pgData->PollutantsData.RtdData.DataTime[0]){
        if(-1 != pgData->PollutantsData.RtdData.Row[POLLUTANT_FLOW_INDEX].rtd){
            model_rtd->setData(model_rtd->index(0, 1), QString("%1 L/s").arg(QString::number(pgData->PollutantsData.RtdData.Row[POLLUTANT_FLOW_INDEX].rtd,'f',1)));
            model_rtd->setData(model_rtd->index(0, 2), QString("%1 m³").arg(QString::number(pgData->PollutantsData.RtdData.Row[POLLUTANT_FLOW_INDEX].cou,'f',0)));
        }else{
            model_rtd->setData(model_rtd->index(0, 1), QString("--"));
            model_rtd->setData(model_rtd->index(0, 2), QString("--"));
        } 
        if(-1 != pgData->PollutantsData.RtdData.Row[POLLUTANT_COD_INDEX].rtd){
            model_rtd->setData(model_rtd->index(1, 1), QString("%1 mg/m³").arg(QString::number(pgData->PollutantsData.RtdData.Row[POLLUTANT_COD_INDEX].rtd,'f',0)));
            model_rtd->setData(model_rtd->index(1, 2), QString("%1 kg").arg(QString::number(pgData->PollutantsData.RtdData.Row[POLLUTANT_COD_INDEX].cou,'f',0)));
        }else{
            model_rtd->setData(model_rtd->index(1, 1), QString("--"));
            model_rtd->setData(model_rtd->index(1, 2), QString("--"));
        }
        if(-1 != pgData->PollutantsData.RtdData.Row[POLLUTANT_PH_INDEX].rtd){        
            model_rtd->setData(model_rtd->index(2, 1), QString("%1").arg(QString::number(pgData->PollutantsData.RtdData.Row[POLLUTANT_PH_INDEX].rtd,'f',0)));
        }else{
            model_rtd->setData(model_rtd->index(2, 1), QString("--"));
        }
    }
    #endif
    //qDebug()<<QString("DataTime:%1").arg(pgPollutantData->RtdData.DataTime);
    //snprintf(pgPollutantData->RtdData.DataTime,sizeof(pgPollutantData->RtdData.DataTime),"20190701111233");
    switch(lab_index){
        case 0:
            pgData->state.InPower == 0 ? ui->lab_top->setText("市电上电"):ui->lab_top->setText("市电掉电");
            if(QY_USER ==  Myapp::UserType){
                pgPara->Mode == 0 ? ui->lab_bottom->setText("企业人员登陆 远程模式"):ui->lab_bottom->setText("企业人员登陆 运维模式");
            }else if(GLY_USER ==  Myapp::UserType){
                pgPara->Mode == 0 ? ui->lab_bottom->setText("运维人员登陆 远程模式"):ui->lab_bottom->setText("运维人员登陆 运维模式");
            }else if(SUPER_USER ==  Myapp::UserType){  
                pgPara->Mode == 0 ? ui->lab_bottom->setText("管理人员登陆 远程模式"):ui->lab_bottom->setText("管理人员登陆 运维模式");
            }else{   
                pgPara->Mode == 0 ? ui->lab_bottom->setText("未登陆 远程模式"):ui->lab_bottom->setText("未登陆 运维模式");
            }
            break;
        case 1:
            if(0 == pgData->state.ValveState){
                uint8_t per;
                per = abs(pgValveControl->per - pgValveControl->per_measure) > 5 ? pgValveControl->per_measure : pgValveControl->per;
                ui->lab_top->setText(QString("阀门开度:%1\%").arg(per));
            }else{
                ui->lab_top->setText(QString("阀门异常"));
            }
            break;
        case 2:
            #ifndef VALVE_AND_PUMP
            ui->lab_top->setText("数据更新时间：\n" + QDateTime::fromString (QString(pgData->PollutantsData.RtdData.DataTime),"yyyyMMddhhmmss").toString("yyyy-MM-dd hh:mm"));
            #else            
            ui->lab_top->setText("数据更新时间：\n" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
            #endif
            ui->lab_bottom->setText("当前时间：\n" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
            break;
        default:
            if(pgNetPara->LTEOpen){
                if(0 == pgData->state.LTE){
                    ui->lab_top->setText("4G网络已链接");
                }else if(1 == pgData->state.LTE){
                    ui->lab_top->setText("4G网络未链接");
                }else if(2 == pgData->state.LTE){
                    ui->lab_top->setText("未检测到SIM卡");
                }else{
                    ui->lab_top->setText("4G模块无响应");
                }
            }
            pgNetPara->VPNOpen != 0 ? (pgData->state.VPN != 0 ? ui->lab_bottom->setText("VPN未链接"):ui->lab_bottom->setText(QString("VPN已登录\n%1 %2 ").arg(pgNetPara->VPNUserName).arg(pgNetPara->VPNIPIP))):ui->lab_bottom->setText("VPN未开启");
	}
    
    for(int iLoop = 0;iLoop < SITE_CNT ; iLoop++){
        site_number++;
        site_number %= SITE_CNT;
        if(pgPara->SitePara[site_number].ServerOpen){
            pgPara->SitePara[site_number].isConnected == 0 ? ui->lab_mid->setText(QString("中心站点%1已断开：\n%2:%3").arg(site_number+1).arg(pgPara->SitePara[site_number].ServerIp).arg(pgPara->SitePara[site_number].ServerPort))\
                                                                                                 :ui->lab_mid->setText(QString("中心站点%1已链接：\n%2:%3").arg(site_number+1).arg(pgPara->SitePara[site_number].ServerIp).arg(pgPara->SitePara[site_number].ServerPort));

            break;
        }
        if( SITE_CNT-1 == iLoop){
            ui->lab_mid->setText(QString("服务器未配置"));
        }
    }
    lab_index++;
    lab_index %=4;
}










