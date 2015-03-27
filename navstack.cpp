#include "navstack.h"

NavStack::NavStack(QStackedWidget* bar, QStackedWidget* stack, QWidget *parent)
    : bar_(bar)
    , stack_(stack)
{
    navList_.append(new MainBar());
    navList_.append(new NavBar());

    navList_.append(new CallWidget());
    navList_.append(new ConfigurationWidget());

    connect(navList_[Main], SIGNAL(minimize()), parent, SLOT(showMinimized()));
    for (int i = 0; i < END; i++) {
        if (i < CallScreen)
            bar_->addWidget(navList_[i]);
        else
            stack_->addWidget(navList_[i]);
        connect(navList_[i], SIGNAL(NavigationRequested(ScreenEnum)), this, SLOT(onNavigationRequested(ScreenEnum)));
        connect(navList_[i], SIGNAL(BackRequested()), this, SLOT(onBackRequested()));
    }
}

NavStack::~NavStack()
{}

void NavStack::onNavigationRequested(ScreenEnum screen) {
    if (screen < CallScreen) {
        bar_->setCurrentWidget(navList_[screen]);
    } else {
        stack_->setCurrentWidget(navList_[screen]);
        setNavBar(navList_[screen]);
        stackNav_.append(screen);
    }
}

void NavStack::onBackRequested() {

    navList_[stackNav_.pop()]->atExit();
    ScreenEnum screen;

    if (stackNav_.size() == 0)
        screen = CallScreen;
    else
        screen = stackNav_.first();

    stack_->setCurrentWidget(navList_[screen]);
    setNavBar(navList_[screen]);
}

void NavStack::setNavBar(NavWidget* navW) {
    if (navW->barDesired != END)
        bar_->setCurrentWidget(navList_[navW->barDesired]);
}
