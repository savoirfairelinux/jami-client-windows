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
#include "recentmodel.h"
#include "contactmethod.h"
#include "person.h"

TransferDialog::TransferDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransferDialog),
    activeProxy_(nullptr),
    confMode_(false)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
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
    ui->contactView->setModel(RecentModel::instance()->peopleProxy());
    if (activeProxy_->rowCount() == 0)
        ui->contactButton->setChecked(true);
    else
        ui->currentCallButton->setChecked(true);
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

    auto c = CallModel::instance()->selectedCall();
    if (c != selectedCall_) {
        if (not confMode_)
            CallModel::instance()->attendedTransfer(c, selectedCall_);
        else
            CallModel::instance()->createConferenceFromCall(c, selectedCall_);
        this->close();
        return;
    }
}

void
TransferDialog::on_activeCallsView_clicked(const QModelIndex &index)
{
    selectedCall_ = CallModel::instance()->getCall(index);
}

void
TransferDialog::setConfMode(bool active)
{
    confMode_ = active;
    ui->transferButton->setVisible(not active);
    ui->numberBar->setVisible(not active);
}

void
TransferDialog::on_currentCallButton_toggled(bool checked)
{
    ui->stackedWidget->setCurrentIndex(checked ? 0 : 1);
}

void
TransferDialog::on_contactView_doubleClicked(const QModelIndex &index)
{
    auto realIndex = RecentModel::instance()->peopleProxy()->mapToSource(index);
    if (not RecentModel::instance()->hasActiveCall(realIndex)) {

        ContactMethod* m = nullptr;
        if (auto cm = realIndex.data(static_cast<int>(Call::Role::ContactMethod)).value<ContactMethod*>()) {
            m = cm;
        } else {
            if (auto person =  realIndex.data(static_cast<int>(Person::Role::Object)).value<Person*>()) {
                m = person->phoneNumbers().first();
            }
        }
        if (m && !RecentModel::instance()->index(0, 0, realIndex).isValid()) {
            Call* c = CallModel::instance()->dialingCall(m);
            auto callList = CallModel::instance()->getActiveCalls();
            for (auto current : callList) {
                if (current->state() == Call::State::CURRENT) {
                  c->setParentCall(current);
                }
            }
            c->performAction(Call::Action::ACCEPT);
        }
    }
    this->close();
}
