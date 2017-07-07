/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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
#include <QMessageBox>

#include "codecmodel.h"
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

    connect(ui->lrcfg_tlsCaListCertificate, &RingButton::clicked, this, &AccountDetails::onCertButtonClicked);
    connect(ui->lrcfg_tlsCertificate, &RingButton::clicked, this, &AccountDetails::onCertButtonClicked);
    connect(ui->lrcfg_tlsPrivateKeyCertificate, &RingButton::clicked, this, &AccountDetails::onCertButtonClicked);

    connect(&RingtoneModel::instance(),
            &RingtoneModel::dataChanged,
            [=](const QModelIndex& topLeft, const QModelIndex& bottomRight) {
        Q_UNUSED(topLeft)
        Q_UNUSED(bottomRight)
        if (not currentAccount_)
            return;
        if (RingtoneModel::instance().isPlaying())
            ui->playButton->setText(tr("Pause"));
        else
            ui->playButton->setText(tr("Play"));

    });

    connect(ui->defaultCipherCheckBox, &QCheckBox::stateChanged, [=] (int state) {
        if (state == Qt::Checked) {
            ui->cipherListView->setVisible(false);
            currentAccount_->cipherModel()->setUseDefault(true);
        } else {
            ui->cipherListView->setVisible(true);
            currentAccount_->cipherModel()->setUseDefault(false);
        }
    });

    connect(ui->lrcfg_alias, &QLineEdit::textEdited, [=](const QString& newAlias) {
        if (currentAccount_ && currentAccount_->protocol() == Account::Protocol::RING)
            currentAccount_->setDisplayName(newAlias);
    });

    connect(ui->lrcfg_nameServiceURL, &QLineEdit::textEdited, [=](const QString& newNSURL) {
        if (currentAccount_ && currentAccount_->protocol() == Account::Protocol::RING)
            currentAccount_->setNameServiceURL(newNSURL);
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

    if (currentAccount_ == nullptr)
        return;

    if (currentAccount_->protocol() == Account::Protocol::RING) {
        ui->usernameLabel->setText(tr("RingID"));
        ui->lrcfg_username->setReadOnly(true);
        if (currentAccount_->registeredName().isEmpty() ){ // If our user isn't registered on the blockhain
            ui->lrcfg_registeredName->clear();
            ui->lrcfg_registeredName->setReadOnly(false);
            ui->registerButton->setText(tr("Register on blockchain"));
            ui->registerButton->show();
            ui->registerButton->setEnabled(true);
        } else {
            ui->lrcfg_registeredName->setText(currentAccount_->registeredName());
            ui->lrcfg_registeredName->setReadOnly(true);
            ui->registerButton->hide();
        }
    } else { // If currentAccount_ is of type SIP
        ui->usernameLabel->setText(tr("Username"));
        ui->lrcfg_username->setReadOnly(false);
    }

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

    connect(ui->publishGroup,
            static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            [=](int id) {
        currentAccount_->setPublishedSameAsLocal(static_cast<bool>(id));
    });

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

    ui->srtpEnabledChkBox->disconnect();
    connect(ui->srtpEnabledChkBox, &QCheckBox::toggled, [=](bool checked) {
        currentAccount_->setSrtpEnabled(checked);
    });

    ui->srtpEnabledChkBox->setChecked(currentAccount_->isSrtpEnabled());

    if (currentAccount_->cipherModel()->useDefault())
        ui->defaultCipherCheckBox->setChecked(true);
    else
        ui->defaultCipherCheckBox->setChecked(false);

    ui->cipherListView->setModel(currentAccount_->cipherModel());

    disconnect(ui->ringtonesBox);
    ui->ringtonesBox->setModel(&RingtoneModel::instance());
    ui->ringtonesBox->setCurrentIndex(RingtoneModel::instance().selectionModel(currentAccount_)->currentIndex().row());
    connect(ui->ringtonesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ringtonesBoxCurrentIndexChanged(int)));

    auto accountProtocol = currentAccount_->protocol();
    if (accountProtocol == Account::Protocol::RING) {
        ui->srtpEnabledChkBox->hide();
        ui->nameServiceURLLabel->show();
        ui->lrcfg_nameServiceURL->show();
        ui->lrcfg_nameServiceURL->setText(currentAccount_->nameServiceURL());
    } else if (accountProtocol == Account::Protocol::SIP) {
        ui->srtpEnabledChkBox->show();
        ui->nameServiceURLLabel->hide();
        ui->lrcfg_nameServiceURL->hide();
    }

    if (ui->defaultCipherCheckBox->checkState() == Qt::Checked)
        ui->cipherListView->setVisible(false);
    else
        ui->cipherListView->setVisible(true);

    ui->tableView->setModel((QAbstractItemModel*)currentAccount_->ringDeviceModel());
    ui->bannedContactsWidget->setAccount(currentAccount_);
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

void
AccountDetails::on_addDeviceButton_clicked()
{
    ui->devicesStackedWidget->setCurrentIndex(1);
}

void
AccountDetails::on_cancelButton_clicked()
{
    ui->devicesStackedWidget->setCurrentIndex(0);
}

void
AccountDetails::on_exportOnRingButton_clicked()
{
    if (ui->passwordArchiveEdit->text().isEmpty())
        return;

    connect(currentAccount_, SIGNAL(exportOnRingEnded(Account::ExportOnRingStatus,QString)), this, SLOT(exportOnRingEnded(Account::ExportOnRingStatus,QString)));
    currentAccount_->exportOnRing(ui->passwordArchiveEdit->text());
    ui->devicesStackedWidget->setCurrentIndex(2);
    ui->pinLabel->setText(tr("Please wait while your PIN is generated."));
}

void
AccountDetails::exportOnRingEnded(Account::ExportOnRingStatus state, const QString& pin) {

    ui->devicesStackedWidget->setCurrentIndex(2);

    ui->pinLabel->clear();

    switch (state) {
    case Account::ExportOnRingStatus::SUCCESS:
    {
        ui->pinLabel->setText(pin);
        ui->pinLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        break;
    }
    case Account::ExportOnRingStatus::NETWORK_ERROR:
    {
        ui->pinLabel->setText(tr("Network Error. Please try again later."));
        break;
    }
    case Account::ExportOnRingStatus::WRONG_PASSWORD:
        ui->pinLabel->setText(tr("Wrong password."));
        break;
    }
}

void
AccountDetails::on_exportEndedOkButton_clicked()
{
    ui->devicesStackedWidget->setCurrentIndex(0);
}

void
AccountDetails::on_cancelAddButton_clicked()
{
    ui->devicesStackedWidget->setCurrentIndex(0);
}

void AccountDetails::on_devicesStackedWidget_currentChanged(int pageNum)
{
    // We clear the password textEdit each time we leave its page
    if (pageNum != ui->devicesStackedWidget->indexOf(ui->passwordAskingPage))
        ui->passwordArchiveEdit->clear();
}

void AccountDetails::on_registerButton_clicked()
{
    ui->registerButton->setEnabled(false);
    ui->registerButton->setText(tr("Registering... It may take some time"));
    bool regSuccess = currentAccount_->registerName(ui->lrcfg_password->text(), ui->lrcfg_registeredName->text());
    if (!regSuccess) {
        QMessageBox::warning(this, "Username not registered", "Username registration failed, try again later.");
        ui->registerButton->setEnabled(true);
        ui->registerButton->setText(tr("Register on blockchain"));
        return;
    }

    connect(currentAccount_, SIGNAL(nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)),
            this, SLOT(handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)));
}


void AccountDetails::handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus status, const QString& name)
{
    disconnect(currentAccount_, SIGNAL(nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)),
               this, SLOT(handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)));
    switch(status) {
    case NameDirectory::RegisterNameStatus::ALREADY_TAKEN:
        QMessageBox::warning(this, "Username not registered", "This username is already taken, try another one.");
        ui->registerButton->setEnabled(true);
        ui->registerButton->setText(tr("Register on blockchain"));
        break;
    case NameDirectory::RegisterNameStatus::INVALID_NAME:
        QMessageBox::warning(this, "Username not registered", "This username is invalid, try another one.");
        ui->registerButton->setEnabled(true);
        ui->registerButton->setText(tr("Register on blockchain"));
        break;
    case NameDirectory::RegisterNameStatus::WRONG_PASSWORD:
        QMessageBox::warning(this, "Username not registered", "Wrong password, try again.");
        ui->registerButton->setEnabled(true);
        ui->registerButton->setText(tr("Register on blockchain"));
        break;
    case NameDirectory::RegisterNameStatus::NETWORK_ERROR:
        QMessageBox::warning(this, "Username not registered", "Network error. Try again later.");
        ui->registerButton->setEnabled(true);
        ui->registerButton->setText(tr("Register on blockchain"));
        break;
    case NameDirectory::RegisterNameStatus::SUCCESS:
        ui->lrcfg_registeredName->setReadOnly(true);
        ui->registerButton->hide();
        QMessageBox::information(this, "Username registered", name + " is registered, you can now share this name.");
        break;
    }

}
