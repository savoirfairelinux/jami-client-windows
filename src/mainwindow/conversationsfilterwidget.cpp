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

#include "conversationsfilterwidget.h"

#include <QPainter>
#include <QDebug>

#include "../misc/ringthemeutils.h"
#include "../bridging/lrcinstance.h"

ConversationsFilterWidget::ConversationsFilterWidget(QWidget *parent)
    : QWidget(parent)
{
}

void ConversationsFilterWidget::updateBadges()
{
    using namespace lrc::api::profile;
    updateNotifier(Type::RING);
    updateNotifier(Type::PENDING);
}

void
ConversationsFilterWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    updateBadges();
}

void
ConversationsFilterWidget::updateNotifier(lrc::api::profile::Type typeFilter)
{
    using namespace lrc::api::profile;
    handleNotifierOverlay((typeFilter == Type::RING) ? "btnConversations" : "btnInvites",
                          (typeFilter == Type::RING) ? unreadMessagesNotifier_ : pendingInvitesNotifier_,
                          typeFilter);
}

static inline const QRect
getNotifierRect(const QRect& buttonRect)
{
    qreal radius = 9;
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
