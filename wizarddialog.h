/**************************************************************************
* Copyright (C) 2015-2017 by Savoir-faire Linux                           *
* Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
* Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include <QDialog>
#include <QtConcurrent/QtConcurrent>

#include "account.h"
#include "person.h"

namespace Ui {
class WizardDialog;
}

class WizardDialog : public QDialog
{
    Q_OBJECT

public:
    enum WizardMode {
        WIZARD,
        NEW_ACCOUNT,
        MIGRATION
    };

public:
    explicit WizardDialog(WizardMode wizardMode = WIZARD, Account* toBeMigrated = nullptr, QWidget* parent = 0);
    ~WizardDialog();

// Overrided function
protected slots:
    void accept();
    void closeEvent(QCloseEvent* event);

//UI Slots
private slots:
    void on_existingPushButton_clicked();
    void on_newAccountButton_clicked();
    void on_nextButton_clicked();
    void on_previousButton_clicked();
    void on_passwordEdit_textChanged(const QString& arg1);

private slots:
    void endSetup(Account::RegistrationState state);
    void on_usernameEdit_textChanged(const QString& arg1);
    void handle_registeredNameFound(Account *account, NameDirectory::LookupStatus status, const QString& address, const QString& name);
    void handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus status, const QString& name);
    void timeoutNameLookupTimer();
    void on_photoTaken(QString fileName);

private:
    Ui::WizardDialog* ui;
    Account* account_;
    WizardMode wizardMode_;
    QMovie* movie_;
    QTimer nameLookupTimer_;

    void setup();
    void changePage(bool existingAccount);
    void usernameFailedRegistration();
};
