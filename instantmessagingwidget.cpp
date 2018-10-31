/***************************************************************************
 * Copyright (C) 2015-2018 by Savoir-faire Linux                           *
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

#include "instantmessagingwidget.h"
#include "ui_instantmessagingwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QBuffer>
#include <QMenu>

#include "globalsystemtray.h"
#include "settingskey.h"
#include "lrcinstance.h"
#include "utils.h"

InstantMessagingWidget::InstantMessagingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InstantMessagingWidget),
    messageView_(nullptr)
{
    ui->setupUi(this);

    this->hide();

}

InstantMessagingWidget::~InstantMessagingWidget()
{
    delete ui;
}

void
InstantMessagingWidget::setupCallMessaging(const std::string& convUid,
    MessageWebView* webView)
{
    messageView_ = webView;
    ui->verticalLayout->addWidget(messageView_);

    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(convUid, *convModel);
    convModel->clearUnreadInteractions(convUid);
    messageView_->clear();
    messageView_->printHistory(*convModel, conversation->interactions);

    // Contact Avatars
    auto accInfo = &LRCInstance::getCurrentAccountInfo();
    auto contactUri = conversation->participants.front();
    try {
        auto& contact = accInfo->contactModel->getContact(contactUri);
        if (!contact.profileInfo.avatar.empty()) {
            messageView_->setSenderImage(
                accInfo->contactModel->getContactProfileId(contactUri),
                contact.profileInfo.avatar);
        } else {
            auto avatar = Utils::conversationPhoto(conversation->uid, *accInfo);
            QByteArray ba;
            QBuffer bu(&ba);
            avatar.save(&bu, "PNG");
            std::string avatarString = ba.toBase64().toStdString();
            messageView_->setSenderImage(
                accInfo->contactModel->getContactProfileId(contactUri),
                avatarString);
        }
    } catch (...) {}

    this->show();
}

void
InstantMessagingWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    if (messageView_)
        messageView_->setFocus();
}

void
InstantMessagingWidget::updateConversationView(const std::string & convUid)
{

}

void
InstantMessagingWidget::onIncomingMessage(const std::string& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction)
{
    if (!QApplication::activeWindow() && settings_.value(SettingsKey::enableNotifications).toBool()) {
        GlobalSystemTray::instance().showMessage("Ring: Message Received", QString::fromStdString(interaction.body));
        QApplication::alert(this, 5000);
    }
    /*auto convModel = LRCInstance::getCurrentConversationModel();
    convModel->clearUnreadInteractions(convUid);
    auto currentConversation = Utils::getConversationFromUid(convUid, *convModel);
    if (currentConversation == convModel->allFilteredConversations().end()) {
        return;
    }
    ui->messageView->printNewInteraction(*convModel, interactionId, interaction);*/
}
