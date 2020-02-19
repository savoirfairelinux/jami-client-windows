/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.  *
 **************************************************************************/

#include "linkdevicedialog.h"
#include "ui_linkdevicedialog.h"

#include <QMovie>
#include <QTimer>

LinkDeviceDialog::LinkDeviceDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::LinkDeviceDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(size());

    setWindowTitle(tr("Link Another Device"));

    timeout_ = new QTimer(this);

    if (LRCInstance::getCurrAccConfig().archiveHasPassword) {
        ui->stackedWidget->setCurrentWidget(ui->passwordConfirmPage);
    } else {
        setGeneratingPage();
    }

    connect(ui->btnPasswordOk, SIGNAL(clicked()), this, SLOT(setGeneratingPage()));
    connect(ui->btnPasswordCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->btnCloseExportDialog, SIGNAL(clicked()), this, SLOT(reject()));
    connect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::exportOnRingEnded,
            this, &LinkDeviceDialog::setExportPage);
}

LinkDeviceDialog::~LinkDeviceDialog()
{
    delete ui;
}

void
LinkDeviceDialog::setGeneratingPage()
{
    if (ui->passwordEdit->text().isEmpty() && LRCInstance::getCurrAccConfig().archiveHasPassword) {
        setExportPage(QString(),
                      lrc::api::account::ExportOnRingStatus::WRONG_PASSWORD,
                      QString());
        return;
    }

    ui->stackedWidget->setCurrentWidget(ui->exportingPage);

    QMovie* movie = new QMovie(":/images/jami_eclipse_spinner.gif");
    movie->setScaledSize(QSize(96, 96));
    ui->exportingSpinner->setMovie(movie);
    ui->exportingSpinner->show();
    movie->start();

    timeout_->setInterval(exportTimeout_);
    timeout_->setSingleShot(true);
    connect(timeout_, &QTimer::timeout, this,
        [this]() {
            setExportPage(QString(),
                          lrc::api::account::ExportOnRingStatus::NETWORK_ERROR,
                          QString());
        });
    timeout_->start();

    LRCInstance::accountModel().exportOnRing(LRCInstance::getCurrAccId(), ui->passwordEdit->text());
}

void
LinkDeviceDialog::setExportPage(const QString& accountId, lrc::api::account::ExportOnRingStatus status, const QString& pin)
{
    Q_UNUSED(accountId);
    timeout_->stop();

    switch (status) {
    case lrc::api::account::ExportOnRingStatus::WRONG_PASSWORD:
        ui->infoLabel->setStyleSheet("color: black;");
        ui->infoLabel->setText(tr("Incorrect password"));
        ui->yourPinLabel->hide();
        ui->exportedPIN->hide();
        break;

    case lrc::api::account::ExportOnRingStatus::SUCCESS:
        ui->infoLabel->setStyleSheet("color: #2b5084;border: 1px solid lightgray;background-color: whitesmoke;padding: 8px;border-radius: 15px;");
        ui->exportedPIN->setText(pin);
        break;

    case lrc::api::account::ExportOnRingStatus::NETWORK_ERROR:
        disconnect();
        ui->infoLabel->setStyleSheet("color: black;");
        ui->infoLabel->setText(tr("Error connecting to the network.\nPlease try again later."));
        ui->yourPinLabel->hide();
        ui->exportedPIN->hide();
    case lrc::api::account::ExportOnRingStatus::INVALID:
        disconnect();
        ui->infoLabel->setStyleSheet("color: black;");
        ui->infoLabel->setText(tr("Something went wrong.\n"));
        ui->yourPinLabel->hide();
        ui->exportedPIN->hide();
        break;
    }

    ui->stackedWidget->setCurrentWidget(ui->exportedPage);
}