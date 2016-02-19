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

#include "transferfiledialog.h"
#include "ui_transferfiledialog.h"

//QT
#include <QTimer>

#include "transferitemdelegate.h"

//LRC
#include "filetransfermodel.h"

TransferFileDialog::TransferFileDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::TransferFileDialog),
    delegate_(nullptr),
    updateProgressBarTimer_(new QTimer())
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    delegate_ = new TransferItemDelegate(this);
    ui->transferItemList->setItemDelegate(delegate_);

    ui->transferItemList->setModel(&FileTransferModel::instance());

    //Don't look a this unless you want to puke
//    updateProgressBarTimer_->setInterval(100); //10FPS
//    connect(updateProgressBarTimer_, &QTimer::timeout, [=](){
//        ui->transferItemList->reset();
//    });
}

TransferFileDialog::~TransferFileDialog()
{
    updateProgressBarTimer_->stop();
    delete delegate_;
    delete updateProgressBarTimer_;
    delete ui;
}

void
TransferFileDialog::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)
    updateProgressBarTimer_->start();
}

void
TransferFileDialog::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event)
    updateProgressBarTimer_->stop();
}
