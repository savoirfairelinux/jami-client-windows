#ifndef NAVSTACK_H
#define NAVSTACK_H

#include <QStackedWidget>
#include <QStack>

#include "navwidget.h"
#include "configurationwidget.h"
#include "mainbar.h"
#include "navbar.h"
#include "callwidget.h"

class NavStack : public QWidget
{
    Q_OBJECT
public:
    NavStack(QStackedWidget* bar, QStackedWidget* stack, QWidget* parent = nullptr);
    ~NavStack();

public slots:
    void onNavigationRequested(ScreenEnum screen);
    void onBackRequested();

private:
    QStackedWidget* bar_;
    QStackedWidget* stack_;
    QList<NavWidget*> navList_;
    QStack<ScreenEnum> stackNav_;
    void setNavBar(NavWidget *navW);
};

#endif // NAVSTACK_H
