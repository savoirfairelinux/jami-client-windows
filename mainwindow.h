/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                                *
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
#include <QNetworkConfigurationManager>

#include "navwidget.h"
#include "settingswidget.h"

// LRC
#include "call.h"

static constexpr char IDM_ABOUTBOX = 0x0010;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    inline static MainWindow& instance()  {
        static auto instance = new MainWindow();
        return *instance;
    }
    void createThumbBar();
    bool init();

    void show();

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result);
    void closeEvent(QCloseEvent* event);

public slots:
    void onRingEvent(const QString& uri);

private slots:
    void slotCurrentChanged(int index);
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void onIncomingCall(Call* call);
    void switchNormalMaximize();
    void notificationClicked();

private:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void setWindowSize(ScreenEnum scr, bool firstUse = false);
    ScreenEnum lastScr_;
    Qt::WindowFlags flags_;

    void readSettingsFromRegistry();

    Ui::MainWindow* ui;
    QNetworkConfigurationManager netManager_;
    QMetaObject::Connection screenChangedConnection_;
};
