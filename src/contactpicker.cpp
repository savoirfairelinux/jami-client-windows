/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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
#include "popupdialog.h"

ContactPicker::ContactPicker(QWidget *parent) :
    PopupWidget(parent),
    ui(new Ui::ContactPicker),
    listModeltype_(SmartListModel::Type::CONFERENCE)
{
    ui->setupUi(this);

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
    auto index = ui->smartList->currentIndex();

    if (index.isValid()) {
        // let parent deal with this as this dialog will be destroyed
        switch (listModeltype_) {
        case SmartListModel::Type::CONFERENCE:
        {
            auto sectionName = index.data(SmartListModel::Role::SectionName).value<QString>();
            if (!sectionName.isEmpty()) {
                smartListModel_->toggleSection(sectionName);
                return;
            }
            auto convUid = index.data(SmartListModel::Role::UID).value<QString>();
            auto accId = index.data(SmartListModel::Role::AccountId).value<QString>();
            auto callId = LRCInstance::getCallIdForConversationUid(convUid, accId);
            if (!callId.isEmpty()) {
                emit callWillJoinConference(callId);
            } else {
                auto contactUri = index.data(SmartListModel::Role::URI).value<QString>();
                emit contactWillJoinConference(contactUri);
            }
        }
            break;
        case SmartListModel::Type::TRANSFER:
        {
            auto contactUri = index.data(SmartListModel::Role::URI).value<QString>();
            emit contactWillDoTransfer(contactUri);
        }
            break;
        default:
            break;
        }
    }

    widgetContainer_->accept();
}

void
ContactPicker::on_ringContactLineEdit_textChanged(const QString &arg1)
{
    if (listModeltype_ == SmartListModel::Type::CONFERENCE) {
        smartListModel_->setConferenceableFilter(arg1);
    }
    selectableProxyModel_->setFilterRegExp(QRegExp(arg1, Qt::CaseInsensitive, QRegExp::FixedString));
}

void
ContactPicker::setTitle(const QString& title)
{
    ui->title->setText(title);
}

void
ContactPicker::setType(const SmartListModel::Type& type)
{
    listModeltype_ = type;
    smartListModel_.reset(new SmartListModel(LRCInstance::getCurrAccId(), this, type, LRCInstance::getCurrentConvUid()));
    selectableProxyModel_->setSourceModel(smartListModel_.get());
    // adjust filter
    switch (listModeltype_) {
    case SmartListModel::Type::CONFERENCE:
        selectableProxyModel_->setPredicate(
            [this](const QModelIndex&, const QRegExp&) {
                return true;
            });
        break;
    case SmartListModel::Type::TRANSFER:
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
