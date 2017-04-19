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


CurrentAccountWidget::CurrentAccountWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentAccountWidget)
{
    ui->setupUi(this);
    setup();
}

CurrentAccountWidget::~CurrentAccountWidget()
{
    delete ui;
}

void
CurrentAccountWidget::setup()
{
    ui->accountsStatus->setText("No enabled account: impossible to communicate!");
    ui->accountsStatus->hide();
    ui->currentAccountSelector->setModel(&AvailableAccountModel::instance());
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
        if (ProfileModel::instance().selectedProfile()) {
            if (ProfileModel::instance().selectedProfile()->person()){
                QImage img = Utils::getCirclePhoto(ProfileModel::instance().selectedProfile()->person()->photo().value<QImage>(),
                                                   ui->idDisplayLayout->contentsRect().height());
                ui->currentAccountPixmap->setPixmap(QPixmap::fromImage(img));
                qDebug() << "CurrentAccount : Photo set";
            } else
                qDebug() << "CurrentAccount : selected profile has no person";
        } else
            qDebug() << "CurrentAccount : Profilemodel: no selected profile";
    } else {
        qDebug() << "CurrentAccount : account not set";
        ui->currentAccountPixmap->setPixmap(QPixmap());
    }
}

void
CurrentAccountWidget::on_currentAccountSelector_currentIndexChanged(int index)
{
    QModelIndex idx = ui->currentAccountSelector->model()->index(index,0);
    Account* ac = AccountModel::instance().getAccountByModelIndex(idx);

    if (ac) {
        AvailableAccountModel::instance().selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
        setPhoto();
    } else {
        qDebug() << "CurrentAccount : account not referenced correctly";
        //null for now
    }
}

void
CurrentAccountWidget::changeSelectedIndex(int index)
{
    if (index != ui->currentAccountSelector->currentIndex()) {
        ui->currentAccountSelector->setCurrentIndex(index);
    }
}

