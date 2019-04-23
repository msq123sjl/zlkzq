#include "myswitchbutton.h"
#include <QDebug>
/* 说明:自定义开关按钮控件实现文件
 * 功能:用来控制配置文件的开关设置
 */
mySwitchButton::mySwitchButton(QWidget *parent): QPushButton(parent)
{    
    setCursor(QCursor(Qt::PointingHandCursor));
    isCheck = false;

    styleOn = "border-image: url(:/images/switch/btncheckon.png); border: 0px;";
    styleOff = "border-image: url(:/images/switch/btncheckoff.png); border: 0px;";

    setFixedSize(80,30);            //不允许变化大小
    setStyleSheet(styleOff);        //设置当前样式
    connect(this,SIGNAL(clicked()),this,SLOT(ChangeOnOff()));
    this->setFlat(true);
}

void mySwitchButton::ChangeOnOff()
{
    
    qDebug()<<QString("ChangeOnOff");
    if (isCheck){
        setStyleSheet(styleOff);
        isCheck = false;
    }else{
        setStyleSheet(styleOn);
        isCheck = true;
    }
}

//设置当前选中状态
void mySwitchButton::SetCheck(bool isCheck)
{
    qDebug()<<QString("start this->isCheck:%1,isCheck:%2").arg(this->isCheck).arg(isCheck);
    if (this->isCheck != isCheck)
    {
        this->isCheck = !isCheck;
        qDebug()<<QString("init ChangeOnOff");
        ChangeOnOff();
    }else{
        if(this->isCheck){
            setStyleSheet(styleOn);
        }else{
            setStyleSheet(styleOff);
        }
    }
    qDebug()<<QString("stop this->isCheck:%1,isCheck:%2").arg(this->isCheck).arg(isCheck);
}
