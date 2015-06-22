/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
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

#include <windows.h>

REGISTER_MEDIA();

int
main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font;
    font.setFamily("Segoe UI");
    a.setFont(font);

    QFile file(":/stylesheet.css");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }

    MainWindow w;

    auto startMinimized = false;

    for (auto string : QCoreApplication::arguments()) {
        if (string == "-m" || string == "--minimized")
            startMinimized = true;
    }

    if (not startMinimized)
        w.show();
    else
        w.showMinimized();

    w.createThumbBar();

    QObject::connect(&a, &QApplication::aboutToQuit, [&a]() {
        delete CallModel::instance();
    });

    return a.exec();
}
