/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "conversationitemdelegate.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>

// Client
#include "smartlistmodel.h"
#include "ringthemeutils.h"
#include "utils.h"
#include "lrcinstance.h"

#include <ciso646>

ConversationItemDelegate::ConversationItemDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

void
ConversationItemDelegate::paint(QPainter* painter
                        , const QStyleOptionViewItem& option
                        , const QModelIndex& index
                        ) const
{
    QStyleOptionViewItem opt(option);
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Not having focus removes dotted lines around the item
    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    auto isContextMenuOpen = index.data(static_cast<int>(SmartListModel::Role::ContextMenuOpen)).value<bool>();
    bool selected = false;
    if (option.state & QStyle::State_Selected) {
        selected = true;
        opt.state ^= QStyle::State_Selected;
    } else if (!isContextMenuOpen) {
        highlightMap_[index.row()] = option.state & QStyle::State_MouseOver;
    }

    // One does not simply keep the highlighted state drawn when the context
    // menu is open�
    auto rowHighlight = highlightMap_.find(index.row());
    if (selected) {
       painter->fillRect(option.rect, RingTheme::smartlistSelection_);
    } else if (rowHighlight != highlightMap_.end() && (*rowHighlight).second) {
       painter->fillRect(option.rect, RingTheme::smartlistHighlight_);
    }
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>().toStdString();
    auto conversation = Utils::getConversationFromUid(convUid, *LRCInstance::getCurrentConversationModel());
    if (LRCInstance::getCurrentCallModel()->hasCall(conversation->callId)) {
        auto color = QColor(RingTheme::blue_.lighter(180)); color.setAlpha(128);
        painter->fillRect(option.rect, color);
    }

    QRect &rect = opt.rect;

    // Avatar drawing
    opt.decorationSize = QSize(sizeImage_, sizeImage_);
    opt.decorationPosition = QStyleOptionViewItem::Left;
    opt.decorationAlignment = Qt::AlignCenter;

    QRect rectAvatar(dx_ + rect.left(), rect.top() + dy_, sizeImage_, sizeImage_);
    drawDecoration(painter, opt, rectAvatar,
                   QPixmap::fromImage(index.data(Qt::DecorationRole).value<QImage>())
                   .scaled(sizeImage_, sizeImage_, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QFont font(painter->font());

    // If there's unread messages, a message count is displayed
    if (auto messageCount = index.data(static_cast<int>(SmartListModel::Role::UnreadMessagesCount)).toInt()) {
        QString messageCountText = (messageCount > 9) ? "9+" : QString::number(messageCount);
        qreal fontSize = messageCountText.count() > 1 ? 7 : 8;
        font.setPointSize(fontSize);

        // ellipse
        QPainterPath ellipse;
        qreal ellipseHeight = sizeImage_ / 6;
        qreal ellipseWidth = ellipseHeight;
        QPointF ellipseCenter(rectAvatar.right() - ellipseWidth, rectAvatar.top() + ellipseHeight + 1);
        QRect ellipseRect(ellipseCenter.x() - ellipseWidth, ellipseCenter.y() - ellipseHeight,
            ellipseWidth * 2, ellipseHeight * 2);
        ellipse.addRoundedRect(ellipseRect, ellipseWidth, ellipseHeight);
        painter->fillPath(ellipse, RingTheme::notificationRed_);

        // text
        painter->setPen(Qt::white);
        painter->setOpacity(1);
        painter->setFont(font);
        ellipseRect.setTop(ellipseRect.top() - 2);
        painter->drawText(ellipseRect, Qt::AlignCenter, messageCountText);
    }

    // Presence indicator
    if (index.data(static_cast<int>(SmartListModel::Role::Presence)).value<bool>()) {
        qreal radius = sizeImage_ / 6;
        QPainterPath outerCircle, innerCircle;
        QPointF center(rectAvatar.right() - radius, (rectAvatar.bottom() - radius) + 1);
        qreal outerCRadius = radius;
        qreal innerCRadius = outerCRadius * 0.75;
        outerCircle.addEllipse(center, outerCRadius, outerCRadius);
        innerCircle.addEllipse(center, innerCRadius, innerCRadius);
        painter->fillPath(outerCircle, Qt::white);
        painter->fillPath(innerCircle, RingTheme::presenceGreen_);
    }

    using namespace lrc::api;
    auto type = Utils::toEnum<profile::Type>(
        index.data(static_cast<int>(SmartListModel::Role::ContactType)).value<int>()
        );
    switch (type) {
    case profile::Type::RING:
    case profile::Type::TEMPORARY:
        paintRingConversationItem(painter, option, rect, index);
        break;
    case profile::Type::PENDING:
        paintRingInviteConversationItem(painter, option, rect, index);
        break;
    case profile::Type::SIP:
        break;
    default:
        paintRingConversationItem(painter, option, rect, index);
        break;
    }
}

QSize
ConversationItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(0, cellHeight_);
}

void
ConversationItemDelegate::paintRingConversationItem(QPainter* painter,
                                                    const QStyleOptionViewItem& option,
                                                    const QRect& rect,
                                                    const QModelIndex& index) const
{
    Q_UNUSED(option);
    QFont font(painter->font());
    font.setPointSize(fontSize_);
    QPen pen(painter->pen());
    painter->setPen(pen);

    auto leftMargin = dx_ + sizeImage_ + dx_ / 2;
    auto rightMargin = dx_;
    auto topMargin = 0;
    auto bottomMargin = 12;

    QRect rectName1(rect.left() + leftMargin,
                    rect.top() + topMargin,
                    rect.width() - leftMargin - infoTextWidth_,
                    rect.height() / 2);

    QRect rectName2(rectName1.left(),
                    rectName1.top() + rectName1.height(),
                    rectName1.width(),
                    rectName1.height() - bottomMargin);

    QRect rectInfo1(rectName1.left() + rectName1.width(),
                    rect.top() + topMargin,
                    infoTextWidth_ - rightMargin,
                    rect.height() / 2);

    QRect rectInfo2(rectInfo1.left(),
                    rectInfo1.top() + rectInfo1.height(),
                    rectInfo1.width(),
                    rectInfo1.height() - bottomMargin);

    QFontMetrics fontMetrics(font);

    // The name is displayed at the avatar's right
    QString nameStr = index.data(static_cast<int>(SmartListModel::Role::DisplayName)).value<QString>();
    if (!nameStr.isNull()) {
        font.setItalic(false);
        font.setBold(true);
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        painter->setFont(font);
        QString elidedNameStr = fontMetrics.elidedText(nameStr, Qt::ElideRight, rectName1.width());
        painter->drawText(rectName1, Qt::AlignVCenter | Qt::AlignLeft, elidedNameStr);
    }

    // Display the ID under the name
    QString idStr = index.data(static_cast<int>(SmartListModel::Role::DisplayID)).value<QString>();
    if (idStr != nameStr && !idStr.isNull()) {
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        painter->setFont(font);
        idStr = fontMetrics.elidedText(idStr, Qt::ElideRight, rectName2.width());
        painter->drawText(rectName2, Qt::AlignVCenter | Qt::AlignLeft, idStr);
    }

    // top-right: last interaction date/time
    QString lastUsedStr = index.data(static_cast<int>(SmartListModel::Role::LastInteractionDate)).value<QString>();
    if (!lastUsedStr.isNull()) {
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        painter->setFont(font);
        lastUsedStr = fontMetrics.elidedText(lastUsedStr, Qt::ElideRight, rectInfo1.width());
        painter->drawText(rectInfo1, Qt::AlignVCenter | Qt::AlignRight, lastUsedStr);
    }

    // bottom-right: last interaction snippet
    QString interactionStr = index.data(static_cast<int>(SmartListModel::Role::LastInteraction)).value<QString>();
    if (!interactionStr.isNull()) {
        painter->save();
        interactionStr = interactionStr.simplified();
        auto type = Utils::toEnum<lrc::api::interaction::Type>(index
            .data(static_cast<int>(SmartListModel::Role::LastInteractionType))
            .value<int>());
        if (type == lrc::api::interaction::Type::CALL ||
            type == lrc::api::interaction::Type::CONTACT) {
            font.setItalic(false);
            font.setBold(false);
            pen.setColor(RingTheme::grey_.darker(140));
            painter->setPen(pen);
            painter->setFont(font);
            // strip emojis if it's a call/contact type message
            VectorUInt emojiless;
            for (auto unicode : interactionStr.toUcs4()) {
                if (!(unicode >= 0x1F000 && unicode <= 0x1FFFF)) {
                    emojiless.push_back(unicode);
                }
            }
            interactionStr = QString::fromUcs4(&emojiless.at(0), emojiless.size());
        } else {
            QFont emojiMsgFont(QStringLiteral("Segoe UI Emoji"));
            emojiMsgFont.setItalic(false);
            emojiMsgFont.setBold(false);
            emojiMsgFont.setPointSize(fontSize_);
            painter->setOpacity(0.7);
            painter->setFont(emojiMsgFont);
        }
        interactionStr = fontMetrics.elidedText(interactionStr, Qt::ElideRight, rectInfo2.width());
        painter->drawText(rectInfo2, Qt::AlignVCenter | Qt::AlignRight, interactionStr);
        painter->restore();
    }
}

void
ConversationItemDelegate::paintRingInviteConversationItem(QPainter* painter,
                                                          const QStyleOptionViewItem& option,
                                                          const QRect& rect,
                                                          const QModelIndex& index) const
{
    QFont font(painter->font());
    font.setPointSize(fontSize_);
    QPen pen(painter->pen());
    painter->setPen(pen);

    auto leftMargin = dx_ + sizeImage_ + dx_ / 2;
    auto rightMargin = dx_;
    if (option.state & QStyle::State_MouseOver) {
        rightMargin = infoTextWidth_ - dx_ * 2;
    }
    auto topMargin = 0;
    auto bottomMargin = 12;

    QRect rectName1(rect.left() + leftMargin,
        rect.top() + topMargin,
        rect.width() - leftMargin - rightMargin,
        rect.height() / 2);

    QRect rectName2(rectName1.left(),
        rectName1.top() + rectName1.height(),
        rectName1.width(),
        rectName1.height() - bottomMargin);

    QFontMetrics fontMetrics(font);

    // The name is displayed at the avatar's right
    QString nameStr = index.data(static_cast<int>(SmartListModel::Role::DisplayName)).value<QString>();
    if (!nameStr.isNull()) {
        font.setItalic(false);
        font.setBold(true);
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        painter->setFont(font);
        QString elidedNameStr = fontMetrics.elidedText(nameStr, Qt::ElideRight, rectName1.width());
        painter->drawText(rectName1, Qt::AlignVCenter | Qt::AlignLeft, elidedNameStr);
    }

    // Display the ID under the name
    QString idStr = index.data(static_cast<int>(SmartListModel::Role::DisplayID)).value<QString>();
    if (idStr != nameStr && !idStr.isNull()) {
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        painter->setFont(font);
        idStr = fontMetrics.elidedText(idStr, Qt::ElideRight, rectName2.width());
        painter->drawText(rectName2, Qt::AlignVCenter | Qt::AlignLeft, idStr);
    }
}

void
ConversationItemDelegate::paintSIPConversationItem(QPainter* painter,
                                                   const QStyleOptionViewItem& option,
                                                   const QModelIndex& index) const
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(index);
}
