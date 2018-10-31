/***************************************************************************
 * Copyright (C) 2015-2018 by Savoir-faire Linux                           *
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


#include "ringthemeutils.h"
#include "settingskey.h"
#include "messagemodel.h"
#include "utils.h"
#include "lrcinstance.h"
#include "selectablemessagewidget.h"

static QString
QStringFromMsgSeq(const MsgSeq& msgseq)
{
    switch (msgseq) {
    case MsgSeq::FIRST_WITHOUT_TIME: return QString("FIRST_WITHOUT_TIME");
    case MsgSeq::FIRST_WITH_TIME: return QString("FIRST_WITH_TIME");
    case MsgSeq::LAST_IN_SEQUENCE: return QString("LAST_IN_SEQUENCE");
    case MsgSeq::MIDDLE_IN_SEQUENCE: return QString("MIDDLE_IN_SEQUENCE");
    case MsgSeq::SINGLE_WITHOUT_TIME: return QString("SINGLE_WITHOUT_TIME");
    case MsgSeq::SINGLE_WITH_TIME: return QString("SINGLE_WITH_TIME");
    }
}

ImDelegate::ImDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

void
ImDelegate::formatMsg(const QModelIndex& index, QString& msgString) const
{
    auto date = QDateTime::fromTime_t(index
        .data(static_cast<int>(MessageModel::Role::InteractionDate))
        .value<time_t>()
    );
    auto now = QDateTime::currentDateTime();
    QString dateString;
    if (now.date() == date.date()) {
        dateString = date.time().toString();
    } else {
        dateString = date.toString();
    }
    msgString = QString("<p>%1</p>").arg(msgString);
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

    painter->setRenderHint(QPainter::Antialiasing);

    //opt.font = fontMsg_;
    //painter->setFont(fontMsg_);

    opt.text.clear();

    auto sequencing = Utils::toEnum<MsgSeq>(index
        .data(static_cast<int>(MessageModel::Role::Sequencing))
        .value<int>()
        );

    //qDebug() << "MSG_SEQ: " << index.row() << ", " << QStringFromMsgSeq(sequencing);

    formatMsg(index, msg);

    QTextDocument  document;
    document.setDefaultStyleSheet(defaultStylesheet_);
    //document.setDefaultFont(fontMsg_);
    document.setHtml(msg);
    auto textOptions = QTextOption(Qt::AlignLeft);
    textOptions.setWrapMode(QTextOption::WrapMode::WrapAtWordBoundaryOrAnywhere);
    document.setDefaultTextOption(textOptions);

    QRect textRect = getBoundingRect(dir, opt, document);
    document.setTextWidth(textRect.width());

    auto shouldDrawAvatar = (sequencing == MsgSeq::LAST_IN_SEQUENCE ||
                             sequencing == MsgSeq::SINGLE_WITHOUT_TIME ||
                             sequencing == MsgSeq::SINGLE_WITH_TIME);

    //painter->fillRect(option.rect, isGenerated ? Qt::lightGray : (isOutgoing ? Qt::red : Qt::blue));

    if (isOutgoing) {
        opt.decorationSize = QSize();
        opt.decorationPosition = QStyleOptionViewItem::Right;
    } else if (shouldDrawAvatar) {
        opt.decorationSize = QSize(sizeImage_, sizeImage_);
        opt.decorationPosition = QStyleOptionViewItem::Left;
        opt.decorationAlignment = Qt::AlignCenter;
        QRect rectAvatar(margin_ + opt.rect.left(), opt.rect.top(), sizeImage_, sizeImage_);
        drawDecoration(painter, opt, rectAvatar,
            QPixmap::fromImage(index.data(Qt::DecorationRole).value<QImage>())
            .scaled(sizeImage_, sizeImage_, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // message bubble
    if (!isGenerated) {
        paintBubble(painter, textRect, sequencing, isOutgoing);
    }

    painter->save();

    // HACK: only draw text if not hovering, when editor is displayed
    if (!(option.state & QStyle::State_MouseOver)) {
        painter->translate(textRect.topLeft());
        document.drawContents(painter);
        painter->restore();
    }
}

QRect ImDelegate::getBoundingRect(const Qt::AlignmentFlag& dir,
                                  const QStyleOptionViewItem &option,
                                  QTextDocument& txtDoc) const
{
    QRect textRect;

    if (dir == Qt::AlignLeft) {
        txtDoc.setTextWidth(option.rect.width());
        textRect.setRect(option.rect.left(),
                         option.rect.top(),
                         txtDoc.idealWidth(),
                         txtDoc.size().height());
    } else if (dir == Qt::AlignHCenter) {
        txtDoc.setTextWidth(option.rect.width());
        auto optCenter = option.rect.left() + option.rect.width() / 2;
        textRect.setRect(optCenter - txtDoc.idealWidth() / 2,
                         option.rect.top(),
                         txtDoc.idealWidth(),
                         txtDoc.size().height());
    } else if (dir == Qt::AlignRight) {
        txtDoc.setTextWidth(option.rect.width());
        textRect.setRect(option.rect.right()- txtDoc.idealWidth(),
                         option.rect.top(),
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

    QString msg = index.data(static_cast<int>(MessageModel::Role::Body)).toString();

    auto isOutgoing = index.data(static_cast<int>(MessageModel::Role::Direction)).value<bool>();
    auto isGenerated = Utils::isInteractionGenerated(
        static_cast<lrc::api::interaction::Type>(index.data(static_cast<int>(MessageModel::Role::Type)).value<int>())
    );
    auto dir = isGenerated ? Qt::AlignHCenter : (isOutgoing ? Qt::AlignRight : Qt::AlignLeft);

    formatMsg(index, msg);

    QTextDocument document;
    document.setDefaultStyleSheet(defaultStylesheet_);
    //document.setDefaultFont(fontMsg_);
    document.setHtml(msg);
    auto textOptions = QTextOption(Qt::AlignLeft);
    textOptions.setWrapMode(QTextOption::WrapMode::WrapAtWordBoundaryOrAnywhere);
    document.setDefaultTextOption(textOptions);

    QRect boundingRect = getBoundingRect(dir, opt, document);
    document.setTextWidth(opt.rect.width());

    QSize size(boundingRect.width(), boundingRect.height());

    /* Keep the minimum height needed. */
    if (!isOutgoing && !isGenerated && size.height() < sizeImage_) {
        size.setHeight(sizeImage_);
    }

    size.setHeight(size.height());

    return size;
}

void
ImDelegate::paintBubble(QPainter* painter, const QRect& rect, const MsgSeq& msgseq, bool outgoing) const
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(rect, bubbleRadius_, bubbleRadius_);
    switch (msgseq) {
    case MsgSeq::FIRST_WITHOUT_TIME:
    case MsgSeq::FIRST_WITH_TIME: {
        int left = outgoing ? rect.right() - bubbleRadius_ : rect.left();
        // bottom
        path.addRect(QRect(left,
            rect.bottom() - bubbleRadius_ + 1,
            bubbleRadius_ + 1,
            bubbleRadius_));
        break;

    }
    case MsgSeq::LAST_IN_SEQUENCE: {
        int left = outgoing ? rect.right() - bubbleRadius_ : rect.left();
        // top
        path.addRect(QRect(left,
            rect.top(),
            bubbleRadius_ + 1,
            bubbleRadius_));
        break;

    }
    case MsgSeq::MIDDLE_IN_SEQUENCE: {
        int left = outgoing ? rect.right() - bubbleRadius_ : rect.left();
        // top
        path.addRect(QRect(left,
            rect.top(),
            bubbleRadius_ + 1,
            bubbleRadius_));
        // bottom
        path.addRect(QRect(left,
            rect.bottom() - bubbleRadius_ + 1,
            bubbleRadius_ + 1,
            bubbleRadius_));
        break;
    }
    case MsgSeq::SINGLE_WITHOUT_TIME:
    case MsgSeq::SINGLE_WITH_TIME: {
        break;
    }
    }
    painter->fillPath(path.simplified(), outgoing ? RingTheme::imGrey_ : RingTheme::imBlue_);
}

QWidget*
ImDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const

{
    SelectableMessageWidget* editor = new SelectableMessageWidget(index, parent);
    QRect rect = option.rect;
    QSize sizeHint = editor->sizeHint();

    if (rect.width() < sizeHint.width()) rect.setWidth(sizeHint.width());
    if (rect.height() < sizeHint.height()) rect.setHeight(sizeHint.height());

    editor->setGeometry(rect);
    return editor;
}