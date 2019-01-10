/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
* Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
* Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
* Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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

class ConversationItemDelegate;
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

    int getLeftPanelWidth();

    // NavWidget
    virtual void navigated(bool to);
    virtual void updateCustomUI();

public slots:
    void on_ringContactLineEdit_returnPressed();

public slots:
    void settingsButtonClicked();
    void showChatView(const QModelIndex& nodeIdx);
    void showChatView(const std::string & accountId, const lrc::api::conversation::Info & convInfo);
    void setupChatView(const lrc::api::conversation::Info& convInfo);
    void slotAcceptInviteClicked(const QModelIndex& index);
    void slotBlockInviteClicked(const QModelIndex& index);
    void slotIgnoreInviteClicked(const QModelIndex& index);
    void slotCustomContextMenuRequested(const QPoint & pos);
    void slotAccountChanged(int index);
    void slotShowCallView(const std::string & accountId, const lrc::api::conversation::Info & convInfo);
    void slotShowIncomingCallView(const std::string & accountId, const lrc::api::conversation::Info & convInfo);
    void slotShowChatView(const std::string & accountId, const lrc::api::conversation::Info & convInfo);
    void slotToggleFullScreenClicked();
    void slotVideoViewDestroyed(const std::string& callid);
    void update();

private slots:
    void on_acceptButton_clicked();
    void on_refuseButton_clicked();
    void on_cancelButton_clicked();
    void on_smartList_doubleClicked(const QModelIndex& index);
    void on_ringContactLineEdit_textChanged(const QString& text);
    void on_imBackButton_clicked();
    void on_sendContactRequestButton_clicked();
    void on_smartList_clicked(const QModelIndex &index);
    void on_qrButton_toggled(bool checked);
    void on_shareButton_clicked();
    void on_btnAudioCall_clicked();
    void on_btnVideoCall_clicked();

private slots:
    void smartListSelectionChanged(const QItemSelection  &selected, const QItemSelection  &deselected);
    void onIncomingMessage(const std::string & convUid, uint64_t interactionId, const lrc::api::interaction::Info & interaction);

private:
    void placeCall();
    void conversationsButtonClicked();
    void invitationsButtonClicked();
    void setupSmartListContextMenu(const QPoint &pos);
    void setupQRCode(QString ringID);
    void backToWelcomePage();

    void selectConversation(const QModelIndex& index);
    bool selectConversation(const lrc::api::conversation::Info& item,
        lrc::api::ConversationModel& convModel);
    void deselectConversation();
    bool connectConversationModel();
    void updateConversationView(const std::string& convUid);
    void showConversationView();
    bool selectSmartlistItem(const std::string& convUid);
    QImage imageForConv(const std::string & convUid);
    void processContactLineEdit();
    void hideMiniSpinner();
    void updateConversationForNewContact(const std::string& convUid);
    void updateSmartList();
    void setSelectedAccount(const std::string & accountId);
    void setConversationFilter(const QString& filter);
    void setConversationFilter(lrc::api::profile::Type filter);
    void updateConversationsFilterWidget();
    void updateComboBox();
    void connectAccount(const std::string& accId);
    void setCallPanelVisibility(bool visible);

    QMenu* menu_;

    Ui::CallWidget* ui;
    QMovie* miniSpinner_;

    constexpr static int qrSize_ = 200;

    // lrc
    Video::Renderer* videoRenderer_;
    std::string lastConvUid_ {};
    lrc::api::profile::Type currentTypeFilter_{};
    std::unique_ptr<SmartListModel> smartListModel_;

    QMetaObject::Connection imConnection_;
    QMetaObject::Connection imVisibleConnection_;
    QMetaObject::Connection callChangedConnection_;
    QMetaObject::Connection imClickedConnection_;
    QMetaObject::Connection crListSelectionConnection_;
    QMetaObject::Connection modelSortedConnection_;
    QMetaObject::Connection modelUpdatedConnection_;
    QMetaObject::Connection filterChangedConnection_;
    QMetaObject::Connection newConversationConnection_;
    QMetaObject::Connection conversationRemovedConnection_;
    QMetaObject::Connection newInteractionConnection_;
    QMetaObject::Connection interactionStatusUpdatedConnection_;
    QMetaObject::Connection conversationClearedConnection;
    QMetaObject::Connection selectedCallChanged_;
    QMetaObject::Connection smartlistSelectionConnection_;
    QMetaObject::Connection interactionRemovedConnection_;
    QMetaObject::Connection contactAddedConnection_;
    QMetaObject::Connection callStatusChangedConnection_;

};
