/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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
#include "advancedsettingswidget.h"
#include "ui_advancedsettingswidget.h"

#include <QFileDialog>

#include "lrcinstance.h"
#include "utils.h"

#include "api/newcodecmodel.h"

AdvancedSettingsWidget::AdvancedSettingsWidget(QWidget* parent)
    :QWidget(parent),
    ui(new Ui::AdvancedSettingsWidget)
{
    ui->setupUi(this);

///////////////////////////////////////////////////////////////////////////////
    // call settings
    connect(ui->checkBoxUntrusted, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setCallsUntrusted);
    connect(ui->checkBoxCustomRingtone, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setEnableRingtone);
    connect(ui->checkBoxAutoAnswer, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setAutoAnswerCalls);
    connect(ui->btnRingtone, &QPushButton::clicked, this, &AdvancedSettingsWidget::openFileCustomRingtone);
    connect(ui->checkBoxCustomRingtone, &QAbstractButton::clicked, [this](int state) { ui->btnRingtone->setEnabled((bool)state); });

    // name server
    connect(ui->lineEditNameServer, &QLineEdit::editingFinished, this, &AdvancedSettingsWidget::setNameServer);

    // openDHT config
    connect(ui->checkBoxEnableProxy, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setEnableProxy);

    connect(ui->lineEditProxy, &QLineEdit::editingFinished, this, &AdvancedSettingsWidget::setProxyAddress);
    connect(ui->lineEditBootstrap, &QLineEdit::editingFinished, this, &AdvancedSettingsWidget::setBootstrapAddress);

    // security
    connect(ui->btnCACert, &QPushButton::clicked, this, &AdvancedSettingsWidget::openFileCACert);
    connect(ui->btnUserCert, &QPushButton::clicked, this, &AdvancedSettingsWidget::openFileUserCert);
    connect(ui->btnPrivateKey, &QPushButton::clicked, this, &AdvancedSettingsWidget::openFilePrivateKey);

    // connectivity
    connect(ui->checkBoxUPnP, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setUseUPnP);
    connect(ui->checkBoxTurnEnable, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setUseTURN);
    connect(ui->checkBoxSTUNEnable, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setUseSTUN);

    connect(ui->lineEditTurnAddress, &QLineEdit::editingFinished, this, &AdvancedSettingsWidget::setTURNAddress);
    connect(ui->lineEditTurnUsername, &QLineEdit::editingFinished, this, &AdvancedSettingsWidget::setTURNUsername);
    connect(ui->lineEditTurnPassword, &QLineEdit::editingFinished, this, &AdvancedSettingsWidget::setTURNPassword);
    connect(ui->lineEditSTUNAddress, &QLineEdit::editingFinished, this, &AdvancedSettingsWidget::setSTUNAddress);

    // codecs
    connect(ui->audioListWidget, &QListWidget::itemChanged, this, &AdvancedSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidget, &QListWidget::itemChanged, this, &AdvancedSettingsWidget::videoCodecsStateChange);

    connect(ui->videoCheckBox, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setVideoState);

    // codec priority setting
    connect(ui->audioDownPushButton, &QPushButton::clicked, this, &AdvancedSettingsWidget::decreaseAudioCodecPriority);
    connect(ui->audioUpPushButton, &QPushButton::clicked, this, &AdvancedSettingsWidget::increaseAudioCodecPriority);

    ui->audioDownPushButton->setEnabled(false);
    ui->audioUpPushButton->setEnabled(false);

    connect(ui->audioListWidget, &QListWidget::itemSelectionChanged,
        [this] {
            bool enabled = ui->audioListWidget->selectedItems().size();
            ui->audioDownPushButton->setEnabled(enabled);
            ui->audioUpPushButton->setEnabled(enabled);
        });

    connect(ui->videoDownPushButton, &QPushButton::clicked, this, &AdvancedSettingsWidget::decreaseVideoCodecPriority);
    connect(ui->videoUpPushButton, &QPushButton::clicked, this, &AdvancedSettingsWidget::increaseVideoCodecPriority);

    ui->videoDownPushButton->setEnabled(false);
    ui->videoUpPushButton->setEnabled(false);

    connect(ui->videoListWidget, &QListWidget::itemSelectionChanged,
        [this] {
            bool enabled = ui->videoListWidget->selectedItems().size();
            ui->videoDownPushButton->setEnabled(enabled);
            ui->videoUpPushButton->setEnabled(enabled);
        });
}

AdvancedSettingsWidget::~AdvancedSettingsWidget()
{
    delete ui;
}

void
AdvancedSettingsWidget::updateAdvancedSettings()
{
    auto config = LRCInstance::getCurrAccConfig();
    //Call Settings
    ui->checkBoxUntrusted->setChecked(config.DHT.PublicInCalls);
    ui->checkBoxAutoAnswer->setChecked(config.autoAnswer);
    ui->checkBoxCustomRingtone->setChecked(config.Ringtone.ringtoneEnabled);

    // Name Server
    ui->lineEditNameServer->setText(config.RingNS.uri);

    //OpenDHT Config
    ui->checkBoxEnableProxy->setChecked(config.proxyEnabled);
    ui->lineEditProxy->setText(config.proxyServer);
    ui->lineEditBootstrap->setText(config.hostname);

    // Security
    ui->btnCACert->setText(QFileInfo(LRCInstance::getCurrAccConfig().TLS.certificateListFile).fileName());
    ui->btnUserCert->setText(QFileInfo(LRCInstance::getCurrAccConfig().TLS.certificateFile).fileName());
    ui->btnPrivateKey->setText(QFileInfo(LRCInstance::getCurrAccConfig().TLS.privateKeyFile).fileName());

    // Connectivity
    ui->checkBoxUPnP->setChecked(config.upnpEnabled);
    ui->checkBoxTurnEnable->setChecked(config.TURN.enable);
    ui->lineEditTurnAddress->setText(config.TURN.server);
    ui->lineEditTurnUsername->setText(config.TURN.username);
    ui->lineEditTurnPassword->setText(config.TURN.password);
    ui->checkBoxSTUNEnable->setChecked(config.STUN.enable);
    ui->lineEditSTUNAddress->setText(config.STUN.server);

    // codecs
    ui->videoCheckBox->setChecked(config.Video.videoEnabled);
    disconnect(ui->audioListWidget, &QListWidget::itemChanged, this, &AdvancedSettingsWidget::audioCodecsStateChange);
    disconnect(ui->videoListWidget, &QListWidget::itemChanged, this, &AdvancedSettingsWidget::videoCodecsStateChange);
    updateAudioCodecs();
    updateVideoCodecs();
    connect(ui->audioListWidget, &QListWidget::itemChanged, this, &AdvancedSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidget, &QListWidget::itemChanged, this, &AdvancedSettingsWidget::videoCodecsStateChange);

    ui->btnRingtone->setEnabled(LRCInstance::getCurrAccConfig().Ringtone.ringtoneEnabled);
    ui->btnRingtone->setText(QFileInfo(LRCInstance::getCurrAccConfig().Ringtone.ringtonePath).fileName());
    ui->lineEditProxy->setEnabled(LRCInstance::getCurrAccConfig().proxyEnabled);
    ui->lineEditSTUNAddress->setEnabled(LRCInstance::getCurrAccConfig().STUN.enable);
}

// call settings
void
AdvancedSettingsWidget::setCallsUntrusted(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.DHT.PublicInCalls = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setAutoAnswerCalls(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.autoAnswer = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setEnableRingtone(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Ringtone.ringtoneEnabled = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSettingsWidget::openFileCustomRingtone()
{
    QString fileUrl;
    auto oldPath = LRCInstance::getCurrAccConfig().Ringtone.ringtonePath;
    auto openPath = oldPath.isEmpty() ? QDir::currentPath() + QString("/ringtones/") : QFileInfo(oldPath).absolutePath();
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a new ringtone"), openPath, tr("Audio Files") + " (*.wav *.ogg *.opus *.mp3 *.aiff *.wma)");
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.Ringtone.ringtonePath = fileUrl;
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
        ui->btnRingtone->setText(QFileInfo(fileUrl).fileName());
    } else if (oldPath.isEmpty()) {
        ui->btnRingtone->setText(tr("Add a custom ringtone"));
    }
}

// name server
void
AdvancedSettingsWidget::setNameServer()
{
    auto text = ui->lineEditNameServer->text();
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.RingNS.uri = text;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

// openDHT config
void
AdvancedSettingsWidget::setEnableProxy(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.proxyEnabled = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);

    state ? ui->lineEditProxy->setEnabled(true) : ui->lineEditProxy->setEnabled(false);
}
void
AdvancedSettingsWidget::setProxyAddress()
{
    auto text = ui->lineEditProxy->text();
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.proxyServer = text;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setBootstrapAddress()
{
    auto text = ui->lineEditBootstrap->text();
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.hostname = text;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

// security

// connectivity
void
AdvancedSettingsWidget::setUseUPnP(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.upnpEnabled = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setUseTURN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.enable = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setUseSTUN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.enable = state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);

    state ? ui->lineEditSTUNAddress->setEnabled(true) : ui->lineEditSTUNAddress->setEnabled(false);
}

void
AdvancedSettingsWidget::setTURNAddress()
{
    auto text = ui->lineEditTurnAddress->text();
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.server = text;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setTURNUsername()
{
    auto text = ui->lineEditTurnUsername->text();
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.username = text;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setTURNPassword()
{
    auto text = ui->lineEditTurnPassword->text();
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.password = text;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setSTUNAddress()
{
    auto text = ui->lineEditSTUNAddress->text();
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.server = text;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSettingsWidget::openFileCACert()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a CA certificate"), QDir::homePath()
        + QString("/.local/share/ring/") + LRCInstance::getCurrentAccountInfo().id);
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.TLS.certificateListFile = fileUrl;
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
    ui->btnCACert->setText(QFileInfo(LRCInstance::getCurrAccConfig().TLS.certificateListFile).fileName());
}

void
AdvancedSettingsWidget::openFileUserCert()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a user certificate"), QDir::homePath()
        + QString("/.local/share/ring/") + LRCInstance::getCurrentAccountInfo().id);
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.TLS.certificateFile = fileUrl;
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
    ui->btnUserCert->setText(QFileInfo(LRCInstance::getCurrAccConfig().TLS.certificateFile).fileName());
}

void
AdvancedSettingsWidget::openFilePrivateKey()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a private key"), QDir::homePath()
        + QString("/.local/share/ring/") + LRCInstance::getCurrentAccountInfo().id);
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.TLS.privateKeyFile = fileUrl;
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
    ui->btnPrivateKey->setText(QFileInfo(LRCInstance::getCurrAccConfig().TLS.privateKeyFile).fileName());
}

void
AdvancedSettingsWidget::updateAudioCodecs()
{
    ui->audioListWidget->clear();

    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    int i = 0;
    for (auto it = audioCodecList.begin(); it != audioCodecList.end(); ++it, ++i) {
        QListWidgetItem* audioItem = new QListWidgetItem(ui->audioListWidget);
        audioItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        audioItem->setCheckState(it->enabled ? Qt::Checked : Qt::Unchecked);
        audioItem->setData(Qt::DisplayRole, it->name + " " + it->samplerate + " Hz");

        ui->audioListWidget->addItem(audioItem);
    }
}

void
AdvancedSettingsWidget::updateVideoCodecs()
{
    ui->videoListWidget->clear();
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    int i = 0;
    for (auto it = videoCodecList.begin(); it != videoCodecList.end(); ++it, ++i) {
        if (!it->name.length()) { continue; } // temporary fix for empty codec entries
        QListWidgetItem* videoItem = new QListWidgetItem(ui->videoListWidget);
        videoItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        videoItem->setCheckState(it->enabled ? Qt::Checked : Qt::Unchecked);
        videoItem->setData(Qt::DisplayRole, it->name);
        ui->audioListWidget->addItem(videoItem);
    }
}

void
AdvancedSettingsWidget::audioCodecsStateChange(QListWidgetItem* item)
{
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    std::advance(it, ui->audioListWidget->row(item));

    LRCInstance::getCurrentAccountInfo().codecModel->enable(it->id, !(it->enabled));
}

void
AdvancedSettingsWidget::videoCodecsStateChange(QListWidgetItem* item)
{
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    std::advance(it, ui->videoListWidget->row(item));

    LRCInstance::getCurrentAccountInfo().codecModel->enable(it->id, !(it->enabled));
}

void
AdvancedSettingsWidget::decreaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidget->row(ui->audioListWidget->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    std::advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, false);

    // swap current item down
    Utils::swapQListWidgetItems(ui->audioListWidget, true);
}

void
AdvancedSettingsWidget::increaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidget->row(ui->audioListWidget->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    std::advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id, false);

    // swap current item up
    Utils::swapQListWidgetItems(ui->audioListWidget, false);
}

void
AdvancedSettingsWidget::decreaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidget->row(ui->videoListWidget->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    std::advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, true);

    // swap current item down
    Utils::swapQListWidgetItems(ui->videoListWidget, true);
}

void
AdvancedSettingsWidget::increaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidget->row(ui->videoListWidget->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    std::advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id, true);

    // swap current item up
    Utils::swapQListWidgetItems(ui->videoListWidget, false);
}

void
AdvancedSettingsWidget::setVideoState(int state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Video.videoEnabled = (bool)state;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
