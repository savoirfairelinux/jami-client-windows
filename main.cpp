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

#include "mainwindow.h"
#include <QApplication>
#include <QFile>

#include "callmodel.h"
#include "media/audio.h"
#include "media/video.h"
#include "media/text.h"
#include "media/file.h"
#include <iostream>

#include <QThread>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

REGISTER_MEDIA();

void
Console()
{
#ifdef Q_OS_WIN32
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
    QApplication a(argc, argv);

    auto startMinimized = false;

    for (auto string : QCoreApplication::arguments()) {
        if (string == "-m" || string == "--minimized")
            startMinimized = true;
        if (string == "-d" || string == "--debug")
            Console();
    }

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

    MainWindow w;

    if (not startMinimized)
        w.show();
    else
        w.showMinimized();

    QObject::connect(&a, &QApplication::aboutToQuit, [&a]() {
        delete CallModel::instance();
    });

    return a.exec();
}
