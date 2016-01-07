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

#include "imdelegate.h"

#include "media/text.h"
#include "media/textrecording.h"

ImDelegate::ImDelegate(QObject *parent)
    : QStyledItemDelegate(parent), showDate_(false), showAuthor_(false)
{}

void ImDelegate::setDisplayOptions(ImDelegate::DisplayOptions opt)
{
    qDebug() << opt;
    showAuthor_ = opt & DisplayOptions::AUTHOR;
    showDate_ = opt & DisplayOptions::DATE;
}

void
ImDelegate::paint(QPainter *painter,
                  const QStyleOptionViewItem &option,
                  const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (index.isValid()) {

        auto msg = index.model()->data(index, Qt::DisplayRole).toString();
        opt.text.clear();
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        auto rect = opt.rect;
        QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled
                ? QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
            cg = QPalette::Inactive;
        painter->setPen(opt.palette.color(cg, QPalette::Text));
        painter->setOpacity(1.0);
        auto dir = index.model()->data(
                    index,
                    static_cast<int>(Media::TextRecording::Role::Direction))
                .value<Media::Media::Direction>() == Media::Media::Direction::IN
                ? Qt::AlignLeft : Qt::AlignRight;

        if (showAuthor_) {
            auto author = index.model()->
                    data(index,
                         static_cast<int>(Media::TextRecording::Role::AuthorDisplayname)).toString();
            msg = QString("(%1) %2").arg(author, msg);
        }
        if (showDate_) {
            auto formattedDate = index.model()->
                    data(index,
                         static_cast<int>(Media::TextRecording::Role::FormattedDate)).toString();
            msg = QString("[%1] %2").arg(formattedDate, msg);
        }
        painter->drawText(QRect(rect.left(), rect.top(), rect.width(), rect.height()),
                          dir, msg);
    }
}

QSize
ImDelegate::sizeHint(const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    result.setHeight(result.height());
    return result;
}

