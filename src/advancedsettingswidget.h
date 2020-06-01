/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                           *
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

#pragma once

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
    class AdvancedSettingsWidget;
}

class AdvancedSettingsWidget : public QWidget {

    Q_OBJECT
        AdvancedSettingsWidget(const AdvancedSettingsWidget& cpy);

public:
    AdvancedSettingsWidget(QWidget* parent = nullptr);
    ~AdvancedSettingsWidget();

    void updateAdvancedSettings();

private:
    Ui::AdvancedSettingsWidget* ui;

private slots:
    // call settings
    void setAutoConnectOnLocalNetwork(bool state);
    void setCallsUntrusted(bool state);
    void setAutoAnswerCalls(bool state);
    void setEnableRingtone(bool state);
    void openFileCustomRingtone();
    void setNameServer();
    void setEnableProxy(bool state);
    void setProxyAddress();
    void setBootstrapAddress();
    void openFileCACert();
    void openFileUserCert();
    void openFilePrivateKey();
    void setUseUPnP(bool state);
    void setUseTURN(bool state);
    void setUseSTUN(bool state);
    void setTURNAddress();
    void setTURNUsername();
    void setTURNPassword();
    void setSTUNAddress();
    void updateAudioCodecs();
    void updateVideoCodecs();
    void audioCodecsStateChange(QListWidgetItem* item = nullptr);
    void videoCodecsStateChange(QListWidgetItem* item = nullptr);
    void decreaseAudioCodecPriority();
    void increaseAudioCodecPriority();
    void decreaseVideoCodecPriority();
    void increaseVideoCodecPriority();
    void setVideoState(int state);

};