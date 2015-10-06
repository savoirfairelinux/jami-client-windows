/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
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

#include "transferdialog.h"
#include "ui_transferdialog.h"

#include "callmodel.h"
#include "phonedirectorymodel.h"

TransferDialog::TransferDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransferDialog),
    activeProxy_(nullptr)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

TransferDialog::~TransferDialog()
{
    delete ui;
}

void
TransferDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    ui->numberBar->clear();
    selectedCall_ = nullptr;
    if (not activeProxy_) {
        activeProxy_ = new ActiveCallsProxyModel(CallModel::instance());
        activeProxy_->setDynamicSortFilter(false);
    }
    ui->activeCallsView->setModel(activeProxy_);
    ui->activeCallsView->clearSelection();
}

void
TransferDialog::on_transferButton_clicked()
{
    removeProxyModel();

    auto callList = CallModel::instance()->getActiveCalls();
    for (auto c : callList) {
        if (c->state() == Call::State::CURRENT) {
            if (not ui->numberBar->text().isEmpty()) {
                auto number = PhoneDirectoryModel::instance()->getNumber(ui->numberBar->text());
                CallModel::instance()->transfer(c, number);
            } else if (selectedCall_) {
                CallModel::instance()->attendedTransfer(c, selectedCall_);
            }
            this->close();
            return;
        }
    }
}

void
TransferDialog::removeProxyModel()
{
    //This prevent a crash happening in Qt5.5 in QSortFilterProxyModel
    ui->activeCallsView->setModel(nullptr);
}

void
TransferDialog::on_activeCallsView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    removeProxyModel();

    auto callList = CallModel::instance()->getActiveCalls();
    for (auto c : callList) {
        if (c->state() == Call::State::CURRENT) {
            auto selectedCall = CallModel::instance()->getCall(index);
            if (c != selectedCall) {
                CallModel::instance()->attendedTransfer(c, selectedCall);
                this->close();
                return;
            }
        }
    }
}

void
TransferDialog::on_activeCallsView_clicked(const QModelIndex &index)
{
    selectedCall_ = CallModel::instance()->getCall(index);
}
