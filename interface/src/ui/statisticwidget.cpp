/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--历史数据界面功能的实现
**************************************************/
#include "statisticwidget.h"
#include "ui_statisticwidget.h"
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QDebug>
#include "myhelper.h"

extern "C"{
#include "tinz_pub_shm.h"
#include "tinz_base_def.h"
#include "tinz_base_data.h"
}
extern pstHistoryData pgHistoryData;

StatisticWidget::StatisticWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticWidget)
{
    ui->setupUi(this);

    initForm();
    initStyle();
    InitTable();
}

StatisticWidget::~StatisticWidget()
{
    delete ui;
}

void StatisticWidget::initStyle()
{
    //m_sqlhelp = new SqlHelp;
}

void StatisticWidget::initForm()
{
    m_currentIndex = 0;
    ui->Retransmit->setEnabled(false);
    //ui->tabWidget->setTabText(0,"1234");
    //ui->tabWidget->setStyleSheet("QTabBar::tab{ max-height: 0px; max-width:60px;}");
    //ui->tabWidget->setTabBar()->setStyleSheet("QTabBar::tab{ min-height: 60px; min-width:60px;}");
    //ui->tabWidget->setTabBar();
    //ui->label_dateTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
}

void StatisticWidget::InitTable()
{
    int iLoop;
    model_calibration = new QStandardItemModel(this);
    model_calibration->setColumnCount(3);
    model_calibration->setHeaderData( 0,Qt::Horizontal,QString::fromLocal8Bit("时间"));
    model_calibration->setHeaderData( 1,Qt::Horizontal,QString::fromLocal8Bit("测量值"));
    model_calibration->setHeaderData( 2,Qt::Horizontal,QString::fromLocal8Bit("单位"));
    ColumnWidths[0]=200;
    ColumnWidths[1]=150;
    ColumnWidths[2]=80;
    ui->tableView->setModel(model_calibration);
    //表头信息显示居中
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    //设置列宽根据内容变化
    for(iLoop=0;iLoop<model_calibration->columnCount();iLoop++){
        ui->tableView->horizontalHeader()->setResizeMode(iLoop,QHeaderView::Interactive);
        ui->tableView->setColumnWidth(iLoop,ColumnWidths[iLoop]);
    }
    //点击表头时不对表头光亮
    ui->tableView->horizontalHeader()->setHighlightSections(false);
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
}
void StatisticWidget::ShowData()
{
    int row=0;
    //int column=0;
    unsigned int iLoop;
    //QString Unit;
    QFont ft;
    ft.setPointSize(9);

    model_calibration->removeRows(0,model_calibration->rowCount()); //清空数据表
    //ui->tableView->scrollToTop();  //滚动到顶部
    for(iLoop=0;iLoop<pgHistoryData->Pollutant.cnt;iLoop++){
        //column = 0;
        QDateTime datatime = QDateTime::fromString(QString(pgHistoryData->Pollutant.Row[iLoop].DataTime),"yyyyMMddhhmmss");
        switch(pgHistoryData->Pollutant.DataType){
            case 1:
                model_calibration->setItem(row,0,new QStandardItem(datatime.toString("yyyy-MM-dd hh:mm")));
                break;
            case 2:
                model_calibration->setItem(row,0,new QStandardItem(datatime.toString("yyyy-MM-dd")));
                break;
            case 5:
                model_calibration->setItem(row,0,new QStandardItem(datatime.toString("yyyy")));
                break;
        default:
                model_calibration->setItem(row,0,new QStandardItem(datatime.toString("yyyy-MM")));
        }
        model_calibration->item(row,0)->setTextAlignment(Qt::AlignCenter);           //设置字符位置
        model_calibration->setItem(row,1,new QStandardItem(QString::number(pgHistoryData->Pollutant.Row[iLoop].data,'f',2)));
        model_calibration->item(row,1)->setTextAlignment(Qt::AlignCenter);        //设置字符位置
        model_calibration->setItem(row,2,new QStandardItem(QString("mg/m³")));
        model_calibration->item(row,2)->setTextAlignment(Qt::AlignCenter);        //设置字符位置
        //ui->tableView->scrollToBottom();//滚动到底部
        //ui->tableView->scrollTo(model_calibration->index(row,1));
        row++;
    }
    ui->tableView->scrollToTop();  //滚动到顶部
}
void StatisticWidget::initData()
{
/*     ColumnNames[0] = tr("device_name");
    ColumnNames[1] = tr("device_value");
    ColumnNames[2] = tr("device_data");
    ColumnNames[3] = tr("device_remark");

    ColumnWidths[0] = 120;
    ColumnWidths[1] = 110;
    ColumnWidths[2] = 160;
    ColumnWidths[3] = 100; */

    //设置需要显示数据的表格和翻页的按钮
   // m_sqlhelp->SetControlTable(ui->tableView,ui->labInfo,ui->btnFirst,ui->btnPre,ui->btnNext,ui->btnLast);
    //加载初始数据,按编号正序显示
    //m_sqlhelp->BindData("v_device_info","device_data","",7,ColumnNames,ColumnWidths);
}

int StatisticWidget::query_term_check()
{
    int res =0;
    int index = ui->comboBox_data_type->currentIndex();
    res = ui->dateTimeStart->dateTime().secsTo(ui->dateTimeStop->dateTime());
    if(res < 0){
        myHelper::showMessageBoxInfo(QString("结束时间必须大于起始时间"));
        return TINZ_ERROR;
    }
    switch(index+1){
       case 1:
           res = ui->dateTimeStart->dateTime().secsTo(ui->dateTimeStop->dateTime());
           if(res > 6 * 60 * 60){
                myHelper::showMessageBoxInfo(QString("实时数据查询时间范围不能超过6小时"));
                return TINZ_ERROR;
           }
           break;
       case 2:
           res = ui->dateTimeStart->dateTime().daysTo(ui->dateTimeStop->dateTime());
           if(res > 180){
                myHelper::showMessageBoxInfo(QString("天数据查询时间范围不能超过180天"));
                return TINZ_ERROR;
           }
           break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            return TINZ_ERROR;
    }
    return TINZ_OK;
}

void StatisticWidget::on_pushButton_clicked()
{
    QString str;
    int iLoop;
    model_calibration->removeRows(0,model_calibration->rowCount()); //清空数据表
    pgHistoryData->Pollutant.DataType =  ui->comboBox_data_type->currentIndex() + 1;
    pgHistoryData->Pollutant.PollutantType = ui->comboBox_pollutant_type->currentIndex();
    str = ui->dateTimeStart->dateTime().toString("yyyyMMddhhmmss");
    myHelper::StringToChar(str,pgHistoryData->Pollutant.StartDataTime,sizeof(pgHistoryData->Pollutant.StartDataTime));
    str = ui->dateTimeStop->dateTime().toString("yyyyMMddhhmmss");
    myHelper::StringToChar(str,pgHistoryData->Pollutant.StopDataTime,sizeof(pgHistoryData->Pollutant.StopDataTime));
    pgHistoryData->Pollutant.cnt = 0;
    pgHistoryData->Pollutant.flag = 1;
    if(TINZ_ERROR != query_term_check()){
        /*等待查询数据*/
        for(iLoop = 0;iLoop<3;iLoop++){
            if(2 == pgHistoryData->Pollutant.flag){break;}
            sleep(1);
        }
        if(2 == pgHistoryData->Pollutant.flag && pgHistoryData->Pollutant.cnt > 0){
            this->ShowData();
        }else{
            myHelper::showMessageBoxInfo(QString("无查询数据，或查询超时"));
        }
    }
}

void StatisticWidget::on_comboBox_data_type_currentIndexChanged(int index)
{
     model_calibration->removeRows(0,model_calibration->rowCount()); //清空数据表
     switch(index+1){
        case 1:
            ui->dateTimeStart->setDateTime(QDateTime::currentDateTime().addSecs(-6*60*60));
            ui->dateTimeStop->setDateTime(QDateTime::currentDateTime());
            break;
        case 2:
            ui->dateTimeStart->setDateTime(QDateTime::currentDateTime().addMonths(-1));
            ui->dateTimeStop->setDateTime(QDateTime::currentDateTime());
            break;
         case 3:
             ui->dateTimeStart->setDateTime(QDateTime::currentDateTime().addYears(-1));
             ui->dateTimeStop->setDateTime(QDateTime::currentDateTime());
             break;
         case 4:
             ui->dateTimeStart->setDateTime(QDateTime::currentDateTime().addYears(-1));
             ui->dateTimeStop->setDateTime(QDateTime::currentDateTime());
             break;
         case 5:
             ui->dateTimeStart->setDateTime(QDateTime::currentDateTime().addYears(-5));
             ui->dateTimeStop->setDateTime(QDateTime::currentDateTime());
             break;
     }
}

void StatisticWidget::on_comboBox_pollutant_type_currentIndexChanged()
{
    model_calibration->removeRows(0,model_calibration->rowCount()); //清空数据表
}
