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

#include "clientaccountmodel.h"

#include <QPaintEvent>


CurrentAccountWidget::CurrentAccountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentAccountWidget)
{
    ui->setupUi(this);
}

CurrentAccountWidget::~CurrentAccountWidget()
{
    delete ui;
}

void
CurrentAccountWidget::setup(ClientAccountModel* accountModel)
{
    ui->accountsStatus->setText("No enabled account: impossible to communicate!");
    ui->accountsStatus->hide();
    accountModel_ = accountModel; // must be set before setmodel()
    ui->currentAccountSelector->setModel(accountModel);
    updateAccounts();

    if (ui->currentAccountSelector->count() > 0) {
        ui->currentAccountSelector->setCurrentIndex(0);
        qDebug() << "CurrentAccount : setup over";
    } else {
        qDebug() << "CurrentAccount : No account available";
    }
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
    auto selector = ui->currentAccountSelector;
    if (selector->count() > 0) {
        if (accountModel_) {
            QImage img = accountModel_->selectedAccountIndex().data(ClientAccountModel::Role::Avatar).value<QImage>();
            img = Utils::getCirclePhoto(img, ui->idDisplayLayout->contentsRect().height());
            photo_ = QPixmap::fromImage(img);
            ui->currentAccountPixmap->setPixmap(photo_);
            qDebug() << "CurrentAccount : Photo set";
        } else
            qDebug() << "no account model";
    } else {
        qDebug() << "Current account widget : no account available";
        ui->currentAccountPixmap->setPixmap(QPixmap());
    }
}

void
CurrentAccountWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    setPhoto();
}

void
CurrentAccountWidget::paintEvent(QPaintEvent *e)
{
    if (photo_.isNull()) {
        setPhoto();
    }
    QWidget::paintEvent(e);
}

void
CurrentAccountWidget::on_currentAccountSelector_currentIndexChanged(int index)
{
    QModelIndex idx = ui->currentAccountSelector->model()->index(index,0);
    accountModel_->setSelectedAccount(idx);
    setPhoto();
}

void
CurrentAccountWidget::changeSelectedIndex(int index)
{
    if (index != ui->currentAccountSelector->currentIndex()) {
        ui->currentAccountSelector->setCurrentIndex(index);
    }
}

