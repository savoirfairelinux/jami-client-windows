/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
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

private:
    Ui::AdvancedSettingsWidget* ui;
    void updateAdvancedSettings();

private slots:
// call settings
    void setCallsUntrusted(bool state);
    void setAutoAnswerCalls(bool state);
    void setEnableRingtone(bool state);

    void openFileCustomRingtone();

// name server
    void setNameServer(const QString& name);

// openDHT config
    void setEnableProxy(bool state);

    void setProxyAddress(const QString& name);
    void setBootstrapAddress(const QString& name);

// security
    void openFileCACert();
    void openFileUserCert();
    void openFilePrivateKey();

// connectivity
    void setUseUPnP(bool state);
    void setUseTURN(bool state);
    void setUseSTUN(bool state);

    void setTURNAddress(const QString& name);
    void setTURNUsername(const QString& name);
    void setTURNPsswd(const QString& name);
    void setSTUNAddress(const QString& name);

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


};