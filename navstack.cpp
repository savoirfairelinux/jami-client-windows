/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                           *
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

#include <QPropertyAnimation>

NavStack::NavStack(QStackedWidget* stack, QWidget *parent)
    : stack_(stack)
{
    Q_UNUSED(parent)

    navList_.append(new CallWidget());
    navList_.append(new ConfigurationWidget());

    for (int i = 0; i < END; i++) {
        stack_->addWidget(navList_[i]);
        connect(navList_[i], SIGNAL(NavigationRequested(ScreenEnum)), this
                                                       , SLOT(onNavigationRequested(ScreenEnum)));
        connect(navList_[i], SIGNAL(BackRequested()), this, SLOT(onBackRequested()));
    }
}

NavStack::~NavStack()
{
    for (int i = 0; i < END; i++) {
        delete navList_[i];
    }
}

NavWidget*
NavStack::getNavWidget(ScreenEnum wantedNavWidget)
{
    return navList_[wantedNavWidget];
}

void
NavStack::onNavigationRequested(ScreenEnum screen)
{
    if (navList_[screen] == stack_->currentWidget())
        return;

    stack_->setCurrentWidget(navList_[screen]);
    stackNav_.append(screen);

    QPropertyAnimation* slideStacked = new QPropertyAnimation(navList_[screen], "pos");
    slideStacked->setTargetObject(navList_[screen]);
    slideStacked->setDuration(animDuration_);
    if( screen == CallScreen)
        slideStacked->setStartValue(-QPoint(navList_[screen]->width(), navList_[screen]->y()));
    else
        slideStacked->setStartValue(QPoint(navList_[screen]->width(), navList_[screen]->y()));
    slideStacked->setEndValue(QPoint(navList_[screen]->x(), navList_[screen]->y()));
    slideStacked->setEasingCurve(QEasingCurve::OutQuad);
    slideStacked->start();

    delete slideStacked;
}
