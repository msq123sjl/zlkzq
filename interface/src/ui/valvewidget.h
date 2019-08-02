/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--阀门控制界面功能的实现
**************************************************/
#ifndef VALVEWIDGET_H
#define VALVEWIDGET_H

#include <QWidget>
#include <QTimer>
//#include "analysisdata.h"

class QToolButton;

namespace Ui {
class ValveWidget;
}

class ValveWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ValveWidget(QWidget *parent = 0);
    ~ValveWidget();

private slots:

    void on_tbn_valve_add_clicked();

    void on_tbn_valve_sub_clicked();

    void on_pbn_valve_contrl_clicked();

    void on_tbn_valve_ok_clicked();
    
    void slotUpdataValveValue();   //更新阀门开度
private:
    Ui::ValveWidget *ui;
    void initForm();
    void init();
    void initConnect();
    void InitUpdataTime();
    void refreshValveValue(int SetValue);

    //设置按钮样式
    void setToolButtonStyle(QToolButton*tbn, const QString &text,
                            const QString iconName);
    QTimer *m_timer;

    void InitValveControl();
};

#endif // KITCHENWIDGET_H
