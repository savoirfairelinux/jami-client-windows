/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                                *
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
#pragma once

#include "api/profile.h"
#include "smartlistselectorbuttonnotifier.h"

#include <QPushButton>
#include <QPainterPath>

class SmartlistSelectorButtonNotifier : public QPushButton
{
    Q_OBJECT

public:
    SmartlistSelectorButtonNotifier(QWidget *parent = 0);
    void setTypeFilter(lrc::api::profile::Type filter);

protected:
    virtual void paintEvent(QPaintEvent *event);

    lrc::api::profile::Type typeFilter_{ lrc::api::profile::Type::INVALID };
};