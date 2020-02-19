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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

#include "ui_deviceitemwidget.h"

#include "deviceitemwidget.h"

#include "lrcinstance.h"

DeviceItemWidget::DeviceItemWidget(const QString& name,
    const QString& id,
    bool isCurrent,
    QWidget* parent)
    : QWidget(parent),
    ui(new Ui::DeviceItemWidget)
{
    ui->setupUi(this);
    ui->labelDeviceId->setText(id);
    ui->editDeviceName->setText(name);

    ui->btnEditDevice->setIcon(
        isCurrent ?
        QIcon(":/images/icons/round-edit-24px.svg") :
        QIcon(":/images/icons/round-remove_circle-24px.svg"));

    ui->btnEditDevice->setToolTip(
        isCurrent ?
        QObject::tr("Edit Device Name") :
        QObject::tr("Unlink Device From Account"));

    ui->editDeviceName->setReadOnly(true);

    ui->labelThisDevice->setVisible(isCurrent);

    connect(ui->editDeviceName,
            SIGNAL(returnPressed()),
            ui->btnEditDevice,
            SIGNAL(clicked()));

    connect(ui->btnEditDevice, &QPushButton::clicked, this,
        [this, isCurrent]() {
            if (isCurrent) {
                toggleEditable();
            } else {
                emit btnRemoveDeviceClicked();
            }
        });

}

DeviceItemWidget::~DeviceItemWidget()
{
    disconnect(this);
    delete ui;
}

void
DeviceItemWidget::toggleEditable()
{
    if (!editable_) {
        ui->editDeviceName->setReadOnly(false);
        ui->editDeviceName->setFocus();
        ui->editDeviceName->setStyleSheet("background: white;");
        ui->btnEditDevice->setIcon(QIcon(":/images/icons/round-save_alt-24px.svg"));
        ui->btnEditDevice->setToolTip(QObject::tr("Save new device name"));
    } else {
        ui->editDeviceName->setReadOnly(true);
        ui->editDeviceName->setStyleSheet("background: transparent;");
        ui->btnEditDevice->setIcon(QIcon(":/images/icons/round-edit-24px.svg"));
        ui->btnEditDevice->setToolTip(QObject::tr("Edit Device Name"));
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.deviceName = ui->editDeviceName->text();
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
    editable_ = !editable_;
}

QSize
DeviceItemWidget::sizeHint() const
{
    return QSize();
}
