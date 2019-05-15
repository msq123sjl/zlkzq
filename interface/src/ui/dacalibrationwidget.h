/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--DA校准界面功能的实现
**************************************************/
#ifndef DACALIBRATIONWIDGET_H
#define DACALIBRATIONWIDGET_H

#include <QWidget>
#include <QTimer>

class QToolButton;

namespace Ui {
class DaCalibrationWidget;
}

class DaCalibrationWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit DaCalibrationWidget(QWidget *parent = 0);
    ~DaCalibrationWidget();

private slots:

    void on_tbn_value_add_clicked();

    void on_tbn_value_sub_clicked();

    void on_tbn_value_ok_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void slotUpdataValue();
private:
    Ui::DaCalibrationWidget *ui;
    void initForm();
    void init();
    void initConnect();
    void InitUpdataTime();

    //设置按钮样式
    void setToolButtonStyle(QToolButton*tbn, const QString &text,
                            const QString iconName);
    QTimer *m_timer;

    void refreshValue();
};

#endif // KITCHENWIDGET_H
