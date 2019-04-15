/*************************************************
Copyright:kevin
Author:Kevin LiQi
Date:2015-12-07
Email:kevinlq0912@163.com
QQ:936563422
Version:V1.0
Description:智能家居---厨房安防功能的实现，主要包括可燃性气体检测，灯光控制,
            发挥部分：添加冰箱的控制
**************************************************/
#ifndef VALVEWIDGET_H
#define VALVEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "analysisdata.h"

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
    void refreshValveValue();

    //设置按钮样式
    void setToolButtonStyle(QToolButton*tbn, const QString &text,
                            const QString iconName);
    QTimer *m_timer;

};

#endif // KITCHENWIDGET_H
