/**************************************************************************
| Copyright (C) 2020 by Savoir-faire Linux                                |
| Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              |
|                                                                         |
| This program is free software; you can redistribute it and/or modify    |
| it under the terms of the GNU General Public License as published by    |
| the Free Software Foundation; either version 3 of the License, or       |
| (at your option) any later version.                                     |
|                                                                         |
| This program is distributed in the hope that it will be useful,         |
| but WITHOUT ANY WARRANTY; without even the implied warranty of          |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
| GNU General Public License for more details.                            |
|                                                                         |
| You should have received a copy of the GNU General Public License       |
| along with this program.  If not, see <https://www.gnu.org/licenses/>.  |
**************************************************************************/

#include "sipcredentialdialog.h"
#include "ui_sipcredentialdialog.h"

SipCredentialDialog::SipCredentialDialog(lrc::api::profile::Info currentSIPAccountInfo, QWidget* parent, int itemIndex, EditMode mode):
    QDialog(parent),
    ui(new Ui::SipCredentialDialog)
{
    ui->setupUi(this);

    setWindowTitle(mode == EditMode::NewCredential ? tr("Add SIP Credential") : tr("Edit SIP Credential"));
    setFixedSize(size());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // setup root creds
    ui->sipPasswordRewriteLineEdit->setEchoMode(QLineEdit::Password);
    if (mode == EditMode::EditCredential) {
        ui->sipUsernameRewriteLineEdit->setText(LRCInstance::accountModel().getSIPAccountCredentials(LRCInstance::getCurrAccId())[itemIndex][usernameKey]);
        ui->sipPasswordRewriteLineEdit->setText(LRCInstance::accountModel().getSIPAccountCredentials(LRCInstance::getCurrAccId())[itemIndex][passwordKey]);
        ui->sipRealmRewriteLineEdit->setText(LRCInstance::accountModel().getSIPAccountCredentials(LRCInstance::getCurrAccId())[itemIndex][realmKey]);
    }


    connect(ui->sipCredConfirmButton, &QAbstractButton::clicked,
        [this, mode, itemIndex] {
            MapStringString credentials;
            credentials[usernameKey] = ui->sipUsernameRewriteLineEdit->text();
            credentials[passwordKey] = ui->sipPasswordRewriteLineEdit->text();
            credentials[realmKey] = ui->sipRealmRewriteLineEdit->text().isEmpty() ? QString("*") : ui->sipRealmRewriteLineEdit->text();
            if (mode == EditMode::NewCredential) {
                auto credentialsVec = LRCInstance::accountModel().getSIPAccountCredentials(LRCInstance::getCurrAccId());
                credentialsVec.append(credentials);
                LRCInstance::accountModel().setSIPAccountCredentials(LRCInstance::getCurrAccId(), credentialsVec);
                // new cred insert
                emit sipCredInfoCreated(credentials[usernameKey], credentials[passwordKey], credentials[realmKey]);
            } else {
                auto currentCreds = LRCInstance::accountModel().getSIPAccountCredentials(LRCInstance::getCurrAccId());
                currentCreds[itemIndex] = credentials;
                LRCInstance::accountModel().setSIPAccountCredentials(LRCInstance::getCurrAccId(), currentCreds);
                // update root cred in settings
                emit sipCredInfoChanged(credentials[usernameKey], credentials[passwordKey], credentials[realmKey]);
            }
            QDialog::accept();
        });
    connect(ui->sipCredCancelButton, &QAbstractButton::clicked, this, &QDialog::reject);
}

const QString SipCredentialDialog::usernameKey = "Account.username";
const QString SipCredentialDialog::passwordKey = "Account.password";
const QString SipCredentialDialog::realmKey = "Account.realm";

SipCredentialDialog::~SipCredentialDialog()
{
    delete ui;
}
