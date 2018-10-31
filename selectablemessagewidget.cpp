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


#include "messagemodel.h"

SelectableMessageWidget::SelectableMessageWidget(QModelIndex index, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SelectableMessageWidget),
    index_(index)
{
    parent_ = parent;
    ui->setupUi(this);
    QString msg = index.data(static_cast<int>(MessageModel::Role::Body)).toString();
    ui->msg->setHtml(QString("<p>%1</p>").arg(msg));
    ui->msg->setStyleSheet("border: none; background-color: transparent;color: '#0f0f0f'; opacity: 100; font-size : 16px; text-align : left;");
}

SelectableMessageWidget::~SelectableMessageWidget()
{
    delete ui;
}

void
SelectableMessageWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHints((QPainter::Antialiasing | QPainter::TextAntialiasing), true);
    QStyleOption opt;
    opt.init(this);
    ui->msg->setGeometry(parent_->frameGeometry());
}