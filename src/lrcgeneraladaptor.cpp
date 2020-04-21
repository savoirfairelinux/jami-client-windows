/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Yang Wang   <yang.wang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lrcgeneraladaptor.h"

LrcGeneralAdaptor::LrcGeneralAdaptor(QObject *parent)
    : QObject(parent)
{}

const QString
LrcGeneralAdaptor::getCurrAccId()
{
    return LRCInstance::getCurrAccId();
}

const QStringList
LrcGeneralAdaptor::getCurrAccList()
{
    return LRCInstance::accountModel().getAccountList();
}

int
LrcGeneralAdaptor::getAccountListSize()
{
    return getCurrAccList().size();
}

void
LrcGeneralAdaptor::setCurrentCall(const QString &accountId, const QString &convUid)
{
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid, accountId);
    auto &accInfo = LRCInstance::getAccountInfo(accountId);
    accInfo.callModel->setCurrentCall(convInfo.callId);
}

void
LrcGeneralAdaptor::startPreviewing(bool force)
{
    LRCInstance::renderer()->startPreviewing(force);
}

void
LrcGeneralAdaptor::stopPreviewing()
{
    if (!LRCInstance::hasVideoCall()) {
        LRCInstance::renderer()->stopPreviewing();
    }
}

bool
LrcGeneralAdaptor::hasVideoCall()
{
    return LRCInstance::hasVideoCall();
}

const QString
LrcGeneralAdaptor::getCallId(const QString &accountId, const QString &convUid)
{
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid, accountId);
    if (convInfo.uid.isEmpty()) {
        return "";
    }

    auto call = LRCInstance::getCallInfoForConversation(convInfo, false);
    if (!call) {
        return "";
    }

    return call->id;
}
