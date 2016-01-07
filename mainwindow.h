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

#include "globalsystemtray.h"

#include <QMainWindow>
#include <QMouseEvent>

#include "navstack.h"

static constexpr char IDM_ABOUTBOX = 0x0010;

class WindowBarUpOne;
class MainWindowToolBar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createThumbBar();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void onIncomingCall(Call *call);
    void switchNormalMaximize();

private:
    Ui::MainWindow* ui;
    NavStack* navStack_;
    WindowBarUpOne* wbOne_;
    MainWindowToolBar* mwToolBar_;
};
