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

    accountListModel_ = std::make_unique<AccountListModel>();
    ui->currentAccountSelector->setModel(accountListModel_.get());
    accountItemDelegate_ = new AccountItemDelegate();
    ui->currentAccountSelector->setItemDelegate(accountItemDelegate_);

    connect(&LRCInstance::accountModel(),
        &lrc::api::NewAccountModel::accountAdded,
        [this](const std::string& accountId) {
            auto accountList = LRCInstance::accountModel().getAccountList();
            auto it = std::find(accountList.begin(), accountList.end(), accountId);
            if (it != accountList.end()) {
                auto index = std::distance(accountList.begin(), it);
                // index should be 0
                QModelIndex idx = ui->currentAccountSelector->model()->index(index, 0);
                emit currentAccountChanged(idx);
                update();
            }
        });
}

CurrentAccountWidget::~CurrentAccountWidget()
{
    delete ui;
}

void
CurrentAccountWidget::update()
{
    if (ui->currentAccountSelector->count() <= 1) {
        ui->currentAccountSelector->setEnabled(false);
        ui->currentAccountSelector->setStyleSheet(
            "QComboBox:disabled{color: black;}"
            "QComboBox{border:0;}"
            "QComboBox::down-arrow{image:url('none');}"
        );
    } else {
        ui->currentAccountSelector->setEnabled(true);
        ui->currentAccountSelector->setStyleSheet("");
    }
    setPhoto();
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
    emit currentAccountChanged(idx);
    setPhoto();
}

void
CurrentAccountWidget::changeSelectedIndex(int index)
{
    if (index != ui->currentAccountSelector->currentIndex()) {
        ui->currentAccountSelector->setCurrentIndex(index);
    }
}

