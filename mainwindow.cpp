/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
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

/* [15/12/03]
 * MainWindow get back the usual MS window borders until I finish to figure
 * how to custom the border correctly. --jn
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSizeGrip>
#include "aboutdialog.h"
// [15/12/03] Temporarily Disabled. --jn
//~ #include "windowbarupone.h"
#include "windowbaruptwo.h"

#include "media/text.h"
#include "media/textrecording.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#endif

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    // [15/12/03] Temporarily Disabled. --jn
    //~ wbOne_(new WindowBarUpOne),
    wbTwo_(new WindowBarUpTwo)
{
    ui->setupUi(this);

    // [15/12/03] Temporarily Disabled. --jn
    //~ this->setWindowFlags(Qt::FramelessWindowHint);

    QIcon icon(":images/ring.png");

    this->setWindowIcon(icon);

    GlobalSystemTray& sysIcon = GlobalSystemTray::instance();
    sysIcon.setIcon(icon);

    QMenu *menu = new QMenu();

    auto configAction = new QAction("Configuration", this);
    menu->addAction(configAction);

    // [15/12/03] Temporarily Disabled. --jn
    //~ auto exitAction = new QAction("Exit", this);
    //~ connect(exitAction, &QAction::triggered, []() {
        //~ QCoreApplication::exit();
    //~ });

    //~ menu->addAction(exitAction);

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

    resize( 1054, 600 );

    // [15/12/03] Temporarily Disabled. --jn
    //~ addToolBar( static_cast<QToolBar*>( wbOne_ ) );
    //~ addToolBarBreak();
    addToolBar( static_cast<QToolBar*>( wbTwo_ ) );

    // [15/12/03] Temporarily Disabled. --jn
    //~ connect( wbOne_->getQuitButton(), &QAction::triggered, this, &QMainWindow::close );
    //~ connect( wbOne_->getMaximizeButton(), &QAction::triggered, this , &MainWindow::switchNormalMaximize );
    //~ connect( wbOne_->getMinimizeButton(), &QAction::triggered, this , &QMainWindow::showMinimized );

    connect( wbTwo_->getSettingsButton(), &QAction::triggered, navStack_->getCallWidget() , &CallWidget::on_settingsButton_clicked );
    connect( wbTwo_->getHistoryButton(), &QAction::triggered, navStack_->getCallWidget() , &CallWidget::on_historicButton_clicked );
    connect( wbTwo_->getContactListButton(), &QAction::triggered, navStack_->getCallWidget() , &CallWidget::on_contactButton_clicked );

}

MainWindow::~MainWindow()
{
    delete ui;
    delete navStack_;
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
MainWindow::switchNormalMaximize(  )
{
    if( isMaximized() )
    {
        showNormal();
    }
    else
    {
        showMaximized();
    }

}
