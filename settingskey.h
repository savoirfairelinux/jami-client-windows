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

namespace SettingsKey {

constexpr static char closeOrMinimized[] = "closeOrMin";
constexpr static char autoAnswer[] = "autoAnswer";
constexpr static char savedSize[] = "savedSize";
constexpr static char savedPos[] = "savedPos";
constexpr static char imShowAuthor[] = "imShowAuthor";
constexpr static char imShowDate[] = "imShowDate";
constexpr static char enableNotifications[] = "enableNotifications";
}

#define accountAutoAnswer(A) (A+SettingsKey::autoAnswer)


