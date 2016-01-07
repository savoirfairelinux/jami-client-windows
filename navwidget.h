/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                                *
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

#pragma once

#include <QWidget>

enum ScreenEnum {
    Nav,
    //DO not add main widget screen before callScreen
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

