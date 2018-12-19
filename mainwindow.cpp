/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QWindow>
#include <QScreen>

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
#include "wizarddialog.h"
#include "version.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),

    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i = 0; i < ui->navStack->count(); ++i) {
        if (auto navWidget = dynamic_cast<NavWidget*>(ui->navStack->widget(i))) {
            connect(navWidget, &NavWidget::NavigationRequested,
                [this](ScreenEnum scr) {
                    Utils::setStackWidget(ui->navStack, ui->navStack->widget(scr));
                });
        }
    }

    connect(ui->navStack, SIGNAL(currentChanged(int)),
            this, SLOT(slotCurrentChanged(int)),
            Qt::QueuedConnection);

    QIcon icon(":images/jami.png");

    this->setWindowIcon(icon);

    GlobalSystemTray& sysIcon = GlobalSystemTray::instance();
    sysIcon.setIcon(icon);

    QMenu* menu = new QMenu();

    auto configAction = new QAction(tr("Settings"), this);
    connect(configAction, &QAction::triggered,
        [this]() {
            Utils::setStackWidget(ui->navStack, ui->settingswidget);
            setWindowState(Qt::WindowActive);
        });
    menu->addAction(configAction);

    auto exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, &QAction::triggered,
        [this]() {
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

    win_sparkle_set_appcast_url("http://dl.jami.net/windows/winsparkle-ring.xml");
    win_sparkle_set_app_details(L"Savoir-faire Linux", L"Jami", QString(VERSION_STRING).toStdWString().c_str());
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

    auto flags_ = windowFlags();

    auto accountList = LRCInstance::accountModel().getAccountList();
    ScreenEnum startScreen;
    if (accountList.size()) {
        readSettingsFromRegistry();
        startScreen = ScreenEnum::CallScreen;
    } else {
        startScreen = ScreenEnum::WizardScreen;
    }

    Utils::setStackWidget(ui->navStack, ui->navStack->widget(startScreen));
    if (startScreen == ScreenEnum::WizardScreen) {
        setWindowSize(startScreen);
    }
    if (auto navWidget = dynamic_cast<NavWidget*>(ui->navStack->widget(startScreen))) {
        navWidget->navigated(true);
    }

    lastScr_ = startScreen;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::slotCurrentChanged(int index)
{
    auto accountList = LRCInstance::accountModel().getAccountList();
    auto firstUse =
        (accountList.size() == 1 && lastScr_ == ScreenEnum::WizardScreen) ||
        !accountList.size();
    for (int i = 0; i < ui->navStack->count(); ++i) {
        if (auto navWidget = dynamic_cast<NavWidget*>(ui->navStack->widget(i))) {
            navWidget->navigated(index == i);
        }
    }

    auto scr = Utils::toEnum<ScreenEnum>(index);

    if (scr == ScreenEnum::SetttingsScreen) {
        ui->settingswidget->updateSettings(ui->callwidget->getLeftPanelWidth());
    }

    setWindowSize(scr, firstUse);
    lastScr_ = scr;
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
        Utils::setStackWidget(ui->navStack, ui->settingswidget);
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
    Video::PreviewManager::instance().stopPreview();
    QSettings settings;
    if (settings.value(SettingsKey::closeOrMinimized).toBool()) {
        this->hide();
        event->ignore();
    } else {
        settings.setValue(SettingsKey::geometry, saveGeometry());
        settings.setValue(SettingsKey::windowState, saveState());
    }
    LRCInstance::reset();
    QMainWindow::closeEvent(event);
}

void
MainWindow::readSettingsFromRegistry()
{
    QSettings settings;
    restoreGeometry(settings.value(SettingsKey::geometry).toByteArray());
    restoreState(settings.value(SettingsKey::windowState).toByteArray());

    LRCInstance::editableDataTransferModel()->downloadDirectory = settings.value(SettingsKey::downloadPath,
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString().toStdString() + "/";

    if (not settings.contains(SettingsKey::enableNotifications)) {
        settings.setValue(SettingsKey::enableNotifications, true);
    }
}

void
MainWindow::setWindowSize(ScreenEnum scr, bool firstUse)
{
    auto screenNumber = qApp->desktop()->screenNumber();
    QScreen* screen = qApp->screens().at(screenNumber);
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (scr == ScreenEnum::WizardScreen && !accountList.size()) {
        hide();
        setFixedSize(wizardDialogWidth, wizardDialogHeight);
    } else {
        setMinimumSize(mainWindowMinWidth, mainWindowMinHeight);
        setMaximumSize(QtMaxDimension, QtMaxDimension);
    }
    if (firstUse || !accountList.size()) {
        setGeometry(
            QStyle::alignedRect(
                Qt::LeftToRight,
                Qt::AlignCenter,
                size(),
                qApp->desktop()->screenGeometry(screenNumber)
            )
        );
        windowHandle()->setScreen(screen);
        if (scr == ScreenEnum::WizardScreen) {
            setWindowFlags(Qt::Dialog);
            setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
            adjustSize();
        } else {
            setWindowFlags(flags_);
        }
        updateGeometry();
        update();
        show();
    }
}

void
MainWindow::show()
{
    QMainWindow::show();
    disconnect(screenChangedConnection_);
    screenChangedConnection_ = connect(windowHandle(), &QWindow::screenChanged,
        [this](QScreen* screen) {
            adjustSize();
            updateGeometry();
            update();
        });
}
