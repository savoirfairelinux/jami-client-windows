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

#include "historydelegate.h"
#include "ringthemeutils.h"

HistoryDelegate::HistoryDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void
HistoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    painter->setRenderHint(QPainter::Antialiasing);

    if (index.column() == 0) {
        // Not having focus removes dotted lines around the item
        if (opt.state & QStyle::State_HasFocus)
            opt.state ^= QStyle::State_HasFocus;

        opt.text.clear();
        // First, we draw the control itself
        QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        QString name, number;
        if (index.parent().isValid()) {
            name = index.data(static_cast<int>(Ring::Role::Name)).value<QString>();
            number = index.data(static_cast<int>(Ring::Role::Number)).value<QString>();
        } else {
            name = index.data(Qt::DisplayRole).toString();
            number = index.data(static_cast<int>(Ring::Role::Number)).value<QString>();
        }

        QRect &rect = opt.rect;

        opt.decorationSize = QSize(sizeImage_, sizeImage_);
        opt.decorationPosition = QStyleOptionViewItem::Left;
        opt.decorationAlignment = Qt::AlignCenter;

        // QRect rectAvatar(16 + rect.left(), rect.top() + dy_, sizeImage_, sizeImage_);
        // drawDecoration(painter, opt, rectAvatar,
        //                QPixmap::fromImage(index.data(Qt::DecorationRole).value<QImage>()));

        QPen pen(painter->pen());
        QFont font(painter->font());
        font.setPointSize(fontSize_);
        font.setBold(true);
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        painter->setFont(font);

        if (not number.isEmpty()) {
            painter->drawText(QRect(rect.left() + sizeImage_ + 5, rect.top(), rect.width(), rect.height()/2),
                              Qt::AlignBottom, name);
            pen.setColor(RingTheme::grey_);
            painter->setPen(pen);
            font.setItalic(true);
            font.setBold(false);
            painter->setFont(font);
            painter->drawText(QRect(rect.left()+ sizeImage_ + 5, rect.top() + rect.height()/2, rect.width(), rect.height()/2),
                              Qt::AlignTop, number);

            /* TODO: add images for call directions
            Call::Direction direction = index.data(static_cast<int>(Call::Role::Direction)).value<Call::Direction>();

            QImage arrow;
            switch (direction) {
            case Call::Direction::INCOMING:
                arrow.load(":/images/arrow-down.png");
                break;
            case Call::Direction::OUTGOING:
                arrow.load(":/images/arrow-up.png");
                break;
            }
            painter->drawImage(QRect(rect.left() - sizeImage_, rect.top() + (rect.height() - sizeImage_)/2, sizeImage_, sizeImage_), arrow)
            */
        } else {
            painter->drawText(QRect(rect.left(), rect.top(), rect.width(), rect.height()),
                              opt.displayAlignment, name);
        }
    }
}

QSize
HistoryDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    // The next condition is false if index refers to a top level element
    // In this case, it is a date label and not an entry of the list
    if (index.parent().isValid())
        size.setHeight(cellHeight_);
    return size;
}
