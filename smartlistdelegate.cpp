/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include "smartlistdelegate.h"

#include <QApplication>
#include <QSortFilterProxyModel>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

// LRC
#include "itemdataroles.h"
#include "person.h"
#include "recentmodel.h"
#include "call.h"

// Client
#include "combar.h"

#include "ringthemeutils.h"

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

    QRect &rect = opt.rect;

    // Avatar drawing
    opt.decorationSize = QSize(sizeImage_, sizeImage_);
    opt.decorationPosition = QStyleOptionViewItem::Left;
    opt.decorationAlignment = Qt::AlignCenter;

    QRect rectAvatar(16 + rect.left(), rect.top() + dy_, sizeImage_, sizeImage_);
    drawDecoration(painter, opt, rectAvatar,
                   QPixmap::fromImage(index.data(Qt::DecorationRole).value<QImage>()));

    QFont font(painter->font());

    // If there's unread messages, a message count is displayed
    if (auto messageCount = index.data(static_cast<int>(Ring::Role::UnreadTextMessageCount)).toInt()) {
        font.setPointSize(8);
        QFontMetrics textFontMetrics(font);

        // If there is more than 10 unread messages, "10+" is displayed
        QString messageCountText = (messageCount >= 10)? "9+" : QString::number(messageCount);

        // This QRect is the bounding one containing the message count to be displayed
        QRect pastilleRect = textFontMetrics.boundingRect(QRect(rectAvatar.left() - (2 * sizeImage_)/9,
                                                                rectAvatar.bottom() - sizeImage_/3 - textFontMetrics.height(), sizeImage_, textFontMetrics.height()),
                                                          Qt::AlignVCenter | Qt::AlignLeft, messageCountText);

        // This one is larger than pastilleRect and is used to prepare the red background
        QRect bubbleRect(pastilleRect.left() - 3, pastilleRect.top(),
                         pastilleRect.width() + 6, pastilleRect.height() + 1);

        // The background is displayed
        QPainterPath path;
        path.addRoundedRect(bubbleRect, 3, 3);
        QPen pen(RingTheme::red_, 5);
        painter->setOpacity(0.9);
        painter->setPen(pen);
        painter->fillPath(path, RingTheme::red_);

        // Then the message count
        painter->setPen(Qt::white);
        painter->setOpacity(1);
        painter->setFont(font);
        painter->drawText(pastilleRect, Qt::AlignCenter, messageCountText);
    }

    // Presence indicator
    QPainterPath outerCircle, innerCircle;
    QPointF center(rectAvatar.right() - sizeImage_/6, (rectAvatar.bottom() - sizeImage_/6) + 1);
    qreal outerCRadius = sizeImage_ / 6, innerCRadius = outerCRadius * 0.75;
    outerCircle.addEllipse(center, outerCRadius, outerCRadius);
    innerCircle.addEllipse(center, innerCRadius, innerCRadius);
    if (index.data(static_cast<int>(Ring::Role::IsPresent)).value<bool>()) {
        painter->fillPath(outerCircle, Qt::white);
        painter->fillPath(innerCircle, RingTheme::green_);
    }
    font.setPointSize(fontSize_);
    QPen pen(painter->pen());

    // A lightGrey separator is painted at the bottom if the current item is not selected
    if (not (opt.state & QStyle::State_Selected)) {
        pen.setColor(RingTheme::lightGrey_);
        painter->setPen(pen);
        painter->drawLine(rect.left() + 20, rect.bottom(),
                          rect.right() - 20,
                          rect.bottom());
    }

    if (index.column() == 0)
    {
        painter->setPen(pen);

        QRect rectTexts(16 + rect.left() + dx_ + sizeImage_,
                        rect.top(),
                        rect.width(),
                        rect.height() / 2);

        // The name is displayed at the avatar's right
        QVariant name = index.data(static_cast<int>(Ring::Role::Name));
        if (name.isValid())
        {
            pen.setColor(RingTheme::lightBlack_);
            painter->setPen(pen);
            font.setBold(true);
            painter->setFont(font);
            QFontMetrics fontMetrics(font);
            QString nameStr = fontMetrics.elidedText(name.toString(), Qt::ElideRight
                                                                    , rectTexts.width()- sizeImage_ - effectiveComBarSize_ - dx_);
            painter->drawText(rectTexts, Qt::AlignVCenter | Qt::AlignLeft, nameStr);
        }

        // Display the ID under the name
        QString idStr = index.data(static_cast<int>(Ring::Role::Number)).value<QString>();
        if (idStr != name.toString()){
            pen.setColor(RingTheme::grey_);
            painter->setPen(pen);
            font.setItalic(true);
            font.setBold(false);
            painter->setFont(font);
            QFontMetrics fontMetrics(font);
            if (!idStr.isNull()){
                idStr = fontMetrics.elidedText(idStr, Qt::ElideRight, rectTexts.width()- sizeImage_ - effectiveComBarSize_ - dx_);
                painter->drawText(QRect(16 + rect.left() + dx_ + sizeImage_,
                                        rect.top() + rect.height()/7,
                                        rect.width(),
                                        rect.height()/2),
                                  Qt::AlignBottom | Qt::AlignLeft, idStr);

            } else {
                qDebug() << "This is not a Person";
            }
        }

        // Finally, either last interaction date or call state is displayed
        QVariant state = index.data(static_cast<int>(Ring::Role::FormattedState));
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        font.setItalic(false);
        font.setBold(false);
        painter->setFont(font);
        rectTexts.moveTop(cellHeight_/2);
        if (state.isValid() && RecentModel::instance().getActiveCall(RecentModel::instance().peopleProxy()->mapToSource(index)))
        {
            painter->drawText(QRect(16 + rect.left() + dx_ + sizeImage_,
                                    rect.top() + rect.height()/2,
                                    rect.width(),
                                    rect.height()/2),
                                Qt::AlignLeft | Qt::AlignVCenter, state.toString());
        }
        else
        {
            QVariant lastUsed = index.data(static_cast<int>(Ring::Role::FormattedLastUsed));
            if (lastUsed.isValid())
            {
                painter->drawText(QRect(16 + rect.left() + dx_ + sizeImage_,
                                        rect.top() + rect.height()/2,
                                        rect.width(),
                                        rect.height()/2),
                                  Qt::AlignLeft | Qt::AlignVCenter, lastUsed.toString());
            }
        }
    }
    else
    {
        QItemDelegate::paint(painter, opt, index);
    }
}

QSize
SmartListDelegate::sizeHint(const QStyleOptionViewItem& option
                           , const QModelIndex& index
                           ) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    size.setHeight(cellHeight_);
    return size;
}
