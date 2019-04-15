/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--COD界面功能的实现
**************************************************/
#ifndef CODRTDWIDGET_H
#define CODRTDWIDGET_H

#include <QWidget>
#include <QToolButton>
//#include "analysisdata.h"

namespace Ui {
class CODRtdwidget;
}

class CODRtdwidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CODRtdwidget(QWidget *parent = 0);
    ~CODRtdwidget();

private:

    void initForm();
    void init();
    void initConnect();

private:
    Ui::CODRtdwidget *ui;
};

#endif // PARLOURWIDGET_H
