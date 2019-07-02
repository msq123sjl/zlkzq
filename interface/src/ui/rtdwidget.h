/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--流量界面功能的实现
**************************************************/
#ifndef RTDWIDGET_H
#define RTDWIDGET_H

#include <QWidget>
#include <QToolButton>
//#include "analysisdata.h"

namespace Ui {
class Rtdwidget;
}

class Rtdwidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Rtdwidget(QWidget *parent = 0);
    ~Rtdwidget();

private slots:
    void slotShowCurrentData();
private:
    Ui::Rtdwidget *ui;
    QTimer *m_timer;
    
    int  columnWidths[3];        //列宽数组
    
    void initForm();
    void init();
    void initConnect();
    void InitShowData();
    void InitRtdTable();
};

#endif // PARLOURWIDGET_H
