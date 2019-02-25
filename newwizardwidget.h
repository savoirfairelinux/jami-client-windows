/**************************************************************************
* Copyright (C) 2018-2019 by Savoir-faire Linux                                *
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
#include <QtConcurrent/QtConcurrent>

#include "lrcinstance.h"
#include "navwidget.h"

namespace Ui {
class NewWizardWidget;
}

using namespace lrc::api::account;

class NewWizardWidget : public NavWidget
{
    Q_OBJECT

public:

    using AccountInfo = Info;

    enum WizardMode { CREATE, IMPORT, MIGRATE };

private:
    enum NameRegistrationUIState { BLANK, INVALID, TAKEN, FREE, SEARCHING };

public:
    explicit NewWizardWidget(QWidget* parent = 0);
    ~NewWizardWidget();

    void setToMigrate(AccountInfo* toBeMigrated);

    // NavWidget
    virtual void navigated(bool to);
    virtual void updateCustomUI();

    //UI Slots
private slots:
    void on_existingPushButton_clicked();
    void on_newAccountButton_clicked();
    void on_nextButton_clicked();
    void on_previousButton_clicked();
    void on_passwordEdit_textChanged(const QString& arg1);

private slots:
    void on_usernameEdit_textChanged(const QString& arg1);
    void slotRegisteredNameFound(const std::string& accountId,
                                 LookupStatus status,
                                 const std::string& address,
                                 const std::string& name);
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
    QLabel* lookupStatusLabel_;
    QString registeredName_;
    bool registrationStateOk_;
    QMetaObject::Connection registeredNameFoundConnection_;
    QString fileToImport_;
    QLabel* passwordStatusLabel_;

    void updateNameRegistrationUi(NameRegistrationUIState state);
    void changePage(QWidget* toPage);
    void setNavBarVisibility(bool visible, bool back=false);
    void validateWizardProgression();
    void createRingAccount(const QString &displayName = QString(),
                           const QString &password = QString(),
                           const QString &pin = QString(),
                           const QString &archivePath = QString());
    void reportFailure();
    void processWizardInformations();
};
