/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
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

#include "itemdataroles.h"
#include "person.h"
#include "recentmodel.h"
#include "call.h"

SmartListDelegate::SmartListDelegate(QObject* parent) :
    QStyledItemDelegate(parent),
    hoveredRow_(QModelIndex()),
    selectedRow_(QModelIndex())
{
}

void
SmartListDelegate::paint(QPainter* painter
                        , const QStyleOptionViewItem& option
                        , const QModelIndex& index
                        ) const
{
    QStyleOptionViewItem opt(option);

    QFont font(painter->font());
    font.setPointSize(10);

    const int currentRow = index.row();

    QRect rect = opt.rect;
    rect.setLeft(0);

    QRect rectTexts(16 + rect.left() + dx+sizeImage_, rect.top(), rect.width(), rect.height()/2);
    QRect rectAvatar(16 + rect.left(), rect.top() + dy, sizeImage_, sizeImage_);

    if (index == hoveredRow_)
    {
        painter->fillRect(rect, QColor(242, 242, 242));

        emit rowHovering(index,rect);
    }
    else if ( RecentModel::instance().getActiveCall(RecentModel::instance().peopleProxy()->mapToSource(index)) )
    {
        painter->fillRect(rect, QColor(235, 235, 235));
    }
    else if (index == selectedRow_)
    {
        painter->fillRect(rect, QColor(229, 229, 229));
    }

    QPen pen(painter->pen());
    pen.setColor(QColor(242, 242, 242));
    painter->setPen(pen);

    painter->drawLine(rect.left() + 20, rect.bottom(),
                      rect.right() - 20,
                      rect.bottom());

    if (index.column() == 0)
    {
        painter->setPen(pen);

        QVariant name = index.data(static_cast<int>(Ring::Role::Name));
        if (name.isValid())
        {
            pen.setColor(QColor(63, 63, 63));
            painter->setPen(pen);
            font.setBold(true);
            painter->setFont(font);
            painter->drawText(rectTexts, Qt::AlignBottom | Qt::AlignLeft, name.toString());
        }

        QVariant state = index.data(static_cast<int>(Ring::Role::FormattedState));

        pen.setColor(QColor(192, 192, 192));
        painter->setPen(pen);

        font.setBold(false);
        painter->setFont(font);

        rectTexts.moveTop(cellHeight_/2);

        if (state.isValid() && RecentModel::instance().getActiveCall(RecentModel::instance().peopleProxy()->mapToSource(index)))
        {
            painter->drawText(QRect(16 + rect.left() + dx + sizeImage_,
                               rect.top() + rect.height()/2,
                               rect.width(), rect.height()/2),
                               Qt::AlignTop | Qt::AlignLeft, state.toString());
        }
        else
        {
            QVariant lastUsed = index.data(static_cast<int>(Ring::Role::FormattedLastUsed));
            if (lastUsed.isValid())
            {
                painter->drawText(QRect(16 + rect.left() + dx + sizeImage_,
                                   rect.top() + rect.height()/2,
                                   rect.width(), rect.height()/2),
                                   Qt::AlignTop | Qt::AlignLeft, lastUsed.toString());
            }
        }

        painter->setPen(pen);
        QVariant var_p = index.data(static_cast<int>(Ring::Role::Object));
        auto person = var_p.value<Person*>();
        if (person && not person->photo().value<QImage>().isNull())
        {
            painter->drawImage(rectAvatar, person->photo().value<QImage>());
        }
        else
        {
            QImage defaultImage(":images/user/btn-default-userpic.svg");
            painter->drawImage(rectAvatar, defaultImage);
        }

        if (index == hoveredRow_)
        {
            QLinearGradient gradient(opt.rect.topLeft(),opt.rect.topRight());

            gradient.setColorAt(0.55, QColor(242, 242, 242, 0));
            gradient.setColorAt(0.65, QColor(242, 242, 242, 255));

            painter->fillRect(opt.rect, gradient);

        }
        else if (index == selectedRow_)
        {
                QLinearGradient gradient(opt.rect.topLeft(),opt.rect.topRight());

                gradient.setColorAt(0.8, QColor(229,229,229,0));
                gradient.setColorAt(1.0, QColor(229,229,229,255));
                
                painter->fillRect(opt.rect, gradient);
        }
        else if ( RecentModel::instance().getActiveCall(RecentModel::instance().peopleProxy()->mapToSource(index)) )
        {
            QLinearGradient gradient(opt.rect.topLeft(),opt.rect.topRight());

            gradient.setColorAt(0.8, QColor(235, 235, 235, 0));
            gradient.setColorAt(1.0, QColor(235, 235, 235, 235));
            
            painter->fillRect(opt.rect, gradient);
        }
        else
        {
            QLinearGradient gradient(opt.rect.topLeft(),opt.rect.topRight());

            gradient.setColorAt(0.8, QColor(255, 255, 255, 0));
            gradient.setColorAt(1.0, QColor(255, 255, 255, 255));

            painter->fillRect(opt.rect, gradient);
        }
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize
SmartListDelegate::sizeHint(const QStyleOptionViewItem& option
                           , const QModelIndex& index
                           ) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(cellHeight_);
    size.setWidth(cellWidth_);

    return size;
}
