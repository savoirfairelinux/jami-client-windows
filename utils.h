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
#include <ciso646>
#undef ERROR
#else //LINUX
#define LPCWSTR char*
#endif

#include <string>
#include <QString>
#include <QImage>
#include <QStackedWidget>

#include "api/conversationmodel.h"
#include "api/account.h"
#include "api/contactmodel.h"
#include "api/contact.h"

namespace Utils
{
    constexpr int animDuration_ = 200; // animation duration for sliding page in ms

    bool CreateStartupLink();
    void DeleteStartupLink();
    bool CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink);
    bool CheckStartupLink();
    QString GetRingtonePath();
    QString GenGUID();
    QString GetISODate();
    QString GetCurrentUserName();
    void InvokeMailto(const QString& subject, const QString& body, const QString& attachement = QString());
    QImage getCirclePhoto(const QImage original, int sizePhoto);
    void slidePage(QStackedWidget *stack, QWidget *widget, bool toRight = false);

    std::string bestIdForConversation(const lrc::api::conversation::Info& conv, const lrc::api::ConversationModel& model);
    std::string bestIdForAccount(const lrc::api::account::Info & account);
    std::string bestNameForAccount(const lrc::api::account::Info & account);
    std::string bestIdForContact(const lrc::api::contact::Info & contact);
    std::string bestNameForContact(const lrc::api::contact::Info & contact);
    std::string bestNameForConversation(const lrc::api::conversation::Info & conv, const lrc::api::ConversationModel & model);
    lrc::api::profile::Type profileType(const lrc::api::conversation::Info & conv, const lrc::api::ConversationModel & model);
}

