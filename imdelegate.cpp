/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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
#include <QTextDocument>
#include <QSettings>
#include <QDateTime>

#include "media/text.h"
#include "media/textrecording.h"

#include "ringthemeutils.h"
#include "settingskey.h"

// Constants
constexpr static QSize iconSize_ {38,38};
constexpr static int padding_ = 10;
constexpr static qreal radius_ratio_ = 5.0;

// Impl
ImDelegate::ImDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

void
ImDelegate::formatMsg(const QModelIndex& index, QString& msg) const
{
    QSettings settings;
    QStringList meta;
    if (settings.value(SettingsKey::imShowAuthor).toBool()) {
        meta << index.data(
                    static_cast<int>(Media::TextRecording::Role::AuthorDisplayname)).toString();
    }
    if (settings.value(SettingsKey::imShowDate).toBool()) {
        auto timeStamp = index.data(
                    static_cast<int>(Media::TextRecording::Role::Timestamp)).value<uint>();
        auto date = QDateTime::fromTime_t(timeStamp);
        auto now = QDateTime::currentDateTime();
        if (now.date() == date.date())
            meta << date.time().toString();
        else
            meta << date.toString();
    }
    msg = QString("%2<footer><em>%1</em></footer>").arg(meta.join(" - "), msg);
}

void
ImDelegate::paint(QPainter* painter,
                  const QStyleOptionViewItem& option,
                  const QModelIndex& index) const
{
    // painter setup
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    painter->setRenderHint(QPainter::Antialiasing);
    opt.font = fontMsg_;
    painter->setFont(fontMsg_);

    if (index.isValid()) {
        auto msg = index.data(static_cast<int>(Media::TextRecording::Role::FormattedHtml)).toString();
        opt.text.clear();
        QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

        auto dir = index.data(static_cast<int>(Media::TextRecording::Role::Direction))
                .value<Media::Media::Direction>() == Media::Media::Direction::IN
                ? Qt::AlignLeft : Qt::AlignRight;

        formatMsg(index, msg);

        QRect textRect = getBoundingRect(dir, msg, opt);

        if (dir == Qt::AlignLeft) {
            opt.decorationSize = iconSize_;
            opt.decorationPosition = (dir == Qt::AlignRight ?
                                          QStyleOptionViewItem::Right : QStyleOptionViewItem::Left);
            opt.decorationAlignment = Qt::AlignTop | Qt::AlignHCenter;
        } else {
            opt.decorationSize = QSize();
        }
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        QPainterPath path;
        path.addRoundedRect(textRect, radius_ratio_, radius_ratio_);

        if (dir == Qt::AlignRight) {
            painter->fillPath(path, RingTheme::imBlue_);
        } else {
            painter->fillPath(path, RingTheme::imGrey_);
        }

        painter->save();

        QTextDocument document;
        document.setDefaultFont(fontMsg_);
        document.setHtml(msg);

        auto textOptions = QTextOption();
        textOptions.setWrapMode(QTextOption::WrapMode::WrapAtWordBoundaryOrAnywhere);
        document.setDefaultTextOption(textOptions);

        painter->translate(textRect.topLeft());
        document.drawContents(painter);
        painter->restore();
    }
}

QRect ImDelegate::getBoundingRect(const Qt::AlignmentFlag& dir,
                                  const QString& msg,
                                  const QStyleOptionViewItem &option) const
{
    QRect textRect;

    QTextDocument txtDoc;
    txtDoc.setDefaultFont(fontMsg_);
    txtDoc.setHtml(msg);
    auto textOptions = QTextOption(Qt::AlignVCenter);
    textOptions.setWrapMode(QTextOption::WrapMode::WrapAtWordBoundaryOrAnywhere);
    txtDoc.setDefaultTextOption(textOptions);

    if (dir == Qt::AlignLeft) {
        textRect.setRect(option.rect.left() + iconSize_.width() + padding_,
                         option.rect.top() + padding_,
                         txtDoc.idealWidth() + padding_,
                         txtDoc.size().height() + padding_);
    } else {
        textRect.setRect(option.rect.right() - txtDoc.idealWidth() - padding_,
                         option.rect.top() + padding_,
                         txtDoc.idealWidth() + padding_,
                         txtDoc.size().height() + padding_);
    }
    return textRect;
}

QSize
ImDelegate::sizeHint(const QStyleOptionViewItem& option,
                     const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    opt.font = fontMsg_;

    QString msg = index.data(static_cast<int>(Media::TextRecording::Role::FormattedHtml)).toString();

    auto dir = index.data(
                static_cast<int>(Media::TextRecording::Role::Direction))
            .value<Media::Media::Direction>() == Media::Media::Direction::IN
            ? Qt::AlignLeft : Qt::AlignRight;

    formatMsg(index, msg);

    QRect boundingRect = getBoundingRect(dir, msg, opt);

    QSize size(option.rect.width(), boundingRect.height());

    /* Keep the minimum height needed. */
    if(size.height() < iconSize_.height())
        size.setHeight(iconSize_.height());

    size.setHeight(size.height() + 2 * padding_);

    return size;
}

