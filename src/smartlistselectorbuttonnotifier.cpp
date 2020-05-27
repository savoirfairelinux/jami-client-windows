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

#include "smartlistselectorbuttonnotifier.h"

#include "lrcinstance.h"
#include "utils.h"
#include "ringthemeutils.h"

#include "api/conversationmodel.h"

#include <QPainter>
#include <QPainterPath>

SmartlistSelectorButtonNotifier::SmartlistSelectorButtonNotifier(QWidget* parent)
    : QPushButton(parent)
{
}

void SmartlistSelectorButtonNotifier::setTypeFilter(lrc::api::profile::Type filter)
{
    typeFilter_ = filter;
}

void
SmartlistSelectorButtonNotifier::paintEvent(QPaintEvent* event)
{
    // nb: don't paint the 'button' here!
    // QPushButton::paintEvent(event);
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font(painter.font());

    int totalUnreadMessages = 0;

    using namespace lrc::api::profile;
    switch (typeFilter_) {
    case Type::RING:
    {
        auto convModel = LRCInstance::getCurrentConversationModel();
        auto ringConversations = convModel->getFilteredConversations(Type::RING);
        std::for_each(ringConversations.begin(), ringConversations.end(),
            [&totalUnreadMessages, convModel](const auto& conversation) {
                totalUnreadMessages += conversation.unreadMessages;
            });
        break;
    }
    case Type::PENDING:
    {
        auto& accountInfo = LRCInstance::getCurrentAccountInfo();
        totalUnreadMessages = accountInfo.contactModel->pendingRequestCount();
        break;
    }
    default:
        break;
    }

    if (totalUnreadMessages) {
        QString messageCountText = (totalUnreadMessages > 9) ? "9+" : QString::number(totalUnreadMessages);
        qreal fontSize = messageCountText.count() > 1 ? 7 : 8;
        font.setPointSize(fontSize);

        QRect buttonRect(this->rect());

        // ellipse
        QPainterPath ellipse;
        qreal radius = buttonRect.width() / 2;
        ellipse.addRoundedRect(buttonRect, radius, radius);
        painter.fillPath(ellipse, RingTheme::notificationRed_);

        // text
        painter.setPen(Qt::white);
        painter.setOpacity(1);
        painter.setFont(font);
        buttonRect.setTop(buttonRect.top() - 2);
        painter.drawText(buttonRect, Qt::AlignCenter, messageCountText);
    }
}