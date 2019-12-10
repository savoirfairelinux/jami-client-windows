/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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
    WizardScreen,
    CallScreen,
    SetttingsScreen,
    END
};

enum SettingsMenu {
    Account,
    General,
    Media
};

class NavWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NavWidget(QWidget* parent = 0);
    ~NavWidget();

    virtual void navigated(bool to) = 0;
    virtual void updateCustomUI() = 0;

signals:
    void NavigationRequested(ScreenEnum screen);
    void NavigationSettingsPageRequested(SettingsMenu settingsPage);

protected slots:
    virtual void slotAccountListChanged() = 0;

};
