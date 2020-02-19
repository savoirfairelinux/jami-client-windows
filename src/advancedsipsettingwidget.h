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

#pragma once

#include <QListWidgetItem>
#include <QWidget>
#include <QPushButton>

namespace Ui {
class AdvancedSIPSettingsWidget;
}

class AdvancedSIPSettingsWidget : public QWidget {

    Q_OBJECT
    AdvancedSIPSettingsWidget(const AdvancedSIPSettingsWidget& cpy);

public:
    AdvancedSIPSettingsWidget(QWidget* parent = nullptr);
    ~AdvancedSIPSettingsWidget();

    void updateAdvancedSIPSettings();
    QString openButtonFilePath(const QString& accConfigFilePath,
                                   const char* windowTitle,
                                   const char* fileTypeDesp,
                                   const QString& fileTypeFilter,
                                   QPushButton* button);

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::AdvancedSIPSettingsWidget* ui;

private slots:

    // call settings
    void setAutoAnswerCalls(bool state);
    void setEnableRingtone(bool state);

    void openFileCustomRingtone();

    // security
    void setUseSRTP(bool state);
    void setUseSDES(bool state);
    void setUseRTPFallback(bool state);
    void setUseTLS(bool state);
    void setVerifyCertificatesServer(bool state);
    void setVerifyCertificatesClient(bool state);
    void setRequireCertificatesIncomingTLS(bool state);

    void btnSIPCAClicked();
    void btnSIPUserCertClicked();
    void btnSIPPrivateKeyClicked();

    void lineEditSIPCertPasswordLineEditTextChanged();
    void tlsProtocolComboBoxIndexChanged(const int& index);
    void outgoingTLSServerNameLineEditTextChanged();
    void negotiationTimeoutSpinBoxValueChanged();

    // connectivity
    void setUseUPnP(bool state);
    void setUseTURN(bool state);
    void setUseSTUN(bool state);

    void registrationTimeoutSpinBoxValueChanged();
    void networkInterfaceSpinBoxValueChanged();
    void setTURNAddress();
    void setTURNUsername();
    void setTURNPassword();
    void setTURNRealm();
    void setSTUNAddress();

    // published address
    void setUseCustomAddressAndPort(bool state);

    void lineEditSIPCustomAddressLineEditTextChanged();
    void customPortSIPSpinBoxValueChanged();

    // codecs
    void updateAudioCodecs();
    void updateVideoCodecs();

    void audioCodecsStateChange(QListWidgetItem* item = nullptr);
    void videoCodecsStateChange(QListWidgetItem* item = nullptr);

    void decreaseAudioCodecPriority();
    void increaseAudioCodecPriority();

    void decreaseVideoCodecPriority();
    void increaseVideoCodecPriority();

    void setVideoState(int state);

    // SDP session negotiation ports
    void audioRTPMinPortSpinBoxEditFinished();
    void audioRTPMaxPortSpinBoxEditFinished();
    void videoRTPMinPortSpinBoxEditFinished();
    void videoRTPMaxPortSpinBoxEditFinished();

    // voicemail
    void lineEditVoiceMailDialCodeEditFinished();
};
