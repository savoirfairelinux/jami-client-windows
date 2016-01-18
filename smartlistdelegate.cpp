/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                                *
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

#include "itemdataroles.h"
#include "person.h"
#include "recentmodel.h"
#include "call.h"

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
    opt.decorationSize = QSize(sizeImage_, sizeImage_);
    opt.decorationPosition = QStyleOptionViewItem::Left;
    opt.decorationAlignment = Qt::AlignCenter;
    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QRect rect = opt.rect;
    rect.setLeft(0);

    QRect rectTexts(16 + rect.left() + dx_+sizeImage_, rect.top(), rect.width(), rect.height()/2);
    QRect rectAvatar(16 + rect.left(), rect.top() + dy_, sizeImage_, sizeImage_);
    drawDecoration(painter, opt, rectAvatar, QPixmap::fromImage(index.data(Qt::DecorationRole).value<QImage>()));

    const int currentRow = index.row();

    if (currentRow == rowHighlighted_)
        emit rowSelected(opt.rect);

    QFont font(painter->font());
    font.setPointSize(fontSize_);

    QPen pen(painter->pen());

    if (not (opt.state & QStyle::State_Selected)) {
        pen.setColor(lightGrey_);
        painter->setPen(pen);
        painter->drawLine(rect.left() + 20, rect.bottom(),
                          rect.right() - 20,
                          rect.bottom());
    }
    if (index.column() == 0)
    {
        painter->setPen(pen);

        QVariant name = index.data(static_cast<int>(Ring::Role::Name));
        if (name.isValid())
        {
            pen.setColor(lightBlack_);
            painter->setPen(pen);
            font.setBold(true);
            painter->setFont(font);
            painter->drawText(rectTexts, Qt::AlignBottom | Qt::AlignLeft, name.toString());
        }

        QVariant state = index.data(static_cast<int>(Ring::Role::FormattedState));
        pen.setColor(grey_);
        painter->setPen(pen);
        font.setBold(false);
        painter->setFont(font);
        rectTexts.moveTop(cellHeight_/2);
        if (state.isValid() && RecentModel::instance().getActiveCall(RecentModel::instance().peopleProxy()->mapToSource(index)))
        {
            painter->drawText(QRect(16 + rect.left() + dx_ + sizeImage_,
                               rect.top() + rect.height()/2,
                               rect.width(), rect.height()/2),
                               Qt::AlignTop | Qt::AlignLeft, state.toString());
        }
        else
        {
            QVariant lastUsed = index.data(static_cast<int>(Ring::Role::FormattedLastUsed));
            if (lastUsed.isValid())
            {
                painter->drawText(QRect(16 + rect.left() + dx_ + sizeImage_,
                                   rect.top() + rect.height()/2,
                                   rect.width(), rect.height()/2),
                                   Qt::AlignTop | Qt::AlignLeft, lastUsed.toString());
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
    size.setWidth(cellWidth_);
    return size;
}
