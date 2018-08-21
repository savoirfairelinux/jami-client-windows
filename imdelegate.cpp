/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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
#include <QSettings>
#include <QDateTime>

#include "media/text.h"
#include "media/textrecording.h"

#include "ringthemeutils.h"
#include "settingskey.h"
#include "messagemodel.h"
#include "utils.h"

ImDelegate::ImDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void
ImDelegate::formatMsg(const QModelIndex& index, QString& msgString) const
{
    auto date = index.data(static_cast<int>(MessageModel::Role::InteractionDate)).value<QDateTime>();
    auto now = QDateTime::currentDateTime();
    QString dateString;
    if (now.date() == date.date()) {
        dateString = date.time().toString();
    } else {
        dateString = date.toString();
    }
    msgString = QString("%1<br><footer><i>%2</i></footer>").arg(msgString, dateString);
}

void
ImDelegate::paint(QPainter* painter,
                  const QStyleOptionViewItem& option,
                  const QModelIndex& index) const
{
    if (!index.isValid()) {
        return;
    }

    auto msg = index.data(static_cast<int>(MessageModel::Role::Body)).toString();
    auto type = static_cast<lrc::api::interaction::Type>(index.data(static_cast<int>(MessageModel::Role::Type)).value<int>());
    auto isOutgoing = index.data(static_cast<int>(MessageModel::Role::Direction)).value<bool>();
    auto isGenerated = Utils::isInteractionGenerated(type);
    auto dir = isGenerated ? Qt::AlignHCenter : (isOutgoing ? Qt::AlignRight : Qt::AlignLeft);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    painter->setRenderHint(QPainter::Antialiasing);

    opt.font = fontMsg_;
    painter->setFont(fontMsg_);

    opt.text.clear();
    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

    formatMsg(index, msg);

    QTextDocument document;
    document.setDefaultStyleSheet(defaultStylesheet_);
    document.setDefaultFont(fontMsg_);
    document.setHtml(msg);
    auto textOptions = QTextOption(Qt::AlignLeft);
    textOptions.setWrapMode(QTextOption::WrapMode::WordWrap);
    document.setDefaultTextOption(textOptions);

    QRect textRect = getBoundingRect(dir, opt, document);
    document.setTextWidth(textRect.width());

    if (dir == Qt::AlignLeft) {
        opt.decorationSize = iconSize_;
        opt.decorationPosition = QStyleOptionViewItem::Left;
        opt.decorationAlignment = Qt::AlignTop | Qt::AlignHCenter;
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
    } else {
        opt.decorationSize = QSize();
        opt.decorationPosition = QStyleOptionViewItem::Right;
    }

    QPainterPath path;
    path.addRoundedRect(textRect, bubbleRadius_, bubbleRadius_);

    if (dir == Qt::AlignRight) {
        painter->fillPath(path, RingTheme::imGrey_);
    } else if (dir == Qt::AlignHCenter) {
        painter->fillPath(path, Qt::transparent);
    } else {
        painter->fillPath(path, RingTheme::imBlue_);
    }

    painter->save();

    painter->translate(textRect.topLeft());
    document.drawContents(painter);
    painter->restore();
}

QRect ImDelegate::getBoundingRect(const Qt::AlignmentFlag& dir,
                                  const QStyleOptionViewItem &option,
                                  QTextDocument& txtDoc) const
{
    QRect textRect;

    if (dir == Qt::AlignLeft) { 
        txtDoc.setTextWidth(option.rect.width() - iconSize_.width() - padding_);
        textRect.setRect(option.rect.left() + iconSize_.width() + padding_,
                         option.rect.top() + padding_,
                         txtDoc.idealWidth(),
                         txtDoc.size().height());
    } else if (dir == Qt::AlignHCenter) {
        txtDoc.setTextWidth(option.rect.width() - padding_);
        auto optCenter = option.rect.left() + option.rect.width() / 2;
        textRect.setRect(optCenter - txtDoc.idealWidth() / 2,
                         option.rect.top() + padding_,
                         txtDoc.idealWidth(),
                         txtDoc.size().height());
    } else {
        txtDoc.setTextWidth(option.rect.width() - padding_);
        textRect.setRect(option.rect.right() - padding_ - txtDoc.idealWidth(),
                         option.rect.top() + padding_,
                         txtDoc.idealWidth(),
                         txtDoc.size().height());
    }
    return textRect;
}

QSize
ImDelegate::sizeHint(const QStyleOptionViewItem& option,
                     const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    opt.font = fontMsg_;

    QString msg = index.data(static_cast<int>(media::TextRecording::Role::FormattedHtml)).toString();

    auto isOutgoing = index.data(static_cast<int>(MessageModel::Role::Direction)).value<bool>();
    auto isGenerated = Utils::isInteractionGenerated(
        static_cast<lrc::api::interaction::Type>(index.data(static_cast<int>(MessageModel::Role::Type)).value<int>())
    );
    auto dir = isGenerated ? Qt::AlignHCenter : (isOutgoing ? Qt::AlignRight : Qt::AlignLeft);

    formatMsg(index, msg);

    QTextDocument document;
    document.setDefaultFont(fontMsg_);
    document.setHtml(msg);
    auto textOptions = QTextOption(Qt::AlignLeft);
    textOptions.setWrapMode(QTextOption::WrapMode::WordWrap);
    document.setDefaultTextOption(textOptions);

    QRect boundingRect = getBoundingRect(dir, opt, document);

    QSize size(boundingRect.width() + 2 * margin_, boundingRect.height());

    /* Keep the minimum height needed. */
    if(size.height() < iconSize_.height())
        size.setHeight(iconSize_.height());

    size.setHeight(size.height() + 2 * margin_);

    return size;
}

