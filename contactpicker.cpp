/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "contactpicker.h"
#include "ui_contactpicker.h"

#include <QMouseEvent>

#include "conversationitemdelegate.h"

ContactPicker::ContactPicker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactPicker)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    smartListModel_.reset(new SmartListModel(LRCInstance::getCurrAccId(), this, true));
    ui->smartList->setModel(smartListModel_.get());
    ui->smartList->setItemDelegate(new ConversationItemDelegate());

    conferenceeProxyModel_ = new ConferenceableProxyModel(smartListModel_.get());

    ui->smartList->setModel(conferenceeProxyModel_);
}

ContactPicker::~ContactPicker()
{
    delete ui;
}

void
ContactPicker::on_smartList_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    this->accept();
}

void
ContactPicker::accept()
{
    auto idx = ui->smartList->currentIndex();

    if (idx.isValid()) {
        auto callModel = LRCInstance::getCurrentCallModel();

        // get current call id
        auto selectedConvUid = LRCInstance::getSelectedConvUid();
        auto convModel = LRCInstance::getCurrentConversationModel();
        auto conversation = Utils::getConversationFromUid(selectedConvUid, *convModel);
        auto thisCallId = conversation->callId;
        auto thisCallIsAudioOnly = callModel->getCall(thisCallId).isAudioOnly;

        // generate new call with peer
        auto contactUri = idx.data(static_cast<int>(SmartListModel::Role::URI)).value<QString>().toStdString();
        auto conferenceeCallId = callModel->createCall(contactUri, thisCallIsAudioOnly);
        pendingConferencees_.insert(QString::fromStdString(conferenceeCallId),
            QObject::connect(callModel, &lrc::api::NewCallModel::callStatusChanged,
                [this, conferenceeCallId, thisCallId] (const std::string& callId) {
                    if (callId != conferenceeCallId)
                        return;
                    using namespace lrc::api::call;
                    auto call = LRCInstance::getCurrentCallModel()->getCall(callId);
                    switch (call.status) {
                    case Status::IN_PROGRESS:
                    {
                        qDebug() << "adding to conference callid=" << QString::fromStdString(callId);
                        auto it = pendingConferencees_.find(QString::fromStdString(conferenceeCallId));
                        if (it != pendingConferencees_.end()) {
                            QObject::disconnect(it.value());
                            pendingConferencees_.erase(it);
                        }
                        LRCInstance::getCurrentCallModel()->joinCalls(thisCallId, conferenceeCallId);
                        return;
                    }
                    default:
                        qDebug() << "failed to add to conference callid=" << QString::fromStdString(callId);
                        break;
                    }
                })
        );
    }

    QDialog::accept();
}

void
ContactPicker::on_ringContactLineEdit_textChanged(const QString &arg1)
{
    conferenceeProxyModel_->setFilterRegExp(QRegExp(arg1, Qt::CaseInsensitive, QRegExp::FixedString));
}

void
ContactPicker::mousePressEvent(QMouseEvent *event)
{
    if (!rect().contains(event->pos())) {
        QDialog::reject();
    }
}
