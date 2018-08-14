/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include <QDesktopWidget>

#include "media/text.h"
#include "media/textrecording.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#endif

#include "aboutdialog.h"
#include "settingskey.h"
#include "winsparkle.h"
#include "callmodel.h"
#include "callwidget.h"
#include "utils.h"

 // New LRC
#undef ERROR
#include "api/lrc.h"
#include "api/account.h"
#include "api/newaccountmodel.h"
#include "api/newcallmodel.h"
#include "api/behaviorcontroller.h"
#include "api/conversation.h"
#include "api/contactmodel.h"
#include "api/contact.h"
#include "api/datatransfermodel.h"

class GlobalLRCWrapper {
public:
    static inline lrc::api::Lrc& getAPI() {
        return *(instance().lrc_);
    };
    static inline void connectivityChanged() {
        instance().lrc_->connectivityChanged();
    };
    static inline const lrc::api::NewAccountModel& accountModel() {
        return instance().lrc_->getAccountModel();
    };
    static inline const lrc::api::BehaviorController& behaviorController() {
        return instance().lrc_->getBehaviorController();
    };
    static inline const lrc::api::DataTransferModel& dataTransferModel() {
        return instance().lrc_->getDataTransferModel();
    };
    static inline bool isConnected() {
        return instance().lrc_->isConnected();
    };

private:
    std::unique_ptr<lrc::api::Lrc> lrc_;

    static GlobalLRCWrapper& instance() {
        static GlobalLRCWrapper instance_;
        return instance_;
    }

    GlobalLRCWrapper() {
        lrc_ = std::make_unique<lrc::api::Lrc>();
    };
};


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->callwidget, &CallWidget::NavigationRequested,
            [this](ScreenEnum scr){Utils::slidePage(ui->navStack, ui->navStack->widget(scr));});
    connect(ui->configurationwidget, &ConfigurationWidget::NavigationRequested,
            [this](ScreenEnum scr){Utils::slidePage(ui->navStack, ui->navStack->widget(scr));});

    QIcon icon(":images/ring.png");

    this->setWindowIcon(icon);

    GlobalSystemTray& sysIcon = GlobalSystemTray::instance();
    sysIcon.setIcon(icon);

    QMenu* menu = new QMenu();

    auto configAction = new QAction(tr("Configuration"), this);
    menu->addAction(configAction);

    auto exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered, [this](){
        QCoreApplication::exit();
    });
    menu->addAction(exitAction);

    sysIcon.setContextMenu(menu);
    sysIcon.show();

    connect(&sysIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    connect(&CallModel::instance(), SIGNAL(incomingCall(Call*)),
            this, SLOT(onIncomingCall(Call*)));

#ifdef Q_OS_WIN
    HMENU sysMenu = ::GetSystemMenu((HWND) winId(), FALSE);
    if (sysMenu != NULL) {
        ::AppendMenuA(sysMenu, MF_SEPARATOR, 0, 0);
        QString aboutTitle = tr("About");
        ::AppendMenuW(sysMenu, MF_STRING, IDM_ABOUTBOX, aboutTitle.toStdWString().c_str());
    }
#endif

    readSettingsFromRegistry();

    win_sparkle_set_appcast_url("http://dl.ring.cx/windows/winsparkle-ring.xml");
    win_sparkle_set_app_details(L"Savoir-faire Linux", L"Ring", QString(NIGHTLY_VERSION).toStdWString().c_str());
    win_sparkle_set_shutdown_request_callback([]() {QCoreApplication::exit();});
    win_sparkle_set_did_find_update_callback([]() {MainWindow::instance().showNormal();});
    win_sparkle_init();

    if (win_sparkle_get_last_check_time() == -1) {
        win_sparkle_set_update_check_interval(86400);
    }

    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [=]() {
        win_sparkle_cleanup();
    });

    setContextMenuPolicy(Qt::NoContextMenu);

    connect(&GlobalSystemTray::instance(), SIGNAL(messageClicked()), this, SLOT(notificationClicked()));

    connect(&netManager_, &QNetworkConfigurationManager::onlineStateChanged, [=](bool online) {
        Q_UNUSED(online)
        AccountModel::instance().slotConnectivityChanged();
    });

    // init LRC
    auto accountList = GlobalLRCWrapper::accountModel().getAccountList();
    for (auto& accountId : accountList) {
        qDebug() << "NEW LRC **** ACC_ID: " << accountId.c_str() << "\n";
    }

    QObject::connect(   &GlobalLRCWrapper::behaviorController(),
                        &lrc::api::BehaviorController::debugMessageReceived,
                        this,
                        &MainWindow::debugMessageReceived);

}

void
MainWindow::debugMessageReceived(const std::string& message)
{
    OutputDebugStringW(L"***************************DAEMON DEBUG***************************");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool
MainWindow::init()
{
    return ui->callwidget->findRingAccount();
}

void
MainWindow::onRingEvent(const QString& uri)
{
    this->showNormal();
    if (not uri.isEmpty()) {
        auto outCall = CallModel::instance().dialingCall();
        outCall->setDialNumber(uri);
        outCall->performAction(Call::Action::ACCEPT);
    }
}

bool
MainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType)

#ifdef Q_OS_WIN
    MSG* msg = (MSG*) message;

    if (msg->message == WM_SYSCOMMAND) {
        if ((msg->wParam & 0xfff0) == IDM_ABOUTBOX) {
            *result = 0;

            AboutDialog aboutDialog;
            aboutDialog.exec();

            return true;
        }
    }
#else
    Q_UNUSED(message)
    Q_UNUSED(result)
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
MainWindow::notificationClicked() {
    showNormal();
    activateWindow();
    raise();
}

void
MainWindow::onIncomingCall(Call* call)
{
    Q_UNUSED(call);
    QWidget::showNormal();
}

void
MainWindow::createThumbBar()
{
#ifdef Q_OS_WIN
    QWinThumbnailToolBar* thumbbar = new QWinThumbnailToolBar(this);
    thumbbar->setWindow(this->windowHandle());
    QWinThumbnailToolButton* settings = new QWinThumbnailToolButton(thumbbar);
    settings->setToolTip("Settings");
    QIcon icon(":/images/settings.png");
    settings->setIcon(icon);
    settings->setDismissOnClick(true);
    connect(settings, &QWinThumbnailToolButton::clicked, [this]() {
        Utils::slidePage(ui->navStack, ui->configurationwidget);
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

void
MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    if (settings.value(SettingsKey::closeOrMinimized).toBool()) {
        this->hide();
        event->ignore();
    } else {
        settings.setValue(SettingsKey::geometry, saveGeometry());
        settings.setValue(SettingsKey::windowState, saveState());
    }
    QMainWindow::closeEvent(event);
}

void
MainWindow::readSettingsFromRegistry()
{
    QSettings settings;
    restoreGeometry(settings.value(SettingsKey::geometry).toByteArray());
    restoreState(settings.value(SettingsKey::windowState).toByteArray());
    if (not settings.contains(SettingsKey::enableNotifications)) {
        settings.setValue(SettingsKey::enableNotifications, true);
    }
}