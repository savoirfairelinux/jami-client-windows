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
#include <QTextDocument>
#include <QSettings>
#include <QDateTime>

#include "media/text.h"
#include "media/textrecording.h"

#include "ringthemeutils.h"
#include "settingskey.h"

ImDelegate::ImDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      linkRegex_(
          QRegularExpression(QStringLiteral("([a-z]{3,9}:|www\\.)([^\\s,.);!>]|[,.);!>](?!\\s|$))+"),
                             QRegularExpression::CaseInsensitiveOption))
{
}

QUrl ImDelegate::getParsedUrl(const QModelIndex& index)
{
    auto it = linkRegex_.globalMatch(index.data().toString());
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        return QUrl::fromUserInput(match.capturedRef().toString());
    }
    return QUrl();
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
    msg = QString("%2<footer><i>%1</i></footer>").arg(meta.join(" - "), msg);
}

QString
ImDelegate::parseForLink(const QString& msg) const
{
    QString re;
    auto p = 0;
    auto it = linkRegex_.globalMatch(msg);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        auto start = match.capturedStart();

        auto url = QUrl::fromUserInput(match.capturedRef().toString());

        if (start > p)
            re.append(msg.mid(p, start - p).toHtmlEscaped().replace(QLatin1Char('\n'),
                                                                    QStringLiteral("<br/>")));
        re.append(QStringLiteral("<a href=\"%1\">%2</a>")
                  .arg(QString::fromLatin1(url.toEncoded()).toHtmlEscaped(),
                       match.capturedRef().toString().toHtmlEscaped()));
        p = match.capturedEnd();
    }

    return QString("<body>%1</body>").arg(re.isEmpty() ? msg : re);
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

        msg = parseForLink(msg);
        formatMsg(index, msg);

        QRect textRect = getBoundingRect(dir, msg, opt);

        opt.decorationSize = iconSize_;
        opt.decorationPosition = (dir == Qt::AlignRight ?
                                      QStyleOptionViewItem::Right : QStyleOptionViewItem::Left);
        opt.decorationAlignment = Qt::AlignTop | Qt::AlignHCenter;
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        QPainterPath path;
        path.addRoundedRect(textRect, padding_, padding_);

        if (dir == Qt::AlignRight) {
            auto status = index.data(static_cast<int>(Media::TextRecording::Role::DeliveryStatus)).toString();
            if (status == QStringLiteral("SENDING"))
                painter->fillPath(path, RingTheme::lightBlue_);
            else if (status == QStringLiteral("FAILURE"))
                painter->fillPath(path, RingTheme::red_);
            else
                painter->fillPath(path, RingTheme::blue_);
        }
        else {
            painter->fillPath(path, Qt::white);
        }

        painter->save();

        QTextDocument document;
        document.setDefaultFont(fontMsg_);

        if (dir == Qt::AlignRight)
            document.setDefaultStyleSheet("body { color : white; } i { opacity: 100; font-size : 11px; text-align : right; }");
        else
            document.setDefaultStyleSheet("body { color : black; } i { opacity: 100; font-size : 11px; text-align : right; }");

        document.setHtml(msg);

        auto textOptions = QTextOption(Qt::AlignLeft);
        textOptions.setWrapMode(QTextOption::WrapMode::WordWrap);
        document.setDefaultTextOption(textOptions);
        document.setTextWidth(textRect.width());

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
    auto textOptions = QTextOption(Qt::AlignLeft);
    textOptions.setWrapMode(QTextOption::WrapMode::WordWrap);
    txtDoc.setDefaultTextOption(textOptions);

    if (dir == Qt::AlignLeft) {
        txtDoc.setTextWidth(option.rect.width() - iconSize_.width() - padding_);
        textRect.setRect(option.rect.left() + iconSize_.width() + padding_,
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

    QString msg = index.data(Qt::DisplayRole).toString();

    auto dir = index.data(
                static_cast<int>(Media::TextRecording::Role::Direction))
            .value<Media::Media::Direction>() == Media::Media::Direction::IN
            ? Qt::AlignLeft : Qt::AlignRight;

    msg = parseForLink(msg);
    formatMsg(index, msg);

    QRect boundingRect = getBoundingRect(dir, msg, opt);

    QSize size(option.rect.width(), boundingRect.height());

    /* Keep the minimum height needed. */
    if(size.height() < iconSize_.height())
        size.setHeight(iconSize_.height());

    size.setHeight(size.height() + 2 * padding_);

    return size;
}

