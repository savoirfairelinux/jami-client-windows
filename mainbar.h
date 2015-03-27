#ifndef MAINBAR_H
#define MAINBAR_H

#include <QWidget>
#include <QMenu>

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
    void atExit();

private slots:
    void on_confButton_clicked();
    void on_callLineEdit_returnPressed();
    void on_callButton_clicked();

    void on_exitButton_clicked();
    void on_toolButton_2_clicked();

private:
    Ui::MainBar *ui;
    void callAction();
    QMenu *menu_;

signals:
    void minimize();
};

#endif // MAINBAR_H
