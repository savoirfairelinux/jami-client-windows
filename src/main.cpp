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

#include "mainapplication.h"
#include "runguard.h"

int
main(int argc, char* argv[])
{
    setlocale(LC_ALL, "en_US.utf8");

    MainApplication::applicationInitialization();

    // runguard to make sure that only one instance runs at a time
    QCryptographicHash appData(QCryptographicHash::Sha256);
    appData.addData(QApplication::applicationName().toUtf8());
    appData.addData(QApplication::organizationDomain().toUtf8());
    RunGuard guard(appData.result());
    if (!guard.tryToRun()) {
        // no need to exitApp since app is not set up
        guard.release();
        return 0;
    }

    char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
    auto newArgv = MainApplication::parseInputArgument(argc, argv, ARG_DISABLE_WEB_SECURITY);

    MainApplication a(argc, newArgv);
    if (!a.applicationSetup()) {
        guard.release();
        a.exitApp();
        return 0;
    }

    // for deployment and register types
    qmlRegisterType<QmlClipboardAdapter>("MyQClipboard", 1, 0, "QClipboard");
    qmlRegisterSingletonType<AccountModelAdapter>(
        "net.jami.AccountModelAdapter", 1, 0, "AccountModelAdapter",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
            Q_UNUSED(engine);
            Q_UNUSED(scriptEngine);
            AccountModelAdapter *example = new AccountModelAdapter();
            return example;
        });
    qmlRegisterType<ConversationModelAdapter>(
        "net.jami.ConversationModelAdapter", 1, 0, "ConversationModelAdapter");

    QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:/src/KeyBoardShortcutTable.qml")));
    //engine.load(QUrl(QStringLiteral("qrc:/src/UserProfileCard.qml")));

    // exec the application
    auto ret = a.exec();

    guard.release();
    return ret;
}
