/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
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

#include "configurationwidget.h"
#include "ui_configurationwidget.h"

#include "video/devicemodel.h"
#include "video/channel.h"
#include "video/resolution.h"
#include "video/rate.h"
#include "video/previewmanager.h"

#include "accountserializationadapter.h"

#include "accountmodel.h"
#include "protocolmodel.h"
#include "accountdetails.h"
#include "callmodel.h"

#include "utils.h"

ConfigurationWidget::ConfigurationWidget(QWidget *parent) :
    NavWidget(Nav, parent),
    ui(new Ui::ConfigurationWidget),
    accountModel_(AccountModel::instance()),
    deviceModel_(Video::DeviceModel::instance()),
    accountDetails_(new AccountDetails())
{
    ui->setupUi(this);

    ui->accountView->setModel(accountModel_);
    accountStateDelegate_ = new AccountStateDelegate();
    ui->accountView->setItemDelegate(accountStateDelegate_);

    isLoading_ = true;
    ui->deviceBox->setModel(deviceModel_);
    connect(deviceModel_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(deviceIndexChanged(int)));

    connect(ui->accountView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(accountSelected(QItemSelection)));

    ui->accountView->setCurrentIndex(accountModel_->index(0));
    ui->accountDetailLayout->addWidget(accountDetails_);
    ui->accountTypeBox->setModel(accountModel_->protocolModel());
    ui->startupBox->setChecked(Utils::CheckStartupLink());
}

void ConfigurationWidget::atExit() {
    if (CallModel::instance()->getActiveCalls().size() == 0 ) {
        ui->videoView->hide();
        Video::PreviewManager::instance()->stopPreview();
    }
    accountModel_->save();
    accountDetails_->save();
}

ConfigurationWidget::~ConfigurationWidget()
{
    delete ui;
    delete accountStateDelegate_;
}

void
ConfigurationWidget::deviceIndexChanged(int index) {
    ui->deviceBox->setCurrentIndex(index);
}

void
ConfigurationWidget::on_deviceBox_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    if (!isLoading_)
        deviceModel_->setActive(index);

    auto device = deviceModel_->activeDevice();

    ui->sizeBox->clear();

    isLoading_ = true;
    if (device->channelList().size() > 0) {
        for (auto resolution : device->channelList()[0]->validResolutions()) {
            ui->sizeBox->addItem(resolution->name());
        }
    }
    ui->sizeBox->setCurrentIndex(
                device->channelList()[0]->activeResolution()->relativeIndex());
    isLoading_ = false;
}

void
ConfigurationWidget::on_sizeBox_currentIndexChanged(int index)
{
    auto device = deviceModel_->activeDevice();

    ui->rateBox->clear();

    if (index < 0)
        return;
    if (!isLoading_)
        device->channelList()[0]->setActiveResolution(
                    device->channelList()[0]->validResolutions()[index]);

    isLoading_ = true;
    for (auto rate
         : device->channelList()[0]->validResolutions()[index]->validRates()) {
        ui->rateBox->addItem(rate->name());
    }
    ui->rateBox->setCurrentIndex(
                device->channelList()[0]->
            activeResolution()->activeRate()->relativeIndex());
    isLoading_ = false;
}

void
ConfigurationWidget::on_rateBox_currentIndexChanged(int index)
{
    if (index < 0 || isLoading_)
        return;
    auto device = deviceModel_->activeDevice();
    device->channelList()[0]->activeResolution()->setActiveRate(index);
}

void
ConfigurationWidget::accountSelected(QItemSelection itemSel) {

    Q_UNUSED(itemSel)
    auto account = accountModel_->getAccountByModelIndex(
                ui->accountView->currentIndex());
    accountDetails_->setAccount(account);
    AccountSerializationAdapter adapter(account, accountDetails_);
}

void
ConfigurationWidget::on_deleteAccountButton_clicked()
{
    auto account = accountModel_->getAccountByModelIndex(
                ui->accountView->currentIndex());
    if (account != accountModel_->ip2ip())
        accountModel_->remove(account);
}

void
ConfigurationWidget::on_addAccountButton_clicked()
{
    auto account = accountModel_->add("New Account",
                                      ui->accountTypeBox->model()->index(
                                          ui->accountTypeBox->currentIndex(), 0));
    account->setRingtonePath(Utils::GetRingtonePath());
    accountModel_->save();
}

void
ConfigurationWidget::on_startupBox_toggled(bool checked)
{
    if (checked)
        Utils::CreateStartupLink();
    else
        Utils::DeleteStartupLink();
}

void
ConfigurationWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (CallModel::instance()->getActiveCalls().size() == 0 ) {
        ui->videoView->show();
        Video::PreviewManager::instance()->startPreview();
    }
}
