#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include <QWidget>

enum ScreenEnum {
    Main,
    Nav,
    //DO not add main widget screen before call
    CallScreen,
    ConfScreen,
    END
};

class NavWidget : public QWidget
{
    Q_OBJECT

public:
    NavWidget(ScreenEnum barDesired, QWidget* parent = 0);
    ~NavWidget();
    ScreenEnum barDesired;
    virtual void atExit() = 0;

signals:
    void NavigationRequested(ScreenEnum screen);
    void BackRequested();
};

#endif // NAVSIGNALHELPER_H
