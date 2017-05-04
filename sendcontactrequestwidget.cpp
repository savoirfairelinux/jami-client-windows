/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include <QDebug>

#include "sendcontactrequestwidget.h"
#include "ui_sendcontactrequestwidget.h"
#include "account.h"
#include "accountmodel.h"
#include "availableaccountmodel.h"
#include "recentmodel.h"
#include "contactmethod.h"
#include "phonedirectorymodel.h"

SendContactRequestWidget::SendContactRequestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendContactRequestWidget)
{
    ui->setupUi(this);
    ui->peerContactID->setText("ContactID"); // TODO: Display RingID/Username
}

SendContactRequestWidget::~SendContactRequestWidget()
{
    disconnect(sendCRClickedConnection_);
    delete ui;
}

void
SendContactRequestWidget::setup(const QModelIndex& nodeIdx)
{
    auto cmVector = RecentModel::instance().getContactMethods(nodeIdx);
    disconnect(sendCRClickedConnection_);
    QString number = cmVector[0]->uri();
    ui->peerContactID->setText(number);
    sendCRClickedConnection_ = connect(ui->sendContactRequestButton, &QPushButton::clicked, [this,nodeIdx]() {
        sendCR(nodeIdx);
    });
}

void SendContactRequestWidget::sendCR(const QModelIndex& nodeIdx)
{
    auto cmVector = RecentModel::instance().getContactMethods(nodeIdx);
    QString number = cmVector[0]->uri();
    auto cm = PhoneDirectoryModel::instance().getNumber(number);

    if(cm->account() != nullptr){
        cm->account()->sendContactRequest(cm);
    } else {
        qDebug() << "no account linked to contact method";
        auto idx = AvailableAccountModel::instance().selectionModel()->currentIndex();
        if (idx.isValid()) {
            cm->setAccount(idx.data(static_cast<int>(Ring::Role::Object)).value<Account*>());
            cm->account()->sendContactRequest(cm);
        }
    }
}
