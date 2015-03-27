#ifndef MAINBAR_H
#define MAINBAR_H

#include <QWidget>
#include "navwidget.h"

namespace Ui {
class MainBar;
}

class MainBar : public NavWidget
{
    Q_OBJECT

public:
    explicit MainBar(QWidget *parent = 0);
    ~MainBar();

private slots:
    void on_confButton_clicked();

    void on_callLineEdit_returnPressed();

    void on_callButton_clicked();

    void on_toolButton_clicked();

private:
    Ui::MainBar *ui;
    void callAction();
};

#endif // MAINBAR_H
