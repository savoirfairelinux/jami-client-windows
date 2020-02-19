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

#include <QTimer>
#include <QtConcurrent/QtConcurrent>

AccountMigrationDialog::AccountMigrationDialog(QWidget *parent, const QString& accountId) :
    QDialog(parent),
    ui(new Ui::AccountMigrationDialog),
    accountId_(accountId)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto& avatarInfo = LRCInstance::accountModel().getAccountInfo(accountId_);

    auto managerUsername = avatarInfo.confProperties.managerUsername;
    auto managerUri = avatarInfo.confProperties.managerUri;
    auto username = avatarInfo.confProperties.username;
    auto alias = LRCInstance::accountModel().getAccountInfo(accountId_).profileInfo.alias;

    // manager uri
    if (!managerUri.isEmpty()) {
        ui->managerUriInputLabel->setText(managerUri);
    } else {
        ui->managerUriLabel->hide();
        ui->managerUriInputLabel->hide();
        ui->gridLayout->removeWidget(ui->managerUriLabel);
        ui->gridLayout->removeWidget(ui->managerUriInputLabel);
        ui->gridLayout->removeItem(ui->managerUriHorizontalSpacer);
    }
    // username
    if (!username.isEmpty()) {
        ui->usernameInputLabel->setText(username);
    } else {
        if (avatarInfo.confProperties.managerUsername.isEmpty()) {
            ui->usernameLabel->hide();
            ui->usernameInputLabel->hide();
            ui->gridLayout->removeWidget(ui->usernameInputLabel);
            ui->gridLayout->removeWidget(ui->usernameLabel);
            ui->gridLayout->removeItem(ui->usernameHorizontalSpacer);
        } else {
            ui->usernameInputLabel->setText(managerUsername);
        }
    }

    // alias
    if (!alias.isEmpty()) {
        ui->aliasInputLabel->setText(alias);
    } else {
        ui->aliasLabel->hide();
        ui->aliasInputLabel->hide();
        ui->gridLayout->removeWidget(ui->aliasLabel);
        ui->gridLayout->removeWidget(ui->aliasInputLabel);
        ui->gridLayout->removeItem(ui->aliasHorizontalSpacer);
    }

    ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::accountPhoto(avatarInfo, ui->avatarLabel->size())));

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
            password_ = text;
        });

    ui->migrationPushButton->setDisabled(true);

    migrationSpinnerMovie_ = new QMovie(":/images/jami_eclipse_spinner.gif");
    ui->spinnerLabel->setMovie(migrationSpinnerMovie_);

    Utils::setStackWidget(ui->stackedWidget, ui->accountMigrationPage);
}

AccountMigrationDialog::~AccountMigrationDialog()
{
    delete ui;
}

void
AccountMigrationDialog::slotPasswordEditingFinished()
{
    password_ = ui->passwordInputLineEdit->text();
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
    Utils::setStackWidget(ui->stackedWidget, ui->migrationWaitingPage);
    migrationSpinnerMovie_->start();

    connect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::migrationEnded,
        [this](const QString& accountId, bool ok) {
            if (accountId_ != accountId) {
                return;
            }
            if (!ok) {
                auto spinnerGeometry = ui->spinnerLabel->frameGeometry();
                auto pixmap = Utils::generateTintedPixmap(":/images/icons/baseline-error_outline-24px.svg", RingTheme::urgentOrange_)
                    .scaled(spinnerGeometry.width(), spinnerGeometry.height());
                ui->spinnerLabel->setPixmap(pixmap);
                ui->progressLabel->setStyleSheet("color:red;");
                ui->progressLabel->setText(tr("Migration Failed"));
                QTimer::singleShot(1000, this,
                    [this] {
                        Utils::setStackWidget(ui->stackedWidget, ui->accountMigrationPage);
                        ui->passwordInputLineEdit->clear();
                        ui->spinnerLabel->setMovie(migrationSpinnerMovie_);
                        ui->progressLabel->setStyleSheet("color:black;");
                        ui->progressLabel->setText(tr("Migrating your Jami account..."));
                    });
            } else {
                done(DialogCode::Accepted);
            }
        });

    QtConcurrent::run(
        [this] {
            auto config = LRCInstance::accountModel().getAccountConfig(accountId_);
            config.archivePassword = password_;
            LRCInstance::accountModel().setAccountConfig(accountId_, config);
        });

}

void
AccountMigrationDialog::closeEvent(QCloseEvent *event)
{
    done(DialogCode::Rejected);
    QDialog::closeEvent(event);
}
