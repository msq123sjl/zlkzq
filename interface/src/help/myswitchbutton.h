#ifndef MYSWITCHBUTTON_H
#define MYSWITCHBUTTON_H

#include <QPushButton>

/* 说明:自定义开关按钮控件头文件
 * 功能:用来控制配置文件的开关设置
 */
class mySwitchButton : public QPushButton
{
    Q_OBJECT    
public:
    explicit mySwitchButton(QWidget *parent = 0);
    QString styleOn;
    QString styleOff;
    //获取当前选中状态
    bool GetCheck() const {return isCheck;}

    //设置当前选中状态
    void SetCheck(bool isCheck);

private:    
    bool isCheck;

    
signals:
    
private slots:    
    void ChangeOnOff();
    
};

#endif //MYSWITCHBUTTON_H
