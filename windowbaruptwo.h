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

class WindowBarUpTwo : public QToolBar
{
    Q_OBJECT

public:
    WindowBarUpTwo(QToolBar *parent = 0);
    ~WindowBarUpTwo();
    inline QAction* getSettingsButton(){ return settings_;};
    inline QAction* getHistoryButton(){ return history_;};
    inline QAction* getContactListButton(){ return contactslist_;};
    void paintEvent(QPaintEvent *event);
    void historyToggled(bool checked);
    void contactslistToggled(bool checked);

private:
    QAction*  contactslist_;
    QAction*  history_;
    QAction*  settings_;
    //~ QAction*  info_; // further use
    QWidget*  spacer_;
    QPoint    dl;

protected:
    void mousePressEvent  (QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent   (QMouseEvent *event);
};
