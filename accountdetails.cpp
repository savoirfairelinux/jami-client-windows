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
#include "ringtonemodel.h"

AccountDetails::AccountDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountDetails),
    codecModel_(nullptr),
    currentAccount_(nullptr)
{
    ui->setupUi(this);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    ui->audioCodecView->setShowGrid(false);
    ui->audioCodecView->verticalHeader()->hide();
    ui->audioCodecView->horizontalHeader()->hide();
    ui->videoCodecView->setShowGrid(false);
    ui->videoCodecView->verticalHeader()->hide();
    ui->videoCodecView->horizontalHeader()->hide();

    ui->lrcfg_username->setAlignment(Qt::AlignCenter);

    connect(ui->lrcfg_tlsCaListCertificate, SIGNAL(clicked(bool)), this, SLOT(onCertButtonClicked()));
    connect(ui->lrcfg_tlsCertificate, SIGNAL(clicked(bool)), this, SLOT(onCertButtonClicked()));
    connect(ui->lrcfg_tlsPrivateKeyCertificate, SIGNAL(clicked(bool)), this, SLOT(onCertButtonClicked()));

    connect(&RingtoneModel::instance(), &RingtoneModel::dataChanged, [=](const QModelIndex& topLeft, const QModelIndex& bottomRight) {
        Q_UNUSED(topLeft)
        Q_UNUSED(bottomRight)
        if (not currentAccount_)
            return;
        if (RingtoneModel::instance().isPlaying())
            ui->playButton->setText(tr("Pause"));
        else
            ui->playButton->setText(tr("Play"));

    });
}

AccountDetails::~AccountDetails()
{
    delete ui;
}

void
AccountDetails::setAccount(Account* currentAccount) {

    if (currentAccount_) {
        stopRingtone();
        save();
    }

    currentAccount_ = currentAccount;

    ui->lrcfg_username->setReadOnly(currentAccount_->protocol() == Account::Protocol::RING);

    codecModel_ = currentAccount->codecModel();
    ui->audioCodecView->setModel(codecModel_->audioCodecs());
    ui->videoCodecView->setModel(codecModel_->videoCodecs());
    connect(ui->audioCodecView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(audioCodecSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->videoCodecView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(videoCodecSelectionChanged(QItemSelection,QItemSelection)));

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

#ifdef Q_OS_WIN
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

    disconnect(ui->ringtonesBox);
    ui->ringtonesBox->setModel(&RingtoneModel::instance());
    ui->ringtonesBox->setCurrentIndex(RingtoneModel::instance().selectionModel(currentAccount_)->currentIndex().row());
    connect(ui->ringtonesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ringtonesBoxCurrentIndexChanged(int)));
}

void
AccountDetails::on_upAudioButton_clicked() {
    codecModel_->moveUp();
}

void
AccountDetails::on_downAudioButton_clicked() {
    codecModel_->moveDown();
}

void
AccountDetails::on_upVideoButton_clicked() {
    codecModel_->moveUp();
}

void
AccountDetails::on_downVideoButton_clicked() {
    codecModel_->moveDown();
}

void
AccountDetails::save() {
    codecModel_->performAction(CodecModel::EditAction::SAVE);
}

void
AccountDetails::onCertButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
    "",
    tr("Files (*)"));

    auto sender = QObject::sender();

    (currentAccount_->*certMap_[sender->objectName()])(fileName);
    if (not fileName.isEmpty())
        static_cast<QPushButton*>(sender)->setText(fileName);
}

void
AccountDetails::audioCodecSelectionChanged(const QItemSelection& selected,
                                           const QItemSelection& deselected) {
    Q_UNUSED(deselected)
    if (not codecModel_ || selected.empty())
        return;
    auto idx = codecModel_->audioCodecs()->mapToSource(selected.indexes().at(0));
    codecModel_->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
}

void
AccountDetails::videoCodecSelectionChanged(const QItemSelection& selected,
                                           const QItemSelection& deselected) {
    Q_UNUSED(deselected)
    if (not codecModel_ || selected.empty())
        return;
    auto idx = codecModel_->videoCodecs()->mapToSource(selected.indexes().at(0));
    codecModel_->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
}

void
AccountDetails::ringtonesBoxCurrentIndexChanged(int index)
{
    RingtoneModel::instance().selectionModel(currentAccount_)->setCurrentIndex(
                RingtoneModel::instance().index(index, 0), QItemSelectionModel::ClearAndSelect);
}

void
AccountDetails::on_playButton_clicked()
{
    RingtoneModel::instance().play(RingtoneModel::instance().index(
                                       ui->ringtonesBox->currentIndex(), 0));
}

void
AccountDetails::stopRingtone() {
    if (not currentAccount_)
        return;
    auto idx = RingtoneModel::instance().selectionModel(currentAccount_)->currentIndex();
    if (RingtoneModel::instance().isPlaying())
        RingtoneModel::instance().play(idx);
}
