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
#include <QMessagebox>

#include "contactpickeritemdelegate.h"

ContactPicker::ContactPicker(QWidget *parent, bool isConference) :
    QDialog(parent),
    ui(new Ui::ContactPicker),
    isConference_ (isConference)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    smartListModel_.reset(new SmartListModel(LRCInstance::getCurrAccId(), this, true));
    ui->smartList->setModel(smartListModel_.get());
    ui->smartList->setItemDelegate(new ContactPickerItemDelegate());

    conferenceeProxyModel_ = new ConferenceableProxyModel(smartListModel_.get());

    ui->smartList->setModel(conferenceeProxyModel_);
}

ContactPicker::~ContactPicker()
{
    delete ui;
}

void
ContactPicker::on_smartList_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    this->accept();
}

void
ContactPicker::accept()
{
    auto idx = ui->smartList->currentIndex();

    if (idx.isValid()) {
        // get current call id and peer uri
        auto selectedConvUid = LRCInstance::getSelectedConvUid();
        auto convModel = LRCInstance::getCurrentConversationModel();
        auto conversation = Utils::getConversationFromUid(selectedConvUid, *convModel);
        auto thisCallId = conversation->callId;
        auto contactUri = idx.data(static_cast<int>(SmartListModel::Role::URI)).value<QString>().toStdString();

        // let parent deal with this as this dialog will be destroyed
        if (isConference_) {
            emit contactWillJoinConference(thisCallId, contactUri);
        } else {
            emit contactWillDoBlindTransfer(thisCallId, contactUri);
        }
    }

    QDialog::accept();
}

void
ContactPicker::on_ringContactLineEdit_textChanged(const QString &arg1)
{
    if (arg1.isEmpty())
        setRegexMatchExcept();
    else
        conferenceeProxyModel_->setFilterRegExp(QRegExp(arg1, Qt::CaseInsensitive, QRegExp::FixedString));
}

void
ContactPicker::mousePressEvent(QMouseEvent *event)
{
    auto contactPickerWidgetRect = ui->contactPickerWidget->rect();
    if (!contactPickerWidgetRect.contains(event->pos())) {
        QDialog::reject();
    }
}

void
ContactPicker::setTitle(const std::string& title)
{
    ui->title->setText(QString::fromStdString(title));
}

void
ContactPicker::setRegexMatchExcept()
{
    QString reg_excep("\\b(?!" + CalleeDisplayName_ + "\\b)\\w+");
    conferenceeProxyModel_->setFilterRegExp(QRegExp(reg_excep));
}

void
ContactPicker::setCurrentCalleeDisplayName(const QString& CalleeDisplayName)
{
    CalleeDisplayName_ = CalleeDisplayName;
}
