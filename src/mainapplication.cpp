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

#include "mainwindow.h"
#include "accountmigrationdialog.h"
#include "globalinstances.h"
#include "lrcinstance.h"
#include "pixbufmanipulator.h"
#include "utils.h"

#include <locale.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#if defined _MSC_VER && !COMPILE_ONLY
#include <gnutls/gnutls.h>
#endif

MainApplication::MainApplication(int &argc, char** argv) :
    QApplication(argc, argv)
{
    QObject::connect(this, &QApplication::aboutToQuit, [this] { exitApp(); });
}

void
MainApplication::applicationInitialization()
{
    // some attributes are needed to be set before the creation of the sapplication
    QApplication::setApplicationName("Ring");
    QApplication::setOrganizationDomain("jami.net");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

void
MainApplication::consoleDebug()
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

void
MainApplication::vsConsoleDebug()
{
#ifdef _MSC_VER
    // Print debug to output window if using VS
    QObject::connect(
        &LRCInstance::behaviorController(),
        &lrc::api::BehaviorController::debugMessageReceived,
        [](const std::string& message) {
            OutputDebugStringA((message + "\n").c_str());
        });
#endif
}

void
MainApplication::fileDebug(QFile* debugFile)
{
    QObject::connect(
        &LRCInstance::behaviorController(),
        &lrc::api::BehaviorController::debugMessageReceived,
        [debugFile](const std::string& message) {
            if (debugFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
                auto msg = (message + "\n").c_str();
                debugFile->write(msg, qstrlen(msg));
                debugFile->close();
            }
        });
}

void
MainApplication::exitApp()
{
    GlobalSystemTray::instance().hide();
#ifdef Q_OS_WIN
    FreeConsole();
#endif
}

char**
MainApplication::parseInputArgument(int& argc, char* argv[], char* argToParse)
{
    // forcefully append argToParse
    int oldArgc = argc;
    argc = argc + 1 + 1;
    char** newArgv = new char* [argc];
    for (int i = 0; i < oldArgc; i++) {
        newArgv[i] = argv[i];
    }
    newArgv[oldArgc] = argToParse;
    newArgv[oldArgc + 1] = nullptr;
    return newArgv;
}

QString
MainApplication::getDebugFilePath()
{
    QDir logPath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    // since logPath will be .../Ring, we use cdUp to remove it.
    logPath.cdUp();
    return QString(logPath.absolutePath() + "/jami/jami.log");
}

void
MainApplication::loadTranslations()
{
    auto appDir = qApp->applicationDirPath() + "/";
    const auto locale_name = QLocale::system().name();
    const auto locale_lang = locale_name.split('_')[0];

    QTranslator qtTranslator_lang;
    QTranslator qtTranslator_name;
    if (locale_name != locale_lang) {
        if (qtTranslator_lang.load("qt_" + locale_lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            installTranslator(&qtTranslator_lang);
    }
    qtTranslator_name.load("qt_" + locale_name, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    installTranslator(&qtTranslator_name);

    QTranslator lrcTranslator_lang;
    QTranslator lrcTranslator_name;
    if (locale_name != locale_lang) {
        if (lrcTranslator_lang.load(appDir + "share/libringclient/translations/lrc_" + locale_lang))
            installTranslator(&lrcTranslator_lang);
    }
    if (lrcTranslator_name.load(appDir + "share/libringclient/translations/lrc_" + locale_name))
        installTranslator(&lrcTranslator_name);

    QTranslator mainTranslator_lang;
    QTranslator mainTranslator_name;
    if (locale_name != locale_lang) {
        if (mainTranslator_lang.load(appDir + "share/ring/translations/ring_client_windows_" + locale_lang))
            installTranslator(&mainTranslator_lang);
    }
    if (mainTranslator_name.load(appDir + "share/ring/translations/ring_client_windows_" + locale_name))
        installTranslator(&mainTranslator_name);
}

void
MainApplication::initLrc()
{
    // init mainwindow and finish splash when mainwindow shows up
    splash_ = std::make_unique<SplashScreen>();
    std::atomic_bool isMigrating(false);
    LRCInstance::init(
        [this, &isMigrating] {
            splash_->setupUI(
                QPixmap(":/images/logo-jami-standard-coul.png"),
                QString("Jami - ") + QObject::tr("Migration needed"),
                QObject::tr("Migration in progress... This may take a while."),
                QColor(232, 232, 232)
            );
            splash_->show();
            isMigrating = true;
            while (isMigrating) {
                this->processEvents();
            }
        },
        [this, &isMigrating] {
            while (!isMigrating) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            isMigrating = false;
        });
    splash_->hide();
    LRCInstance::subscribeToDebugReceived();
    LRCInstance::getAPI().holdConferences = false;
}

void
MainApplication::processInputArgument(bool& startMinimized)
{
    debugFile_ = std::make_unique<QFile>(getDebugFilePath());
    QString uri = "";

    for (auto string : QCoreApplication::arguments()) {
        if (string.startsWith("jami:")) {
            uri = string;
        }
        else {
            if (string == "-m" || string == "--minimized") {
                startMinimized = true;
            }
            auto dbgFile = string == "-f" || string == "--file";
            auto dbgConsole = string == "-c" || string == "--vsconsole";
            if (dbgFile || dbgConsole) {
                if (dbgFile) {
                    debugFile_->open(QIODevice::WriteOnly | QIODevice::Truncate);
                    debugFile_->close();
                    fileDebug(debugFile_.get());
                }
#ifdef _MSC_VER
                if (dbgConsole) {
                    vsConsoleDebug();
                }
#endif
            }
        }
    }
}

void
MainApplication::setApplicationStyleSheet()
{
#ifndef DEBUG_STYLESHEET
#ifdef Q_OS_LINUX
    QFile file(":/stylesheet.linux.css");
#else
    QFile file(":/stylesheet.css");
#endif
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(file.readAll());
        file.close();
    }
#endif
}

bool
MainApplication::startAccountMigration()
{
    auto accountList = LRCInstance::accountModel().getAccountList();

    for (const std::string& i : accountList) {
        auto accountStatus = LRCInstance::accountModel().getAccountInfo(i).status;
        if (accountStatus == lrc::api::account::Status::ERROR_NEED_MIGRATION) {
            std::unique_ptr<AccountMigrationDialog> dialog = std::make_unique<AccountMigrationDialog>(nullptr, i);
            int status = dialog->exec();

            //migration failed
            return status == 0 ? false : true;
        }
    }
    return true;
}

void
MainApplication::setApplicationFont()
{
    QFont font;
    font.setFamily("Segoe UI");
    setFont(font);
    QFontDatabase::addApplicationFont(":/images/FontAwesome.otf");
}

bool
MainApplication::applicationSetup()
{
#ifdef Q_OS_LINUX
    if (!getenv("QT_QPA_PLATFORMTHEME"))
        setenv("QT_QPA_PLATFORMTHEME", "gtk3", true);
#endif

    // start debug console
    for (auto string : QCoreApplication::arguments()) {
        if (string == "-d" || string == "--debug") {
            consoleDebug();
        }
    }

    // remove old version files
    Utils::removeOldVersions();

    // load translations
    loadTranslations();

    // set font
    setApplicationFont();

    // set style sheet
    setApplicationStyleSheet();

#if defined _MSC_VER && !COMPILE_ONLY
    gnutls_global_init();
#endif

    // init pixmap manipulator
    GlobalInstances::setPixmapManipulator(std::make_unique<PixbufManipulator>());

    // init lrc and its possible migration ui
    initLrc();
    // init mainwindow and finish splash when mainwindow shows up
    // release its source after
    splash_->finish(&MainWindow::instance());

    // process input argument
    bool startMinimized{ false };
    processInputArgument(startMinimized);

    // start possible account migration
    if (!startAccountMigration())
        return false;

    // create jami.net settings in Registry if it is not presented
    QSettings settings("jami.net", "Jami");

    // set mainwindow show size
    if (not startMinimized) {
        MainWindow::instance().showWindow();
    }
    else {
        MainWindow::instance().showMinimized();
        MainWindow::instance().hide();
    }

    return true;
}