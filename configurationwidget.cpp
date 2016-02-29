/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                                *
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

#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>
#include <QPropertyAnimation>

#include "video/devicemodel.h"
#include "video/channel.h"
#include "video/resolution.h"
#include "video/rate.h"
#include "video/previewmanager.h"

#include "media/recordingmodel.h"

#include "accountserializationadapter.h"
#include "accountstatedelegate.h"
#include "settingskey.h"
#include "utils.h"

#include "accountmodel.h"
#include "protocolmodel.h"
#include "accountdetails.h"
#include "callmodel.h"
#include "ringtonemodel.h"
#include "categorizedhistorymodel.h"

#ifdef ENABLE_AUTOUPDATE
#include "winsparkle.h"
#endif

ConfigurationWidget::ConfigurationWidget(QWidget *parent) :
    NavWidget(parent),
    ui(new Ui::ConfigurationWidget),
    accountModel_(&AccountModel::instance()),
    deviceModel_(&Video::DeviceModel::instance()),
    accountDetails_(new AccountDetails())
{
    ui->setupUi(this);

    connect(ui->exitSettingsButton, &QPushButton::clicked, this, [=](){
        if (CallModel::instance().getActiveCalls().size() == 0
                && Video::PreviewManager::instance().isPreviewing()) {
            Video::PreviewManager::instance().stopPreview();
        }
        accountModel_->save();
        accountDetails_->save();
    });

    connect(accountDetails_->getDeleteAccountButton(), &QPushButton::clicked, this, [=](){
        auto account = accountModel_->getAccountByModelIndex(
                    ui->accountView->currentIndex());
        if (account != accountModel_->ip2ip())
            accountModel_->remove(account);
    });

    connect(ui->exitSettingsButton, &QPushButton::clicked, this, [=](){
        emit NavigationRequested(ScreenEnum::CallScreen);
    });

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

    ui->historyDaySettingsSpinBox->setValue(
                CategorizedHistoryModel::instance().historyLimit());
    ui->closeOrMinCheckBox->setChecked(settings_.value(
                                           SettingsKey::closeOrMinimized).toBool());
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, [](int index) {
        if (index == 1
                && CallModel::instance().getActiveCalls().size() == 0) {
            Video::PreviewManager::instance().startPreview();
        } else {
            if (CallModel::instance().getActiveCalls().size() == 0
                    && Video::PreviewManager::instance().isPreviewing()) {
                Video::PreviewManager::instance().stopPreview();
            }
        }
    });

    ui->videoView->setIsFullPreview(true);

    auto recordPath = Media::RecordingModel::instance().recordPath();
    if (recordPath.isEmpty()) {
        recordPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        Media::RecordingModel::instance().setRecordPath(recordPath);
    }
    ui->recordPath->setText(Media::RecordingModel::instance().recordPath());

    ui->alwaysRecordCheckBox->setChecked(Media::RecordingModel::instance().isAlwaysRecording());
    connect(ui->alwaysRecordCheckBox, &QCheckBox::clicked, [](bool checked){
        Media::RecordingModel::instance().setAlwaysRecording(checked);
    });

    connect(ui->generalTabButton, &QPushButton::toggled, [=] (bool toggled) {
        if(toggled)
        {
            ui->stackedWidget->setCurrentWidget(ui->generalPage);
            ui->videoTabButton->setChecked(false);
            ui->accountTabButton->setChecked(false);
        }
    });

    connect(ui->videoTabButton, &QPushButton::toggled, [=] (bool toggled) {
        if(toggled)
        {
            ui->stackedWidget->setCurrentWidget(ui->videoPage);
            ui->accountTabButton->setChecked(false);
            ui->generalTabButton->setChecked(false);
        }
    });

    connect(ui->accountTabButton, &QPushButton::toggled, [=] (bool toggled) {
        if(toggled)
        {
            ui->stackedWidget->setCurrentWidget(ui->accountPage);
            ui->videoTabButton->setChecked(false);
            ui->generalTabButton->setChecked(false);
        }
    });

    ui->generalTabButton->setChecked(true);

#ifndef ENABLE_AUTOUPDATE
    ui->checkUpdateButton->hide();
    ui->intervalUpdateCheckSpinBox->hide();
    ui->updateDayLabel->hide();
    ui->autoUpdateCheckBox->hide();
#endif
}

void ConfigurationWidget::showPreview()
{
    if (ui->stackedWidget->currentIndex() == 1
            && CallModel::instance().getActiveCalls().size() == 0) {
        ui->previewUnavailable->hide();
        ui->videoView->show();
        Video::PreviewManager::instance().startPreview();
    } else {
        ui->previewUnavailable->show();
        ui->videoView->hide();
    }
}

void
ConfigurationWidget::showEvent(QShowEvent *event) {

#ifdef ENABLE_AUTOUPDATE
    if (win_sparkle_get_automatic_check_for_updates()) {
        ui->autoUpdateCheckBox->setChecked(true);
    }
    ui->intervalUpdateCheckSpinBox->setValue(win_sparkle_get_update_check_interval() / 86400);
#endif
    QWidget::showEvent(event);
    showPreview();
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

    if (index < 0)
        return;
    if (!isLoading_)
        device->channelList()[0]->setActiveResolution(
                    device->channelList()[0]->validResolutions()[index]);
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
ConfigurationWidget::on_addAccountButton_clicked()
{
    auto account = accountModel_->add(tr("New Account"),
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
ConfigurationWidget::on_clearHistoryButton_clicked()
{
    QMessageBox confirmationDialog;

    confirmationDialog.setText(tr("Are you sure you want to clear all your history?"));
    confirmationDialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    auto ret = confirmationDialog.exec();

    if (ret == QMessageBox::Ok)
        CategorizedHistoryModel::instance().clearAllCollections();
}

void
ConfigurationWidget::on_historyDaySettingsSpinBox_valueChanged(int limit)
{
    if (CategorizedHistoryModel::instance().historyLimit() != limit)
        CategorizedHistoryModel::instance().setHistoryLimit(limit);
}

void
ConfigurationWidget::on_closeOrMinCheckBox_toggled(bool checked)
{
    settings_.setValue(SettingsKey::closeOrMinimized, checked);
}

void
ConfigurationWidget::on_checkUpdateButton_clicked()
{
#ifdef ENABLE_AUTOUPDATE
    win_sparkle_check_update_with_ui();
#endif
}

void
ConfigurationWidget::on_autoUpdateCheckBox_toggled(bool checked)
{
#ifdef ENABLE_AUTOUPDATE
    win_sparkle_set_automatic_check_for_updates(checked);
#endif
}

void
ConfigurationWidget::on_intervalUpdateCheckSpinBox_valueChanged(int arg1)
{
#ifdef ENABLE_AUTOUPDATE
    win_sparkle_set_update_check_interval(arg1 * 86400);
#endif
}

void
ConfigurationWidget::on_stackedWidget_currentChanged(int index)
{
    Q_UNUSED(index)
    showPreview();
}

void
ConfigurationWidget::on_recordPath_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
                                                 Media::RecordingModel::instance().recordPath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (not dir.isEmpty()) {
        Media::RecordingModel::instance().setRecordPath(dir);
        ui->recordPath->setText(dir);
    }
}
