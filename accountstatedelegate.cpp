/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                                *
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

#include "accountstatedelegate.h"

#include "accountmodel.h"
#include "account.h"

AccountStateDelegate::AccountStateDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{}

void
AccountStateDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->setRenderHint(QPainter::Antialiasing);
    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);
    if (index.column() == 0) {
        // name & checkbox
        auto name = index.model()->data(index, Qt::DisplayRole).toString();
        opt.text = QString();
        QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        auto rect = opt.rect;
        auto font = painter->font();
        font.setPointSize(12);
        painter->setFont(font);
        painter->setOpacity(1.0);
        opt.displayAlignment = Qt::AlignTop;

        painter->setPen(Qt::black);
        painter->drawText(QRect(rect.left() + 25, rect.top(),
                                rect.width(), rect.height()),
                                opt.displayAlignment, name);

        // status
        auto account = AccountModel::instance().getAccountByModelIndex(index);
        QString stateColor(account->stateColorName());
        QString accountStatus = account->toHumanStateName();

        painter->setPen(stateColor);

        opt.displayAlignment = Qt::AlignBottom|Qt::AlignLeft;

        painter->drawText(QRect(rect.left() + 25, rect.top(),
                                rect.width(), rect.height()),
                                opt.displayAlignment, accountStatus);
    }
}

QSize
AccountStateDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    return result;
}
