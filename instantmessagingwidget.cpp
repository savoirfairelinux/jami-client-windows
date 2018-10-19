/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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
InstantMessagingWidget::setMediaText(Call *call)
{
    ui->listMessageView->disconnect();
    ui->messageEdit->disconnect();
    if (call != nullptr) {
        connect(call, SIGNAL(mediaAdded(media::Media*)),
                this, SLOT(mediaAdd(media::Media*)));
        media::Text *textMedia = nullptr;
        if (call->hasMedia(media::Media::Type::TEXT, media::Media::Direction::OUT)) {
            textMedia = call->firstMedia<media::Text>(media::Media::Direction::OUT);
        } else {
            textMedia = call->addOutgoingMedia<media::Text>();
        }
        if (textMedia) {
            ui->listMessageView->setModel(textMedia->recording()->instantMessagingModel());
            ui->listMessageView->scrollToBottom();
            connect(ui->messageEdit, &QLineEdit::returnPressed, [=]() {
                if (not ui->messageEdit->text().trimmed().isEmpty()) {
                    QMap<QString, QString> messages;
                    messages["text/plain"] = ui->messageEdit->text();
                    textMedia->send(messages);
                    ui->messageEdit->clear();
                    ui->listMessageView->scrollToBottom();
                }
            });
        }
    }
}

void
InstantMessagingWidget::mediaAdd(media::Media *media)
{
    switch(media->type()) {
    case media::Media::Type::AUDIO:
        break;
    case media::Media::Type::VIDEO:
        break;
    case media::Media::Type::TEXT:
        if (media->direction() == media::Text::Direction::IN) {
            connect(static_cast<media::Text*>(media),
                    SIGNAL(messageReceived(QMap<QString,QString>)),
                    this,
                    SLOT(onMsgReceived(QMap<QString,QString>)));
            this->show();
        }
        break;
    case media::Media::Type::FILE:
        break;
    default:
        break;
    }
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
InstantMessagingWidget::on_sendButton_clicked()
{
    emit ui->messageEdit->returnPressed();
}

void
InstantMessagingWidget::onMsgReceived(const QMap<QString,QString>& message)
{
    if (!QApplication::activeWindow() && settings_.value(SettingsKey::enableNotifications).toBool()) {
        GlobalSystemTray::instance().showMessage("Ring: Message Received", message["text/plain"]);
        QApplication::alert(this, 5000);
    }
    ui->listMessageView->scrollToBottom();
    this->show();
}
