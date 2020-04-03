/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                           *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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
#include "advancedsipsettingwidget.h"
#include "ui_advancedsipsettingwidget.h"

#include <QFileDialog>

#include "lrcinstance.h"
#include "utils.h"

#include "api/newcodecmodel.h"

AdvancedSIPSettingsWidget::AdvancedSIPSettingsWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AdvancedSIPSettingsWidget)
{
    ui->setupUi(this);

    ///////////////////////////////////////////////////////////////////////////////
    // call settings
    connect(ui->checkBoxCustomRingtoneSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setEnableRingtone);
    connect(ui->checkBoxAutoAnswerSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setAutoAnswerCalls);
    connect(ui->btnRingtoneSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::openFileCustomRingtone);
    connect(ui->checkBoxCustomRingtoneSIP, &QAbstractButton::clicked, [this](int state) {
        ui->btnRingtoneSIP->setEnabled((bool)state);
    });

    // security
    connect(ui->encryptMediaStreamsToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseSRTP);
    connect(ui->enableSDESToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseSDES);
    connect(ui->fallbackRTPToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseRTPFallback);
    connect(ui->encryptMediaStreamsToggle, &QAbstractButton::clicked, [this](int state) {
        ui->enableSDESToggle->setEnabled((bool)state);
        ui->fallbackRTPToggle->setEnabled((bool)state);
    });

    connect(ui->encryptNegotitationToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseTLS);
    connect(ui->encryptNegotitationToggle, &QAbstractButton::clicked, [this](int state) {
        ui->btnSIPCACert->setEnabled((bool)state);
        ui->btnSIPUserCert->setEnabled((bool)state);
        ui->btnSIPPrivateKey->setEnabled((bool)state);
        ui->lineEditSIPCertPassword->setEnabled((bool)state);
    });

    connect(ui->btnSIPCACert, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::btnSIPCAClicked);
    connect(ui->btnSIPUserCert, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::btnSIPUserCertClicked);
    connect(ui->btnSIPPrivateKey, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::btnSIPPrivateKeyClicked);

    connect(ui->verifyIncomingCertificatesServerToogle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setVerifyCertificatesServer);
    connect(ui->verifyIncomingCertificatesClientToogle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setVerifyCertificatesClient);
    connect(ui->requireCeritificateForTLSIncomingToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setRequireCertificatesIncomingTLS);

    connect(ui->lineEditSIPCertPassword, &QLineEdit::editingFinished, this, &AdvancedSIPSettingsWidget::lineEditSIPCertPasswordLineEditTextChanged);
    connect(ui->outgoingTLSServerNameLineEdit, &QLineEdit::editingFinished, this, &AdvancedSIPSettingsWidget::outgoingTLSServerNameLineEditTextChanged);

    ui->tlsProtocolComboBox->insertItem(0, "Default");
    ui->tlsProtocolComboBox->insertItem(1, "TLSv1");
    ui->tlsProtocolComboBox->insertItem(2, "TLSv1.1");
    ui->tlsProtocolComboBox->insertItem(3, "TLSv1.2");

    // connectivity
    connect(ui->checkBoxUPnPSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseUPnP);
    connect(ui->checkBoxTurnEnableSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseTURN);
    connect(ui->checkBoxSTUNEnableSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseSTUN);

    connect(ui->lineEditTurnAddressSIP, &QLineEdit::editingFinished, this, &AdvancedSIPSettingsWidget::setTURNAddress);
    connect(ui->lineEditTurnUsernameSIP, &QLineEdit::editingFinished, this, &AdvancedSIPSettingsWidget::setTURNUsername);
    connect(ui->lineEditTurnPsswdSIP, &QLineEdit::editingFinished, this, &AdvancedSIPSettingsWidget::setTURNPassword);
    connect(ui->lineEditTurnRealmSIP, &QLineEdit::editingFinished, this, &AdvancedSIPSettingsWidget::setTURNRealm);
    connect(ui->lineEditSTUNAddressSIP, &QLineEdit::editingFinished, this, &AdvancedSIPSettingsWidget::setSTUNAddress);

    connect(ui->checkBoxTurnEnableSIP, &QAbstractButton::clicked, [this](int state) {
        ui->lineEditTurnAddressSIP->setEnabled((bool)state);
        ui->lineEditTurnUsernameSIP->setEnabled((bool)state);
        ui->lineEditTurnPsswdSIP->setEnabled((bool)state);
        ui->lineEditTurnRealmSIP->setEnabled((bool)state);
    });

    connect(ui->checkBoxSTUNEnableSIP, &QAbstractButton::clicked, [this](int state) {
        ui->lineEditSTUNAddressSIP->setEnabled((bool)state);
    });

    // published address
    connect(ui->checkBoxCustomAddressPort, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseCustomAddressAndPort);
    connect(ui->checkBoxCustomAddressPort, &QAbstractButton::clicked, [this](int state) {
        ui->lineEditSIPCustomAddress->setEnabled((bool)state);
        ui->customPortSIPSpinBox->setEnabled((bool)state);
    });
    connect(ui->lineEditSIPCustomAddress, &QLineEdit::editingFinished, this, &AdvancedSIPSettingsWidget::lineEditSIPCustomAddressLineEditTextChanged);

    // codecs
    connect(ui->audioListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::videoCodecsStateChange);

    connect(ui->videoCheckBoxSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setVideoState);

    // codec priority setting
    connect(ui->audioDownPushButtonSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::decreaseAudioCodecPriority);
    connect(ui->audioUpPushButtonSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::increaseAudioCodecPriority);

    ui->audioDownPushButtonSIP->setEnabled(false);
    ui->audioUpPushButtonSIP->setEnabled(false);

    connect(ui->audioListWidgetSIP, &QListWidget::itemSelectionChanged,
        [this] {
            bool enabled = ui->audioListWidgetSIP->selectedItems().size();
            ui->audioDownPushButtonSIP->setEnabled(enabled);
            ui->audioUpPushButtonSIP->setEnabled(enabled);
        });

    connect(ui->videoDownPushButtonSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::decreaseVideoCodecPriority);
    connect(ui->videoUpPushButtonSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::increaseVideoCodecPriority);

    ui->videoDownPushButtonSIP->setEnabled(false);
    ui->videoUpPushButtonSIP->setEnabled(false);

    connect(ui->videoListWidgetSIP, &QListWidget::itemSelectionChanged,
        [this] {
            bool enabled = ui->videoListWidgetSIP->selectedItems().size();
            ui->videoDownPushButtonSIP->setEnabled(enabled);
            ui->videoUpPushButtonSIP->setEnabled(enabled);
        });

    // voicemail
    connect(ui->lineEditVoiceMailDialCode, &QLineEdit::editingFinished, this,
                                           &AdvancedSIPSettingsWidget::lineEditVoiceMailDialCodeEditFinished);

    // prevent wheel event for spinbox
    ui->negotiationTimeoutSpinBox->installEventFilter(this);
    ui->registrationExpireTimeoutSpinBox->installEventFilter(this);
    ui->networkInterfaceSpinBox->installEventFilter(this);
    ui->customPortSIPSpinBox->installEventFilter(this);
    ui->audioRTPMinPortSpinBox->installEventFilter(this);
    ui->audioRTPMaxPortSpinBox->installEventFilter(this);
    ui->videoRTPMinPortSpinBox->installEventFilter(this);
    ui->videoRTPMaxPortSpinBox->installEventFilter(this);
}

AdvancedSIPSettingsWidget::~AdvancedSIPSettingsWidget()
{
    delete ui;
}

void AdvancedSIPSettingsWidget::updateAdvancedSIPSettings()
{
    auto config = LRCInstance::getCurrAccConfig();

    // Call Settings
    ui->checkBoxAutoAnswerSIP->setChecked(config.autoAnswer);
    ui->checkBoxCustomRingtoneSIP->setChecked(config.Ringtone.ringtoneEnabled);

    // security
    ui->btnSIPCACert->setEnabled(config.TLS.enable);
    ui->btnSIPUserCert->setEnabled(config.TLS.enable);
    ui->btnSIPPrivateKey->setEnabled(config.TLS.enable);
    ui->lineEditSIPCertPassword->setEnabled(config.TLS.enable);
    ui->enableSDESToggle->setEnabled(config.SRTP.enable);
    ui->fallbackRTPToggle->setEnabled(config.SRTP.enable);

    ui->btnSIPCACert->setText(QFileInfo(config.TLS.certificateListFile).fileName());
    ui->btnSIPUserCert->setText(QFileInfo(config.TLS.certificateFile).fileName());
    ui->btnSIPPrivateKey->setText(QFileInfo(config.TLS.privateKeyFile).fileName());
    ui->lineEditSIPCertPassword->setText(config.TLS.password);

    ui->encryptMediaStreamsToggle->setChecked(config.SRTP.enable);
    ui->enableSDESToggle->setChecked(config.SRTP.keyExchange == lrc::api::account::KeyExchangeProtocol::SDES);
    ui->fallbackRTPToggle->setChecked(config.SRTP.rtpFallback);
    ui->encryptNegotitationToggle->setChecked(config.TLS.enable);
    ui->verifyIncomingCertificatesServerToogle->setChecked(config.TLS.verifyServer);
    ui->verifyIncomingCertificatesClientToogle->setChecked(config.TLS.verifyClient);
    ui->requireCeritificateForTLSIncomingToggle->setChecked(config.TLS.requireClientCertificate);

    auto method = config.TLS.method;
    ui->tlsProtocolComboBox->setCurrentIndex(static_cast<int>(method));
    connect(ui->tlsProtocolComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdvancedSIPSettingsWidget::tlsProtocolComboBoxIndexChanged);

    ui->outgoingTLSServerNameLineEdit->setText(config.TLS.serverName);

    ui->negotiationTimeoutSpinBox->setValue(config.TLS.negotiationTimeoutSec);
    connect(ui->negotiationTimeoutSpinBox, &QSpinBox::editingFinished, this, &AdvancedSIPSettingsWidget::negotiationTimeoutSpinBoxValueChanged);

    // Connectivity
    ui->checkBoxUPnPSIP->setChecked(config.upnpEnabled);
    ui->checkBoxTurnEnableSIP->setChecked(config.TURN.enable);
    ui->lineEditTurnAddressSIP->setText(config.TURN.server);
    ui->lineEditTurnUsernameSIP->setText(config.TURN.username);
    ui->lineEditTurnPsswdSIP->setText(config.TURN.password);
    ui->lineEditTurnRealmSIP->setText(config.TURN.realm);
    ui->lineEditTurnAddressSIP->setEnabled(config.TURN.enable);
    ui->lineEditTurnUsernameSIP->setEnabled(config.TURN.enable);
    ui->lineEditTurnPsswdSIP->setEnabled(config.TURN.enable);
    ui->lineEditTurnRealmSIP->setEnabled(config.TURN.enable);

    ui->checkBoxSTUNEnableSIP->setChecked(config.STUN.enable);
    ui->lineEditSTUNAddressSIP->setText(config.STUN.server);
    ui->lineEditSTUNAddressSIP->setEnabled(config.STUN.enable);

    ui->registrationExpireTimeoutSpinBox->setValue(config.Registration.expire);
    connect(ui->registrationExpireTimeoutSpinBox, &QSpinBox::editingFinished, this, &AdvancedSIPSettingsWidget::registrationTimeoutSpinBoxValueChanged);
    ui->networkInterfaceSpinBox->setValue(config.localPort);
    connect(ui->networkInterfaceSpinBox, &QSpinBox::editingFinished, this, &AdvancedSIPSettingsWidget::networkInterfaceSpinBoxValueChanged);

    // published address
    ui->checkBoxCustomAddressPort->setChecked(!config.publishedSameAsLocal);
    ui->lineEditSIPCustomAddress->setText(config.publishedAddress);
    ui->customPortSIPSpinBox->setValue(config.publishedPort);

    connect(ui->customPortSIPSpinBox, &QSpinBox::editingFinished, this, &AdvancedSIPSettingsWidget::customPortSIPSpinBoxValueChanged);

    // codecs
    ui->videoCheckBoxSIP->setChecked(config.Video.videoEnabled);
    disconnect(ui->audioListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::audioCodecsStateChange);
    disconnect(ui->videoListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::videoCodecsStateChange);
    updateAudioCodecs();
    updateVideoCodecs();
    connect(ui->audioListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::videoCodecsStateChange);

    ui->btnRingtoneSIP->setEnabled(config.Ringtone.ringtoneEnabled);
    ui->btnRingtoneSIP->setText(QFileInfo(config.Ringtone.ringtonePath).fileName());
    ui->lineEditSTUNAddressSIP->setEnabled(config.STUN.enable);

    // SDP session negotiation ports
    ui->audioRTPMinPortSpinBox->setValue(config.Audio.audioPortMin);
    ui->audioRTPMaxPortSpinBox->setValue(config.Audio.audioPortMax);
    ui->videoRTPMinPortSpinBox->setValue(config.Video.videoPortMin);
    ui->videoRTPMaxPortSpinBox->setValue(config.Video.videoPortMax);

    connect(ui->audioRTPMinPortSpinBox, &QSpinBox::editingFinished, this, &AdvancedSIPSettingsWidget::audioRTPMinPortSpinBoxEditFinished);
    connect(ui->audioRTPMaxPortSpinBox, &QSpinBox::editingFinished, this, &AdvancedSIPSettingsWidget::audioRTPMaxPortSpinBoxEditFinished);
    connect(ui->videoRTPMinPortSpinBox, &QSpinBox::editingFinished, this, &AdvancedSIPSettingsWidget::videoRTPMinPortSpinBoxEditFinished);
    connect(ui->videoRTPMaxPortSpinBox, &QSpinBox::editingFinished, this, &AdvancedSIPSettingsWidget::videoRTPMaxPortSpinBoxEditFinished);

    // voicemail
    ui->lineEditVoiceMailDialCode->setText(config.mailbox);
}

// call settings
void AdvancedSIPSettingsWidget::setAutoAnswerCalls(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.autoAnswer = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void AdvancedSIPSettingsWidget::setEnableRingtone(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Ringtone.ringtoneEnabled = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void AdvancedSIPSettingsWidget::openFileCustomRingtone()
{
    QString fileUrl;
    auto oldPath = LRCInstance::getCurrAccConfig().Ringtone.ringtonePath;
    auto openPath = oldPath.isEmpty() ? QDir::currentPath() + QString("/ringtones/") : QFileInfo(oldPath).absolutePath();
    fileUrl = QFileDialog::getOpenFileName(this, tr("Select a new ringtone"), openPath, tr("Audio Files") + " (*.wav *.ogg *.opus *.mp3 *.aiff *.wma)");

    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.Ringtone.ringtonePath = fileUrl;
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
        ui->btnRingtoneSIP->setText(QFileInfo(fileUrl).fileName());

    } else if (oldPath.isEmpty()) {
        ui->btnRingtoneSIP->setText(tr("Add a custom ringtone"));
    }
}

// connectivity
void AdvancedSIPSettingsWidget::setUseUPnP(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.upnpEnabled = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void AdvancedSIPSettingsWidget::setUseTURN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.enable = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void AdvancedSIPSettingsWidget::setUseSTUN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.enable = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);

    state ? ui->lineEditSTUNAddressSIP->setEnabled(true) : ui->lineEditSTUNAddressSIP->setEnabled(false);
}

void AdvancedSIPSettingsWidget::setTURNAddress()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.server = ui->lineEditTurnAddressSIP->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void AdvancedSIPSettingsWidget::setTURNUsername()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.username = ui->lineEditTurnUsernameSIP->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void AdvancedSIPSettingsWidget::setTURNPassword()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.password = ui->lineEditTurnPsswdSIP->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::setTURNRealm()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.realm = ui->lineEditTurnRealmSIP->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void AdvancedSIPSettingsWidget::setSTUNAddress()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.server = ui->lineEditSTUNAddressSIP->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void AdvancedSIPSettingsWidget::updateAudioCodecs()
{
    ui->audioListWidgetSIP->clear();

    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    int i = 0;

    for (auto it = audioCodecList.begin(); it != audioCodecList.end(); ++it, ++i) {
        QListWidgetItem* audioItem = new QListWidgetItem(ui->audioListWidgetSIP);
        audioItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        audioItem->setCheckState(it->enabled ? Qt::Checked : Qt::Unchecked);
        audioItem->setData(Qt::DisplayRole, it->name + " " + it->samplerate + " Hz");

        ui->audioListWidgetSIP->addItem(audioItem);
    }
}

void AdvancedSIPSettingsWidget::updateVideoCodecs()
{
    ui->videoListWidgetSIP->clear();

    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    int i = 0;

    for (auto it = videoCodecList.begin(); it != videoCodecList.end(); ++it, ++i) {
        if (!it->name.length()) { continue; } // temporary fix for empty codec entries
        QListWidgetItem* videoItem = new QListWidgetItem(ui->videoListWidgetSIP);
        videoItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        videoItem->setCheckState(it->enabled ? Qt::Checked : Qt::Unchecked);
        videoItem->setData(Qt::DisplayRole, it->name + "\n");
        ui->audioListWidgetSIP->addItem(videoItem);
    }
}

void AdvancedSIPSettingsWidget::audioCodecsStateChange(QListWidgetItem* item)
{
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    std::advance(it, ui->audioListWidgetSIP->row(item));

    LRCInstance::getCurrentAccountInfo().codecModel->enable(it->id, !(it->enabled));
}

void AdvancedSIPSettingsWidget::videoCodecsStateChange(QListWidgetItem* item)
{
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    std::advance(it, ui->videoListWidgetSIP->row(item));

    LRCInstance::getCurrentAccountInfo().codecModel->enable(it->id, !(it->enabled));
}

void AdvancedSIPSettingsWidget::decreaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidgetSIP->row(ui->audioListWidgetSIP->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    std::advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, false);

    // swap current item down
    Utils::swapQListWidgetItems(ui->audioListWidgetSIP, true);
}

void AdvancedSIPSettingsWidget::increaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidgetSIP->row(ui->audioListWidgetSIP->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    std::advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id, false);
    updateAudioCodecs();

    // swap current item up
    Utils::swapQListWidgetItems(ui->audioListWidgetSIP, false);
}

void AdvancedSIPSettingsWidget::decreaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidgetSIP->row(ui->videoListWidgetSIP->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    std::advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, true);

    // swap current item down
    Utils::swapQListWidgetItems(ui->videoListWidgetSIP, true);
}

void AdvancedSIPSettingsWidget::increaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidgetSIP->row(ui->videoListWidgetSIP->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    std::advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id, true);

    // swap current item up
    Utils::swapQListWidgetItems(ui->videoListWidgetSIP, false);
}

void AdvancedSIPSettingsWidget::setVideoState(int state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Video.videoEnabled = (bool)state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::setUseSRTP(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.SRTP.enable = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setUseTLS(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.enable = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::setVerifyCertificatesServer(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.verifyServer = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setVerifyCertificatesClient(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.verifyClient = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setRequireCertificatesIncomingTLS(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.requireClientCertificate = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::tlsProtocolComboBoxIndexChanged(const int& index)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());

    if (static_cast<int>(confProps.TLS.method) != index) {
        if (index == 0) {
            confProps.TLS.method = lrc::api::account::TlsMethod::DEFAULT;
        } else if (index == 1) {
            confProps.TLS.method = lrc::api::account::TlsMethod::TLSv1;
        } else if (index == 2) {
            confProps.TLS.method = lrc::api::account::TlsMethod::TLSv1_1;
        } else {
            confProps.TLS.method = lrc::api::account::TlsMethod::TLSv1_2;
        }
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
}

void
AdvancedSIPSettingsWidget::outgoingTLSServerNameLineEditTextChanged()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.serverName = ui->outgoingTLSServerNameLineEdit->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::negotiationTimeoutSpinBoxValueChanged()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.negotiationTimeoutSec = ui->negotiationTimeoutSpinBox->value();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::setUseSDES(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.SRTP.keyExchange = state ? lrc::api::account::KeyExchangeProtocol::SDES : lrc::api::account::KeyExchangeProtocol::NONE;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::setUseRTPFallback(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.SRTP.rtpFallback = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::btnSIPCAClicked()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.certificateListFile = openButtonFilePath(LRCInstance::getCurrAccConfig().TLS.certificateListFile,
                                        "Select a CA Certificate File",
                                        "Certificate File",
                                        " (*.crt)",
                                        ui->btnSIPCACert);
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::btnSIPUserCertClicked()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.certificateFile = openButtonFilePath(LRCInstance::getCurrAccConfig().TLS.certificateFile,
                                        "Select a User Certificate File",
                                        "Certificate File",
                                        " (*.crt)",
                                        ui->btnSIPUserCert);
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::btnSIPPrivateKeyClicked()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.privateKeyFile = openButtonFilePath(LRCInstance::getCurrAccConfig().TLS.privateKeyFile,
                                        "Select a Private Key File",
                                        "Key File",
                                        " (*.key)",
                                        ui->btnSIPPrivateKey);
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::lineEditSIPCertPasswordLineEditTextChanged()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.password = ui->lineEditSIPCertPassword->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

QString
AdvancedSIPSettingsWidget::openButtonFilePath(const QString& accConfigFilePath,
                                              const char* windowTitle,
                                              const char* fileTypeDesp,
                                              const QString& fileTypeFilter,
                                              QPushButton* button)
{
    QString fileUrl;
    auto oldPath = accConfigFilePath;
    auto openPath = oldPath.isEmpty() ? QDir::currentPath() : QFileInfo(oldPath).absolutePath();
    fileUrl = QFileDialog::getOpenFileName(this, tr(windowTitle), openPath, tr(fileTypeDesp) + fileTypeFilter);
    if (!fileUrl.isEmpty()) {
        button->setText(QFileInfo(fileUrl).fileName());
    } else {
        button->setText("");
    }
    return fileUrl;
}

void
AdvancedSIPSettingsWidget::registrationTimeoutSpinBoxValueChanged()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Registration.expire = ui->registrationExpireTimeoutSpinBox->value();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::networkInterfaceSpinBoxValueChanged()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.localPort = ui->networkInterfaceSpinBox->value();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::setUseCustomAddressAndPort(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.publishedSameAsLocal = !state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::lineEditSIPCustomAddressLineEditTextChanged()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.publishedAddress = ui->lineEditSIPCustomAddress->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::customPortSIPSpinBoxValueChanged()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.publishedPort = ui->customPortSIPSpinBox->value();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::audioRTPMinPortSpinBoxEditFinished()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    if (confProps.Audio.audioPortMax < ui->audioRTPMinPortSpinBox->value()) {
        ui->audioRTPMinPortSpinBox->setValue(confProps.Audio.audioPortMin);
        return;
    }
    confProps.Audio.audioPortMin = ui->audioRTPMinPortSpinBox->value();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::audioRTPMaxPortSpinBoxEditFinished()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    if (ui->audioRTPMaxPortSpinBox->value() < confProps.Audio.audioPortMin) {
        ui->audioRTPMaxPortSpinBox->setValue(confProps.Audio.audioPortMax);
        return;
    }
    confProps.Audio.audioPortMax = ui->audioRTPMaxPortSpinBox->value();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::videoRTPMinPortSpinBoxEditFinished()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    if (confProps.Video.videoPortMax < ui->videoRTPMinPortSpinBox->value()) {
        ui->videoRTPMinPortSpinBox->setValue(confProps.Video.videoPortMin);
        return;
    }
    confProps.Video.videoPortMin = ui->videoRTPMinPortSpinBox->value();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::videoRTPMaxPortSpinBoxEditFinished()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    if (ui->videoRTPMaxPortSpinBox->value() < confProps.Video.videoPortMin) {
        ui->videoRTPMaxPortSpinBox->setValue(confProps.Video.videoPortMax);
        return;
    }
    confProps.Video.videoPortMax = ui->videoRTPMaxPortSpinBox->value();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::lineEditVoiceMailDialCodeEditFinished()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.mailbox = ui->lineEditVoiceMailDialCode->text();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

bool
AdvancedSIPSettingsWidget::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::Wheel) {
        return true;
    }
    return false;
}
