/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                                *
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

#pragma once

//Needed for OS detection
#include <QtGlobal>

#ifdef Q_OS_WIN
#include <windows.h>
#else //LINUX
#define LPCWSTR char*
#endif

#include <string>
#include <QString>
#include <QImage>

class Utils
{
public:
    static bool CreateStartupLink();
    static void DeleteStartupLink();
    static bool CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink);
    static bool CheckStartupLink();
    static QString GetRingtonePath();
    static QString GenGUID();
    static QString GetISODate();
    static QString GetCurrentUserName();
    static void InvokeMailto(const QString& subject, const QString& body, const QString& attachement = QString());
    static QImage getCirclePhoto(const QImage original, int sizePhoto);
};

