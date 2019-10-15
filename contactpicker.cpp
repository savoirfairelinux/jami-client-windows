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

#include "contactpickeritemdelegate.h"

ContactPicker::ContactPicker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactPicker),
    type_(Type::CONFERENCE)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    ui->smartList->setItemDelegate(new ContactPickerItemDelegate());

    selectableProxyModel_ = new SelectableProxyModel(smartListModel_.get());
    ui->smartList->setModel(selectableProxyModel_);

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
        auto conversation = LRCInstance::getCurrentConversation();
        if (conversation.uid.empty()) {
            return;
        }
        auto thisCallId = conversation.callId;
        auto contactUri = idx.data(static_cast<int>(SmartListModel::Role::URI)).value<QString>().toStdString();

        // let parent deal with this as this dialog will be destroyed
        switch (type_) {
        case Type::CONFERENCE:
            emit contactWillJoinConference(thisCallId, contactUri);
            break;
        case Type::TRANSFER:
            emit contactWillDoTransfer(thisCallId, contactUri);
            break;
        default:
            break;
        }
    }

    QDialog::accept();
}

void
ContactPicker::on_ringContactLineEdit_textChanged(const QString &arg1)
{
    selectableProxyModel_->setFilterRegExp(QRegExp(arg1, Qt::CaseInsensitive, QRegExp::FixedString));
}

void
ContactPicker::mousePressEvent(QMouseEvent *event)
{
    auto contactPickerWidgetRect = ui->contactPickerWidget->rect();
    if (!contactPickerWidgetRect.contains(event->pos())) {
        emit willClose(event);
    }
}

void
ContactPicker::setTitle(const QString& title)
{
    ui->title->setText(title);
}

void
ContactPicker::setType(const Type& type)
{
    type_ = type;
    smartListModel_.reset(new SmartListModel(LRCInstance::getCurrAccId(), this, true));
    selectableProxyModel_->setSourceModel(smartListModel_.get());
    // adjust filter
    switch (type_) {
    case Type::CONFERENCE:
        selectableProxyModel_->setPredicate(
            [this](const QModelIndex& index, const QRegExp& regexp) {
                bool match = regexp.indexIn(index.data(Qt::DisplayRole).toString()) != -1;
                auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>().toStdString();
                auto conversation = LRCInstance::getConversationFromConvUid(convUid);
                if (conversation.uid.empty()) {
                    return false;
                }
                auto callModel = LRCInstance::getCurrentCallModel();
                return  match &&
                        !(callModel->hasCall(conversation.callId) || callModel->hasCall(conversation.confId)) &&
                        !index.parent().isValid();
            });
        break;
    case Type::TRANSFER:
        selectableProxyModel_->setPredicate(
            [this](const QModelIndex& index, const QRegExp& regexp) {
                // Regex to remove current callee
                QRegExp matchExcept= QRegExp(QString("\\b(?!" + CalleeDisplayName_ + "\\b)\\w+"));
                bool match = false;
                bool match_non_self = matchExcept.indexIn(index.data(Qt::DisplayRole).toString()) != -1;
                if (match_non_self) {
                    match = regexp.indexIn(index.data(Qt::DisplayRole).toString()) != -1;
                }
                return  match && !index.parent().isValid();
            });
        break;
    default:
        break;
    }
    selectableProxyModel_->invalidate();
}

void
ContactPicker::setCurrentCalleeDisplayName(const QString& CalleeDisplayName)
{
    CalleeDisplayName_ = CalleeDisplayName;
}
