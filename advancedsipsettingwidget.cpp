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
    connect(ui->btnRingtoneSIP, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::openFileCustomRingtone);
    connect(ui->checkBoxCustomRingtoneSIP, &QAbstractButton::clicked, [this](int state) {
        ui->btnRingtoneSIP->setEnabled((bool)state);
    });

    // security
    connect(ui->encryptMediaStreamsToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseSRTP);
    connect(ui->enableSDESToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseSDES);
    connect(ui->fallbackRTPToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseRTPFallback);
    connect(ui->encryptMediaStreamsToggle, &QAbstractButton::clicked, [this](int state) {
        ui->enableSDESToggle->setEnabled((bool)state);
    });
    connect(ui->encryptMediaStreamsToggle, &QAbstractButton::clicked, [this](int state) {
        ui->fallbackRTPToggle->setEnabled((bool)state);
    });

    connect(ui->encryptNegotitationToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseTLS);
    connect(ui->encryptNegotitationToggle, &QAbstractButton::clicked, [this](int state) {
        ui->btnSIPCACert->setEnabled((bool)state);
    });
    connect(ui->encryptNegotitationToggle, &QAbstractButton::clicked, [this](int state) {
        ui->btnSIPUserCert->setEnabled((bool)state);
    });
    connect(ui->encryptNegotitationToggle, &QAbstractButton::clicked, [this](int state) {
        ui->btnSIPPrivateKey->setEnabled((bool)state);
    });
    connect(ui->encryptNegotitationToggle, &QAbstractButton::clicked, [this](int state) {
        ui->lineEditSIPCertPassword->setEnabled((bool)state);
    });

    connect(ui->btnSIPCACert, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::btnSIPCAClicked);
    connect(ui->btnSIPUserCert, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::btnSIPUserCertClicked);
    connect(ui->btnSIPPrivateKey, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::btnSIPPrivateKeyClicked);

    connect(ui->verifyIncomingCertificatesServerToogle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setVerifyCertificatesServer);
    connect(ui->verifyIncomingCertificatesClientToogle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setVerifyCertificatesClient);
    connect(ui->requireCeritificateForTLSIncomingToggle, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setRequireCertificatesIncomingTLS);

    connect(ui->tlsProtocolComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdvancedSIPSettingsWidget::tlsProtocolComboBoxIndexChanged);
    connect(ui->outgoingTLSServerNameLineEdit, &QLineEdit::textEdited, this, &AdvancedSIPSettingsWidget::outgoingTLSServerNameLineEdit);
    connect(ui->negotiationTimeoutSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &AdvancedSIPSettingsWidget::negotiationTimeoutSpinBoxValuechanged);


    // connectivity
    connect(ui->checkBoxUPnPSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseUPnP);
    connect(ui->checkBoxTurnEnableSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseTURN);
    connect(ui->checkBoxSTUNEnableSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setUseSTUN);

    connect(ui->lineEditTurnAddressSIP, &QLineEdit::textEdited, this, &AdvancedSIPSettingsWidget::setTURNAddress);
    connect(ui->lineEditTurnUsernameSIP, &QLineEdit::textEdited, this, &AdvancedSIPSettingsWidget::setTURNUsername);
    connect(ui->lineEditTurnPsswdSIP, &QLineEdit::textEdited, this, &AdvancedSIPSettingsWidget::setTURNPsswd);
    connect(ui->lineEditSTUNAddressSIP, &QLineEdit::textEdited, this, &AdvancedSIPSettingsWidget::setSTUNAddress);

    // codecs
    connect(ui->audioListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::videoCodecsStateChange);

    connect(ui->videoCheckBoxSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setVideoState);

    // codec priority setting
    connect(ui->audioDownPushButtonSIP, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::decreaseAudioCodecPriority);
    connect(ui->audioUpPushButtonSIP, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::increaseAudioCodecPriority);

    ui->audioDownPushButtonSIP->setEnabled(false);
    ui->audioUpPushButtonSIP->setEnabled(false);

    connect(ui->audioListWidgetSIP, &QListWidget::itemSelectionChanged,
        [this] {
            bool enabled = ui->audioListWidgetSIP->selectedItems().size();
            ui->audioDownPushButtonSIP->setEnabled(enabled);
            ui->audioUpPushButtonSIP->setEnabled(enabled);
        });

    connect(ui->videoDownPushButtonSIP, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::decreaseVideoCodecPriority);
    connect(ui->videoUpPushButtonSIP, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::increaseVideoCodecPriority);

    ui->videoDownPushButtonSIP->setEnabled(false);
    ui->videoUpPushButtonSIP->setEnabled(false);

    connect(ui->videoListWidgetSIP, &QListWidget::itemSelectionChanged,
        [this] {
            bool enabled = ui->videoListWidgetSIP->selectedItems().size();
            ui->videoDownPushButtonSIP->setEnabled(enabled);
            ui->videoUpPushButtonSIP->setEnabled(enabled);
        });
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

    ui->btnSIPCACert->setText(QFileInfo(QString::fromStdString(LRCInstance::getCurrAccConfig().TLS.certificateListFile)).fileName());
    ui->btnSIPUserCert->setText(QFileInfo(QString::fromStdString(LRCInstance::getCurrAccConfig().TLS.certificateFile)).fileName());
    ui->btnSIPPrivateKey->setText(QFileInfo(QString::fromStdString(LRCInstance::getCurrAccConfig().TLS.privateKeyFile)).fileName());
    ui->lineEditSIPCertPassword->setText(QString::fromStdString(config.TLS.password));

    ui->encryptMediaStreamsToggle->setChecked(config.SRTP.enable);
    ui->enableSDESToggle->setChecked(config.SRTP.keyExchange == lrc::api::account::KeyExchangeProtocol::SDES);
    ui->fallbackRTPToggle->setChecked(config.SRTP.rtpFallback);
    ui->encryptNegotitationToggle->setChecked(config.TLS.enable);
    ui->verifyIncomingCertificatesServerToogle->setChecked(config.TLS.verifyServer);
    ui->verifyIncomingCertificatesClientToogle->setChecked(config.TLS.verifyClient);
    ui->requireCeritificateForTLSIncomingToggle->setChecked(config.TLS.requireClientCertificate);

    // Connectivity
    ui->checkBoxUPnPSIP->setChecked(config.upnpEnabled);
    ui->checkBoxTurnEnableSIP->setChecked(config.TURN.enable);
    ui->lineEditTurnAddressSIP->setText(QString::fromStdString(config.TURN.server));
    ui->lineEditTurnUsernameSIP->setText(QString::fromStdString(config.TURN.username));
    ui->lineEditTurnPsswdSIP->setText(QString::fromStdString(config.TURN.password));
    ui->checkBoxSTUNEnableSIP->setChecked(config.STUN.enable);
    ui->lineEditSTUNAddressSIP->setText(QString::fromStdString(config.STUN.server));

    // codecs
    ui->videoCheckBoxSIP->setChecked(config.Video.videoEnabled);
    disconnect(ui->audioListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::audioCodecsStateChange);
    disconnect(ui->videoListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::videoCodecsStateChange);
    updateAudioCodecs();
    updateVideoCodecs();
    connect(ui->audioListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidgetSIP, &QListWidget::itemChanged, this, &AdvancedSIPSettingsWidget::videoCodecsStateChange);

    ui->btnRingtoneSIP->setEnabled(LRCInstance::getCurrAccConfig().Ringtone.ringtoneEnabled);
    ui->btnRingtoneSIP->setText(QFileInfo(QString::fromStdString(LRCInstance::getCurrAccConfig().Ringtone.ringtonePath)).fileName());
    ui->lineEditSTUNAddressSIP->setEnabled(LRCInstance::getCurrAccConfig().STUN.enable);
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
    auto oldPath = QString::fromStdString(LRCInstance::getCurrAccConfig().Ringtone.ringtonePath);
    auto openPath = oldPath.isEmpty() ? QDir::currentPath() + QString("/ringtones/") : QFileInfo(oldPath).absolutePath();
    fileUrl = QFileDialog::getOpenFileName(this, tr("Select a new ringtone"), openPath, tr("Audio Files") + " (*.wav *.ogg *.opus *.mp3 *.aiff *.wma)");

    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.Ringtone.ringtonePath = fileUrl.toStdString();
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

void AdvancedSIPSettingsWidget::setTURNAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.server = name.toStdString();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void AdvancedSIPSettingsWidget::setTURNUsername(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.username = name.toStdString();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void AdvancedSIPSettingsWidget::setTURNPsswd(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.password = name.toStdString();
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void AdvancedSIPSettingsWidget::setSTUNAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.server = name.toStdString();
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
        audioItem->setData(Qt::DisplayRole, QString::fromStdString(it->name) + " " + QString::fromStdString(it->samplerate) + " Hz");

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
        videoItem->setData(Qt::DisplayRole, QString::fromStdString(it->name) + "\n");
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

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, false);

    // swap current item down
    Utils::swapQListWidgetItems(ui->audioListWidgetSIP, true);
}

void AdvancedSIPSettingsWidget::increaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidgetSIP->row(ui->audioListWidgetSIP->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    advance(it, selectedRow);
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

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, true);

    // swap current item down
    Utils::swapQListWidgetItems(ui->videoListWidgetSIP, true);
}

void AdvancedSIPSettingsWidget::increaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidgetSIP->row(ui->videoListWidgetSIP->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    advance(it, selectedRow);
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
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setUseTLS(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.enable = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::setVerifyCertificatesServer(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.verifyServer = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setVerifyCertificatesClient(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.verifyClient = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setRequireCertificatesIncomingTLS(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TLS.requireClientCertificate = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::tlsProtocolComboBoxIndexChanged(const int& index)
{
}

void
AdvancedSIPSettingsWidget::outgoingTLSServerNameLineEdit(const QString& text)
{
}

void
AdvancedSIPSettingsWidget::negotiationTimeoutSpinBoxValuechanged(const int& value)
{
}

void
AdvancedSIPSettingsWidget::setUseSDES(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.SRTP.keyExchange = state ? lrc::api::account::KeyExchangeProtocol::SDES : lrc::api::account::KeyExchangeProtocol::NONE;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::setUseRTPFallback(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.SRTP.rtpFallback = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::btnSIPCAClicked()
{
    /*QString fileUrl;
    auto oldPath = QString::fromStdString(LRCInstance::getCurrAccConfig().TLS.certificateListFile);
    auto openPath = oldPath.isEmpty() ? QDir::currentPath() : QFileInfo(oldPath).absolutePath();
    fileUrl = QFileDialog::getOpenFileName(this, tr("Select a CA Certificate File"), openPath, tr("Certificate File") + " (*.crt)");

    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.Ringtone.ringtonePath = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
        ui->btnRingtoneSIP->setText(QFileInfo(fileUrl).fileName());

    }*/
}

void
AdvancedSIPSettingsWidget::btnSIPUserCertClicked()
{
}

void
AdvancedSIPSettingsWidget::btnSIPPrivateKeyClicked()
{
}
