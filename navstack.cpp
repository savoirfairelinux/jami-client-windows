/***************************************************************************
 * Copyright (C) 2011-2015 by Savoir-Faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

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
        connect(navList_[i], SIGNAL(NavigationRequested(ScreenEnum)),
                this, SLOT(onNavigationRequested(ScreenEnum)));
        connect(navList_[i], SIGNAL(BackRequested()),
                this, SLOT(onBackRequested()));
    }
}

NavStack::~NavStack()
{}

void
NavStack::onNavigationRequested(ScreenEnum screen) {
    if (screen < CallScreen) {
        bar_->setCurrentWidget(navList_[screen]);
    } else {
        stack_->setCurrentWidget(navList_[screen]);
        setNavBar(navList_[screen]);
        stackNav_.append(screen);
    }
}

void
NavStack::onBackRequested() {

    navList_[stackNav_.pop()]->atExit();
    ScreenEnum screen;

    if (stackNav_.size() == 0)
        screen = CallScreen;
    else
        screen = stackNav_.first();

    stack_->setCurrentWidget(navList_[screen]);
    setNavBar(navList_[screen]);
}

void
NavStack::setNavBar(NavWidget* navW) {
    if (navW->barDesired != END)
        bar_->setCurrentWidget(navList_[navW->barDesired]);
}
