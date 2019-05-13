/*************************************************
Copyright:tinz
Author:tinz msq
Date:2019-04-01
Email:718657309@qq.com
QQ:718657309
Version:V1.0
Description:总量控制器--历史数据界面功能的实现
**************************************************/
#ifndef STATISTICWIDGET_H
#define STATISTICWIDGET_H

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class StatisticWidget;
}

class StatisticWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit StatisticWidget(QWidget *parent = 0);
    ~StatisticWidget();

private slots:

    void on_pushButton_clicked();

    void on_comboBox_data_type_currentIndexChanged(int index);

private:
    Ui::StatisticWidget *ui;
    void initStyle();                       //初始化样式
    void initForm();                        //初始化界面
    //显示历史数据
    void initData();
    void InitTable();

    int m_currentIndex;                       //当前界面id
    QString ColumnNames[4];                    //列名数组声明
    int ColumnWidths[4];                       //列宽数组声明
    QStandardItemModel  *model_calibration;
    void ShowData();
    int query_term_check();
};

#endif // STATISTICWIDGET_H
