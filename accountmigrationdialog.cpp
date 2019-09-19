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

AccountMigrationDialog::AccountMigrationDialog(QWidget *parent, const std::string& accountId) :
    QDialog(parent),
    ui(new Ui::AccountMigrationDialog),
    accountId_(accountId)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto& avatarInfo = LRCInstance::accountModel().getAccountInfo(accountId_);
    // manager uri
    if (!avatarInfo.confProperties.managerUri.empty()) {
        ui->managerUriInputLabel->setText(QString::fromStdString(avatarInfo.confProperties.managerUri));
    }
    else {
        ui->managerUriLabel->hide();
        ui->managerUriInputLabel->hide();
        ui->gridLayout->removeWidget(ui->managerUriLabel);
        ui->gridLayout->removeWidget(ui->managerUriInputLabel);
        ui->gridLayout->removeItem(ui->managerUriHorizontalSpacer);
    }
    // username
    if (!avatarInfo.confProperties.username.empty()) {
        ui->usernameInputLabel->setText(QString::fromStdString(avatarInfo.confProperties.username));
    } else {
        ui->usernameInputLabel->hide();
        ui->usernameLabel->hide();
        ui->gridLayout->removeWidget(ui->usernameInputLabel);
        ui->gridLayout->removeWidget(ui->usernameLabel);
        ui->gridLayout->removeItem(ui->usernameHorizontalSpacer);
    }
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

    migrationSpinnerMovie_ = new QMovie(":/images/jami_eclipse_spinner.gif");
    ui->spinnerLabel->setMovie(migrationSpinnerMovie_);
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
    Utils::setStackWidget(ui->stackedWidget, ui->migrationWaittingPage);
    migrationSpinnerMovie_->start();

    connect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::migrationEnded,
        [this](const std::string& accountId, bool ok) {
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
                        done(DialogCode::Rejected);
                    });
            }
            else {
                done(DialogCode::Accepted);
            }
        });

    auto config = LRCInstance::accountModel().getAccountConfig(accountId_);
    config.archivePassword = password_;
    LRCInstance::accountModel().setAccountConfig(accountId_, config);
}

void
AccountMigrationDialog::closeEvent(QCloseEvent *event)
{
    done(DialogCode::Rejected);
    QDialog::closeEvent(event);
}
