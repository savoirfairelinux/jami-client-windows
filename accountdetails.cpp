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

#include "accountdetails.h"
#include "ui_accountdetails.h"

#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QPushButton>

#include "accountdetails.h"
#include "audio/codecmodel.h"
#include "protocolmodel.h"
#include "certificate.h"
#include "ciphermodel.h"

AccountDetails::AccountDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountDetails),
    codecModel_(nullptr),
    currentAccount_(nullptr)
{
    ui->setupUi(this);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

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

    ui->lrcfg_username->setAlignment(Qt::AlignCenter);

    connect(ui->lrcfg_tlsCaListCertificate, SIGNAL(clicked(bool)), this, SLOT(onCertButtonClicked()));
    connect(ui->lrcfg_tlsCertificate, SIGNAL(clicked(bool)), this, SLOT(onCertButtonClicked()));
    connect(ui->lrcfg_tlsPrivateKeyCertificate, SIGNAL(clicked(bool)), this, SLOT(onCertButtonClicked()));
}

AccountDetails::~AccountDetails()
{
    delete ui;
}

void
AccountDetails::reloadCodec(CodecType type)
{
    ui->audioCodecView->disconnect();
    ui->videoCodecView->disconnect();

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
    connect(ui->audioCodecView, SIGNAL(cellChanged(int,int)),
            this, SLOT(audio_codec_checked(int, int)));
    connect(ui->videoCodecView, SIGNAL(cellChanged(int,int)),
            this, SLOT(video_codec_checked(int,int)));
}

void
AccountDetails::setAccount(Account* currentAccount) {

    if (currentAccount_) {
        currentAccount_->performAction(Account::EditAction::SAVE);
    }

    currentAccount_ = currentAccount;

    codecModel_ = currentAccount->codecModel();

    ui->typeValueLabel->setText(currentAccount_->protocolModel()->
                                selectionModel()->currentIndex().data().value<QString>());

    ui->publishGroup->disconnect();

    if (currentAccount_->isPublishedSameAsLocal())
        ui->puslishedSameAsLocalRadio->setChecked(true);
    else
        ui->customPublishedRadio->setChecked(true);

    ui->publishGroup->setId(ui->puslishedSameAsLocalRadio, 1);
    ui->publishGroup->setId(ui->customPublishedRadio, 0);
    ui->lrcfg_publishedAddress->setEnabled(!currentAccount_->isPublishedSameAsLocal());
    ui->lrcfg_publishedPort->setEnabled(!currentAccount_->isPublishedSameAsLocal());

    connect(ui->publishGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
        [=](int id) {
        currentAccount_->setPublishedSameAsLocal(static_cast<bool>(id));
    });

    switch (currentAccount_->DTMFType()) {
    case DtmfType::OverRtp:
        ui->rtpRadio->setChecked(true);
        break;
    case DtmfType::OverSip:
        ui->sipRadio->setChecked(true);
        break;
    }

    ui->dtmfGroup->disconnect();
    ui->dtmfGroup->setId(ui->rtpRadio, DtmfType::OverRtp);
    ui->dtmfGroup->setId(ui->sipRadio, DtmfType::OverSip);

    connect(ui->dtmfGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
        [=](int id){ currentAccount_->setDTMFType(static_cast<DtmfType>(id)); });

    ui->keyExchangeModelCombo->setModel(currentAccount_->keyExchangeModel());
    ui->tlsProtocoCombo->setModel(currentAccount_->tlsMethodModel());

    if (currentAccount_->tlsCaListCertificate())
        ui->lrcfg_tlsCaListCertificate->setText(currentAccount_->tlsCaListCertificate()->path());
    if (currentAccount_->tlsCertificate())
        ui->lrcfg_tlsCertificate->setText(currentAccount_->tlsCertificate()->path());
    if (not currentAccount_->tlsPrivateKey().isEmpty())
        ui->lrcfg_tlsPrivateKeyCertificate->setText(currentAccount_->tlsPrivateKey());

#ifdef Q_OS_WIN32
    certMap_[ui->lrcfg_tlsCaListCertificate->objectName()] = &currentAccount_->setTlsCaListCertificate;
    certMap_[ui->lrcfg_tlsCertificate->objectName()] = &currentAccount_->setTlsCertificate;
    certMap_[ui->lrcfg_tlsPrivateKeyCertificate->objectName()] = &currentAccount_->setTlsPrivateKey;
#endif

    ui->srtpEnabled->disconnect();
    connect(ui->srtpEnabled, &QCheckBox::toggled, [=](bool checked) {
        currentAccount_->setSrtpEnabled(checked);
        ui->lrcfg_srtpRtpFallback->setEnabled(checked);
        ui->keyExchangeModelCombo->setEnabled(checked);
    });

    ui->srtpEnabled->setChecked(currentAccount_->isSrtpEnabled());

    if (currentAccount_->cipherModel()->useDefault())
        ui->defaultCipherRadio->setChecked(true);
    else
        ui->customCipherRadio->setChecked(true);

    connect(ui->defaultCipherRadio, &QRadioButton::toggled, [=](bool checked) {
        currentAccount_->cipherModel()->setUseDefault(checked);
    });

    ui->cipherListView->setModel(currentAccount_->cipherModel());

    reloadCodec();
}

void
AccountDetails::audio_codec_checked(int row, int column) {
    if (column != 0)
        return;
    auto item = ui->audioCodecView->item(row, 0);
    auto idx = codecModel_->audioCodecs()->index(row, 0);
    codecModel_->audioCodecs()->setData(idx, item->checkState(),
                                        Qt::CheckStateRole);
}

void
AccountDetails::video_codec_checked(int row, int column) {
    if (column != 0)
        return;
    auto item = ui->videoCodecView->item(row, 0);
    auto idx = codecModel_->videoCodecs()->index(row, 0);
    codecModel_->videoCodecs()->setData(idx, item->checkState(),
                                        Qt::CheckStateRole);
}

void
AccountDetails::on_upAudioButton_clicked()
{
    codecModel_->moveUp();
    reloadCodec(CodecType::AUDIO);
}

void
AccountDetails::on_downAudioButton_clicked()
{
    codecModel_->moveDown();
    reloadCodec(CodecType::AUDIO);
}

void
AccountDetails::on_upVideoButton_clicked()
{
    codecModel_->moveUp();
    reloadCodec(CodecType::VIDEO);
}

void
AccountDetails::on_downVideoButton_clicked()
{
    codecModel_->moveDown();
    reloadCodec(CodecType::VIDEO);
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
    codecModel_->performAction(CodecModel::EditAction::SAVE);
}

void
AccountDetails::on_usernameEdit_editingFinished()
{
    currentAccount_->setUsername(ui->usernameEdit->text());
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


void
AccountDetails::onCertButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
    "",
    tr("Files (*)"));

    auto sender = QObject::sender();

    (currentAccount_->*certMap_[sender->objectName()])(fileName);

    static_cast<QPushButton*>(sender)->setText(fileName);
}
