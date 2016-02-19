/***************************************************************************
 * Copyright (C) 2016 by Savoir-faire Linux                                *
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

#include "transferitemdelegate.h"

//Qt
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QProxyStyle>
#include <QStandardPaths>

//Client
#include "ringthemeutils.h"

//LRC
#include "filetransfermodel.h"
#include "transfer.h"
#include "contactmethod.h"

TransferItemDelegate::TransferItemDelegate(QObject* parent):
    QStyledItemDelegate(parent)
{
    acceptState_ =  QStyle::State_Raised;
    refuseState_ =  QStyle::State_Raised;
    cancelState_ =  QStyle::State_Raised;
}

void
TransferItemDelegate::paint(QPainter* painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);
    painter->setRenderHint(QPainter::Antialiasing);

    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    auto state = index.data(static_cast<int>(Ring::Role::State)).toInt();
    auto isInProgress = !index.data(static_cast<int>(FileTransferModel::Role::isWaitingAcceptance)).toBool();
    auto isError = (state >= 400);

    auto fileName = index.data(static_cast<int>(Ring::Role::DisplayRole)).toString();
    auto senderAccount =  "SENDER ACCOUNT"; //transfer->contactMethod() ? transfer->contactMethod()->primaryName() : "Unknown" ;
    auto msg = isInProgress ? QString(tr("Downloading file : %1")).arg(fileName)
                            : QString(tr("%1 would like to send you the file : %2")).arg(senderAccount).arg(fileName);

    QFontMetrics textFontMetrics(opt.font);
    auto rectName = textFontMetrics.boundingRect(opt.rect,
                                                 Qt::TextWordWrap | Qt::AlignHCenter, msg);
    painter->drawText(rectName, msg);

    if (isInProgress || isError) {
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.state = QStyle::State_Enabled;
        progressBarOption.direction = QApplication::layoutDirection();
        progressBarOption.rect = QRect(opt.rect.left(),
                                       opt.rect.top() + rectName.height() + 2 * margin_,
                                       opt.rect.width() - RingTheme::smallButton_.width() - margin_,
                                       RingTheme::smallButton_.height());
        progressBarOption.fontMetrics = QApplication::fontMetrics();
        progressBarOption.minimum = 0;
        progressBarOption.textAlignment = Qt::AlignCenter;
        progressBarOption.textVisible = true;
        int progress = index.data(static_cast<int>(FileTransferModel::Role::Progress)).toInt();
        if (isError || progress == 0) {
            progressBarOption.text = index.data(static_cast<int>(Ring::Role::FormattedState)).toString();
            progressBarOption.maximum = 0;
        } else {
            progressBarOption.maximum = 100;
            progressBarOption.progress = progress < 0 ? 0 : progress;
            progressBarOption.text = QString().sprintf("%d%%", progressBarOption.progress);
        }

        /* Nice little trick to avoid the non-updating animation on QProgessBar
         * learned from qBittorent code */
        QProxyStyle st("fusion");
        st.drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);

        auto cancelRect = QRect(opt.rect.left() + progressBarOption.rect.width() + margin_,
                                progressBarOption.rect.top(),
                                RingTheme::smallButton_.width(),
                                RingTheme::smallButton_.height());

        painter->save();
        if (cancelState_ == QStyle::State_Raised) {
            painter->setPen(RingTheme::red_);
            painter->setBrush(RingTheme::red_);
        } else {
            painter->setPen(RingTheme::darkRed_);
            painter->setBrush(RingTheme::darkRed_);
        }
        painter->drawEllipse(cancelRect);
        QPixmap cancelPixmap("://images/icons/ic_close_white_24dp.png");
        QRect cancelButtonIconRect = QRect(cancelRect);
        cancelButtonIconRect.setTop(cancelRect.top() + iconInset_);
        cancelButtonIconRect.setLeft(cancelRect.left() + iconInset_);
        cancelButtonIconRect.setSize(RingTheme::smallButtonIcon_);
        painter->drawPixmap(cancelButtonIconRect,
                            cancelPixmap.scaled(RingTheme::smallButtonIcon_,
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
        painter->restore();
    } else {

        auto acceptButtonRect = QRect(opt.rect.left() + opt.rect.width() / 2 - RingTheme::smallButton_.width(),
                                      opt.rect.top() + opt.rect.height() / 2 - RingTheme::smallButton_.height() / 2,
                                      RingTheme::smallButton_.width(),
                                      RingTheme::smallButton_.height());
        painter->save();
        if (acceptState_ == QStyle::State_Raised) {
            painter->setPen(RingTheme::green_);
            painter->setBrush(RingTheme::green_);
        } else {
            painter->setPen(RingTheme::darkGreen_);
            painter->setBrush(RingTheme::darkGreen_);
        }
        painter->drawEllipse(acceptButtonRect);
        QPixmap acceptPixmap("://images/icons/ic_done_white_24dp.png");
        QRect acceptButtonIconRect = QRect(acceptButtonRect);
        acceptButtonIconRect.setTop(acceptButtonRect.top() + iconInset_);
        acceptButtonIconRect.setLeft(acceptButtonRect.left() + iconInset_);
        acceptButtonIconRect.setSize(RingTheme::smallButtonIcon_);
        painter->drawPixmap(acceptButtonIconRect,
                            acceptPixmap.scaled(RingTheme::smallButtonIcon_,
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
        painter->restore();

        painter->save();
        auto refuseRect = QRect(acceptButtonRect.left() + acceptButtonRect.width() + margin_,
                                acceptButtonRect.top(),
                                RingTheme::smallButton_.width(),
                                RingTheme::smallButton_.height());

        if (refuseState_ == QStyle::State_Raised) {
            painter->setPen(RingTheme::red_);
            painter->setBrush(RingTheme::red_);
        } else {
            painter->setPen(RingTheme::darkRed_);
            painter->setBrush(RingTheme::darkRed_);
        }
        painter->drawEllipse(refuseRect);
        QPixmap refusePixmap("://images/icons/ic_close_white_24dp.png");
        QRect refuseButtonIconRect = QRect(refuseRect);
        refuseButtonIconRect.setTop(refuseRect.top() + iconInset_);
        refuseButtonIconRect.setLeft(refuseRect.left() + iconInset_);
        refuseButtonIconRect.setSize(RingTheme::smallButtonIcon_);
        painter->drawPixmap(refuseButtonIconRect,
                            refusePixmap.scaled(RingTheme::smallButtonIcon_,
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
        painter->restore();
    }
}

bool
TransferItemDelegate::editorEvent(QEvent* event,
                                  QAbstractItemModel* model,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index)
{
    Q_UNUSED(model)

    if(event->type() != QEvent::MouseButtonPress &&  event->type() != QEvent::MouseButtonRelease) {
        acceptState_ = refuseState_ = cancelState_ = QStyle::State_Raised;
        return true;
    }

    auto isInProgress = !index.data(static_cast<int>(Ring::Role::IndexedLastUsed)).toInt();

    QFontMetrics textFontMetrics(option.font);

    auto fileName = index.data(static_cast<int>(FileTransferModel::Role::isWaitingAcceptance)).toBool();
    auto senderAccount = "SENDER ACCOUNT"; //transfer->contactMethod() ? transfer->contactMethod()->primaryName() : "Unknown" ;
    auto msg = isInProgress ? QString(tr("Downloading file : %1")).arg(fileName)
                            : QString(tr("%1 would like to send you the file : %2")).arg(senderAccount).arg(fileName);
    auto rectName = textFontMetrics.boundingRect(option.rect, Qt::TextWordWrap | Qt::AlignLeft, msg);
    auto acceptButtonRect = QRect(option.rect.left() + option.rect.width() / 2  - RingTheme::smallButton_.width(),
                                  option.rect.top() + option.rect.height() / 2 - RingTheme::smallButton_.height() / 2,
                                  RingTheme::smallButton_.width(),
                                  RingTheme::smallButton_.height());
    auto refuseButtonRect = QRect(acceptButtonRect.left() + acceptButtonRect.width() + margin_,
                                  acceptButtonRect.top(),
                                  RingTheme::smallButton_.width(),
                                  RingTheme::smallButton_.height());
    auto progressBarRect = QRect(option.rect.left(),
                                 option.rect.top() + rectName.height() + 2 * margin_,
                                 option.rect.width() - RingTheme::smallButton_.width() - margin_,
                                 RingTheme::smallButton_.height());
    auto cancelButtonRect = QRect(option.rect.left() + progressBarRect.width() + margin_,
                                  progressBarRect.top(),
                                  RingTheme::smallButton_.height(),
                                  RingTheme::smallButton_.width());

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    auto isAcceptButton = acceptButtonRect.contains(mouseEvent->pos());
    auto isRefuseButton = refuseButtonRect.contains(mouseEvent->pos());
    auto isCancelButton = cancelButtonRect.contains(mouseEvent->pos());
    if(!isAcceptButton && !isRefuseButton && !isCancelButton) {
        acceptState_ = refuseState_ = cancelState_ = QStyle::State_Raised;
        return true;
    }

    if(event->type() == QEvent::MouseButtonPress) {
        if (isAcceptButton && !isInProgress)
            acceptState_ = QStyle::State_Sunken;
        else if (isRefuseButton && !isInProgress)
            refuseState_ = QStyle::State_Sunken;
        else if (isCancelButton)
            cancelState_ = QStyle::State_Sunken;
    } else if(event->type() == QEvent::MouseButtonRelease) {
        if (isAcceptButton && !isInProgress) {
            acceptState_ = QStyle::State_Raised;
            FileTransferModel::instance().acceptTransferByModelIndex(index,
                               QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        }
        else if (isRefuseButton && !isInProgress) {
            refuseState_ = QStyle::State_Raised;
            FileTransferModel::instance().cancelTransferByModelIndex(index);
        } else if (isCancelButton && isInProgress) {
            cancelState_ = QStyle::State_Raised;
            FileTransferModel::instance().cancelTransferByModelIndex(index);
        }
    }
    return true;
}

QSize TransferItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)

    return QSize(option.rect.width(), 65);
}
