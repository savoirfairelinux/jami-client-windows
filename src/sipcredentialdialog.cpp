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

#include <QtConcurrent/QtConcurrent>

SipCredentialDialog::SipCredentialDialog(lrc::api::profile::Info currentSIPAccountInfo, QWidget* parent, int itemIndex, EditMode mode) :
    QDialog(parent),
    ui(new Ui::SipCredentialDialog)
{
    ui->setupUi(this);

    setWindowTitle(mode == EditMode::NewCredential ? tr("Add SIP Credential") : tr("Edit SIP Credential"));
    setFixedSize(size());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // setup root creds
    ui->duplicateUsernameLabel->hide();
    ui->sipPasswordRewriteLineEdit->setEchoMode(QLineEdit::Password);

    auto initialUserName = QString::fromStdString(LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId()).credentials[itemIndex][usernameKey]);
    if (mode == EditMode::EditCredential) {
        ui->sipUsernameRewriteLineEdit->setText(initialUserName);
        ui->sipPasswordRewriteLineEdit->setText(QString::fromStdString(LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId()).credentials[itemIndex][passwordKey]));
        ui->sipRealmRewriteLineEdit->setText(QString::fromStdString(LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId()).credentials[itemIndex][realmKey]));
    }


    connect(ui->sipCredConfirmButton, &QAbstractButton::clicked,
        [this, mode, itemIndex, initialUserName] {
            auto credentialsVec = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId()).credentials;
            auto conf = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());

            for (auto& i : credentialsVec) {
                if (i[usernameKey] == ui->sipUsernameRewriteLineEdit->text().toStdString() && ui->sipUsernameRewriteLineEdit->text() != initialUserName) {
                    ui->duplicateUsernameLabel->show();
                    return;
                }
            }

            std::map<std::string, std::string> credentials;
            credentials[usernameKey] = ui->sipUsernameRewriteLineEdit->text().toStdString();
            credentials[passwordKey] = ui->sipPasswordRewriteLineEdit->text().toStdString();
            credentials[realmKey] = ui->sipRealmRewriteLineEdit->text().isEmpty() ? "*" : ui->sipRealmRewriteLineEdit->text().toStdString();

            if (mode == EditMode::NewCredential) {
                credentialsVec.push_back(credentials);
                conf.credentials = credentialsVec;
                LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), conf);
                // new cred insert
                emit sipCredInfoCreated(QString::fromStdString(credentials[usernameKey]), QString::fromStdString(credentials[passwordKey]), QString::fromStdString(credentials[realmKey]));
            } else {
                credentialsVec[itemIndex] = credentials;
                conf.credentials = credentialsVec;
                LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), conf);
                // update root cred in settings
                emit sipCredInfoChanged(QString::fromStdString(credentials[usernameKey]), QString::fromStdString(credentials[passwordKey]), QString::fromStdString(credentials[realmKey]));
            }
            accept();
        });
    connect(ui->sipCredCancelButton, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(ui->sipUsernameRewriteLineEdit, &QLineEdit::textChanged,
        [this] {
            ui->duplicateUsernameLabel->hide();
        });
}

const char* SipCredentialDialog::usernameKey = "Account.username";
const char* SipCredentialDialog::passwordKey = "Account.password";
const char* SipCredentialDialog::realmKey = "Account.realm";

SipCredentialDialog::~SipCredentialDialog()
{
    delete ui;
}