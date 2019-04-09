/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                           *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>
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

#include "api/newcodecmodel.h"

#include "lrcinstance.h"
#include "utils.h"

AdvancedSIPSettingsWidget::AdvancedSIPSettingsWidget(QWidget* parent)
    :QWidget(parent),
    ui(new Ui::AdvancedSIPSettingsWidget)
{
    ui->setupUi(this);

	///////////////////////////////////////////////////////////////////////////////
    // call settings
    connect(ui->checkBoxCustomRingtoneSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setEnableRingtone);
    connect(ui->checkBoxAutoAnswerSIP, &QAbstractButton::clicked, this, &AdvancedSIPSettingsWidget::setAutoAnswerCalls);
    connect(ui->btnRingtoneSIP, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::openFileCustomRingtone);
    connect(ui->checkBoxCustomRingtoneSIP, &QAbstractButton::clicked, [this](int state) { ui->btnRingtoneSIP->setEnabled((bool)state); });

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

    connect(ui->videoDownPushButtonSIP, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::decreaseVideoCodecPriority);
    connect(ui->videoUpPushButtonSIP, &QPushButton::clicked, this, &AdvancedSIPSettingsWidget::increaseVideoCodecPriority);

}

AdvancedSIPSettingsWidget::~AdvancedSIPSettingsWidget()
{
    delete ui;
}

void
AdvancedSIPSettingsWidget::updateAdvancedSIPSettings()
{
    auto config = LRCInstance::getCurrAccConfig();
    //Call Settings
    ui->checkBoxAutoAnswerSIP->setChecked(config.autoAnswer);
    ui->checkBoxCustomRingtoneSIP->setChecked(config.Ringtone.ringtoneEnabled);

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
void
AdvancedSIPSettingsWidget::setAutoAnswerCalls(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.autoAnswer = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setEnableRingtone(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Ringtone.ringtoneEnabled = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::openFileCustomRingtone()
{
    QString fileUrl;
    auto oldPath = QString::fromStdString(LRCInstance::getCurrAccConfig().Ringtone.ringtonePath);
    auto openPath = oldPath.isEmpty() ? QDir::currentPath() + QString("/ringtones/") : QFileInfo(oldPath).absolutePath();
    fileUrl = QFileDialog::getOpenFileName(this, QString("Select a new ringtone"), openPath, tr("Audio Files (*.wav *.ogg *.opus *.mp3 *aiff *wma)"));
    if (!fileUrl.isEmpty()) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.Ringtone.ringtonePath = fileUrl.toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
        ui->btnRingtoneSIP->setText(QFileInfo(fileUrl).fileName());
    } else if (oldPath.isEmpty()) {
        ui->btnRingtoneSIP->setText(tr("Add a custom ringtone"));
    }
}

// connectivity
void
AdvancedSIPSettingsWidget::setUseUPnP(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.upnpEnabled = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setUseTURN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.enable = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setUseSTUN(bool state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.enable = state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);

    state ? ui->lineEditSTUNAddressSIP->setEnabled(true) : ui->lineEditSTUNAddressSIP->setEnabled(false);
}

void
AdvancedSIPSettingsWidget::setTURNAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.server = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setTURNUsername(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.username = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setTURNPsswd(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.TURN.password = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
void
AdvancedSIPSettingsWidget::setSTUNAddress(const QString& name)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.STUN.server = name.toStdString();
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
AdvancedSIPSettingsWidget::updateAudioCodecs()
{
    ui->audioListWidgetSIP->clear();

    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    int i = 0;
    for (auto it = audioCodecList.begin(); it != audioCodecList.end(); ++it, ++i) {
        QListWidgetItem* audioItem = new QListWidgetItem(ui->audioListWidgetSIP);
        audioItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        Qt::CheckState state;
        it->enabled ? state = Qt::Checked : state = Qt::Unchecked;
        audioItem->setCheckState(state);
        audioItem->setData(Qt::DisplayRole, QString::fromStdString(it->name) + "\n" + QString::fromStdString(it->samplerate)
            + " Hz");

        ui->audioListWidgetSIP->addItem(audioItem);
    }
}

void
AdvancedSIPSettingsWidget::updateVideoCodecs()
{
    ui->videoListWidgetSIP->clear();

    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    int i = 0;

    for (auto it = videoCodecList.begin(); it != videoCodecList.end(); ++it, ++i) {
        if (it->name.length()) { // [temporary fix]
            QListWidgetItem* videoItem = new QListWidgetItem(ui->videoListWidgetSIP);
            videoItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            Qt::CheckState state;
            it->enabled ? state = Qt::Checked : state = Qt::Unchecked;
            videoItem->setCheckState(state);
            videoItem->setData(Qt::DisplayRole, QString::fromStdString(it->name) + "\n");

            ui->audioListWidgetSIP->addItem(videoItem);
        }
    }
}

void
AdvancedSIPSettingsWidget::audioCodecsStateChange(QListWidgetItem* item)
{
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    std::advance(it, ui->audioListWidgetSIP->row(item));

    LRCInstance::getCurrentAccountInfo().codecModel->enable(it->id, !(it->enabled));
}

void
AdvancedSIPSettingsWidget::videoCodecsStateChange(QListWidgetItem* item)
{
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    std::advance(it, ui->videoListWidgetSIP->row(item));

    LRCInstance::getCurrentAccountInfo().codecModel->enable(it->id, !(it->enabled));
}

void
AdvancedSIPSettingsWidget::decreaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidgetSIP->row(ui->audioListWidgetSIP->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, false);
    updateAudioCodecs();
}

void
AdvancedSIPSettingsWidget::increaseAudioCodecPriority()
{
    int selectedRow = ui->audioListWidgetSIP->row(ui->audioListWidgetSIP->selectedItems().at(0));
    auto audioCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getAudioCodecs();
    auto it = audioCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id, false);
    updateAudioCodecs();
}

void
AdvancedSIPSettingsWidget::decreaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidgetSIP->row(ui->videoListWidgetSIP->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->decreasePriority(it->id, true);
    updateVideoCodecs();
}

void
AdvancedSIPSettingsWidget::increaseVideoCodecPriority()
{
    int selectedRow = ui->videoListWidgetSIP->row(ui->videoListWidgetSIP->selectedItems().at(0));
    auto videoCodecList = LRCInstance::getCurrentAccountInfo().codecModel->getVideoCodecs();
    auto it = videoCodecList.begin();

    advance(it, selectedRow);
    LRCInstance::getCurrentAccountInfo().codecModel->increasePriority(it->id, true);
    updateVideoCodecs();
}

void
AdvancedSIPSettingsWidget::setVideoState(int state)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.Video.videoEnabled = (bool)state;
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}
