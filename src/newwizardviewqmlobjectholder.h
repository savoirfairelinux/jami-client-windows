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

#include <QLabel>
#include <QObject>
#include <QTimer>
#include <QMap>

#include <map>

#include "lrcinstance.h"

#include "namedirectory.h"

using namespace lrc::api::account;

class NewWizardViewQmlObjectHolder : public QObject
{
    Q_OBJECT

    Q_PROPERTY(WizardMode wizardMode READ getWizardMode WRITE setWizardMode NOTIFY wizardModeChanged)
    Q_PROPERTY(bool registrationStateOk READ getRegistrationStateOk WRITE setRegistrationStateOk
                   NOTIFY registrationStateOkChanged)
    Q_PROPERTY(
        QString fileToImport READ getFileToImport WRITE setFileToImport NOTIFY fileToImportChanged)
    Q_PROPERTY(QString registeredName READ getRegisteredName WRITE setRegisteredName NOTIFY
                   registeredNameChanged)
    Q_PROPERTY(QMap<QString, QString> inputPara READ getInputPara WRITE setInputPara NOTIFY imputParaChanged)

public:
    using AccountInfo = Info;

    enum WizardMode { CREATE, IMPORT, MIGRATE, CREATESIP, CONNECTMANAGER };
    enum NameRegistrationUIState { BLANK, INVALID, TAKEN, FREE, SEARCHING };

    Q_ENUM(WizardMode)
    Q_ENUM(NameRegistrationUIState)

public:
    explicit NewWizardViewQmlObjectHolder(QObject *parent = nullptr);

public:
    // property setters and getters
    void setWizardMode(WizardMode mode);
    WizardMode
    getWizardMode() const
    {
        return wizardMode_;
    };

    void setRegistrationStateOk(bool ok);
    bool
    getRegistrationStateOk() const
    {
        return registrationStateOk_;
    };

    void setFileToImport(QString para_fileToImport);
    QString
    getFileToImport()
    {
        return fileToImport_;
    }

    void setRegisteredName(QString para_registeredName);
    QString
    getRegisteredName()
    {
        return registeredName_;
    }

    void setInputPara(QMap<QString, QString> para);
    QMap<QString, QString> getInputPara();

    // functions to call in QML
    Q_INVOKABLE void setNewWizardViewQmlObject(QObject *obj);

    // function wrappers for LRC
    Q_INVOKABLE int getAccountListSize();
    Q_INVOKABLE void timeoutNameLookupTimer(const QString &nameServiceURL, const QString &name);

    // set up variables in current class
    Q_INVOKABLE void clearInputPara();
    Q_INVOKABLE void setInputParaHashPair(QString key, QString value);
    Q_INVOKABLE bool getIfKeyExistInParaHashPair(QString key);

    Q_INVOKABLE void createAccount();
    Q_INVOKABLE void settingsNeverShowAgainBox(bool checked);

    // regular utils function wrapper for qml
    Q_INVOKABLE QString stringSimplifier(QString input);
    Q_INVOKABLE QString toNativeSeparators(QString inputDir);
    Q_INVOKABLE QString toFileInfoName(QString inputFileName);

    // function wrappers for for handling signals and slots
    Q_INVOKABLE void exportBtn_clicked(QString fileDir);
    Q_INVOKABLE void skipBtn_clicked();

signals:
    // show main view signal
    void showMainViewWindow();

    // property change notification signal
    void wizardModeChanged(WizardMode para_mode);
    void registrationStateOkChanged(bool para_ok);
    void fileToImportChanged(QString para_fileToImport);
    void registeredNameChanged(QString para_registeredName);
    void imputParaChanged(QMap<QString, QString> para);

    // object holder signals
    void updateNameRegistrationUi(NameRegistrationUIState state);
    // page indexs:
    // welcomePageStackId: 0
    // createAccountPageId : 1
    // createSIPAccountPageId : 2
    // importFromBackupPageId : 3
    // backupKeysPageId : 4
    // importFromDevicePageId : 5
    // connectToAccountManagerPageId : 6
    // spinnerPageId : 7
    void changePage(int pageIndex);
    void reportFailure();
    void updateRepaintQML();
    void updateQuickWindow();

private:
    QObject *newWizardViewQmlObject;

    WizardMode wizardMode_;
    bool registrationStateOk_ = false;
    QMetaObject::Connection registeredNameFoundConnection_;

    AccountInfo *account_;
    QString registeredName_;
    QString fileToImport_;
    std::map<QString, QString> inputPara_;
};
