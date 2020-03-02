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
 * LrcGeneralAdapter
 */

LrcGeneralAdapter::LrcGeneralAdapter(QObject *parent)
    : QObject(parent)
{};

LrcGeneralAdapter::~LrcGeneralAdapter() {}

const QString
LrcGeneralAdapter::getCurrAccId()
{
    return LRCInstance::getCurrAccId();
}

const QStringList
LrcGeneralAdapter::getCurrAccList()
{
    return LRCInstance::accountModel().getAccountList();
}

void
LrcGeneralAdapter::setCurrentCall(const QString &accountId, const QString &convUid)
{
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid, accountId);
    auto &accInfo = LRCInstance::getAccountInfo(accountId);
    accInfo.callModel->setCurrentCall(convInfo.callId);
}
Q_INVOKABLE void
LrcGeneralAdapter::startPreviewing(bool force)
{
    LRCInstance::renderer()->startPreviewing(force);
}

Q_INVOKABLE void
LrcGeneralAdapter::stopPreviewing()
{
    if (!LRCInstance::hasVideoCall()) {
        LRCInstance::renderer()->stopPreviewing();
    }
}

int
LrcGeneralAdapter::getAccountListSize()
{
    return LRCInstance::accountModel().getAccountList().size();
}


Q_INVOKABLE NewAccountModel*
LrcGeneralAdapter::accountModel()
{
    return &(LRCInstance::accountModel());
}
