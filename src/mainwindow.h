/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
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

#include "navwidget.h"
#include "settingswidget.h"
#include "utils.h"
#include "connectivitymonitor.h"
#include "globalsystemtray.h"
#include "widgethelpers.h"

#include <QMainWindow>
#include <QMouseEvent>

static constexpr char IDM_ABOUTBOX = 0x0010;
static constexpr char IDM_SHORTCUTSBOX = 0x0011;

static constexpr uint32_t QtMaxDimension = 16777215;
static constexpr uint32_t mainWindowMinWidth = 1088;
static constexpr uint32_t mainWindowMinHeight = 638;
static constexpr uint32_t wizardDialogWidth = 512;
static constexpr uint32_t wizardDialogHeight = 638;

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
    bool init();
    void showWindow();
    void darken();
    void lighten();

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyReleaseEvent(QKeyEvent* ke) override;
    void changeEvent(QEvent* e) override;

signals:
    void keyReleased(QKeyEvent*ke);

protected:
    const QUrl qmlKeyBoardShortcutTable_ { QStringLiteral("qrc:/src/KeyBoardShortcutTable.qml") };

private slots:
    void slotCurrentChanged(int index);
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void notificationClicked();
    void slotScreenChanged(QScreen* screen);
    void slotAccountListChanged();
    void generateQmlDialog(const QUrl& qmlSource);

private:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    Ui::MainWindow* ui;

    void setWindowSize(ScreenEnum scr, bool firstUse = false);
    ScreenEnum lastScr_;
    int lastAccountCount_;
    Qt::WindowFlags flags_;
    Qt::WindowState currentWindowState_{ Qt::WindowState::WindowNoState };

    void readSettingsFromRegistry();

    QAction* settingsAction_;
    QAction* exitAction_;
    QAction* keyboardShortcutsAction_;
#ifdef Q_OS_WIN
    std::unique_ptr<ConnectivityMonitor> connectivityMonitor_;
#endif // Q_OS_WIN
    QMetaObject::Connection screenChangedConnection_;

    QTimer* updateTimer_;

    Darkenable* overlay_;
};
