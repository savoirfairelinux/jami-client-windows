/**************************************************************************
* Copyright (C) 2015-2018 by Savoir-faire Linux                           *
* Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
* Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include <QDialog>
#include <QtConcurrent/QtConcurrent>

#include "account.h"
#include "person.h"

// new lrc
#include "lrcinstance.h"

namespace Ui {
class WizardWidget;
}

class WizardWidget : public QWidget
{
    Q_OBJECT

public:
    using AccountInfo = lrc::api::account::Info;

    enum WizardMode {
        WIZARD,
        NEW_ACCOUNT,
        IMPORT,
        MIGRATION
    };

public:
    explicit WizardWidget(WizardMode wizardMode = WIZARD, AccountInfo* toBeMigrated = nullptr, QWidget* parent = 0);
    ~WizardWidget();

//UI Slots
private slots:
    void on_existingPushButton_clicked();
    void on_newAccountButton_clicked();
    void on_nextButton_clicked();
    void on_previousButton_clicked();
    void on_passwordEdit_textChanged(const QString& arg1);

private slots:
    void on_usernameEdit_textChanged(const QString& arg1);
    void handle_registeredNameFound(Account *account, NameDirectory::LookupStatus status, const QString& address, const QString& name);
    void handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus status, const QString& name);
    void timeoutNameLookupTimer();
    void on_photoTaken(QString fileName);
    void on_signUpCheckbox_toggled(bool checked);
    void on_archivePathSelector_clicked();
    void on_dhtImportBtn_clicked();
    void on_fileImportBtn_clicked();

private:
    Ui::WizardWidget* ui;
    AccountInfo* account_;
    WizardMode wizardMode_;
    QMovie* movie_;
    QTimer nameLookupTimer_;

    void setup();
    void changePage(bool existingAccount);
    void validateFileImport();
    void createRingAccount(const QString &displayName = QString(),
                           const QString &password = QString(),
                           const QString &pin = QString(),
                           const QString &archivePath = QString());
    void processWizardInformations();
};
