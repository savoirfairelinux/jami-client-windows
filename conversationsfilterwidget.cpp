/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
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

#include "conversationsfilterwidget.h"

#include "ringthemeutils.h"

#include <QPainter>

static inline const QRect
getNotifierRect(const QRect& buttonRect)
{
    qreal radius = 8;
    QPointF ellipseCenter(buttonRect.right() - radius * 2, buttonRect.top());
    return QRect(ellipseCenter.x() - radius, ellipseCenter.y() - radius, radius * 2, radius * 2);
}

void
ConversationsFilterWidget::handleNotifierOverlay(const QString& buttonName,
                                                 SmartlistSelectorButtonNotifier*& notifier,
                                                 lrc::api::profile::Type filter)
{
    auto button = this->findChild<QPushButton*>(buttonName);
    if (!button) {
        return;
    }
    if (!notifier) {
        notifier = new SmartlistSelectorButtonNotifier(this);
        button->stackUnder(notifier);
        notifier->setTypeFilter(filter);
        notifier->hide();
        QObject::connect(notifier, SIGNAL(clicked()), button, SLOT(click()));
    } else {
        notifier->setGeometry(getNotifierRect(button->frameGeometry()));
        notifier->show();
    }
}

ConversationsFilterWidget::ConversationsFilterWidget(QWidget *parent)
    : QWidget(parent)
{
}

void ConversationsFilterWidget::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);

    using namespace lrc::api::profile;
    handleNotifierOverlay("buttonConversations", unreadMessagesNotifier_, Type::RING);
    handleNotifierOverlay("buttonInvites", pendingInvitesNotifier_, Type::PENDING);
}
