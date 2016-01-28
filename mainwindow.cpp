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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSizeGrip>

#include "media/text.h"
#include "media/textrecording.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#endif

#include "aboutdialog.h"
#include "mainwindowtoolbar.h"

#ifdef ENABLE_AUTOUPDATE
#include "winsparkle.h"
#endif

#include "callmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mwToolBar_(new MainWindowToolBar)
{
    ui->setupUi(this);

    QIcon icon(":images/ring.png");

    this->setWindowIcon(icon);

    GlobalSystemTray& sysIcon = GlobalSystemTray::instance();
    sysIcon.setIcon(icon);

    QMenu *menu = new QMenu();

    auto configAction = new QAction("Configuration", this);
    menu->addAction(configAction);

    sysIcon.setContextMenu(menu);
    sysIcon.show();

    connect(&sysIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    connect(&CallModel::instance(), SIGNAL(incomingCall(Call*)),
            this, SLOT(onIncomingCall(Call*)));

    navStack_ = new NavStack(ui->bar, ui->stackedWidgetView, this);
    connect(configAction, &QAction::triggered, [this]() {
        navStack_->onNavigationRequested(ScreenEnum::ConfScreen);
    });

#ifdef Q_OS_WIN32
    HMENU sysMenu = ::GetSystemMenu((HWND) winId(), FALSE);
    if (sysMenu != NULL) {
        ::AppendMenuA(sysMenu, MF_SEPARATOR, 0, 0);
        QString aboutTitle = tr("About");
        ::AppendMenuA(sysMenu, MF_STRING, IDM_ABOUTBOX, aboutTitle.toStdString().c_str());
    }
#endif

    resize(1054, 600);

    addToolBar(static_cast<QToolBar*>(mwToolBar_));

    auto callWidget = static_cast<CallWidget*>(navStack_->getNavWidget(ScreenEnum::CallScreen));

    connect(mwToolBar_->getSettingsButton(), &QAction::triggered,
            callWidget, &CallWidget::settingsButton_clicked);
    connect(mwToolBar_->getHistoryButton(), &QAction::triggered,
            callWidget, &CallWidget::historicButton_clicked);
    connect(mwToolBar_->getContactListButton(), &QAction::triggered,
            callWidget, &CallWidget::contactButton_clicked);

#ifdef ENABLE_AUTOUPDATE
    win_sparkle_set_appcast_url("http://gpl.savoirfairelinux.net/ring-download/windows/winsparkle-ring.xml");
    win_sparkle_set_app_details(L"Savoir-faire Linux", L"Ring", QString(NIGHTLY_VERSION).toStdWString().c_str());
    win_sparkle_set_shutdown_request_callback([]() {QCoreApplication::exit();});
    win_sparkle_set_did_find_update_callback([]() {MainWindow::instance().showNormal();});
    win_sparkle_init();

    if (win_sparkle_get_last_check_time() == -1) {
        win_sparkle_set_update_check_interval(7 * 86400);
    }

    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [=]() {
        win_sparkle_cleanup();
    });
#endif

    setContextMenuPolicy(Qt::NoContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete navStack_;
}

void
MainWindow::onRingEvent(const QString &uri)
{
    this->showNormal();
    if (not uri.isEmpty()) {
        auto outCall = CallModel::instance().dialingCall();
        outCall->setDialNumber(uri);
        outCall->performAction(Call::Action::ACCEPT);
    }
}

bool
MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)

#ifdef Q_OS_WIN32
    MSG *msg = (MSG*) message;

    if (msg->message == WM_SYSCOMMAND) {
        if ((msg->wParam & 0xfff0) == IDM_ABOUTBOX) {
            *result = 0;

            AboutDialog aboutDialog;
            aboutDialog.exec();

            return true;
        }
    }
#endif
    return false;
}

void
MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason != QSystemTrayIcon::ActivationReason::Context)
        this->show();
}

void
MainWindow::onIncomingCall(Call *call)
{
    Q_UNUSED(call);
    QWidget::showNormal();
}

void
MainWindow::createThumbBar()
{
#ifdef Q_OS_WIN32
    QWinThumbnailToolBar *thumbbar = new QWinThumbnailToolBar(this);
    thumbbar->setWindow(this->windowHandle());
    QWinThumbnailToolButton *settings = new QWinThumbnailToolButton(thumbbar);
    settings->setToolTip("Settings");
    QIcon icon(":/images/settings.png");
    settings->setIcon(icon);
    settings->setDismissOnClick(true);
    connect(settings, &QWinThumbnailToolButton::clicked, [this]() {
        navStack_->onNavigationRequested(ScreenEnum::ConfScreen);
    });

    thumbbar->addButton(settings);
#endif
}

void
MainWindow::switchNormalMaximize()
{
    if(isMaximized())
        showNormal();
    else
        showMaximized();
}
