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

#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QTimer>
#include <QPushButton>

//LRC
#include "filetransfermodel.h"

#include <QDebug>

TransferItemDelegate::TransferItemDelegate(QObject* parent):
    QStyledItemDelegate(parent)
{
    acceptState_ =  QStyle::State_Enabled;
    refuseState_ =  QStyle::State_Enabled;
    cancelState_ =  QStyle::State_Enabled;
}

void TransferItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);

    if (opt.state & QStyle::State_HasFocus)
        opt.state ^= QStyle::State_HasFocus;

    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    //TODO: Hook that to index state
    auto isInProgress = false;

    auto fileName = index.data().toString();
    auto senderAccount = "SENDER ACCOUNT";
    auto msg = isInProgress ? QString(tr("Downloading file : %1")).arg(fileName)
                            : QString(tr("%1 would like to send you the file : %2")).arg(senderAccount).arg(fileName);

    QFontMetrics textFontMetrics(QApplication::font());
    auto rectName = textFontMetrics.boundingRect(opt.rect, Qt::TextWordWrap | Qt::AlignLeft, msg);
    painter->drawText(rectName, msg);

    if (isInProgress) {
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.state = QStyle::State_Enabled;
        progressBarOption.direction = QApplication::layoutDirection();
        progressBarOption.rect = QRect(opt.rect.left(),
                                       opt.rect.top() + rectName.height(),
                                       opt.rect.width() - 40,
                                       30);
        progressBarOption.fontMetrics = QApplication::fontMetrics();
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.textAlignment = Qt::AlignCenter;
        progressBarOption.textVisible = true;

        // Set the progress and text values of the style option.
        //TODO: Hook that to model
        int progress = 42;
        progressBarOption.progress = progress < 0 ? 0 : progress;
        progressBarOption.text = QString().sprintf("%d%%", progressBarOption.progress);

        // Draw the progress bar onto the view.
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);

        QStyleOptionButton cancelButtonOption;
        cancelButtonOption.state = cancelState_ | QStyle::State_Enabled;
        cancelButtonOption.direction = QApplication::layoutDirection();
        cancelButtonOption.fontMetrics = QApplication::fontMetrics();
        cancelButtonOption.icon = QIcon("://images/call/btn-ignore.svg");
        cancelButtonOption.iconSize = QSize(24, 24);
        cancelButtonOption.rect = QRect(opt.rect.left() + progressBarOption.rect.width(),
                                        progressBarOption.rect.top(),
                                        30,
                                        30);

        QApplication::style()->drawControl(QStyle::CE_PushButton, &cancelButtonOption, painter);
    } else {

        QStyleOptionButton acceptButtonOption;
        acceptButtonOption.state = acceptState_ | QStyle::State_Enabled;
        acceptButtonOption.direction = QApplication::layoutDirection();
        acceptButtonOption.fontMetrics = textFontMetrics;
        acceptButtonOption.icon = QIcon("://images/call/btn-call.svg");
        acceptButtonOption.iconSize = QSize(32, 32);
        acceptButtonOption.rect = QRect(opt.rect.left() + opt.rect.width() / 2 - 20,
                                        opt.rect.top() + opt.rect.height() / 2 - 20,
                                        40,
                                        40);

        QApplication::style()->drawControl(QStyle::CE_PushButton, &acceptButtonOption, painter);

        QStyleOptionButton refuseButtonOption;
        refuseButtonOption.state = refuseState_ | QStyle::State_Enabled;
        refuseButtonOption.direction = QApplication::layoutDirection();
        refuseButtonOption.fontMetrics = textFontMetrics;
        refuseButtonOption.icon = QIcon("://images/call/btn-ignore.svg");
        refuseButtonOption.iconSize = QSize(32, 32);
        refuseButtonOption.rect = QRect(acceptButtonOption.rect.left() + acceptButtonOption.rect.width(),
                                        acceptButtonOption.rect.top(),
                                        40,
                                        40);

        QApplication::style()->drawControl(QStyle::CE_PushButton, &refuseButtonOption, painter);
    }
}

bool TransferItemDelegate::editorEvent(QEvent* event,
                                       QAbstractItemModel* model,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index)
{
    Q_UNUSED(model)

    if(event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease) {
    } else {
        //ignoring other mouse event and reseting button's state
        acceptState_ = QStyle::State_Raised;
        return true;
    }

    auto isInProgress = false;

    QFontMetrics textFontMetrics(QApplication::font());

    auto fileName = index.data().toString();
    auto senderAccount = "SENDER ACCOUNT";
    auto msg = isInProgress ? QString(tr("Downloading file : %1")).arg(fileName)
                            : QString(tr("%1 would like to send you the file : %2")).arg(senderAccount).arg(fileName);
    auto rectName = textFontMetrics.boundingRect(option.rect, Qt::TextWordWrap | Qt::AlignLeft, msg);

    auto acceptButtonRect = QRect(option.rect.left() + option.rect.width() / 2  - 20,
                                  option.rect.top()+ option.rect.height() / 2 - 20,
                                  40,
                                  40);
    auto refuseButtonRect = QRect(acceptButtonRect.left()+ acceptButtonRect.width(),
                                  acceptButtonRect.top(),
                                  40,
                                  40);
    auto progressBarRect = QRect(option.rect.left(),
                                 option.rect.top() + rectName.height(),
                                 option.rect.width() - 40,
                                 30);
    auto cancelButtonRect = QRect(option.rect.left() + progressBarRect.width(),
                                  progressBarRect.top(),
                                  30,
                                  30);

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    auto isAcceptButton = acceptButtonRect.contains(mouseEvent->pos());
    auto isRefuseButton = refuseButtonRect.contains(mouseEvent->pos());
    auto isCancelButton = cancelButtonRect.contains(mouseEvent->pos());
    if(!isAcceptButton && !isRefuseButton && !isCancelButton) {
        acceptState_ = QStyle::State_Raised;
        refuseState_ = QStyle::State_Raised;
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
            //I dont think we ll need signal for that
            //emit acceptClicked(index);
        }
        else if (isRefuseButton && !isInProgress) {
            refuseState_ = QStyle::State_Raised;
            //emit cancelClicked(index);
        } else if (isCancelButton && isInProgress) {
            cancelState_ = QStyle::State_Raised;
        }
        qDebug() << "INDEX CLICKED : " << index;
    }
    return true;
}

QSize TransferItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)

    return QSize(option.rect.width(), 100);
}
