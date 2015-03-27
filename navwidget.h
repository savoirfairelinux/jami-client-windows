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

#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include <QWidget>

enum ScreenEnum {
    Main,
    Nav,
    //DO not add main widget screen before callScreen
    CallScreen,
    //PivotScreen,
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
