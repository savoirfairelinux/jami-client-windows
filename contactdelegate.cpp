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

#include "contactdelegate.h"

#include "person.h"
#include "contactmethod.h"

ContactDelegate::ContactDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}


void
ContactDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (index.column() == 0) {
        opt.text.clear();
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        QRect rect = opt.rect;
        QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ?
                    QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
            cg = QPalette::Inactive;
        painter->setPen(opt.palette.color(cg, QPalette::Text));
        painter->setOpacity(1.0);
        QVariant var_c = index.child(0,0).data(
                    static_cast<int>(Person::Role::Object));
        if (var_c.isValid()) {
            Person *c = var_c.value<Person *>();
            painter->drawText(QRect(rect.left()+sizeImage_+5, rect.top(),
                                    rect.width(), rect.height()/2),
                                    opt.displayAlignment, c->formattedName());
            QVariant var_p = c->photo();
            if (var_p.isValid()) {
                painter->drawImage(QRect(rect.left()+1, rect.top()+1,
                                         sizeImage_, sizeImage_),
                                   var_p.value<QImage>());
            } else {
                QImage defaultImage(":images/account.png");
                painter->drawImage(QRect(rect.left(), rect.top(),
                                         sizeImage_, sizeImage_),
                                   defaultImage);
            }
            switch (c->phoneNumbers().size()) {
            case 0:
                break;
            case 1:
            {
                QString number;
                QVariant var_n =
                        c->phoneNumbers().first()->roleData(Qt::DisplayRole);
                if (var_n.isValid())
                    number = var_n.value<QString>();

                painter->drawText(QRect(rect.left()+sizeImage_+5,
                                        rect.top() + rect.height()/2,
                                        rect.width(), rect.height()/2),
                                  opt.displayAlignment, number);
                break;
            }
            default:
                painter->drawText(QRect(rect.left()+sizeImage_+5,
                                        rect.top() + rect.height()/2,
                                        rect.width(), rect.height()/2),
                                  opt.displayAlignment, tr("<Multiple contact methods>"));
                break;
            }
        }
    }
}

QSize
ContactDelegate::sizeHint(const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    auto height = (result.height()*2)+2;
    if (height < sizeImage_)
        height = sizeImage_;
    result.setHeight(height);
    return result;
}
