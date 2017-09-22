/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "pathpassworddialog.h"
#include "ui_pathpassworddialog.h"

#include <QFileDialog>

// LRC
#include "accountmodel.h"
#include "profilemodel.h"
#include "profile.h"
#include "person.h"

PathPasswordDialog::PathPasswordDialog(bool exportMode, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PathPasswordDialog),
    exportMode_(exportMode)
{
    ui->setupUi(this);
    ui->infoLabel->hide();

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flags);
}

PathPasswordDialog::~PathPasswordDialog()
{
    delete ui;
}

void
PathPasswordDialog::on_okButton_clicked()
{
    // reset original color
    ui->passwordLineEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
    // A file has been selected
    if ( ! path_.isEmpty()){
        /*// Password filled accordingly to archive
        if ( ! ui->passwordLineEdit->isEnabled() ||
             ( ! password_.isEmpty() && ! password_.isNull())) {*/

            ui->passwordLineEdit->hide();
            ui->infoLabel->show();
            ui->infoLabel->setText("Loading account...");
            ui->okButton->setEnabled(false);
            ui->cancelButton->setEnabled(false);

            Profile *profile = ProfileModel::instance().selectedProfile();
            account_ = AccountModel::instance().add(profile->person()->formattedName(), Account::Protocol::RING);
            connect(account_, &Account::stateChanged, this, &PathPasswordDialog::endAccountMove);
            account_->setPassword(password_);
            account_->setArchivePath(path_);
            account_->save();
        //}
    }
}


void
PathPasswordDialog::endAccountMove(Account::RegistrationState state)
{
    switch (state) {
    case Account::RegistrationState::READY:
    {
        account_->performAction(Account::EditAction::RELOAD);
        accept();
    }
    case Account::RegistrationState::UNREGISTERED:
    case Account::RegistrationState::TRYING:
    case Account::RegistrationState::COUNT__:
        break;
    }
}

void
PathPasswordDialog::on_pathButtonEdit_clicked()
{
    QString filePath;
    if (exportMode_) {


        filePath = QFileDialog::getSaveFileName(this,
                                                tr("Save File"),
                                                QString(), // current account archive file name
                                                tr("Ring archive files (*.gz);; All files (*)"));
    } else {
        filePath = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                QString(),
                                                tr("Ring archive files (*.gz);; All files (*)"));
    }

    // for export get current account archive path
    // for import use set path
    // check if archive has password
    // if it has, prompt for password
    path_ = QDir::toNativeSeparators(filePath);
    ui->pathButtonEdit->setText(path_);
}

void
PathPasswordDialog::on_passwordLineEdit_textChanged(const QString& password)
{
    password_ = password;
}

void PathPasswordDialog::on_cancelButton_clicked()
{
    accept();
}
