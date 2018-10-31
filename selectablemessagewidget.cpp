/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
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

#include "ui_selectablemessagewidget.h"

#include "selectablemessagewidget.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QEvent>

#include "messagemodel.h"
#include "ringthemeutils.h"
#include "utils.h"

SelectableMessageWidget::SelectableMessageWidget(QModelIndex index, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SelectableMessageWidget),
    index_(index)
{
    parent_ = parent;
    ui->setupUi(this);
    QString msg_ = index
        .data(static_cast<int>(MessageModel::Role::Body))
        .value<QString>();
    ui->msg->setHtml(msg_);
    ui->msg->setStyleSheet("border: none; background-color: transparent; color: '#0f0f0f'; opacity: 100; font-size : 16px; text-align : left;");
    connect(ui->msg, SIGNAL(startDrag()), this, SLOT(slotStartDrag()));
}

SelectableMessageWidget::~SelectableMessageWidget()
{
    delete ui;
}

bool SelectableMessageWidget::isFreeable()
{
    return isFreeable_;
}

void SelectableMessageWidget::makeFreeable()
{
    isFreeable_ = true;
}

void SelectableMessageWidget::leaveEvent(QEvent * event)
{
    emit exited();
}

void SelectableMessageWidget::enterEvent(QEvent * event)
{
    emit entered();
}

void SelectableMessageWidget::slotStartDrag()
{
    isFreeable_ = false;
}

void
SelectableMessageWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHints((QPainter::Antialiasing | QPainter::TextAntialiasing), true);
    QStyleOptionViewItem opt;
    opt.init(this);
    auto isOutgoing = index_
        .data(static_cast<int>(MessageModel::Role::Direction))
        .value<bool>();
    QRect cellRect;
    if (isOutgoing) {
        QTextDocument  document;
        document.setDefaultStyleSheet(RingTheme::defaultMsgStylesheet_);
        document.setHtml(msg_);
        auto textOptions = QTextOption(Qt::AlignLeft);
        textOptions.setWrapMode(QTextOption::WrapMode::WrapAtWordBoundaryOrAnywhere);
        document.setDefaultTextOption(textOptions);
        cellRect = Utils::getBoundingRect(Qt::AlignRight, opt, document);
        //qDebug() << "cellRect:" << cellRect;
    } else {
        cellRect = QRect(parent_->frameGeometry());
    }
    ui->msg->setGeometry(cellRect);
}
