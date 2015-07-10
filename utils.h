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

#ifndef UTILS_H
#define UTILS_H

//Needed for OS detection
#include <QtGlobal>

#ifdef Q_OS_WIN32
#include <windows.h>
#else //LINUX
#define LPCWSTR char*
#endif

#include <string>
#include <QString>

class Utils
{
public:
    static bool CreateStartupLink();
    static void DeleteStartupLink();
    static bool CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink);
    static bool CheckStartupLink();
    static QString GetRingtonePath();
};

#endif // UTILS_H
