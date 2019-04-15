#ifndef MODELCHOOSEWIDGET_H
#define MODELCHOOSEWIDGET_H

#include <QWidget>

namespace Ui {
class ModelChooseWidget;
}

class ModelChooseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModelChooseWidget(QWidget *parent = 0);
    ~ModelChooseWidget();

private:
    void initForm();

private slots:

    void on_pbn_operation_clicked();
private:
    Ui::ModelChooseWidget *ui;
};

#endif // MODELCHOOSEWIDGET_H
