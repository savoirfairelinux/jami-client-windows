/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
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

#include "accountmigrationdialog.h"
#include "globalinstances.h"
#include "networkmanager.h"
#include "lrcinstance.h"
#include "pixbufmanipulator.h"
#include "runguard.h"
#include "utils.h"
#include "splashscreen.h"
#include "aboutdialog.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QFontDatabase>
#include <QLibraryInfo>
#include <QTranslator>

#include <thread>
#include <ciso646>
#include <locale.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#if defined _MSC_VER && !COMPILE_ONLY
#include <gnutls/gnutls.h>
#endif

void
consoleDebug()
{
#ifdef Q_OS_WIN
    AllocConsole();
    SetConsoleCP(CP_UTF8);

    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    COORD coordInfo;
    coordInfo.X = 130;
    coordInfo.Y = 9000;

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coordInfo);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);
#endif
}

#ifdef _MSC_VER
void
vsConsoleDebug()
{
    // Print debug to output window if using VS
    QObject::connect(
        &LRCInstance::behaviorController(),
        &lrc::api::BehaviorController::debugMessageReceived,
        [](const std::string& message) {
            OutputDebugStringA((message + "\n").c_str());
        });
}
#endif

void
fileDebug(QFile& debugFile)
{
    QObject::connect(
        &LRCInstance::behaviorController(),
        &lrc::api::BehaviorController::debugMessageReceived,
        [&debugFile](const std::string& message) {
            if (debugFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
                auto msg = (message + "\n").c_str();
                debugFile.write(msg, qstrlen(msg));
                debugFile.close();
            }
        });
}

void
exitApp(RunGuard& guard)
{
    GlobalSystemTray::instance().hide();
    guard.release();
}

int
main(int argc, char* argv[])
{
    setlocale(LC_ALL, "en_US.utf8");

    char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
    int newArgc = argc + 1 + 1;
    char** newArgv = new char*[newArgc];
    for (int i = 0; i < argc; i++) {
        newArgv[i] = argv[i];
    }
    newArgv[argc] = ARG_DISABLE_WEB_SECURITY;
    newArgv[argc + 1] = nullptr;

#if defined(Q_OS_WIN) && (PROCESS_DPI_AWARE)
    SetProcessDPIAware();
#endif // Q_OS_WIN

#ifdef Q_OS_LINUX
    if (!getenv("QT_QPA_PLATFORMTHEME"))
        setenv("QT_QPA_PLATFORMTHEME", "gtk3", true);
#endif

    QApplication a(newArgc, newArgv);

    QCoreApplication::setApplicationName("Ring");
    QCoreApplication::setOrganizationDomain("jami.net");

    QCryptographicHash appData(QCryptographicHash::Sha256);
    appData.addData(QApplication::applicationName().toUtf8());
    appData.addData(QApplication::organizationDomain().toUtf8());
    RunGuard guard(appData.result());
    if (!guard.tryToRun()) {
        return 0;
    }

    for (auto string : QCoreApplication::arguments()) {
        if (string == "-d" || string == "--debug") {
            consoleDebug();
        }
    }

    Utils::removeOldVersions();

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, false);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    auto startMinimized = false;
    QString uri = "";

    auto appDir = qApp->applicationDirPath() + "/";
    const auto locale_name = QLocale::system().name();
    const auto locale_lang = locale_name.split('_')[0];

    QTranslator qtTranslator_lang;
    QTranslator qtTranslator_name;
    if (locale_name != locale_lang) {
        if (qtTranslator_lang.load("qt_" + locale_lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            a.installTranslator(&qtTranslator_lang);
    }
    qtTranslator_name.load("qt_" + locale_name, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator_name);

    QTranslator lrcTranslator_lang;
    QTranslator lrcTranslator_name;
    if (locale_name != locale_lang) {
        if (lrcTranslator_lang.load(appDir + "share/libringclient/translations/lrc_" + locale_lang))
            a.installTranslator(&lrcTranslator_lang);
    }
    if (lrcTranslator_name.load(appDir + "share/libringclient/translations/lrc_" + locale_name))
        a.installTranslator(&lrcTranslator_name);

    QTranslator mainTranslator_lang;
    QTranslator mainTranslator_name;
    if (locale_name != locale_lang) {
        if (mainTranslator_lang.load(appDir + "share/ring/translations/ring_client_windows_" + locale_lang))
            a.installTranslator(&mainTranslator_lang);
    }
    if (mainTranslator_name.load(appDir + "share/ring/translations/ring_client_windows_" + locale_name))
        a.installTranslator(&mainTranslator_name);

    QFont font;
    font.setFamily("Segoe UI");
    a.setFont(font);

#ifndef DEBUG_STYLESHEET
#ifdef Q_OS_LINUX
    QFile file(":/stylesheet.linux.css");
#else
    QFile file(":/stylesheet.css");
#endif
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        a.setStyleSheet(file.readAll());
        file.close();
    }
#endif

#if defined _MSC_VER && !COMPILE_ONLY
    gnutls_global_init();
#endif

    GlobalInstances::setPixmapManipulator(std::make_unique<PixbufManipulator>());

    SplashScreen* splash = new SplashScreen();
    std::atomic_bool isMigrating(false);
    LRCInstance::init(
        [&splash, &a, &isMigrating] {
            splash->setupUI(
                QPixmap(":/images/logo-jami-standard-coul.png"),
                QString("Jami - ") + QObject::tr("Migration needed"),
                QObject::tr("Migration in progress... This may take a while."),
                QColor(232, 232, 232)
            );
            splash->show();
            isMigrating = true;
            while (isMigrating) {
                a.processEvents();
            }
        },
        [&splash, &isMigrating] {
            while (!isMigrating) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            isMigrating = false;
        });
    splash->hide();
    LRCInstance::subscribeToDebugReceived();
    LRCInstance::getAPI().holdConferences = false;

    QDir logPath(QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation));
    // since logPath will be .../Ring, we use cdUp to remove it.
    logPath.cdUp();
    QFile debugFile(logPath.absolutePath() + "/jami/jami.log");

    for (auto string : QCoreApplication::arguments()) {
        if (string.startsWith("jami:")) {
            uri = string;
        } else {
            if (string == "-m" || string == "--minimized") {
                startMinimized = true;
            }
            auto dbgFile = string == "-f" || string == "--file";
            auto dbgConsole = string == "-c" || string == "--vsconsole";
            if (dbgFile || dbgConsole) {
                if (dbgFile) {
                    debugFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
                    debugFile.close();
                    fileDebug(debugFile);
                }
#ifdef _MSC_VER
                if (dbgConsole) {
                    vsConsoleDebug();
                }
#endif
            }
        }
    }

    auto accountList = LRCInstance::accountModel().getAccountList();

    for (const std::string& i : accountList) {
        auto accountStatus = LRCInstance::accountModel().getAccountInfo(i).status;
        if (accountStatus == lrc::api::account::Status::ERROR_NEED_MIGRATION) {
            std::unique_ptr<AccountMigrationDialog> dialog = std::make_unique<AccountMigrationDialog>(nullptr, i);
            int status = dialog->exec();

            //migration failed
            if (!status) {
#ifdef Q_OS_WIN
                FreeConsole();
#endif
                exitApp(guard);
                return status;
            }
        }
    }

    splash->finish(&MainWindow::instance());
    splash->deleteLater();

    QFontDatabase::addApplicationFont(":/images/FontAwesome.otf");

    QSettings settings("jami.net", "Jami");
    if (not startMinimized) {
        MainWindow::instance().showWindow();
    } else {
        MainWindow::instance().showMinimized();
        MainWindow::instance().hide();
    }

    QObject::connect(&a, &QApplication::aboutToQuit, [&guard] { exitApp(guard); });

    // for deployment only
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/src/KeyBoardShortcutTable.qml")));

    auto ret = a.exec();

#ifdef Q_OS_WIN
    FreeConsole();
#endif

    return ret;
}
