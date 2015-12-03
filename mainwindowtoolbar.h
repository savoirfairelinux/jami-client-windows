/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
 * Author: Jäger Nicolas <nicolas.jager@savoirfairelinux.com>              *
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

#include <QToolBar>

class QPoint;

class MainWindowToolBar : public QToolBar
{
    Q_OBJECT

public:
    MainWindowToolBar(QToolBar* parent = 0);
    ~MainWindowToolBar();
    inline QAction* getSettingsButton(){ return settings_;};
    inline QAction* getHistoryButton(){ return history_;};
    inline QAction* getContactListButton(){ return contactslist_;};
    void historyToggled(bool checked);
    void contactslistToggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QAction*  contactslist_;
    QAction*  history_;
    QAction*  settings_;
    QWidget*  spacer_;
};
