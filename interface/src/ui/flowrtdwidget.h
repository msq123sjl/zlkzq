/*************************************************
Copyright:kevin
Author:Kevin LiQi
Date:2015-12-04
Email:kevinlq0912@163.com
QQ:936563422
Version:V1.0
Description:智能家居--客厅界面的实现，包括客厅的灯，温湿度
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

private:

    void initForm();
    void init();
    void initConnect();

private:
    Ui::FlowRtdwidget *ui;
};

#endif // PARLOURWIDGET_H
