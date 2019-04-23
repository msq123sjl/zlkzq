/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--流量界面功能的实现
**************************************************/
#ifndef FLOWRTDWIDGET_H
#define FLOWRTDWIDGET_H

#include <QWidget>
#include <QToolButton>
//#include "analysisdata.h"

namespace Ui {
class FlowRtdwidget;
}

class FlowRtdwidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit FlowRtdwidget(QWidget *parent = 0);
    ~FlowRtdwidget();

private slots:
    void slotShowCurrentData();
private:
    Ui::FlowRtdwidget *ui;
    QTimer *m_timer;
 
    void initForm();
    void init();
    void initConnect();
    void InitShowData();
};

#endif // PARLOURWIDGET_H
