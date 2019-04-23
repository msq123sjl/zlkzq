#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
class QToolButton;

class frmconfig;
class FlowRtdwidget;
class CODRtdwidget;
class PHRtdwidget;
class ValveWidget;
class ModelChooseWidget;

//class StatisticWidget;
class frmlogin;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    //场景名称枚举
    enum enum_widget{
        E_RTD_WIDGET = 0,       //实时数据
        E_HOME_WIDGET,              //首页
        E_FLOW_WIDGET,           //流量
        E_COD_WIDGET,             //COD
        E_PH_WIDGET,              //PH
        E_VALVE_WIDGET,       //阀门控制
        E_MODEL_WIDGET        //模式
    };
    enum E_TEXTSIZE{
        E_NORMAL = 0,
        E_BIG = !E_NORMAL
    };
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
private slots:
    void setCurrentWidget(enum_widget enum_widget_name);
    void slotShowCurrentDataTime();//显示当前的日期和时间
    void on_tbnRtd_clicked();

    void on_tbnValve_clicked();

    void on_tbnStatistic_clicked();

    void on_tbnADDA_clicked();

    void on_tbnModel_clicked();

    void on_tbnUser_clicked();

    void on_tbnNull_clicked();

    void on_tbnFlow_clicked();

    void on_tbnCOD_clicked();

    void on_tbnPH_clicked();

    void on_tbnSafety_clicked();

    void on_tbnSetting_clicked();

    void on_tbnHome_clicked();

private:
    Ui::Widget *ui;
    bool m_isPress;
    frmconfig *m_frmconfig;           //设置界面
    FlowRtdwidget *m_flowrtdWidget;                 //流量界面
    CODRtdwidget *m_codrtdWidget;                 //COD界面
    PHRtdwidget *m_phrtdWidget;                 //PH界面
    ValveWidget *m_valveWidget;                 //厨房界面
    //StatisticWidget *m_statisticWidget;             //统计界面

    ModelChooseWidget *m_modelWidget;               //模式选择控制界面
    frmlogin *m_frmlogin;


    QTimer *m_timer;
    int blk_time;
        
    void initDataTime();
    void initForm();
    void setToolButtonStyle(QToolButton *tbn, const QString &text, int textSize, const QString iconName);  //设置按钮样式
    void initWidget();
    void initConnect();
    void initToolTip();
    void deletWidget();
    void startAnimation();

protected:
    void mousePressEvent(QMouseEvent *);

};

#endif // WIDGET_H
