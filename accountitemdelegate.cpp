/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include "accountitemdelegate.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>

#include "utils.h"
#include "accountlistmodel.h"
#include "ringthemeutils.h"

#undef REGISTERED

AccountItemDelegate::AccountItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

void
AccountItemDelegate::paint(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);
    painter->setRenderHint(QPainter::Antialiasing);

    // Not having focus removes dotted lines around the item
    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    // First, we draw the control itself
    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    bool selected = false;
    if (option.state & QStyle::State_Selected) {
        selected = true;
        opt.state ^= QStyle::State_Selected;
        painter->fillRect(option.rect, RingTheme::smartlistSelection_);
    }
    else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, RingTheme::smartlistHighlight_);
    }

    QRect &rect = opt.rect;

    // Avatar drawing
    opt.decorationSize = QSize(avatarSize_, avatarSize_);
    opt.decorationPosition = QStyleOptionViewItem::Left;
    opt.decorationAlignment = Qt::AlignCenter;

    QRect rectAvatar(dy_ + rect.left(), rect.top() + dy_, avatarSize_, avatarSize_);
    drawDecoration(painter, opt, rectAvatar,
        QPixmap::fromImage(index.data(AccountListModel::Role::Picture).value<QImage>())
        .scaled(avatarSize_, avatarSize_, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QFont font(painter->font());

    // Presence indicator
    QPainterPath outerCircle, innerCircle;
    QPointF center(rectAvatar.right() - avatarSize_ / 6, (rectAvatar.bottom() - avatarSize_ / 6) + 1);
    qreal outerCRadius = avatarSize_ / 6, innerCRadius = outerCRadius * 0.75;
    outerCircle.addEllipse(center, outerCRadius, outerCRadius);
    innerCircle.addEllipse(center, innerCRadius, innerCRadius);
    auto status = index.data(static_cast<int>(AccountListModel::Role::Status)).value<int>();
    auto isPresent = Utils::toEnum<lrc::api::account::Status>(status) == lrc::api::account::Status::REGISTERED;
    if (isPresent) {
        painter->fillPath(outerCircle, Qt::white);
        painter->fillPath(innerCircle, RingTheme::green_);
    }

    font.setPointSize(fontSize_);
    QPen pen(painter->pen());

    painter->setPen(pen);

    QRect rectTexts(dx_ + rect.left() + dx_ + avatarSize_,
                    rect.top(),
                    rect.width(),
                    rect.height() / 2);

    // The name is displayed at the avatar's right
    QVariant name = index.data(static_cast<int>(AccountListModel::Role::Alias));
    if (name.isValid())
    {
        font.setItalic(false);
        font.setBold(true);
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        painter->setFont(font);
        QFontMetrics fontMetrics(font);
        QString nameStr = fontMetrics.elidedText(name.value<QString>(), Qt::ElideRight,
            rectTexts.width() - avatarSize_ - dx_);
        painter->drawText(rectTexts, Qt::AlignVCenter | Qt::AlignLeft, nameStr);
    }

    // Display the ID under the name
    QString idStr = index.data(static_cast<int>(AccountListModel::Role::Username)).value<QString>();
    if (idStr != name.toString()) {
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        painter->setFont(font);
        QFontMetrics fontMetrics(font);
        if (!idStr.isNull()) {
            idStr = fontMetrics.elidedText(idStr, Qt::ElideRight,
 rectTexts.width() - avatarSize_ - dx_ * 2);
            painter->drawText(QRect(dx_ + rect.left() + dx_ + avatarSize_,
                              rect.top() + dy_ * 3,
                              rect.width(),
                              rect.height() / 2),
                              Qt::AlignBottom | Qt::AlignLeft, idStr);
        }
    }
}

QSize AccountItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    size.setHeight(cellHeight_);
    return size;
}
