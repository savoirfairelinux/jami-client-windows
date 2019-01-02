/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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
#include "direct.h"
#include <QFileDialog>

#include "lrcinstance.h"
#include "api/newcodecmodel.h"

#include "advancedsettingswidget.h"
#include "ui_advancedsettingswidget.h"

AdvancedSettingsWidget::AdvancedSettingsWidget(QWidget* parent)
    :QWidget(parent),
    ui(new Ui::AdvancedSettingsWidget)
{
    ui->setupUi(this);

    updateAdvancedSettings();
///////////////////////////////////////////////////////////////////////////////
    // call settings
    connect(ui->checkBoxUntrusted, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setCallsUntrusted);
    connect(ui->checkBoxCustomRingtone, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setEnableRingtone);
    connect(ui->checkBoxAutoAnswer, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setAutoAnswerCalls);
    connect(ui->btnRingtone, &QPushButton::clicked, this, &AdvancedSettingsWidget::openFileCustomRingtone);
    connect(ui->checkBoxCustomRingtone, &QAbstractButton::clicked, [this](int state) { ui->btnRingtone->setEnabled((bool)state); });

    // name server
    connect(ui->lineEditNameServer, &QLineEdit::textChanged, this, &AdvancedSettingsWidget::setNameServer);

    // openDHT config
    connect(ui->checkBoxEnableProxy, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setEnableProxy);

    connect(ui->lineEditProxy, &QLineEdit::textChanged, this, &AdvancedSettingsWidget::setProxyAddress);
    connect(ui->lineEditBootstrap, &QLineEdit::textChanged, this, &AdvancedSettingsWidget::setBootstrapAddress);

    // security
    connect(ui->btnCACert, &QPushButton::clicked, this, &AdvancedSettingsWidget::openFileCACert);
    connect(ui->btnUserCert, &QPushButton::clicked, this, &AdvancedSettingsWidget::openFileUserCert);
    connect(ui->btnPrivateKey, &QPushButton::clicked, this, &AdvancedSettingsWidget::openFilePrivateKey);

    // connectivity
    connect(ui->checkBoxUPnP, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setUseUPnP);
    connect(ui->checkBoxTurnEnable, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setUseTURN);
    connect(ui->checkBoxSTUNEnable, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setUseSTUN);

    connect(ui->lineEditTurnAddress, &QLineEdit::textChanged, this, &AdvancedSettingsWidget::setTURNAddress);
    connect(ui->lineEditTurnUsername, &QLineEdit::textChanged, this, &AdvancedSettingsWidget::setTURNUsername);
    connect(ui->lineEditTurnPsswd, &QLineEdit::textChanged, this, &AdvancedSettingsWidget::setTURNPsswd);
    connect(ui->lineEditSTUNAddress, &QLineEdit::textChanged, this, &AdvancedSettingsWidget::setSTUNAddress);

    // codecs
    connect(ui->audioListWidget, &QListWidget::itemChanged, this, &AdvancedSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidget, &QListWidget::itemChanged, this, &AdvancedSettingsWidget::videoCodecsStateChange);

    connect(ui->videoCheckBox, &QAbstractButton::clicked, this, &AdvancedSettingsWidget::setVideoState);

    // codec priority setting
    connect(ui->audioDownPushButton, &QPushButton::clicked, this, &AdvancedSettingsWidget::decreaseAudioCodecPriority);
    connect(ui->audioUpPushButton, &QPushButton::clicked, this, &AdvancedSettingsWidget::increaseAudioCodecPriority);

    connect(ui->videoDownPushButton, &QPushButton::clicked, this, &AdvancedSettingsWidget::decreaseVideoCodecPriority);
    connect(ui->videoUpPushButton, &QPushButton::clicked, this, &AdvancedSettingsWidget::increaseVideoCodecPriority);


///////////////////////////////////////////////////////////////////////////////
    ui->btnRingtone->setEnabled(LRCInstance::getCurrAccConfig().Ringtone.ringtoneEnabled);
    ui->lineEditProxy->setEnabled(LRCInstance::getCurrAccConfig().proxyEnabled);
    ui->lineEditSTUNAddress->setEnabled(LRCInstance::getCurrAccConfig().STUN.enable);


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
    ui->checkBoxUntrusted->setChecked(config.allowIncoming);
    ui->checkBoxAutoAnswer->setChecked(config.autoAnswer);
    ui->checkBoxCustomRingtone->setChecked(config.Ringtone.ringtoneEnabled);

    // Name Server
    ui->lineEditNameServer->setText(QString::fromStdString(config.RingNS.uri));

    //OpenDHT Config
    ui->checkBoxEnableProxy->setChecked(config.proxyEnabled);
    ui->lineEditProxy->setText(QString::fromStdString(config.proxyServer));
    ui->lineEditBootstrap->setText(QString::fromStdString(config.hostname));

    // Security
    ui->btnCACert->setText(QString::fromStdString(config.TLS.certificateListFile));
    ui->btnUserCert->setText(QString::fromStdString(config.TLS.certificateFile));
    ui->btnPrivateKey->setText(QString::fromStdString(config.TLS.privateKeyFile));

    // Connectivity
    ui->checkBoxUPnP->setChecked(config.upnpEnabled);
    ui->checkBoxTurnEnable->setChecked(config.TURN.enable);
    ui->lineEditTurnAddress->setText(QString::fromStdString(config.TURN.server));
    ui->lineEditTurnUsername->setText(QString::fromStdString(config.TURN.username));
    ui->lineEditTurnPsswd->setText(QString::fromStdString(config.TURN.password));
    ui->checkBoxSTUNEnable->setChecked(config.STUN.enable);
    ui->lineEditSTUNAddress->setText(QString::fromStdString(config.STUN.server));

    // codecs
    ui->videoCheckBox->setChecked(config.Video.videoEnabled);
    updateAudioCodecs();
    updateVideoCodecs();
}

// call settings
void
AdvancedSettingsWidget::setCallsUntrusted(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.allowIncoming = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setAutoAnswerCalls(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.autoAnswer = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setEnableRingtone(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Ringtone.ringtoneEnabled = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSettingsWidget::openFileCustomRingtone()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a new ringtone"), QDir::currentPath() + QString("/ringtones/"));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.Ringtone.ringtonePath = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
        ui->btnRingtone->setText(QString::fromStdString(LRCInstance::getCurrAccConfig().Ringtone.ringtonePath));

    } else {
        ui->btnRingtone->setText(tr("Add a custom ringtone"));
    }
}

// name server
void
AdvancedSettingsWidget::setNameServer(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.RingNS.uri = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

// openDHT config
void
AdvancedSettingsWidget::setEnableProxy(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.proxyEnabled = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);

    state ? ui->lineEditProxy->setEnabled(true) : ui->lineEditProxy->setEnabled(false);
}
void
AdvancedSettingsWidget::setProxyAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.proxyServer = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setBootstrapAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.hostname = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

// security

// connectivity
void
AdvancedSettingsWidget::setUseUPnP(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.upnpEnabled = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setUseTURN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.enable = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setUseSTUN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.enable = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);

    state ? ui->lineEditSTUNAddress->setEnabled(true) : ui->lineEditSTUNAddress->setEnabled(false);
}

void
AdvancedSettingsWidget::setTURNAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.server = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setTURNUsername(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.username = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setTURNPsswd(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.password = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSettingsWidget::setSTUNAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.server = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSettingsWidget::openFileCACert()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a CA certificate"), QDir::homePath()
        + QString("/.local/share/ring/") + QString::fromStdString(LRCInstance::getCurrentAccountInfo().id));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.TLS.certificateListFile = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
    ui->btnCACert->setText(QString::fromStdString(LRCInstance::getCurrAccConfig().TLS.certificateListFile));
}

void
AdvancedSettingsWidget::openFileUserCert()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a user certificate"), QDir::homePath()
        + QString("/.local/share/ring/") + QString::fromStdString(LRCInstance::getCurrentAccountInfo().id));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.TLS.certificateFile = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
    ui->btnUserCert->setText(QString::fromStdString(LRCInstance::getCurrAccConfig().TLS.certificateFile));
}

void
AdvancedSettingsWidget::openFilePrivateKey()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a private key"), QDir::homePath()
        + QString("/.local/share/ring/") + QString::fromStdString(LRCInstance::getCurrentAccountInfo().id));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.TLS.privateKeyFile = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
    ui->btnPrivateKey->setText(QString::fromStdString(LRCInstance::getCurrAccConfig().TLS.privateKeyFile));
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
        Qt::CheckState state;
        it->enabled ? state = Qt::Checked : state = Qt::Unchecked;
        audioItem->setCheckState(state);
        audioItem->setData(Qt::DisplayRole, QString::fromStdString(it->name) + "\n" + QString::fromStdString(it->samplerate)
            + " Hz");

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
        if (it->name.length()) { // [temporary fix]
            QListWidgetItem* videoItem = new QListWidgetItem(ui->videoListWidget);
            videoItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            Qt::CheckState state;
            it->enabled ? state = Qt::Checked : state = Qt::Unchecked;
            videoItem->setCheckState(state);
            videoItem->setData(Qt::DisplayRole, QString::fromStdString(it->name) + "\n");

            ui->audioListWidget->addItem(videoItem);
        }
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

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, false);
    updateAudioCodecs();
}

void
AdvancedSettingsWidget::increaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidget->row(ui->audioListWidget->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id, false);
    updateAudioCodecs();
}

void
AdvancedSettingsWidget::decreaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidget->row(ui->videoListWidget->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, true);
    updateVideoCodecs();
}

void
AdvancedSettingsWidget::increaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidget->row(ui->videoListWidget->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id, true);
    updateVideoCodecs();
}

void
AdvancedSettingsWidget::setVideoState(int state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Video.videoEnabled = (bool)state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}