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
class QMenu;
class QMenuBar;

class WindowBarUpOne : public QToolBar
{
    Q_OBJECT

public:
    WindowBarUpOne(QToolBar* parent = 0);
    ~WindowBarUpOne();
    inline QAction* getQuitButton(){ return quit_;};
    inline QAction* getMaximizeButton(){ return maximize_;};
    inline QAction* getMinimizeButton(){ return minimize_;};
    void paintEvent(QPaintEvent *event);

private:
    QAction*  minimize_;
    QAction*  maximize_;
    QAction*  quit_;
    QWidget*  spacer_;
    QPoint    dl;

protected:
    void mousePressEvent  (QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent   (QMouseEvent* event);
};
