/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--PH界面功能的实现
**************************************************/
#ifndef PHRTDWIDGET_H
#define PHRTDWIDGET_H

#include <QWidget>
#include <QToolButton>
//#include "analysisdata.h"

namespace Ui {
class PHRtdwidget;
}

class PHRtdwidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PHRtdwidget(QWidget *parent = 0);
    ~PHRtdwidget();

private:

    void initForm();
    void init();
    void initConnect();

private:
    Ui::PHRtdwidget *ui;
};

#endif // PARLOURWIDGET_H
