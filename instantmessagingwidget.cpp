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
#include <QMenu>

#include "media/text.h"
#include "media/textrecording.h"

#include "imdelegate.h"
#include "globalsystemtray.h"
#include "settingskey.h"
#include "lrcinstance.h"
#include "utils.h"

InstantMessagingWidget::InstantMessagingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InstantMessagingWidget)
{
    ui->setupUi(this);

    this->hide();

    imDelegate_ = new ImDelegate();
    ui->listMessageView->setItemDelegate(imDelegate_);
    ui->listMessageView->setContextMenuPolicy(Qt::ActionsContextMenu);
    auto copyAction = new QAction(tr("Copy"), this);
    ui->listMessageView->addAction(copyAction);
    connect(copyAction, &QAction::triggered, [=]() {
        copyToClipboard();
    });
}

InstantMessagingWidget::~InstantMessagingWidget()
{
    delete ui;
    delete imDelegate_;
}

void
InstantMessagingWidget::setupCallMessaging(const std::string& callId,
                                           MessageModel *messageModel)
{
    ui->listMessageView->disconnect();
    ui->messageEdit->disconnect();
    if (messageModel == nullptr) {
        return;
    }

    using namespace lrc::api;

    ui->listMessageView->setModel(messageModel);
    ui->listMessageView->scrollToBottom();
    connect(ui->messageEdit, &QLineEdit::returnPressed,
        [=]() {
            auto msg = ui->messageEdit->text();
            if (msg.trimmed().isEmpty()) {
                return;
            }
            ui->messageEdit->clear();
            try {
                auto selectedConvUid = LRCInstance::getSelectedConvUid();
                LRCInstance::getCurrentConversationModel()->sendMessage(selectedConvUid, msg.toStdString());
            }
            catch (...) {
                qDebug() << "exception when sending message";
            }
            ui->listMessageView->scrollToBottom();
        });

    QObject::disconnect(newInteractionConnection_);
    newInteractionConnection_ = QObject::connect(LRCInstance::getCurrentConversationModel(), &ConversationModel::newInteraction,
        [this](const std::string& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction) {
            onIncomingMessage(convUid, interactionId, interaction);
        });
}

void
InstantMessagingWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy)) {
        copyToClipboard();
    }
}

void
InstantMessagingWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    ui->messageEdit->setFocus();
}

void
InstantMessagingWidget::copyToClipboard()
{
    auto idx = ui->listMessageView->currentIndex();
    if (idx.isValid()) {
        auto text = ui->listMessageView->model()->data(idx);

        QApplication::clipboard()->setText(text.value<QString>());
    }
}

void
InstantMessagingWidget::updateConversationView(const std::string & convUid)
{
    auto& currentAccountInfo = LRCInstance::getCurrentAccountInfo();
    auto currentConversationModel = currentAccountInfo.conversationModel.get();
    currentConversationModel->clearUnreadInteractions(convUid);
    auto currentConversation = Utils::getConversationFromUid(convUid, *currentConversationModel);
    if (currentConversation == currentConversationModel->allFilteredConversations().end()) {
        return;
    }
    messageModel_.reset(new MessageModel(*currentConversation, currentAccountInfo, this->parent()));
    ui->listMessageView->setModel(messageModel_.get());
    ui->listMessageView->scrollToBottom();
    this->show();
}

void
InstantMessagingWidget::on_sendButton_clicked()
{
    emit ui->messageEdit->returnPressed();
}

void
InstantMessagingWidget::onIncomingMessage(const std::string& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction)
{
    if (!QApplication::activeWindow() && settings_.value(SettingsKey::enableNotifications).toBool()) {
        GlobalSystemTray::instance().showMessage("Ring: Message Received", QString::fromStdString(interaction.body));
        QApplication::alert(this, 5000);
    }
    updateConversationView(convUid);
}
