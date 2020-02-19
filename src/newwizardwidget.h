/**************************************************************************
* Copyright (C) 2018-2019 by Savoir-faire Linux                           *
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

#pragma once

#include <QLabel>
#include <QTimer>
#include <map>

#include "collapsiblepasswordwidget.h"
#include "lrcinstance.h"
#include "navwidget.h"

#include "namedirectory.h"

namespace Ui {
class NewWizardWidget;
}

using namespace lrc::api::account;

class NewWizardWidget : public NavWidget
{
    Q_OBJECT

public:

    using AccountInfo = Info;

    enum WizardMode { CREATE, IMPORT, MIGRATE, CREATESIP, CONNECTMANAGER };

private:
    enum NameRegistrationUIState { BLANK, INVALID, TAKEN, FREE, SEARCHING };

public:
    explicit NewWizardWidget(QWidget* parent = 0);
    ~NewWizardWidget();

    // NavWidget
    virtual void navigated(bool to);
    virtual void updateCustomUI();
public slots:
    virtual void slotAccountListChanged();

    //UI Slots
private slots:

    void on_newSIPAccountButton_clicked();
    void on_showAdvancedButton_clicked();
    void on_fromDeviceButton_clicked();
    void on_newAccountButton_clicked();
    void on_connectAccountManagerButton_clicked();
    void on_fromBackupButton_clicked();
    void on_nextButton_clicked();
    void on_previousButton_clicked();
    void on_pinInfoBtn_clicked();
    void on_backupInfoBtn_clicked();
    void on_neverShowAgainBox_clicked();
    void on_skipBtn_clicked();
    void on_exportBtn_clicked();

private slots:
    void on_usernameEdit_textChanged(const QString& arg1);
    void slotRegisteredNameFound(const QString& accountId,
                                 LookupStatus status,
                                 const QString& address,
                                 const QString& name);
    void handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus status, const QString& name);
    void timeoutNameLookupTimer();
    void on_signUpCheckbox_toggled(bool checked);

private:
    Ui::NewWizardWidget* ui;

    WizardMode wizardMode_;
    QPixmap statusSuccessPixmap_;
    QPixmap statusInvalidPixmap_;
    QPixmap statusErrorPixmap_;

    AccountInfo* account_;
    QMovie* creationSpinnerMovie_;

    QMovie* lookupSpinnerMovie_;
    QTimer lookupTimer_;
    QString registeredName_;
    bool registrationStateOk_;
    QMetaObject::Connection registeredNameFoundConnection_;
    QString fileToImport_;
    std::map<QString, QString> inputPara_;

    CollapsiblePasswordWidget* collapsiblePasswordWidget_;

    void updateNameRegistrationUi(NameRegistrationUIState state);
    void changePage(QWidget* toPage);
    void setNavBarVisibility(bool visible, bool back=false);
    void validateWizardProgression();
    void createAccount();
    void connectToAccountManager();
    void reportFailure();
    void processWizardInformations();
};
