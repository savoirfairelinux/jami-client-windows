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

#include <QTimer>

#include "transferitemdelegate.h"

#include "recentmodel.h"

TransferFileDialog::TransferFileDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::TransferFileDialog),
    delegate_(nullptr)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flags);
    setAttribute(Qt::WA_DeleteOnClose, true);

    delegate_ = new TransferItemDelegate(this);
    ui->transferItemList->setItemDelegate(delegate_);

    //TODO: THIS IS FOR DEBUG PURPOSES
    ui->transferItemList->setModel(&RecentModel::instance());

}

TransferFileDialog::~TransferFileDialog()
{
    delete delegate_;
    delete ui;
}

void TransferFileDialog::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)
    //TODO Set the label
    //ui->fileNameLabel->setText(QString("%1").arg("NAME OF FILE"));
    //ui->queryLabel->setText(QString("%1 wants to send you a file : %2").arg("SOMEONE").arg("NAME OF THE FILE"));
}
