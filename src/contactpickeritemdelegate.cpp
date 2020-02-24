/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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

#include "contactpickeritemdelegate.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>

 // Client
#include "smartlistmodel.h"
#include "ringthemeutils.h"
#include "utils.h"
#include "lrcinstance.h"

#include <ciso646>

ContactPickerItemDelegate::ContactPickerItemDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

void
ContactPickerItemDelegate::paint(QPainter* painter
    , const QStyleOptionViewItem& option
    , const QModelIndex& index
) const
{
    QStyleOptionViewItem opt(option);
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Not having focus removes dotted lines around the item
    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    bool selected = false;
    if (option.state & QStyle::State_Selected) {
        selected = true;
        opt.state ^= QStyle::State_Selected;
    } else {
        highlightMap_[index.row()] = option.state & QStyle::State_MouseOver;
    }

    QRect &rect = opt.rect;

    QString sectionName = index.data(static_cast<int>(SmartListModel::Role::SectionName)).value<QString>();
    if (!sectionName.isEmpty()) {
        painter->fillRect(option.rect, RingTheme::lightGrey_);
        QFont font(painter->font());
        QPen pen(painter->pen());
        auto scalingRatio = Utils::getCurrentScalingRatio();
        if (scalingRatio > 1.0) {
            font.setPointSize(fontSize_ - 2);
        } else {
            font.setPointSize(fontSize_);
        }
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        painter->setFont(font);
        QMargins padding(4, 0, 0, 0);
        painter->drawText(rect - padding, Qt::AlignVCenter | Qt::AlignLeft, sectionName);
        return;
    }

    QColor presenceBorderColor = Qt::white;
    auto rowHighlight = highlightMap_.find(index.row());
    if (selected) {
        painter->fillRect(option.rect, RingTheme::smartlistSelection_);
        presenceBorderColor = RingTheme::smartlistSelection_;
    } else if (rowHighlight != highlightMap_.end() && (*rowHighlight).second) {
        painter->fillRect(option.rect, RingTheme::smartlistHighlight_);
        presenceBorderColor = RingTheme::smartlistHighlight_;
    }

    // Avatar drawing
    opt.decorationSize = QSize(sizeImage_, sizeImage_);
    opt.decorationPosition = QStyleOptionViewItem::Left;
    opt.decorationAlignment = Qt::AlignCenter;

    QRect rectAvatar(dx_ + rect.left(), rect.top() + dy_, sizeImage_, sizeImage_);
    drawDecoration(painter, opt, rectAvatar,
        QPixmap::fromImage(index.data(Qt::DecorationRole).value<QImage>())
        .scaled(sizeImage_, sizeImage_, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QFont font(painter->font());

    // Presence indicator
    if (index.data(static_cast<int>(SmartListModel::Role::Presence)).value<bool>()) {
        qreal radius = sizeImage_ / 6;
        QPainterPath outerCircle, innerCircle;
        QPointF center(rectAvatar.right() - radius + 2, (rectAvatar.bottom() - radius) + 1 + 2);
        qreal outerCRadius = radius;
        qreal innerCRadius = outerCRadius * 0.75;
        outerCircle.addEllipse(center, outerCRadius, outerCRadius);
        innerCircle.addEllipse(center, innerCRadius, innerCRadius);
        painter->fillPath(outerCircle, presenceBorderColor);
        painter->fillPath(innerCircle, RingTheme::presenceGreen_);
    }

    using namespace lrc::api;
    auto type = Utils::toEnum<profile::Type>(
        index.data(static_cast<int>(SmartListModel::Role::ContactType)).value<int>()
        );
    switch (type) {
    case profile::Type::RING:
    case profile::Type::TEMPORARY:
        paintRingContactItem(painter, option, rect, index);
        break;
    case profile::Type::SIP:
        paintSIPContactItem(painter, option, rect, index);
        break;
    default:
        paintRingContactItem(painter, option, rect, index);
        break;
    }
}

QSize
ContactPickerItemDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    Q_UNUSED(option);
    if (!index.data(static_cast<int>(SmartListModel::Role::SectionName))
        .value<QString>()
        .isEmpty()) {
        return QSize(0, sectionCellHeight_);
    }
    return QSize(0, cellHeight_);
}

void
ContactPickerItemDelegate::paintRingContactItem(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QRect& rect,
    const QModelIndex& index) const
{
    Q_UNUSED(option);
    QFont font(painter->font());
    QPen pen(painter->pen());

    auto scalingRatio = Utils::getCurrentScalingRatio();
    if (scalingRatio > 1.0) {
        font.setPointSize(fontSize_ - 2);
    } else {
        font.setPointSize(fontSize_);
    }

    auto leftMargin = dx_ + sizeImage_ + dx_;
    auto topMargin = 4;
    auto bottomMargin = 8;

    QRect rectName1(rect.left() + leftMargin,
        rect.top() + topMargin,
        rect.width() - leftMargin * 2,
        rect.height() / 2 - 2);

    QRect rectName2(rectName1.left(),
        rectName1.top() + rectName1.height(),
        rectName1.width(),
        rectName1.height() - bottomMargin);

    QFontMetrics fontMetrics(font);

    // The name is displayed at the avatar's right
    QString nameStr = index.data(static_cast<int>(SmartListModel::Role::DisplayName)).value<QString>();
    if (!nameStr.isNull()) {
        font.setItalic(false);
        font.setBold(false);
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
ContactPickerItemDelegate::paintSIPContactItem(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QRect& rect,
    const QModelIndex& index) const
{
    Q_UNUSED(option);
    QRect rectName1;
    QRect rectName2;
    QFont font(painter->font());
    QPen pen(painter->pen());
    painter->setPen(pen);

    auto scalingRatio = Utils::getCurrentScalingRatio();
    if (scalingRatio > 1.0) {
        font.setPointSize(fontSize_ - 2);
    } else {
        font.setPointSize(fontSize_);
    }

    auto leftMargin = dx_ + sizeImage_ + dx_;
    auto topMargin = 4;
    auto bottomMargin = 8;

    if (!rect.isEmpty()) {

        rectName1 = QRect(rect.left() + leftMargin,
        rect.top() + topMargin,
        rect.width() - leftMargin * 2,
        rect.height() / 2 - 2);

        rectName2 =  QRect(rectName1.left(),
            rectName1.top() + rectName1.height(),
            rectName1.width(),
            rectName1.height() - bottomMargin);
    }

    QFontMetrics fontMetrics(font);

    // The name is displayed at the avatar's right
    QString nameStr = index.data(static_cast<int>(SmartListModel::Role::DisplayName)).value<QString>();
    if (!nameStr.isNull()) {
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        painter->setFont(font);
        if (!rectName1.isEmpty()) {
            QString elidedNameStr = fontMetrics.elidedText(nameStr, Qt::ElideRight, rectName1.width());
            painter->drawText(rectName1, Qt::AlignVCenter | Qt::AlignLeft, elidedNameStr);
        }
    }

    // Display the ID under the name
    QString idStr = index.data(static_cast<int>(SmartListModel::Role::DisplayID)).value<QString>();
    if (idStr != nameStr && !idStr.isNull()) {
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        painter->setFont(font);
        if (!rectName2.isEmpty()) {
            idStr = fontMetrics.elidedText(idStr, Qt::ElideRight, rectName2.width());
            painter->drawText(rectName2, Qt::AlignVCenter | Qt::AlignLeft, idStr);
        }
    }
}
