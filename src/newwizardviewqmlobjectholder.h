/***************************************************************************
* Copyright (C) 2019-2020 by Savoir-faire Linux                            *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                       *
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 3 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* This program is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
***************************************************************************/

#pragma once

#include <QObject>
#include <QLabel>
#include <QTimer>

#include <map>

#include "lrcinstance.h"

#include "namedirectory.h"

using namespace lrc::api::account;

class NewWizardViewQmlObjectHolder : public QObject
{
    Q_OBJECT

    Q_PROPERTY(WizardMode wizardMode READ getWizardMode WRITE setWizardMode NOTIFY wizardModeChanged)
    Q_PROPERTY(bool registrationStateOk READ getRegistrationStateOk WRITE setRegistrationStateOk NOTIFY registrationStateOkChanged)

public:
    using AccountInfo = Info;

    enum WizardMode { CREATE, IMPORT, MIGRATE, CREATESIP, CONNECTMANAGER };
    enum NameRegistrationUIState { BLANK, INVALID, TAKEN, FREE, SEARCHING };

    Q_ENUM(WizardMode)
    Q_ENUM(NameRegistrationUIState)

public:
    explicit NewWizardViewQmlObjectHolder(QObject* parent = nullptr);
    void createAccount();

public:
    // property setters and getters
    void setRegistrationStateOk(bool ok);
    bool getRegistrationStateOk() const {
        return registrationStateOk_;
    };

    void setWizardMode(WizardMode mode);
    WizardMode getWizardMode() const {
        return wizardMode_;
    };

    // functions to call in QML
    Q_INVOKABLE void setNewWizardViewQmlObject(QObject* obj);

signals:
    // property change notification signal
    void wizardModeChanged(WizardMode);
    void registrationStateOkChanged(bool);

    // object holder signals
    void updateNameRegistrationUi(NameRegistrationUIState state);
    void reportFailure();

private:
    QObject* newWizardViewQmlObject;

    WizardMode wizardMode_;
    bool registrationStateOk_ = false;

    AccountInfo* account_;
    QString registeredName_;
    QString fileToImport_;
    std::map<QString, QString> inputPara_;
};
