#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
class QToolButton;

class frmconfig;
class Rtdwidget;
class ModelChooseWidget;
class StatisticWidget;
class frmlogin;
class AdCalibrationWidget;
class DaCalibrationWidget;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    //场景名称枚举
    enum enum_widget{
        E_POLLUTER_WIDGET = 0,       //排放量
        E_HOME_WIDGET,              //首页
        E_CALIBRATION_WIDGET,              //校准
        E_RTD_WIDGET,       //实时数据
        E_STATISTIC_WIDGET,        //统计
        E_MODEL_WIDGET,        //模式
        E_AD_WIDGET,       //AD校准
        E_DA_WIDGET       //DA校准
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

    void on_tbnStatistic_clicked();

    void on_tbnCalibration_clicked();

    void on_tbnModel_clicked();

    void on_tbnUser_clicked();

    void on_tbnNull_clicked();


    void on_tbnSafety_clicked();

    void on_tbnSetting_clicked();

    void on_tbnHome_clicked();

    void on_tbnDA_clicked();

    void on_tbnAD_clicked();

    void on_tbnPolluter_clicked();
private:
    Ui::Widget *ui;
    bool m_isPress;
    frmconfig *m_frmconfig;           //设置界面
    Rtdwidget *m_rtdWidget;                 //流量界面
    StatisticWidget *m_statisticWidget;             //统计界面

    ModelChooseWidget *m_modelWidget;               //模式选择控制界面
    frmlogin *m_frmlogin;
    AdCalibrationWidget *m_adcalibrationWidget;                 //AD校准界面
    DaCalibrationWidget *m_dacalibrationWidget;                 //DA校准界面

    QTimer *m_timer;
        
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
   /*void keyPressEvent(QKeyEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent*);
    void closeEvent(QCloseEvent *);*/

};

#endif // WIDGET_H
