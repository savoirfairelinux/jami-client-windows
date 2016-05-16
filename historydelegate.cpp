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

    if (index.column() == 0) {
        auto name = index.model()->data(index, Qt::DisplayRole).toString();
        auto number = index.model()->data(index, static_cast<int>(Call::Role::Number)).toString();
        Call::Direction direction = index.model()->data(index, static_cast<int>(Call::Role::Direction)).value<Call::Direction>();

        opt.text.clear();
        opt.decorationSize = QSize(imgSize_,imgSize_);
        opt.decorationPosition = QStyleOptionViewItem::Left;
        opt.decorationAlignment = Qt::AlignCenter;
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        auto rect = opt.rect;
        QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
            cg = QPalette::Inactive;
        painter->setPen(opt.palette.color(cg, QPalette::Text));
        painter->setOpacity(1.0);
        if (not number.isEmpty()) {
            painter->drawText(QRect(rect.left() +imgSize_ + 5, rect.top(), rect.width(), rect.height()/2),
                              Qt::AlignBottom, name);
            painter->setOpacity(0.7);
            painter->drawText(QRect(rect.left()+imgSize_ + 5, rect.top() + rect.height()/2, rect.width(), rect.height()/2),
                              Qt::AlignTop, number);
            painter->setOpacity(1.0);
            QImage arrow;
            switch (direction) {
            case Call::Direction::INCOMING:
                arrow.load("://images/arrow-down.png");
                break;
            case Call::Direction::OUTGOING:
                arrow.load("://images/arrow-up.png");
                break;
            }
            painter->drawImage(QRect(rect.left() -imgSize_, rect.top() + (rect.height()-imgSize_)/2, imgSize_, imgSize_), arrow);
        } else {
            painter->drawText(QRect(rect.left(), rect.top(), rect.width(), rect.height()),
                              opt.displayAlignment, name);
        }
    }
}

QSize
HistoryDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    return result;
}
