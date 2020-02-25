/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                                *
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

#include "accountitemdelegate.h"

#include <QApplication>
#include <QTextDocument>
#include <QPixmap>

#include "utils.h"
#include "accountlistmodel.h"
#include "ringthemeutils.h"
#include "lrcinstance.h"

#undef REGISTERED

AccountItemDelegate::AccountItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

void
AccountItemDelegate::paint(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);
    //painter->setRenderHint(QPainter::Antialiasing);

    // Not having focus removes dotted lines around the item
    if (opt.state & QStyle::State_HasFocus) {
        opt.state.setFlag(QStyle::State_HasFocus, false);
    }

    // First, we draw the control itself
    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QColor presenceBorderColor = Qt::white;
    bool selected = false;
    if (option.state & QStyle::State_Selected) {
        selected = true;
        opt.state ^= QStyle::State_Selected;
        painter->fillRect(option.rect, RingTheme::smartlistSelection_);
        presenceBorderColor = RingTheme::smartlistSelection_;
    }
    else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, RingTheme::smartlistHighlight_);
        presenceBorderColor = RingTheme::smartlistHighlight_;
    }

    QRect &rect = opt.rect;

    // define and set the two fonts
    QFont fontPrimary = painter->font();
    QFont fontSecondary = painter->font();
    fontPrimary.setWeight(QFont::ExtraLight);
    auto scalingRatio = Utils::getCurrentScalingRatio();
    if (scalingRatio > 1.0) {
        fontPrimary.setPointSize(fontSize_ - 1);
        fontSecondary.setPointSize(fontSize_ - 2);
    } else {
        fontPrimary.setPointSize(fontSize_);
        fontSecondary.setPointSize(fontSize_ - 1);
    }

    QPen pen(painter->pen());

    // is it the add account row?
    if (index.row() == LRCInstance::accountModel().getAccountList().size()) {
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        painter->setFont(fontPrimary);
        QFontMetrics fontMetrics(fontPrimary);
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, tr("Add Account") + "+");
        return;
    }

    // Avatar drawing
    opt.decorationSize = QSize(avatarSize_, avatarSize_);
    opt.decorationPosition = QStyleOptionViewItem::Left;
    opt.decorationAlignment = Qt::AlignCenter;

    QRect rectAvatar(
        leftPadding_ + rect.left(),
        rect.top() + topPadding_,
        avatarSize_,
        avatarSize_
    );
    drawDecoration(painter, opt, rectAvatar,
        QPixmap::fromImage(index.data(AccountListModel::Role::Picture).value<QImage>())
        .scaled(avatarSize_, avatarSize_, Qt::KeepAspectRatio));

    auto status = index.data(static_cast<int>(AccountListModel::Role::Status)).value<int>();
    auto isPresent = Utils::toEnum<lrc::api::account::Status>(status) == lrc::api::account::Status::REGISTERED;
    // Presence indicator
    if (isPresent) {
        QPainterPath outerCircle, innerCircle;
        QPointF center(rectAvatar.right() - avatarSize_ / 6, (rectAvatar.bottom() - avatarSize_ / 6) + 1);
        qreal outerCRadius = avatarSize_ / 6, innerCRadius = outerCRadius * 0.75;
        outerCircle.addEllipse(center, outerCRadius, outerCRadius);
        innerCircle.addEllipse(center, innerCRadius, innerCRadius);
        painter->fillPath(outerCircle, presenceBorderColor);
        painter->fillPath(innerCircle, RingTheme::presenceGreen_);
    }

    painter->setPen(pen);

    QRect rectTexts(leftPadding_ + rect.left() + leftPadding_ + avatarSize_,
                    rect.top() + topPadding_ / 2,
                    rect.width(),
                    rect.height() / 2);

    // The name is displayed at the avatar's right
    QVariant name = index.data(static_cast<int>(AccountListModel::Role::Alias));
    if (name.isValid())
    {
        pen.setColor(RingTheme::lightBlack_);
        painter->setPen(pen);
        QFont emojiMsgFont(QStringLiteral("Segoe UI Emoji"));
        emojiMsgFont.setPointSize(scalingRatio > 1.0 ? fontSize_ - 2 : fontSize_);
        painter->setFont(emojiMsgFont);
        QString nameStr = QFontMetrics(emojiMsgFont).elidedText(name.value<QString>(), Qt::ElideRight,
            rectTexts.width() - avatarSize_ - leftPadding_ - rightPadding_ * 2);
        painter->drawText(rectTexts, Qt::AlignVCenter | Qt::AlignLeft, nameStr);
        painter->setFont(fontPrimary);
    }

    // Display the secondary ID under the name
    QString idStr = index.data(static_cast<int>(AccountListModel::Role::Username)).value<QString>();
    if (idStr != name.toString()) {
        fontSecondary.setItalic(false);
        fontSecondary.setBold(false);
        pen.setColor(RingTheme::grey_);
        painter->setPen(pen);
        painter->setFont(fontSecondary);
        QFontMetrics fontMetrics(fontSecondary);
        if (!idStr.isNull()) {
            idStr = fontMetrics.elidedText(idStr, Qt::ElideRight,
                rectTexts.width() - avatarSize_ - leftPadding_ - rightPadding_ * 2);
            painter->drawText(QRect(leftPadding_ + rect.left() + leftPadding_ + avatarSize_,
                              rect.top() + topPadding_ * 3,
                              rect.width(),
                              rect.height() / 2),
                              Qt::AlignBottom | Qt::AlignLeft, idStr);
        }
    }
}

QSize AccountItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    size.setHeight(cellHeight_);
    return size;
}
