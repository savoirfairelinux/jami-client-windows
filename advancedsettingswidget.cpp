/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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
#include "advancedsettingswidget.h"
#include "ui_advancedsettingswidget.h"

#include <QFileDialog>

#include "api/newcodecmodel.h"

#include "lrcinstance.h"
#include "utils.h"

AdvancedSettingsWidget::AdvancedSettingsWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AdvancedSettingsWidget)
{
    ui->setupUi(this);

    // call settings
    connect(ui->checkBoxUntrusted,
        &QAbstractButton::clicked,
        this,
        &AdvancedSettingsWidget::setCallsUntrusted);
    connect(ui->checkBoxCustomRingtone,
        &QAbstractButton::clicked,
        this,
        &AdvancedSettingsWidget::setEnableRingtone);
    connect(ui->checkBoxAutoAnswer,
        &QAbstractButton::clicked,
        this,
        &AdvancedSettingsWidget::setAutoAnswerCalls);
    connect(ui->btnRingtone,
        &QPushButton::clicked,
        this,
        &AdvancedSettingsWidget::openFileCustomRingtone);
    connect(ui->checkBoxCustomRingtone,
        &QAbstractButton::clicked,
        [this](int state) { ui->btnRingtone->setEnabled((bool)state); });

    // name server
    connect(ui->lineEditNameServer,
        &QLineEdit::textEdited,
        this,
        &AdvancedSettingsWidget::setNameServer);

    // openDHT config
    connect(ui->checkBoxEnableProxy,
        &QAbstractButton::clicked,
        this,
        &AdvancedSettingsWidget::setEnableProxy);
    connect(ui->lineEditProxy,
        &QLineEdit::textEdited,
        this,
        &AdvancedSettingsWidget::setProxyAddress);
    connect(ui->lineEditBootstrap,
        &QLineEdit::textEdited,
        this,
        &AdvancedSettingsWidget::setBootstrapAddress);

    // security
    connect(ui->btnCACert,
        &QPushButton::clicked,
        this,
        &AdvancedSettingsWidget::openFileCACert);
    connect(ui->btnUserCert,
        &QPushButton::clicked,
        this,
        &AdvancedSettingsWidget::openFileUserCert);
    connect(ui->btnPrivateKey,
        &QPushButton::clicked,
        this,
        &AdvancedSettingsWidget::openFilePrivateKey);

    // connectivity
    connect(ui->checkBoxUPnP,
        &QAbstractButton::clicked,
        this,
        &AdvancedSettingsWidget::setUseUPnP);
    connect(ui->checkBoxTurnEnable,
        &QAbstractButton::clicked,
        this,
        &AdvancedSettingsWidget::setUseTURN);
    connect(ui->checkBoxSTUNEnable,
        &QAbstractButton::clicked,
        this,
        &AdvancedSettingsWidget::setUseSTUN);

    connect(ui->lineEditTurnAddress,
        &QLineEdit::textEdited,
        this,
        &AdvancedSettingsWidget::setTURNAddress);
    connect(ui->lineEditTurnUsername,
        &QLineEdit::textEdited,
        this,
        &AdvancedSettingsWidget::setTURNUsername);
    connect(ui->lineEditTurnPsswd,
        &QLineEdit::textEdited,
        this,
        &AdvancedSettingsWidget::setTURNPsswd);
    connect(ui->lineEditSTUNAddress,
        &QLineEdit::textEdited,
        this,
        &AdvancedSettingsWidget::setSTUNAddress);

    // codecs
    connect(ui->audioListWidget,
        &QListWidget::itemChanged,
        this,
        &AdvancedSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidget,
        &QListWidget::itemChanged,
        this,
        &AdvancedSettingsWidget::videoCodecsStateChange);

    connect(ui->videoCheckBox,
        &QAbstractButton::clicked,
        this,
        &AdvancedSettingsWidget::setVideoState);

    // codec priority setting
    connect(ui->audioDownPushButton,
        &QPushButton::clicked,
        this,
        &AdvancedSettingsWidget::decreaseAudioCodecPriority);
    connect(ui->audioUpPushButton,
        &QPushButton::clicked,
        this,
        &AdvancedSettingsWidget::increaseAudioCodecPriority);
    connect(ui->videoDownPushButton,
        &QPushButton::clicked,
        this,
        &AdvancedSettingsWidget::decreaseVideoCodecPriority);
    connect(ui->videoUpPushButton,
        &QPushButton::clicked,
        this,
        &AdvancedSettingsWidget::increaseVideoCodecPriority);
}

AdvancedSettingsWidget::~AdvancedSettingsWidget()
{
    delete ui;
}

void AdvancedSettingsWidget::updateAdvancedSettings()
{
    accountId_ = LRCInstance::getCurrAccId();
    auto config = LRCInstance::getCurrAccConfig();

    // Call Settings
    ui->checkBoxUntrusted->setChecked(config.allowIncoming);
    ui->checkBoxAutoAnswer->setChecked(config.autoAnswer);
    ui->checkBoxCustomRingtone->setChecked(config.Ringtone.ringtoneEnabled);

    // Name Server
    ui->lineEditNameServer->setText(QString::fromStdString(config.RingNS.uri));

    // OpenDHT Config
    ui->checkBoxEnableProxy->setChecked(config.proxyEnabled);
    ui->lineEditProxy->setText(QString::fromStdString(config.proxyServer));
    ui->lineEditBootstrap->setText(QString::fromStdString(config.hostname));

    // Security
    ui->btnCACert->setText(
        QFileInfo(QString::fromStdString(config.TLS.certificateListFile))
            .fileName());
    ui->btnUserCert->setText(
        QFileInfo(QString::fromStdString(config.TLS.certificateFile)).fileName());
    ui->btnPrivateKey->setText(
        QFileInfo(QString::fromStdString(config.TLS.privateKeyFile)).fileName());

    // Connectivity
    ui->checkBoxUPnP->setChecked(config.upnpEnabled);
    ui->checkBoxTurnEnable->setChecked(config.TURN.enable);
    ui->lineEditTurnAddress->setText(QString::fromStdString(config.TURN.server));
    ui->lineEditTurnUsername->setText(
        QString::fromStdString(config.TURN.username));
    ui->lineEditTurnPsswd->setText(QString::fromStdString(config.TURN.password));
    ui->checkBoxSTUNEnable->setChecked(config.STUN.enable);
    ui->lineEditSTUNAddress->setText(QString::fromStdString(config.STUN.server));

    // codecs
    ui->videoCheckBox->setChecked(config.Video.videoEnabled);
    disconnect(ui->audioListWidget,
        &QListWidget::itemChanged,
        this,
        &AdvancedSettingsWidget::audioCodecsStateChange);
    disconnect(ui->videoListWidget,
        &QListWidget::itemChanged,
        this,
        &AdvancedSettingsWidget::videoCodecsStateChange);
    updateAudioCodecs();
    updateVideoCodecs();
    connect(ui->audioListWidget,
        &QListWidget::itemChanged,
        this,
        &AdvancedSettingsWidget::audioCodecsStateChange);
    connect(ui->videoListWidget,
        &QListWidget::itemChanged,
        this,
        &AdvancedSettingsWidget::videoCodecsStateChange);

    ui->btnRingtone->setEnabled(config.Ringtone.ringtoneEnabled);
    ui->btnRingtone->setText(
        QFileInfo(QString::fromStdString(config.Ringtone.ringtonePath)).fileName());
    ui->lineEditProxy->setEnabled(config.proxyEnabled);
    ui->lineEditSTUNAddress->setEnabled(config.STUN.enable);
}

// call settings
void AdvancedSettingsWidget::setCallsUntrusted(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.allowIncoming = state;
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
void AdvancedSettingsWidget::setAutoAnswerCalls(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.autoAnswer = state;
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
void AdvancedSettingsWidget::setEnableRingtone(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.Ringtone.ringtoneEnabled = state;
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}

void AdvancedSettingsWidget::openFileCustomRingtone()
{
    QString fileUrl;
    auto oldPath = QString::fromStdString(
        LRCInstance::getCurrAccConfig().Ringtone.ringtonePath);
    auto openPath = oldPath.isEmpty()
        ? QDir::currentPath() + QString("/ringtones/")
        : QFileInfo(oldPath).absolutePath();
    fileUrl = QFileDialog::getOpenFileName(
        this,
        QString("Select a new ringtone"),
        openPath,
        tr("Audio Files (*.wav *.ogg *.opus *.mp3 *aiff *wma)"));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
        confProps.Ringtone.ringtonePath = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(accountId_,
            confProps);
        ui->btnRingtone->setText(QFileInfo(fileUrl).fileName());
    } else if (oldPath.isEmpty()) {
        ui->btnRingtone->setText(tr("Add a custom ringtone"));
    }
}

// name server
void AdvancedSettingsWidget::setNameServer(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.RingNS.uri = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}

// openDHT config
void AdvancedSettingsWidget::setEnableProxy(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.proxyEnabled = state;
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);

    state ? ui->lineEditProxy->setEnabled(true)
          : ui->lineEditProxy->setEnabled(false);
}
void AdvancedSettingsWidget::setProxyAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.proxyServer = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
void AdvancedSettingsWidget::setBootstrapAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.hostname = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}

// security

// connectivity
void AdvancedSettingsWidget::setUseUPnP(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.upnpEnabled = state;
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
void AdvancedSettingsWidget::setUseTURN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.TURN.enable = state;
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
void AdvancedSettingsWidget::setUseSTUN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.STUN.enable = state;
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);

    state ? ui->lineEditSTUNAddress->setEnabled(true)
          : ui->lineEditSTUNAddress->setEnabled(false);
}

void AdvancedSettingsWidget::setTURNAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.TURN.server = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
void AdvancedSettingsWidget::setTURNUsername(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.TURN.username = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
void AdvancedSettingsWidget::setTURNPsswd(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.TURN.password = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
void AdvancedSettingsWidget::setSTUNAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.STUN.server = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}

void AdvancedSettingsWidget::openFileCACert()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(
        this,
        QString("Select a CA certificate"),
        QDir::homePath() + QString("/.local/share/ring/") + QString::fromStdString(LRCInstance::getCurrentAccountInfo().id));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
        confProps.TLS.certificateListFile = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(accountId_,
            confProps);
    }
    ui->btnCACert->setText(
        QFileInfo(QString::fromStdString(
                      LRCInstance::getCurrAccConfig().TLS.certificateListFile))
            .fileName());
}

void AdvancedSettingsWidget::openFileUserCert()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(
        this,
        QString("Select a user certificate"),
        QDir::homePath() + QString("/.local/share/ring/") + QString::fromStdString(LRCInstance::getCurrentAccountInfo().id));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
        confProps.TLS.certificateFile = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(accountId_,
            confProps);
    }
    ui->btnUserCert->setText(
        QFileInfo(QString::fromStdString(
                      LRCInstance::getCurrAccConfig().TLS.certificateFile))
            .fileName());
}

void AdvancedSettingsWidget::openFilePrivateKey()
{
    QString fileUrl;
    fileUrl = QFileDialog::getOpenFileName(
        this,
        QString("Select a private key"),
        QDir::homePath() + QString("/.local/share/ring/") + QString::fromStdString(LRCInstance::getCurrentAccountInfo().id));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
        confProps.TLS.privateKeyFile = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(accountId_,
            confProps);
    }
    ui->btnPrivateKey->setText(
        QFileInfo(QString::fromStdString(
                      LRCInstance::getCurrAccConfig().TLS.privateKeyFile))
            .fileName());
}

void AdvancedSettingsWidget::updateAudioCodecs()
{
    ui->audioListWidget->clear();

    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    int i = 0;
    for (auto it = audioCodecList.begin(); it != audioCodecList.end();
         ++it, ++i) {
        QListWidgetItem* audioItem = new QListWidgetItem(ui->audioListWidget);
        audioItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        Qt::CheckState state;
        it->enabled ? state = Qt::Checked : state = Qt::Unchecked;
        audioItem->setCheckState(state);
        audioItem->setData(Qt::DisplayRole,
            QString::fromStdString(it->name) + "\n" + QString::fromStdString(it->samplerate) + " Hz");

        ui->audioListWidget->addItem(audioItem);
    }
}

void AdvancedSettingsWidget::updateVideoCodecs()
{
    ui->videoListWidget->clear();

    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    int i = 0;

    for (auto it = videoCodecList.begin(); it != videoCodecList.end();
         ++it, ++i) {
        if (it->name.length()) { // [temporary fix]
            QListWidgetItem* videoItem = new QListWidgetItem(ui->videoListWidget);
            videoItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            Qt::CheckState state;
            it->enabled ? state = Qt::Checked : state = Qt::Unchecked;
            videoItem->setCheckState(state);
            videoItem->setData(Qt::DisplayRole,
                QString::fromStdString(it->name) + "\n");

            ui->audioListWidget->addItem(videoItem);
        }
    }
}

void AdvancedSettingsWidget::audioCodecsStateChange(QListWidgetItem* item)
{
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    std::advance(it, ui->audioListWidget->row(item));

    LRCInstance::getCurrentAccountInfo().codecModel->enable(it->id,
        !(it->enabled));
}

void AdvancedSettingsWidget::videoCodecsStateChange(QListWidgetItem* item)
{
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    std::advance(it, ui->videoListWidget->row(item));

    LRCInstance::getCurrentAccountInfo().codecModel->enable(it->id,
        !(it->enabled));
}

void AdvancedSettingsWidget::decreaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidget->row(ui->audioListWidget->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id,
        false);
    updateAudioCodecs();
}

void AdvancedSettingsWidget::increaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidget->row(ui->audioListWidget->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id,
        false);
    updateAudioCodecs();
}

void AdvancedSettingsWidget::decreaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidget->row(ui->videoListWidget->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id,
        true);
    updateVideoCodecs();
}

void AdvancedSettingsWidget::increaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidget->row(ui->videoListWidget->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id,
        true);
    updateVideoCodecs();
}

void AdvancedSettingsWidget::setVideoState(int state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(accountId_);
    confProps.Video.videoEnabled = (bool)state;
    LRCInstance::editableAccountModel()->setAccountConfig(accountId_, confProps);
}
