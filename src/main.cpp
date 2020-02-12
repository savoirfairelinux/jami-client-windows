/***************************************************************************
 * Copyright (C) 2015-2020 by Savoir-faire Linux                           *
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

#include "lrcinstance.h"
#include "mainapplication.h"
#include "runguard.h"
#include "qmlclipboardadapter.h"
#include "d3d11qtquickvideowidgetbae.h"

#include <QCryptographicHash>
#include <QQmlEngine>
#include <QQmlApplicationEngine>

#include <clocale>

int
main(int argc, char* argv[])
{
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Direct3D11Rhi);

    setlocale(LC_ALL, "en_US.utf8");

    MainApplication::applicationInitialization();

    char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
    auto newArgv = MainApplication::parseInputArgument(argc, argv, ARG_DISABLE_WEB_SECURITY);

    MainApplication a(argc, newArgv);

    // runguard to make sure that only one instance runs at a time
    // Note: needs to be after the creation of the application
    QCryptographicHash appData(QCryptographicHash::Sha256);
    appData.addData(QApplication::applicationName().toUtf8());
    appData.addData(QApplication::organizationDomain().toUtf8());
    RunGuard guard(appData.result());
    if (!guard.tryToRun()) {
        // no need to exitApp since app is not set up
        return 0;
    }

    if (!a.applicationSetup()) {
        guard.release();
        a.exitApp();
        return 0;
    }

    // for deployment and register types
    qmlRegisterType<QmlClipboardAdapter>("MyQClipboard", 1, 0, "QClipboard");
    qmlRegisterType<D3D11QtQuickVideoWidgetBase>("D3D11QML",1,0,"D3D11QtQuickVideoWidgetBase");
    qmlRegisterType<D3D11QtQuickVideoPreviewWidget>("D3D11QML", 1, 0, "D3D11QtQuickVideoPreviewWidget");
    qmlRegisterType<D3D11QtQuickVideoDistantWidget>("D3D11QML", 1, 0, "D3D11QtQuickVideoDistantWidget");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/src/KeyBoardShortcutTable.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/src/UserProfileCard.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/src/VideoPreviewBase.qml")));

    QQuickWindow* viewwindow = qobject_cast<QQuickWindow*>(engine.rootObjects().value(2));
    LRCInstance::renderer()->startPreviewing();

    //QWidget* container = QWidget::createWindowContainer(viewwindow);
    //container->show();

    viewwindow->show();

    // exec the application
    auto ret = a.exec();

    guard.release();
    return ret;
}
