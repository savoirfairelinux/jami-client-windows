/*
| Copyright (C) 2020 by Savoir-faire Linux
| Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>
|
| This program is free software; you can redistribute it and/or modify
| it under the terms of the GNU General Public License as published by
| the Free Software Foundation; either version 3 of the License, or
| (at your option) any later version.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "lrcinterface.h"

/*
 * AccountModelAdapter
 */

AccountModelAdapter::AccountModelAdapter(QObject* parent)
    : QObject(parent)
    , model_(LRCInstance::accountModel())
{
    qDebug() << "AccountModelAdapter::AccountModelAdapter";
}

AccountModelAdapter::~AccountModelAdapter()
{
    qDebug() << "AccountModelAdapter::~AccountModelAdapter";
}

QVariant
AccountModelAdapter::getAccountListSize()
{
    return model_.getAccountList().size();
}

/*
 * ConversationModelAdapter
 */

ConversationModelAdapter::ConversationModelAdapter(QObject * parent)
    : QObject(parent)
    , model_(*(LRCInstance::getCurrentAccountInfo().conversationModel))
{
}

ConversationModelAdapter::~ConversationModelAdapter()
{
}

QVariant
ConversationModelAdapter::getConversationsSize()
{
    return model_.allFilteredConversations().size();
}
