/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                           *
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

#include <QApplication>

#include "media/text.h"
#include "media/textrecording.h"

#include "ringthemeutils.h"

ImDelegate::ImDelegate(QObject *parent)
    : QStyledItemDelegate(parent), showDate_(false), showAuthor_(false)
{}

void ImDelegate::setDisplayOptions(ImDelegate::DisplayOptions opt)
{
    showAuthor_ = opt & DisplayOptions::AUTHOR;
    showDate_ = opt & DisplayOptions::DATE;
    emit sizeHintChanged(QModelIndex());
}

void
ImDelegate::formatMsg(const QModelIndex& index, QString& msg) const
{
    if (showAuthor_) {
        auto author = index.data(
                    static_cast<int>(Media::TextRecording::Role::AuthorDisplayname)).toString();
        msg = QString("(%1)\n%2").arg(author, msg);
    }
    if (showDate_) {
        auto formattedDate = index.data(
                    static_cast<int>(Media::TextRecording::Role::FormattedDate)).toString();
        msg = QString("%2\n%1").arg(formattedDate, msg);
    }
}

void
ImDelegate::paint(QPainter* painter,
                  const QStyleOptionViewItem& option,
                  const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    painter->setRenderHint(QPainter::Antialiasing);

    opt.font = fontMsg_;
    painter->setFont(fontMsg_);

    if (index.isValid()) {
        auto msg = index.data(Qt::DisplayRole).toString();
        opt.text.clear();
        QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
        QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled
                ? QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
            cg = QPalette::Inactive;

        auto dir = index.data(static_cast<int>(Media::TextRecording::Role::Direction))
                .value<Media::Media::Direction>() == Media::Media::Direction::IN
                ? Qt::AlignLeft : Qt::AlignRight;

        formatMsg(index, msg);

        QRect textRect = getBoundingRect(dir, msg, opt);

        QRect bubbleRect(textRect.left() - padding_,
                         textRect.top() - padding_,
                         textRect.width() + 2 * padding_,
                         textRect.height() + 2 * padding_ );

        opt.decorationSize = iconSize_;
        opt.decorationPosition = (dir == Qt::AlignRight ?
                                      QStyleOptionViewItem::Right : QStyleOptionViewItem::Left);
        opt.decorationAlignment = Qt::AlignTop | Qt::AlignHCenter;
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        QPainterPath path;
        path.addRoundedRect(bubbleRect, padding_, padding_);

        if (dir == Qt::AlignRight) {
            auto status = index.data(static_cast<int>(Media::TextRecording::Role::DeliveryStatus)).toString();
            if (status == QStringLiteral("SENDING"))
                painter->fillPath(path, RingTheme::lightBlack_);
            else if (status == QStringLiteral("FAILURE"))
                painter->fillPath(path, RingTheme::red_);
            else
                painter->fillPath(path, RingTheme::blue_);
            painter->setPen(Qt::white);
        }
        else {
            painter->fillPath(path, Qt::white);
            painter->setPen(Qt::black);
        }

        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, msg);
    }
}

QRect ImDelegate::getBoundingRect(const Qt::AlignmentFlag& dir, const QString& msg, const QStyleOptionViewItem &option) const
{
    QFont textFont = option.font;
    QFontMetrics textFontMetrics(textFont);
    QRect textRect;

    if (dir == Qt::AlignRight) {
        textRect = textFontMetrics.boundingRect(option.rect.left() + 2 * padding_,
                                                option.rect.top() + 2 * padding_,
                                                option.rect.width() - iconSize_.width() - 4 * padding_,
                                                0,
                                                dir|Qt::AlignTop|Qt::TextWordWrap,
                                                msg);
    } else {
        textRect = textFontMetrics.boundingRect(option.rect.left() + iconSize_.width() + 2 * padding_,
                                                option.rect.top() + 2 * padding_,
                                                option.rect.width() - iconSize_.width() - 4 * padding_ ,
                                                0,
                                                dir|Qt::AlignTop|Qt::TextWordWrap,
                                                msg);
    }
    return textRect;
}

QSize
ImDelegate::sizeHint(const QStyleOptionViewItem& option,
                     const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    opt.font = fontMsg_;

    QString msg = index.data(Qt::DisplayRole).toString();

    auto dir = index.data(
                static_cast<int>(Media::TextRecording::Role::Direction))
            .value<Media::Media::Direction>() == Media::Media::Direction::IN
            ? Qt::AlignLeft : Qt::AlignRight;

    formatMsg(index, msg);

    QRect boundingRect = getBoundingRect(dir, msg, opt);

    QSize size(option.rect.width(), boundingRect.height() + padding_);

    /* Keep the minimum height needed. */
    if(size.height() < iconSize_.height())
        size.setHeight(iconSize_.height() + padding_);

    return size;
}

