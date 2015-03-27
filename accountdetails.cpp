/***************************************************************************
 * Copyright (C) 2011-2015 by Savoir-Faire Linux                           *
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

#include "accountdetails.h"
#include "ui_accountdetails.h"

#include <QSortFilterProxyModel>

#include "accountmodel.h"
#include "account.h"
#include "accountdetails.h"
#include "audio/codecmodel.h"

AccountDetails::AccountDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountDetails),
    codecModel_(nullptr),
    codecModelModified(false)
{
    ui->setupUi(this);

    ui->audioCodecView->setColumnCount(4);
    QStringList audioHeader {"Enabled", "Name", "Bitrate", "Samplerate"};
    ui->audioCodecView->setHorizontalHeaderLabels(audioHeader);
    ui->audioCodecView->verticalHeader()->hide();
    ui->audioCodecView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->audioCodecView->setShowGrid(false);

    ui->videoCodecView->setColumnCount(3);
    QStringList videoHeader {"Enabled", "Name", "Bitrate"};
    ui->videoCodecView->setHorizontalHeaderLabels(videoHeader);
    ui->videoCodecView->verticalHeader()->hide();
    ui->videoCodecView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->videoCodecView->setShowGrid(false);

    ui->hashValue->setAlignment(Qt::AlignCenter);

    connect(ui->audioCodecView, SIGNAL(cellChanged(int,int)),
            this, SLOT(audio_codec_checked(int, int)));
    connect(ui->videoCodecView, SIGNAL(cellChanged(int,int)),
            this, SLOT(video_codec_checked(int,int)));
    connect(ui->videoEnabledCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(setVideoEnabled(bool)));
    connect(ui->autoAnswerCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(setAutoAnswer(bool)));
    connect(ui->aliasValue, SIGNAL(editingFinished()),
            this, SLOT(aliasChanged()));
    connect(ui->bootstrapLineEdit, SIGNAL(editingFinished()),
            this, SLOT(bootstrapChanged()));
}

AccountDetails::~AccountDetails()
{
    delete ui;
}

void
AccountDetails::reloadCodec(CodecType type)
{
    auto selectedIdx = codecModel_->selectionModel()->currentIndex();

    if (type != CodecType::VIDEO) {
        ui->audioCodecView->clearContents();
        ui->audioCodecView->setRowCount(codecModel_->audioCodecs()->rowCount());

        for (int i = 0; i < codecModel_->audioCodecs()->rowCount(); i++) {

            auto idx = codecModel_->audioCodecs()->index(i, 0);

            auto checkBoxItem = new QTableWidgetItem();
            checkBoxItem->setCheckState(
                        codecModel_->audioCodecs()->data(idx ,
                                                         Qt::CheckStateRole).toBool()
                        ? Qt::Checked : Qt::Unchecked);
            ui->audioCodecView->setItem(i, 0, checkBoxItem);

            auto item = new QTableWidgetItem(
                        codecModel_->audioCodecs()->data(idx).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->audioCodecView->setItem(i, 1, item);

            item = new QTableWidgetItem(
                        codecModel_->audioCodecs()->
                        data(idx,CodecModel::Role::BITRATE).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->audioCodecView->setItem(i, 2, item);

            item = new QTableWidgetItem(
                        codecModel_->audioCodecs()->
                        data(idx, CodecModel::Role::SAMPLERATE).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->audioCodecView->setItem(i, 3, item);

            if (codecModel_->audioCodecs()->mapToSource(idx) == selectedIdx) {
                ui->audioCodecView->setCurrentCell(i, 0);
            }
        }
    }
    if (type != CodecType::AUDIO) {
        ui->videoCodecView->clearContents();
        ui->videoCodecView->setRowCount(codecModel_->videoCodecs()->rowCount());
        for (int i = 0; i < codecModel_->videoCodecs()->rowCount(); i++) {

            auto idx = codecModel_->videoCodecs()->index(i, 0);

            auto checkBoxItem = new QTableWidgetItem();
            checkBoxItem->setCheckState(codecModel_->videoCodecs()->
                                        data(idx , Qt::CheckStateRole).toBool()
                                        ? Qt::Checked : Qt::Unchecked);
            ui->videoCodecView->setItem(i, 0, checkBoxItem);

            auto item = new QTableWidgetItem(
                        codecModel_->videoCodecs()->data(idx).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->videoCodecView->setItem(i, 1, item);

            item = new QTableWidgetItem(codecModel_->videoCodecs()->
                                        data(idx,CodecModel::Role::BITRATE).toString());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->videoCodecView->setItem(i, 2, item);

            if (codecModel_->videoCodecs()->mapToSource(idx) == selectedIdx) {
                ui->videoCodecView->setCurrentCell(i, 0);
            }
        }
    }
}

void
AccountDetails::setAccount(Account* currentAccount) {

    currentAccount_ = currentAccount;

    if (codecModel_ && codecModelModified)
        codecModel_->save();

    codecModel_ = currentAccount->codecModel();

    //FIX ME : Reorganize this in group
    ui->hashValue->hide();
    ui->hashLabel->hide();
    ui->aliasValue->hide();
    ui->aliasLabel->hide();
    ui->bootstrapLabel->hide();
    ui->bootstrapLineEdit->hide();
    ui->hostnameLabel->hide();
    ui->hostnameEdit->hide();
    ui->usernameEdit->hide();
    ui->usernameLabel->hide();
    ui->passwordEdit->hide();
    ui->passwordLabel->hide();
    ui->proxyEdit->hide();
    ui->proxyLabel->hide();
    ui->voicemailLabel->hide();
    ui->voicemailEdit->hide();

    switch (currentAccount_->protocol()) {
    case Account::Protocol::SIP:
        if (currentAccount_ != AccountModel::instance()->ip2ip()) {
            ui->typeValueLabel->setText("SIP");
            ui->aliasValue->setText(currentAccount_->alias());
            ui->hostnameEdit->setText(currentAccount_->hostname());
            ui->usernameEdit->setText(currentAccount_->username());
            ui->passwordEdit->setText(currentAccount_->password());
            ui->proxyEdit->setText(currentAccount_->proxy());
            ui->voicemailEdit->setText(currentAccount_->mailbox());

            ui->aliasLabel->show();
            ui->aliasValue->show();
            ui->hostnameLabel->show();
            ui->hostnameEdit->show();
            ui->usernameEdit->show();
            ui->usernameLabel->show();
            ui->passwordEdit->show();
            ui->passwordLabel->show();
            ui->proxyEdit->show();
            ui->proxyLabel->show();
            ui->voicemailLabel->show();
            ui->voicemailEdit->show();
        } else {
            ui->typeValueLabel->setText("IP2IP");
        }
        break;
    case Account::Protocol::IAX:
        ui->typeValueLabel->setText("IAX");
        break;
    case Account::Protocol::RING:
        ui->typeValueLabel->setText("RING");
        ui->aliasValue->setText(currentAccount_->alias());
        ui->hashValue->setText(currentAccount_->username());
        ui->bootstrapLineEdit->setText(currentAccount_->hostname());
        ui->hashValue->show();
        ui->hashLabel->show();
        ui->aliasLabel->show();
        ui->aliasValue->show();
        ui->bootstrapLabel->show();
        ui->bootstrapLineEdit->show();
        break;
    default:
        break;
    }

    reloadCodec();

    ui->videoEnabledCheckBox->setChecked(currentAccount_->isVideoEnabled());
    ui->autoAnswerCheckBox->setChecked(currentAccount_->isAutoAnswer());
}

void
AccountDetails::audio_codec_checked(int row, int column) {
    if (column != 0)
        return;
    auto item = ui->audioCodecView->item(row, 0);
    auto idx = codecModel_->audioCodecs()->index(row, 0);
    codecModel_->audioCodecs()->setData(idx, item->checkState(),
                                        Qt::CheckStateRole);
    codecModelModified = true;
}

void
AccountDetails::video_codec_checked(int row, int column) {
    if (column != 0)
        return;
    auto item = ui->videoCodecView->item(row, 0);
    auto idx = codecModel_->videoCodecs()->index(row, 0);
    codecModel_->videoCodecs()->setData(idx, item->checkState(),
                                        Qt::CheckStateRole);
    codecModelModified = true;
}

void
AccountDetails::setVideoEnabled(bool enabled) {
    currentAccount_->setVideoEnabled(enabled);
}

void
AccountDetails::setAutoAnswer(bool enabled) {
    currentAccount_->setAutoAnswer(enabled);
}

void
AccountDetails::aliasChanged() {
    currentAccount_->setAlias(ui->aliasValue->text());
}

void
AccountDetails::bootstrapChanged() {
    currentAccount_->setHostname(ui->bootstrapLineEdit->text());
}

void
AccountDetails::on_upAudioButton_clicked()
{
    codecModel_->moveUp();
    reloadCodec(CodecType::AUDIO);
    codecModelModified = true;
}

void
AccountDetails::on_downAudioButton_clicked()
{
    codecModel_->moveDown();
    reloadCodec(CodecType::AUDIO);
    codecModelModified = true;
}

void
AccountDetails::on_upVideoButton_clicked()
{
    codecModel_->moveUp();
    reloadCodec(CodecType::VIDEO);
    codecModelModified = true;
}

void
AccountDetails::on_downVideoButton_clicked()
{
    codecModel_->moveDown();
    reloadCodec(CodecType::VIDEO);
    codecModelModified = true;
}

void
AccountDetails::on_audioCodecView_itemSelectionChanged()
{
    int row = ui->audioCodecView->currentRow();
    auto idx = codecModel_->audioCodecs()->index(row, 0);
    auto srcIdx = codecModel_->audioCodecs()->mapToSource(idx);
    codecModel_->selectionModel()->setCurrentIndex(srcIdx,
                                                   QItemSelectionModel::Select);
}

void
AccountDetails::on_videoCodecView_itemSelectionChanged()
{
    int row = ui->videoCodecView->currentRow();
    auto idx = codecModel_->videoCodecs()->index(row, 0);
    auto srcIdx = codecModel_->videoCodecs()->mapToSource(idx);
    codecModel_->selectionModel()->setCurrentIndex(srcIdx,
                                                   QItemSelectionModel::Select);
}

void
AccountDetails::save() {
    if (codecModelModified)
        codecModel_->save();
}

void
AccountDetails::on_hostnameEdit_editingFinished()
{
    currentAccount_->setHostname(ui->hostnameEdit->text());
}

void
AccountDetails::on_usernameEdit_editingFinished()
{
    currentAccount_->setUsername(ui->usernameEdit->text());
}

void
AccountDetails::on_passwordEdit_editingFinished()
{
    currentAccount_->setPassword(ui->passwordEdit->text());
}

void
AccountDetails::on_proxyEdit_editingFinished()
{
    currentAccount_->setProxy(ui->proxyEdit->text());
}

void
AccountDetails::on_voicemailEdit_editingFinished()
{
    currentAccount_->setMailbox(ui->voicemailEdit->text());
}

void
AccountDetails::on_tabWidget_currentChanged(int index)
{
    if (index == 1) {
        ui->audioCodecView->setCurrentItem(ui->audioCodecView->item(0, 0));
    } else if (index == 2) {
        ui->videoCodecView->setCurrentItem(ui->videoCodecView->item(0, 0));
    }
}
