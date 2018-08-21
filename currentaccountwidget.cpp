/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include "currentaccountwidget.h"
#include "ui_currentaccountwidget.h"

#include "globalinstances.h"
#include "availableaccountmodel.h"
#include "account.h"
#include "qstandarditemmodel.h"
#include "profilemodel.h"
#include "profile.h"
#include "person.h"

#include "utils.h"
#include "lrcinstance.h"
#include "pixbufmanipulator.h"

CurrentAccountWidget::CurrentAccountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentAccountWidget)
{
    ui->setupUi(this);

    // select last used(first in list) account
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (!accountList.empty()) {
        LRCInstance::setSelectedAccountId(accountList.at(0));
    }

    accountListModel_ = std::make_unique<AccountListModel>();
    ui->currentAccountSelector->setModel(accountListModel_.get());
    accountItemDelegate_ = new AccountItemDelegate();
    ui->currentAccountSelector->setItemDelegate(accountItemDelegate_);
    //connect(ui->currentAccountSelector, SIGNAL(currentIndexChanged(const QString&)),
    //    [this](const QModelIndex& current) {
    //        qDebug() << "CurrentAccountWidget::accountSelectionChanged";
    //        //this->selectedAccountChanged(current, QModelIndex());
    //    });

    ui->accountsStatus->setText("No enabled account: impossible to communicate!");
    ui->accountsStatus->hide();
    updateAccounts();
    if (ui->currentAccountSelector->count() > 0) {
        ui->currentAccountSelector->setCurrentIndex(0);
        qDebug() << "CurrentAccount : setup over";
    }
    else {
        qDebug() << "CurrentAccount : No account available";
    }
}

CurrentAccountWidget::~CurrentAccountWidget()
{
    delete ui;
}

void
CurrentAccountWidget::update()
{
    updateAccounts();
}

void
CurrentAccountWidget::updateAccounts()
{
    auto selector = ui->currentAccountSelector;

    if (selector->count() <= 1){
        selector->hide();
        if (selector->count() < 1) {
            ui->accountsStatus->show();
            setPhoto();
        } else {
            ui->accountsStatus->hide();
        }
    } else {
        selector->show();
        ui->accountsStatus->hide();
    }
}

void
CurrentAccountWidget::setPhoto()
{
    auto accountId = LRCInstance::getSelectedAccountId();
    try {
        auto& accountInfo = LRCInstance::accountModel().getAccountInfo(accountId);

        if (ui->currentAccountSelector->count() > 0) {
            QVariant avatarImage = PixbufManipulator::accountPhoto(accountInfo);
            QImage image = Utils::getCirclePhoto(avatarImage.value<QImage>(), ui->idDisplayLayout->contentsRect().height());
            ui->currentAccountPixmap->setPixmap(QPixmap::fromImage(image));
        }
        else {
            qDebug() << "CurrentAccount : account not set";
            ui->currentAccountPixmap->setPixmap(QPixmap());
        }
    }
    catch (...) {}
}

void
CurrentAccountWidget::on_currentAccountSelector_currentIndexChanged(int index)
{
    QModelIndex idx = ui->currentAccountSelector->model()->index(index, 0);
    emit accountSelectionChanged(idx);
    setPhoto();
}

void
CurrentAccountWidget::changeSelectedIndex(int index)
{
    if (index != ui->currentAccountSelector->currentIndex()) {
        ui->currentAccountSelector->setCurrentIndex(index);
    }
}

