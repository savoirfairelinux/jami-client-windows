/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "contactsearchbarqmlobjectholder.h"

#include "lrcinstance.h"

ContactSearchBarQmlObjectHolder::ContactSearchBarQmlObjectHolder(QObject *parent)
    : QObject(parent)
{}

ContactSearchBarQmlObjectHolder::~ContactSearchBarQmlObjectHolder() {}

void
ContactSearchBarQmlObjectHolder::setContactSearchBarQmlObject(QObject *obj)
{
    contactSearchBarQmlObject_ = obj;
}

void
ContactSearchBarQmlObjectHolder::setConversationFilter(const QString &filter)
{
    LRCInstance::getCurrentConversationModel()->setFilter(filter);
}