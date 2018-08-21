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

#include "smartlistdelegate.h"

#include <QApplication>
#include <QSortFilterProxyModel>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

// Client
#include "smartlistmodel.h"
#include "ringthemeutils.h"

#include <ciso646>

SmartListDelegate::SmartListDelegate(QObject* parent) :
    QItemDelegate(parent)
{
}

void
SmartListDelegate::paint(QPainter* painter
                        , const QStyleOptionViewItem& option
                        , const QModelIndex& index
                        ) const
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
    opt.decorationSize = QSize(sizeImage_, sizeImage_);
    opt.decorationPosition = QStyleOptionViewItem::Left;
    opt.decorationAlignment = Qt::AlignCenter;

    QRect rectAvatar(16 + rect.left(), rect.top() + dy_, sizeImage_, sizeImage_);
    drawDecoration(painter, opt, rectAvatar,
                   QPixmap::fromImage(index.data(Qt::DecorationRole).value<QImage>())
                   .scaled(sizeImage_, sizeImage_, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QFont font(painter->font());

    // If there's unread messages, a message count is displayed
    if (auto messageCount = index.data(static_cast<int>(SmartListModel::Role::UnreadMessagesCount)).toInt()) {
        QString messageCountText = (messageCount > 9) ? "9+" : QString::number(messageCount);
        qreal fontSize = messageCountText.count() > 1 ? 7 : 8;
        font.setPointSize(fontSize);
        QFontMetrics textFontMetrics(font); 

        // ellipse
        QPainterPath ellipse;
        qreal ellipseHeight = sizeImage_ / 6;
        qreal ellipseWidth = ellipseHeight;
        QPointF ellipseCenter(rectAvatar.right() - ellipseWidth, rectAvatar.top() + ellipseHeight + 1);
        QRect ellipseRect(ellipseCenter.x() - ellipseWidth, ellipseCenter.y() - ellipseHeight,
            ellipseWidth * 2, ellipseHeight * 2);
        ellipse.addRoundedRect(ellipseRect, ellipseWidth, ellipseHeight);
        painter->fillPath(ellipse, RingTheme::red_);

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
        painter->fillPath(innerCircle, RingTheme::green_);
    }

    if (index.column() != 0) {
        if (selected) {
            opt.state ^= QStyle::State_Selected;
        }
        QItemDelegate::paint(painter, opt, index);
        return;
    }

    font.setPointSize(fontSize_);
    QPen pen(painter->pen());
    painter->setPen(pen);

    QRect rectTexts(16 + rect.left() + dx_ + sizeImage_,
                    rect.top(),
                    rect.width(),
                    rect.height() / 2);

    // The name is displayed at the avatar's right
    QVariant name = index.data(static_cast<int>(SmartListModel::Role::DisplayName));
    if (name.isValid()) {
        font.setItalic(false);
        font.setBold(true);
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        painter->setFont(font);
        QFontMetrics fontMetrics(font);
        QString nameStr = fontMetrics.elidedText(name.value<QString>(), Qt::ElideRight,
                                                 rectTexts.width()- sizeImage_ - dx_);
        painter->drawText(rectTexts, Qt::AlignVCenter | Qt::AlignLeft, nameStr);
    }

    // Display the ID under the name
    QString idStr = index.data(static_cast<int>(SmartListModel::Role::DisplayID)).value<QString>();
    if (idStr != name.toString()){
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        painter->setFont(font);
        QFontMetrics fontMetrics(font);
        if (!idStr.isNull()){
            idStr = fontMetrics.elidedText(idStr, Qt::ElideRight, rectTexts.width()- sizeImage_ - dx_);
            painter->drawText(QRect(16 + rect.left() + dx_ + sizeImage_,
                              rect.top() + rect.height()/7,
                              rect.width(),
                              rect.height()/2),
                              Qt::AlignBottom | Qt::AlignLeft, idStr);

        } else {
            qDebug() << "This is not a Person";
        }
    }

    // top-right: last interaction date/time
    QVariant lastUsed = index.data(static_cast<int>(SmartListModel::Role::LastInteractionDate));
    if (lastUsed.isValid()) {
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        painter->setFont(font);
        painter->drawText(QRect(16 + rect.left() + dx_ + sizeImage_,
                          rect.top() + rect.height() / 2,
                          rect.width(),
                          rect.height() / 2),
                          Qt::AlignLeft | Qt::AlignVCenter, lastUsed.toString());
    }

    // bottom-right: last interaction snippet
    // TODO
    /*QVariant state = index.data(static_cast<int>(SmartListModel::Role::LastInteraction));
    if (state.isValid()) {
        font.setItalic(false);
        font.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        font.setItalic(false);
        font.setBold(false);
        painter->setFont(font);
        rectTexts.moveTop(cellHeight_ / 2);
        painter->drawText(QRect(16 + rect.left() + dx_ + sizeImage_,
                          rect.top() + rect.height() / 2,
                          rect.width(),
                          rect.height() / 2),
                          Qt::AlignLeft | Qt::AlignVCenter, state.toString());
    }*/
}

QSize
SmartListDelegate::sizeHint(const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    size.setHeight(cellHeight_);
    return size;
}
