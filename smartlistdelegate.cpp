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

SmartListDelegate::SmartListDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void
SmartListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (index.column() == 0) {
        opt.text.clear();
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        QRect rect = opt.rect;
        QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ?
                    QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
            cg = QPalette::Inactive;
        painter->setPen(opt.palette.color(cg, QPalette::Text));
        painter->setOpacity(1.0);
        QVariant name = index.data(static_cast<int>(Ring::Role::Name));
        if (name.isValid()) {
            painter->drawText(QRect(rect.left()+sizeImage_+5, rect.top(),
                                    rect.width(), rect.height()/2),
                                    opt.displayAlignment, name.toString());
        }

        QVariant state = index.data(static_cast<int>(Ring::Role::FormattedState));
        if (state.isValid() && RecentModel::instance()->getActiveCall(RecentModel::instance()->peopleProxy()->mapToSource(index))) {
            painter->drawText(QRect(rect.left()+sizeImage_+5,
                                    rect.top() + rect.height()/2,
                                    rect.width(), rect.height()/2),
                              opt.displayAlignment, state.toString());
        } else {
            QVariant lastUsed = index.data(static_cast<int>(Ring::Role::FormattedLastUsed));
            if (lastUsed.isValid()) {
                painter->drawText(QRect(rect.left()+sizeImage_+5,
                                        rect.top() + rect.height()/2,
                                        rect.width(), rect.height()/2),
                                  opt.displayAlignment, lastUsed.toString());
            }
        }
        QVariant var_p = index.data(static_cast<int>(Ring::Role::Object));
        auto person = var_p.value<Person*>();
        if (person && not person->photo().value<QImage>().isNull()) {
            painter->drawImage(QRect(rect.left()+1, rect.top()+1,
                                     sizeImage_, sizeImage_),
                               person->photo().value<QImage>());
        } else {
            QImage defaultImage(":images/account.png");
            painter->drawImage(QRect(rect.left(), rect.top(),
                                     sizeImage_, sizeImage_),
                               defaultImage);
        }
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize
SmartListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    result.setHeight((result.height()*2));
    return result;
}

