#ifndef NAVBAR_H
#define NAVBAR_H

#include <QWidget>

#include "navwidget.h"

namespace Ui {
class NavBar;
}

class NavBar : public NavWidget
{
    Q_OBJECT

public:
    explicit NavBar(QWidget *parent = 0);
    ~NavBar();
    void atExit();

private slots:
    void on_backButton_clicked();

private:
    Ui::NavBar *ui;
};

#endif // NAVBAR_H
