/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "accountmigrationdialog.h"
#include "ui_accountmigration.h"

#include <QMessageBox>

AccountMigrationDialog::AccountMigrationDialog(QWidget *parent, const std::string& accountId) :
    QDialog(parent),
    ui(new Ui::AccountMigrationDialog),
    accountId_(accountId)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto& avatarInfo = LRCInstance::accountModel().getAccountInfo(accountId_);
    ui->idInputLabel->setText(QString::fromStdString(accountId_));
    ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::accountPhoto(avatarInfo, ui->avatarLabel->size())));
    ui->aliasInputLabel->setText(QString::fromStdString(LRCInstance::accountModel().getAccountInfo(accountId_).profileInfo.alias));

    connect(ui->passwordInputLineEdit, &QLineEdit::editingFinished, this, &AccountMigrationDialog::slotPasswordEditingFinished);
    connect(ui->deleteAccountPushButton, &QAbstractButton::clicked, this, &AccountMigrationDialog::slotDeleteButtonClicked);
    connect(ui->migrationPushButton, &QAbstractButton::clicked, this, &AccountMigrationDialog::slotMigrationButtonClicked);
    connect(ui->passwordInputLineEdit, &QLineEdit::textChanged,
        [this](const QString& text) {
            if (text.isEmpty()) {
                ui->migrationPushButton->setDisabled(true);
            } else {
                ui->migrationPushButton->setDisabled(false);
            }
            password_ = text.toStdString();
        });

    ui->migrationPushButton->setDisabled(true);
}

AccountMigrationDialog::~AccountMigrationDialog()
{
    delete ui;
}

void
AccountMigrationDialog::slotPasswordEditingFinished()
{
    password_ = ui->passwordInputLineEdit->text().toStdString();
}

void
AccountMigrationDialog::slotDeleteButtonClicked()
{
    LRCInstance::accountModel().removeAccount(accountId_);
    done(DialogCode::Accepted);
}

void
AccountMigrationDialog::slotMigrationButtonClicked()
{
    connect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::migrationEnded,
        [this] (const std::string& accountId, bool ok){
            if (accountId_ != accountId) {
                return;
            }
            if (!ok) {
                QMessageBox::warning(0, "Error!!!", "Account Migration Failed");
                done(DialogCode::Rejected);
            } else {
                QMessageBox::information(0, "Success!", "Account Migration Successed");
                done(DialogCode::Accepted);
            }
        });
    auto config = LRCInstance::accountModel().getAccountConfig(accountId_);
    config.password = password_;
    LRCInstance::accountModel().setAccountConfig(accountId_, config);
    hide();
}

void
AccountMigrationDialog::closeEvent(QCloseEvent *event)
{
    done(DialogCode::Rejected);
    QDialog::closeEvent(event);
}
