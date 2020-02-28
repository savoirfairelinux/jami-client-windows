/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "aboutdialog.h"
#include "callwidget.h"
#include "settingskey.h"
#include "utils.h"
#include "version.h"

#include <QDesktopWidget>
#include <QDir>
#include <QScreen>
#include <QStandardPaths>
#include <QTimer>
#include <QWindow>
#include <QGraphicsDropShadowEffect>

#ifdef Q_OS_WIN
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#include <windows.h>
#endif

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(isBeta ? "Jami (Beta)" : "Jami");

    installEventFilter(this);

    for (int i = 0; i < ui->navStack->count(); ++i) {
        if (auto navWidget = dynamic_cast<NavWidget*>(ui->navStack->widget(i))) {
            connect(navWidget, &NavWidget::NavigationRequested,
                [this](ScreenEnum scr) {
                    for (int i = 0; i < ui->navStack->count(); ++i) {
                        if (auto navWidget = dynamic_cast<NavWidget*>(ui->navStack->widget(i))) {
                            navWidget->navigated(scr == i);
                        }
                    }
                    Utils::setStackWidget(ui->navStack, ui->navStack->widget(scr));
                });
        }
    }

    connect(ui->callwidget, &NavWidget::NavigationSettingsPageRequested,
        [this](SettingsMenu settingsPage) {
            ui->settingswidget->navigated(true);
            Utils::setStackWidget(ui->navStack, ui->navStack->widget(ScreenEnum::SetttingsScreen));
            ui->settingswidget->setSelected(settingsPage);
        });

    connect(ui->navStack, SIGNAL(currentChanged(int)),
        this, SLOT(slotCurrentChanged(int)),
        Qt::QueuedConnection);

    QIcon icon(":images/jami.png");
    this->setWindowIcon(icon);

    setContextMenuPolicy(Qt::NoContextMenu);

    connect(&GlobalSystemTray::instance(), SIGNAL(messageClicked()), this, SLOT(notificationClicked()));
#ifdef Q_OS_WIN
    connectivityMonitor_ = std::make_unique<ConnectivityMonitor>(this);
    connect(connectivityMonitor_.get(), &ConnectivityMonitor::connectivityChanged,
            [this] { LRCInstance::connectivityChanged(); });
#endif // Q_OS_WIN
    auto flags_ = windowFlags();

    auto accountList = LRCInstance::accountModel().getAccountList();
    lastAccountCount_ = accountList.size();
    ScreenEnum startScreen;
    if (accountList.size()) {
        readSettingsFromRegistry();
        startScreen = ScreenEnum::CallScreen;
        emit LRCInstance::instance().accountListChanged();
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

    // systray menu
    GlobalSystemTray& sysIcon = GlobalSystemTray::instance();
    sysIcon.setIcon(icon);

    QMenu* menu = new QMenu();

    settingsAction_ = new QAction(tr("Settings"), this);
    connect(settingsAction_, &QAction::triggered,
        [this]() {
            if (auto currentWidget = dynamic_cast<NavWidget*>(ui->navStack->currentWidget())) {
                emit currentWidget->NavigationRequested(ScreenEnum::SetttingsScreen);
                showWindow();
            }
        });

    keyboardShortcutsAction_ = new QAction(tr("Shortcuts"), this);
    connect(keyboardShortcutsAction_, &QAction::triggered,
        [this]() {
            showWindow();
            // has to put a single shot here, otherwise, the dialog will not show
            QTimer::singleShot(0,
                [this]{
                    generateQmlDialog(qmlKeyBoardShortcutTable_);
                });
        });

    exitAction_ = new QAction(tr("Exit"), this);
    connect(exitAction_, &QAction::triggered,
            [this]() { QCoreApplication::exit(); });

    sysIcon.setContextMenu(menu);
    sysIcon.show();

    connect(&sysIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    slotAccountListChanged();

    // only add settings action once in the principal interface
    connect(&LRCInstance::instance(), &LRCInstance::accountListChanged,
            this, &MainWindow::slotAccountListChanged);

    // about menu
#ifdef Q_OS_WIN
    HMENU sysMenu = ::GetSystemMenu((HWND)winId(), FALSE);
    if (sysMenu != NULL) {
        ::AppendMenuA(sysMenu, MF_SEPARATOR, 0, 0);
        QString aboutTitle = tr("About");
        ::AppendMenuW(sysMenu, MF_STRING, IDM_ABOUTBOX, aboutTitle.toStdWString().c_str());
        QString keyboardShortcutTitle = tr("Shortcuts");
        ::AppendMenuW(sysMenu, MF_STRING, IDM_SHORTCUTSBOX, keyboardShortcutTitle.toStdWString().c_str());
    }

    // check for updates and start automatic update check if needed
    QSettings settings("jami.net", "Jami");
    if (!settings.contains(SettingsKey::autoUpdate)) {
        settings.setValue(SettingsKey::autoUpdate, true);
    }
    if (settings.value(SettingsKey::autoUpdate).toBool()) {
        Utils::checkForUpdates(false, this);
    }
    updateTimer_ = new QTimer(this);
    connect(updateTimer_, &QTimer::timeout,
        [this]() {
        QSettings settings("jami.net", "Jami");
        if (settings.value(SettingsKey::autoUpdate).toBool()) {
            Utils::checkForUpdates(false, this);
        }
    });
    long updateCheckDelay = 4 * 86400 * 1000;
    updateTimer_->start(updateCheckDelay);
#endif

#ifdef DEBUG_STYLESHEET
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
        [this]() {
            QString fileName = ":/stylesheet.css";
            QDir filePath = QDir(QCoreApplication::applicationDirPath());
            filePath.cdUp();
            filePath.cdUp();
            QFile file(filePath.filePath(fileName));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                auto fileContent = file.readAll();
                setStyleSheet(fileContent);
                file.close();
            }
        });
    timer->start(1000);
#endif

    connect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::accountRemoved,
        [this](const QString& accountId) {
            Q_UNUSED(accountId);
            emit LRCInstance::instance().accountListChanged();
        });

    overlay_ = new Darkenable(this);
}

MainWindow::~MainWindow()
{
#ifdef Q_OS_WIN
    connectivityMonitor_.reset();
#endif // Q_OS_WIN
    updateTimer_->stop();
    delete ui;
}

void MainWindow::slotCurrentChanged(int index)
{
    auto accountList = LRCInstance::accountModel().getAccountList();
    auto firstUse = (accountList.size() == 1 && lastScr_ == ScreenEnum::WizardScreen && lastAccountCount_ < accountList.size()) || !accountList.size();
    if (lastScr_ == ScreenEnum::WizardScreen && lastAccountCount_ < accountList.size()) {
        lastAccountCount_ = accountList.size();
    }

    auto scr = Utils::toEnum<ScreenEnum>(index);

    setWindowSize(scr, firstUse);
    lastScr_ = scr;
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType)

#ifdef Q_OS_WIN
    MSG* msg = (MSG*)message;

    if (msg->message == WM_SYSCOMMAND) {
        if (msg->wParam == IDM_ABOUTBOX) {
            *result = 0;
            AboutDialog aboutDialog(this);
            aboutDialog.getContainer().toStrongRef()->exec();
            return true;
        }
        if (msg->wParam == IDM_SHORTCUTSBOX) {
            generateQmlDialog(qmlKeyBoardShortcutTable_);
            return true;
        }
    }
#else
    Q_UNUSED(message)
    Q_UNUSED(result)
#endif
    return false;
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason != QSystemTrayIcon::ActivationReason::Context)
        showWindow();
}

void MainWindow::showWindow()
{
    if (currentWindowState_ == Qt::WindowMaximized) {
        showMaximized();
    } else {
        showNormal();
    }

    if (QGuiApplication::platformName() != "wayland") {
        activateWindow();
    }
    raise();
#if defined(Q_OS_WIN)
    disconnect(screenChangedConnection_);
    screenChangedConnection_ = connect(windowHandle(), &QWindow::screenChanged,
        this, &MainWindow::slotScreenChanged);
#endif
}

void
MainWindow::darken()
{
    overlay_->darken();
}

void
MainWindow::lighten()
{
    overlay_->lighten();
}

void MainWindow::notificationClicked()
{
    if (auto currentWidget = dynamic_cast<NavWidget*>(ui->navStack->currentWidget())) {
        emit currentWidget->NavigationRequested(ScreenEnum::CallScreen);
        setWindowState(Qt::WindowActive);
        showWindow();
    }
}

void MainWindow::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent* event = static_cast<QWindowStateChangeEvent*>(e);
        if (event->oldState() == Qt::WindowNoState && windowState() == Qt::WindowMaximized) {
            currentWindowState_ = Qt::WindowMaximized;
        } else if (event->oldState() == Qt::WindowMaximized && windowState() == Qt::WindowNoState) {
            currentWindowState_ = Qt::WindowNoState;
        }
    }
    QWidget::changeEvent(e);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings("jami.net", "Jami");
    if (LRCInstance::accountModel().getAccountList().size() &&
        settings.value(SettingsKey::closeOrMinimized).toBool()) {
        this->hide();
        event->ignore();
    } else {
        /* HACK: temp fix for Chromium embedded framework crash
           https://www.magpcss.org/ceforum/viewtopic.php?f=6&t=14382#p31786
           fixed as of Qt 5.12 */
        if (auto callWidget = ui->navStack->widget(ScreenEnum::CallScreen)) {
            delete callWidget;
        }
        settings.setValue(SettingsKey::geometry, saveGeometry());
        settings.setValue(SettingsKey::windowState, saveState());
        this->disconnect(screenChangedConnection_);
        QMainWindow::closeEvent(event);
#ifdef Q_OS_WIN
        connectivityMonitor_.reset();
#endif // Q_OS_WIN
    }
}

void MainWindow::readSettingsFromRegistry()
{
    QSettings settings("jami.net", "Jami");

    restoreGeometry(settings.value(SettingsKey::geometry).toByteArray());
    restoreState(settings.value(SettingsKey::windowState).toByteArray());

    LRCInstance::dataTransferModel().downloadDirectory = settings.value(SettingsKey::downloadPath,
                                                                              QStandardPaths::writableLocation(QStandardPaths::DownloadLocation))
                                                                      .toString()
        + "/";

    if (not settings.contains(SettingsKey::enableNotifications)) {
        settings.setValue(SettingsKey::enableNotifications, true);
    }

    if (not settings.contains(SettingsKey::closeOrMinimized)) {
        settings.setValue(SettingsKey::closeOrMinimized, true);
    }
}

void MainWindow::setWindowSize(ScreenEnum scr, bool firstUse)
{
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (firstUse || !accountList.size()) {
        auto screenNumber = qApp->desktop()->screenNumber();
        if (scr == ScreenEnum::WizardScreen) {
            setGeometry(
                QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    size(),
                    qApp->desktop()->screenGeometry(screenNumber)));
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

void MainWindow::slotScreenChanged(QScreen* screen)
{
    Utils::setCurrentScalingRatio(screen->logicalDotsPerInchX() / 96);
    qobject_cast<NavWidget*>(ui->navStack->currentWidget())->updateCustomUI();
    if (Utils::getCurrentScalingRatio() > 1.0) {
        isScreenChanged_ = true;
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    qobject_cast<NavWidget*>(ui->navStack->currentWidget())->updateCustomUI();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    Q_UNUSED(watched)
    if (event->type() == QEvent::NonClientAreaMouseButtonRelease) {
        if (isScreenChanged_) {
            resize(minimumSizeHint());
            isScreenChanged_ = false;
        }
    }
    return false;
}

void MainWindow::keyReleaseEvent(QKeyEvent* ke)
{
    emit keyReleased(ke);
    QMainWindow::keyReleaseEvent(ke);
}

void MainWindow::slotAccountListChanged()
{
    auto systrayMenu = GlobalSystemTray::instance().contextMenu();
    systrayMenu->clear();
    if (LRCInstance::accountModel().getAccountList().size()) {
        systrayMenu->addAction(settingsAction_);
        systrayMenu->addAction(keyboardShortcutsAction_);
        systrayMenu->addAction(exitAction_);
    } else {
        systrayMenu->addAction(exitAction_);
    }
    if (!LRCInstance::accountModel().getAccountList().size()) {
        if (auto currentWidget = dynamic_cast<NavWidget*>(ui->navStack->currentWidget())) {
            emit currentWidget->NavigationRequested(ScreenEnum::WizardScreen);
        }
    }
}

void
MainWindow::generateQmlDialog(const QUrl& qmlSource)
{
    QmlPopupWidget qmlKeyboardShortcuts(qmlSource, this);

    qmlKeyboardShortcuts.setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    qmlKeyboardShortcuts.setAttribute(Qt::WA_AlwaysStackOnTop);
    qmlKeyboardShortcuts.setClearColor(Qt::transparent);
    qmlKeyboardShortcuts.setMinimumWidth(qmlKeyboardShortcuts.rootObject()->property("minWidth").toInt());
    qmlKeyboardShortcuts.setMinimumHeight(qmlKeyboardShortcuts.rootObject()->property("minHeight").toInt());

    qmlKeyboardShortcuts.getContainer().toStrongRef()->exec();
}