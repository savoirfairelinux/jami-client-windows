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

#include "globalinstances.h"
#include "downloadmanager.h"
#include "lrcinstance.h"
#include "pixbufmanipulator.h"
#include "runguard.h"
#include "utils.h"
#include "deleteaccountdialog.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QFontDatabase>
#include <QLibraryInfo>
#include <QTranslator>
#include <QVBoxLayout>

#include <ciso646>

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

int
main(int argc, char* argv[])
{
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
    QFile file(":/stylesheet.css");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        a.setStyleSheet(file.readAll());
        file.close();
    }
#endif

#if defined _MSC_VER && !COMPILE_ONLY
    gnutls_global_init();
#endif

    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    GlobalInstances::setPixmapManipulator(std::make_unique<PixbufManipulator>());
    QMessageBox msgBox;
    LRCInstance::init(
        [&msgBox] {
            QGridLayout* layout = (QGridLayout*)msgBox.layout();
            QMovie* movie = new QMovie(":/images/jami_eclipse_spinner.gif");
            QLabel* spinnerLabel = new QLabel();
            QLabel* textLabel = new QLabel(QObject::tr("Migrating..."));
            movie->start();
            spinnerLabel->setMovie(movie);
            msgBox.setStyleSheet("background:white;");
            textLabel->setFont(QFont("Sans Serif", 10, QFont::ExtraLight, false));
            layout->addWidget(textLabel, 0, 0, 1, layout->columnCount());
            layout->addWidget(spinnerLabel, 1, 0, 1, layout->columnCount());
            msgBox.setStandardButtons(0);
            //msgBox.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
            msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
            msgBox.setWindowTitle("Jami");
            msgBox.exec();
        },
        [&msgBox] { msgBox.done(0); });

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    qDebug() << "LRC loaded " << duration.count() << endl;

    QFile debugFile(qApp->applicationDirPath() + "/" + "jami.log");

    for (auto string : QCoreApplication::arguments()) {
        if (string.startsWith("jami:")) {
            uri = string;
        } else {
            if (string == "-m" || string == "--minimized") {
                startMinimized = true;
            }
            if (string == "-f" || string == "--file") {
                debugFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
                debugFile.close();
                fileDebug(debugFile);
            }
#ifdef _MSC_VER
            if (string == "-c" || string == "--vsconsole") {
                vsConsoleDebug();
            }
#endif
        }
    }

    QFontDatabase::addApplicationFont(":/images/FontAwesome.otf");

    MainWindow::instance().createThumbBar();

    if (not startMinimized)
        MainWindow::instance().showWindow();
    else {
        MainWindow::instance().showMinimized();
        MainWindow::instance().hide();
    }

    QObject::connect(&a, &QApplication::aboutToQuit, [&guard] { guard.release(); });

    auto ret = a.exec();

#ifdef Q_OS_WIN
    FreeConsole();
#endif

    QCoreApplication::exit();
    GlobalSystemTray::instance().deleteLater();
    GlobalSystemTray::instance().hide();

    return ret;
}
