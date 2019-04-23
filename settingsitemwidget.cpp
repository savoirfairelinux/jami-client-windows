/****************************************************************************
 *    Copyright (C) 2018-2019 Savoir-faire Linux Inc.                       *
 *   Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                     *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "settingsitemwidget.h"

#include "lrcinstance.h"
#include "api/newdevicemodel.h"
#include "ringthemeutils.h"

SettingsItemWidget::SettingsItemWidget(int height, int index, bool banned, QListWidget* parent)
    : QListWidgetItem(parent),
    banned_(banned),
    index_(index),
    height_(height),
    nameEdit_(new QLineEdit(parent)),
    idlabel_(new QLabel(parent)),
    button_(new QPushButton(parent))
{
    setFlags(Qt::NoItemFlags);
    // for all items:
    button_->setGeometry(parent->width() - 50, verMargin_ + height_ * index_, 30, 36);
    nameEdit_->setStyleSheet("border: 0px; border-radius: 3px; background: transparent;");
    idlabel_->setStyleSheet("border: 0px; border-radius: 3px; background: transparent;");
    button_->setStyleSheet("border: 0px; border-radius: 3px; background: transparent;");
    button_->setIconSize(QSize(22, 22));

    nameEdit_->setGeometry(horMargin_, verMargin_ + height_ * index_, 300, 22);
    nameEdit_->setReadOnly(true);

    idlabel_->setGeometry(horMargin_, verMargin_ + 20 + height_ * index_, 400, 22);
    idlabel_->setTextInteractionFlags(Qt::NoTextInteraction);

    // end all items

    if (index_ % 2) {
        setBackgroundColor(Qt::white);
    }
    else {
        setBackgroundColor(RingTheme::smartlistHighlight_);
    }

    if (!banned_) { // populate the device list
        auto deviceList = LRCInstance::getCurrentAccountInfo().deviceModel->getAllDevices();
        auto it = deviceList.begin();
        std::advance(it, index_);

        nameEdit_->setText(QString::fromStdString(it->name));
        idlabel_->setText(QString::fromStdString(it->id));

        if (!index_) {
            button_->setIcon(QPixmap(":/images/icons/round-edit-24px.svg"));
            button_->setToolTip(QObject::tr("Edit Device Name"));

            QObject::connect(button_, &QPushButton::pressed, [this]() { toggleEditable(); });
        } else {
            button_->setIcon(QPixmap(":/images/icons/round-remove_circle-24px.svg"));
            button_->setToolTip(QObject::tr("Unlink Device From Account"));
        }
        nameEdit_->show();
        idlabel_->show();
        button_->show();
    }
    else { // populate the banned contacts list
        auto bannedContactList = LRCInstance::getCurrentAccountInfo().contactModel->getBannedContacts();
        auto it = bannedContactList.begin();
        std::advance(it, index_);

        auto contactInfo = LRCInstance::getCurrentAccountInfo().contactModel->getContact(*it);

        nameEdit_->setText(QString::fromStdString(contactInfo.registeredName));
        idlabel_->setText(QString::fromStdString(contactInfo.profileInfo.uri));
        button_->setIcon(QPixmap(":/images/icons/round-undo-24px.svg"));
        button_->setToolTip(QObject::tr("Unblock Contact"));

        nameEdit_->show();
        idlabel_->show();
        button_->show();
    }

    QObject::connect(nameEdit_, &QLineEdit::editingFinished, [this]() { finishEdit(); });
}

SettingsItemWidget::~SettingsItemWidget()
{
    delete nameEdit_;
    delete idlabel_;
    delete button_;
}

void
SettingsItemWidget::toggleEditable()
{
    if (!editable_) {
        nameEdit_->setReadOnly(false);
        nameEdit_->setStyleSheet("border: 0px; border-radius: 3px; background: white;");
        nameEdit_->setFocus();
        button_->setIcon(QPixmap(":/images/icons/round-save_alt-24px.svg"));
    }
    else {
        finishEdit();
    }
    editable_ =! editable_;
}

void
SettingsItemWidget::finishEdit()
{
    if (!index_) {
        nameEdit_->setReadOnly(true);
        nameEdit_->setStyleSheet("border: 0px; border-radius: 3px; background: transparent;");
        button_->setIcon(QPixmap(":/images/icons/round-edit-24px.svg"));

        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.deviceName = nameEdit_->text().toStdString();
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
    }
}
