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

LinkDeviceDialog::LinkDeviceDialog(QDialog* parent)
    : QDialog(parent),
    ui(new Ui::LinkDeviceDialog)
{
    ui->setupUi(this);

    if (LRCInstance::getCurrAccConfig().archiveHasPassword) {
        ui->stackedWidget->setCurrentWidget(ui->start);
    } else {
        setGeneratingPage();
    }

    connect(ui->enterBtn, &QPushButton::clicked,
            this, &LinkDeviceDialog::setGeneratingPage);
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
    ui->stackedWidget->setCurrentWidget(ui->middle);

    QMovie* movie = new QMovie(":/images/ajax-loader.gif");
    ui->spinningLabel->setMovie(movie);
    ui->spinningLabel->show();
    movie->start();

    timeout_ = new QTimer(this);
    timeout_->setInterval(exportTimeout_);
    timeout_->setSingleShot(true);
    connect(timeout_, &QTimer::timeout, this,
        [&]() {
            setExportPage(std::string(),
                          lrc::api::account::ExportOnRingStatus::NETWORK_ERROR,
                          std::string());
        });
    timeout_->start();

    LRCInstance::accountModel().exportOnRing(LRCInstance::getCurrAccId(), ui->passwordLineEdit->text().toStdString());
}

void
LinkDeviceDialog::setExportPage(const std::string& accountId, lrc::api::account::ExportOnRingStatus status, const std::string& pin)
{
    Q_UNUSED(accountId);
    timeout_->stop();

    ui->stackedWidget->setCurrentWidget(ui->end);

    switch (status) {
    case lrc::api::account::ExportOnRingStatus::WRONG_PASSWORD:
        ui->label_4->setText(tr("Your account password was incorrect"));
        ui->label_5->hide();
        ui->label_6->hide();
        accept();
        break;

    case lrc::api::account::ExportOnRingStatus::SUCCESS:
        ui->label_5->setText(QString::fromStdString(pin));
        accept();
        break;

    default:
        disconnect();
        ui->label_4->setText(tr("Something went wrong.\nPlease try again later."));
        ui->label_5->hide();
        ui->label_6->hide();
        accept();
    }
}