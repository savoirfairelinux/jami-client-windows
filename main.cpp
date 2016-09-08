/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                                *
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

#include "mainwindow.h"
#include <QApplication>
#include <QFile>

#include "callmodel.h"
#include "media/audio.h"
#include "media/video.h"
#include "media/text.h"
#include "media/file.h"


#include <QThread>
#include <QTranslator>
#include <QLibraryInfo>


#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef URI_PROTOCOL
#include "shmclient.h"
#endif

REGISTER_MEDIA();

void
Console()
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

int
main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    auto startMinimized = false;
    QString uri = "";

    for (auto string : QCoreApplication::arguments()) {
        if (string == "-m" || string == "--minimized")
            startMinimized = true;
        if (string == "-d" || string == "--debug")
            Console();
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

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator lrcTranslator;
    lrcTranslator.load("share/libringclient/translations/lrc_" + QLocale::system().name());
    a.installTranslator(&lrcTranslator);

    QTranslator mainTranslator;
    mainTranslator.load("share/ring/translations/ring_client_windows_" + QLocale::system().name());
    a.installTranslator(&mainTranslator);

    QFont font;
    font.setFamily("Segoe UI");
    a.setFont(font);

    QFile file(":/stylesheet.css");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }

    QCoreApplication::setOrganizationName("Savoir-faire Linux");
    QCoreApplication::setOrganizationDomain("ring.cx");
    QCoreApplication::setApplicationName("Ring");

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

    MainWindow::instance().createThumbBar();

#ifdef URI_PROTOCOL
    QObject::connect(shmClient, SIGNAL(RingEvent(QString)), &MainWindow::instance(), SLOT(onRingEvent(QString)));

    QObject::connect(&a, &QApplication::aboutToQuit, [&a, &shmClient, &shm, &sem]() {
        shmClient->terminate();
        delete shmClient;
        delete shm;
        delete sem;
    });
#endif

    return a.exec();
}
