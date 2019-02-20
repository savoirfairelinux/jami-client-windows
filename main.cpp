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

#include <QApplication>
#include <QFile>

#include "callmodel.h"
#include "media/audio.h"
#include "media/video.h"
#include "media/text.h"
#include "media/file.h"
#include "globalinstances.h"

#include <QTranslator>
#include <QLibraryInfo>
#include <QFontDatabase>

#include <ciso646>

#include "lrcinstance.h"
#include "utils.h"
#include "runguard.h"
#include "pixbufmanipulator.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#if defined _MSC_VER && !COMPILE_ONLY
#include <gnutls/gnutls.h>
#endif

#ifdef URI_PROTOCOL
#include "shmclient.h"
#endif

REGISTER_MEDIA();

void
consoleDebug()
{
#ifdef Q_OS_WIN
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    COORD coordInfo;
    coordInfo.X = 130;
    coordInfo.Y = 9000;

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coordInfo);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),ENABLE_QUICK_EDIT_MODE| ENABLE_EXTENDED_FLAGS);
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

int
main(int argc, char *argv[])
{
    RunGuard guard("680b3e5eaf");
    if (!guard.tryToRun())
        return 0;

    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
    int newArgc = argc + 1 + 1;
    char** newArgv = new char*[newArgc];
    for (int i = 0; i < argc; i++) {
        newArgv[i] = argv[i];
    }
    newArgv[argc] = ARG_DISABLE_WEB_SECURITY;
    newArgv[argc + 1] = nullptr;

#ifdef Q_OS_WIN
    SetProcessDPIAware();
#endif // Q_OS_WIN

    QApplication a(newArgc, newArgv);

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, false);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QCoreApplication::setOrganizationDomain("jami.net");
    QCoreApplication::setApplicationName("Ring");

    auto startMinimized = false;
    QString uri = "";

#if defined _MSC_VER && !COMPILE_ONLY
    gnutls_global_init();
#endif

    GlobalInstances::setPixmapManipulator(std::unique_ptr<PixbufManipulator>(new PixbufManipulator()));
    LRCInstance::init();

    QFile debugFile("debug.log");

    for (auto string : QCoreApplication::arguments()) {
        if (string == "-m" || string == "--minimized") {
            startMinimized = true;
        }
        if (string == "-f" || string == "--file") {
            debugFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            debugFile.close();
            fileDebug(debugFile);
        }
        if (string == "-d" || string == "--debug") {
            consoleDebug();
        }
#ifdef _MSC_VER
        else if (string == "-c" || string == "--vsconsole") {
            vsConsoleDebug();
        }
#endif
        if (string.startsWith("ring:")) {
            uri = string;
        }
    }

#ifdef URI_PROTOCOL
    QSharedMemory* shm = new QSharedMemory("RingShm");
    QSystemSemaphore* sem = new QSystemSemaphore("RingSem", 0);

    if (not shm->create(1024)) {
        if (not uri.isEmpty()) {
            shm->attach();
            shm->lock();
            char *to = (char*) shm->data();
            QChar *data = uri.data();
            while (!data->isNull())
            {
                memset(to, data->toLatin1(), 1);
                ++data;
                ++to;
            }
            memset(to, 0, 1); //null terminator
            shm->unlock();
        }
        sem->release();

        delete shm;
        exit(EXIT_SUCCESS);
    }
    //Client listening to shm event
    memset((char*)shm->data(), 0, shm->size());
    ShmClient* shmClient = new ShmClient(shm, sem);
#endif

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

    QFile file(":/stylesheet.css");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }

    QFontDatabase::addApplicationFont(":/images/FontAwesome.otf");

    MainWindow::instance().createThumbBar();

    if (not uri.isEmpty()) {
        startMinimized = false;
        MainWindow::instance().onRingEvent(uri);
    }

    if (not startMinimized)
        MainWindow::instance().show();
    else {
        MainWindow::instance().showMinimized();
        MainWindow::instance().hide();
    }

#ifdef URI_PROTOCOL
    QObject::connect(shmClient, SIGNAL(RingEvent(QString)), &MainWindow::instance(), SLOT(onRingEvent(QString)));

    QObject::connect(&a, &QApplication::aboutToQuit, [&a, &shmClient, &shm, &sem]() {
        shmClient->terminate();
        delete shmClient;
        delete shm;
        delete sem;
    });
#endif

    auto ret = a.exec();

#ifdef Q_OS_WIN
    FreeConsole();
#endif

    QCoreApplication::exit();
    GlobalSystemTray::instance().deleteLater();
    GlobalSystemTray::instance().hide();

    return ret;
}
