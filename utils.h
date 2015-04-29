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

#include <windows.h>
#include <lmcons.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <string>

class Utils
{
public:
    static bool CreateStartupLink();
    static void DeleteStartupLink();
    static bool CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink);
    static bool CheckStartupLink();
};

#endif // UTILS_H
