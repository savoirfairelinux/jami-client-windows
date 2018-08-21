/**************************************************************************
* Copyright (C) 2015-2017 by Savoir-faire Linux                           *
* Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
* Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#pragma once

#include <QWidget>
#include <QVector>
#include <QString>
#include <QMenu>
#include <QItemSelection>
#include <QMovie>

#include "navwidget.h"
#include "instantmessagingwidget.h"
#include "smartlistmodel.h"

// old LRC
#include "callmodel.h"
#include "video/renderer.h"
#include "video/previewmanager.h"
#include "accountmodel.h"
#include "categorizedhistorymodel.h"
#include "media/textrecording.h"

// new LRC
#include "globalinstances.h"
#include "api/newaccountmodel.h"
#include "api/conversationmodel.h"
#include "api/account.h"
#include "api/contact.h"
#include "api/contactmodel.h"
#include "api/newcallmodel.h"

class SmartListDelegate;
class ImDelegate;
class QPropertyAnimation;

namespace Ui {
class CallWidget;
}

class CallWidget : public NavWidget
{
    Q_OBJECT

public:
    explicit CallWidget(QWidget* parent = 0);
    ~CallWidget();
    bool findRingAccount();

public slots:
    void settingsButtonClicked();
    void showIMOutOfCall(const QModelIndex& nodeIdx);
    void btnComBarVideoClicked();
    void update();

//UI SLOTS
public slots:
    void on_ringContactLineEdit_returnPressed();
    inline void on_entered(const QModelIndex& i){if (i.isValid()) highLightedIndex_ = i;}

private slots:
    void on_acceptButton_clicked();
    void on_refuseButton_clicked();
    void on_cancelButton_clicked();
    void on_smartList_doubleClicked(const QModelIndex& index);
    void on_sendIMButton_clicked();
    void on_imMessageEdit_returnPressed();
    void on_ringContactLineEdit_textChanged(const QString& text);
    void on_imBackButton_clicked();
    void on_sendContactRequestPageButton_clicked();
    void on_sendCRBackButton_clicked();
    void on_smartList_clicked(const QModelIndex &index);
    void on_qrButton_toggled(bool checked);
    void on_shareButton_clicked();
    void on_pendingCRBackButton_clicked();

private slots:
    void smartListSelectionChanged(const QItemSelection  &selected, const QItemSelection  &deselected);
    void contactReqListCurrentChanged(const QModelIndex &currentIdx, const QModelIndex &previousIdx);
    void slotAccountMessageReceived(const QMap<QString,QString> message,ContactMethod* cm, media::Media::Direction dir);
    void onIncomingMessage(::media::TextRecording* t, ContactMethod* cm);
    void onIncomingMessage(const lrc::api::interaction::Info& interaction);
    void searchBtnClicked();
    void currentAccountChanged(const QModelIndex &current);
    void on_contactRequestList_clicked(const QModelIndex &index);

private:
    int outputVolume_;
    int inputVolume_;
    QMenu* menu_;
    SmartListDelegate* smartListDelegate_;
    QPersistentModelIndex highLightedIndex_;
    ImDelegate* imDelegate_;
    QMetaObject::Connection imConnection_;
    QMetaObject::Connection imVisibleConnection_;
    QMetaObject::Connection callChangedConnection_;
    QMetaObject::Connection imClickedConnection_;
    QMetaObject::Connection crListSelectionConnection_;
    QPropertyAnimation* pageAnim_;
    QMenu* shareMenu_;
    Video::Renderer* videoRenderer_;

    // new lrc
    Ui::CallWidget* ui;
    QMovie* miniSpinner_;
    std::string previousConvUid_ {};
    std::unique_ptr<SmartListModel> smartListModel_;
    std::unique_ptr<MessageModel> messageModel_;
    QMetaObject::Connection modelSortedConnection_;
    QMetaObject::Connection modelUpdatedConnection_;
    QMetaObject::Connection filterChangedConnection_;
    QMetaObject::Connection newConversationConnection_;
    QMetaObject::Connection conversationRemovedConnection_;
    QMetaObject::Connection newInteractionConnection_;
    QMetaObject::Connection interactionStatusUpdatedConnection_;
    QMetaObject::Connection conversationClearedConnection;
    QMetaObject::Connection selectedCallChanged_;

    constexpr static int qrSize_ = 200;

private:
    void placeCall();
    void setupOutOfCallIM();
    void setupSmartListMenu(const QPoint &pos);
    void setupQRCode(QString ringID);
    void backToWelcomePage();
    void triggerDeleteContactDialog(ContactMethod *cm, Account *ac);

    // new lrc
    void selectConversation(const QModelIndex& current);
    bool connectConversationModel();
    void updateConversationView(const std::string& convUid);
    void showConversationView();
    void selectSmartlistItem(const std::string& convUid);
    QImage imageForSelectedConv();
    void processContactLineEdit();
    void hideMiniSpinner();
    void updateConversationForNewContact(const std::string& convUid);
    void updateSmartList();
    void setSelectedAccount(const std::string & accountId);

    const std::string& selectedAccountId();
    const std::string& selectedConvUid();
};
