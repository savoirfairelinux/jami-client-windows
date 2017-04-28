/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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
 ***************************************************************************/

#include "contactrequestitemdelegate.h"
#include "ringthemeutils.h"

#include "accountmodel.h"
#include "pendingcontactrequestmodel.h"
#include "contactrequest.h"
#include "globalinstances.h"
#include "pixmapmanipulatordefault.h"

#include <QPainter>
#include <QApplication>

ContactRequestItemDelegate::ContactRequestItemDelegate(QObject* parent) :
    QItemDelegate(parent)
{}

void
ContactRequestItemDelegate::paint(QPainter* painter
                        , const QStyleOptionViewItem& option
                        , const QModelIndex& index
                        ) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    QStyleOptionViewItem opt(option);

    // Not having focus removes dotted lines around the item
    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    // First, we draw the control itself
    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    // Then, we print the text
    QFont font(painter->font());
    font.setPointSize(10);
    font.setBold(true);
    painter->setFont(font);

    QFontMetrics fontMetrics(font);

    QRect rectText(opt.rect);
    rectText.setLeft(opt.rect.left() + dxText_);
    rectText.setTop(opt.rect.top() + dyText_);
    rectText.setBottom(rectText.top() + fontMetrics.height());

    QString text(index.data().toString());
    text = fontMetrics.elidedText(text, Qt::ElideRight, rectText.width());

    QPen pen(painter->pen());

    pen.setColor(RingTheme::lightBlack_);
    painter->setPen(pen);

    painter->drawText(rectText,text);

    // Draw the picture from the vCard
    QRect rectPic(opt.rect.left() + dxImage_, opt.rect.top() + dyImage_, sizeImage_, sizeImage_);
    auto cr = index.data(static_cast<int>(Ring::Role::Object)).value<ContactRequest*>();
    auto photo = GlobalInstances::pixmapManipulator().contactPhoto(cr->peer(), QSize(sizeImage_, sizeImage_), false);
    drawDecoration(painter, opt, rectPic, QPixmap::fromImage(photo.value<QImage>()));

    // Draw separator when item is not selected
    if (not (opt.state & QStyle::State_Selected)) {
        QRect rect(opt.rect);
        pen.setColor(RingTheme::lightGrey_);
        painter->setPen(pen);
        painter->drawLine(rect.left() + separatorYPadding_, rect.bottom(),
                          rect.right() - separatorYPadding_,
                          rect.bottom());
    }
}

QSize
ContactRequestItemDelegate::sizeHint(const QStyleOptionViewItem& option
                           , const QModelIndex& index
                           ) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    size.setHeight(cellHeight_);
    return size;
}
