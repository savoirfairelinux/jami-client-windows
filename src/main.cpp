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

#include <QCryptographicHash>

int
main(int argc, char* argv[])
{
    setlocale(LC_ALL, "en_US.utf8");

    MainApplication::applicationInitialization();

    // runguard to make sure that only one instance runs at a time
    QCryptographicHash appData(QCryptographicHash::Sha256);
    appData.addData(QGuiApplication::applicationName().toUtf8());
    appData.addData(QGuiApplication::organizationDomain().toUtf8());
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

    // exec the application
    auto ret = a.exec();

    guard.release();
    return ret;
}
