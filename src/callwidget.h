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

#include "navwidget.h"
#include "smartlistmodel.h"
#include "videoview.h"

#include "api/account.h"
#include "api/contact.h"
#include "api/contactmodel.h"
#include "api/conversationmodel.h"
#include "api/newaccountmodel.h"
#include "api/newcallmodel.h"
#include "globalinstances.h"

#include <QClipboard>
#include <QItemSelection>
#include <QMenu>
#include <QMovie>
#include <QString>
#include <QVector>
#include <QWidget>

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

signals:
    void slotAccountChangedFinished();

public slots:
    virtual void slotAccountListChanged();

private slots:
    void on_ringContactLineEdit_returnPressed();
    void settingsButtonClicked();
    void showChatView(const QModelIndex& nodeIdx);
    void showChatView(const QString& accountId, const lrc::api::conversation::Info & convInfo);
    void setConversationProfileData(const lrc::api::conversation::Info & convInfo);
    void setupChatView(const lrc::api::conversation::Info& convInfo);
    void slotAcceptInviteClicked(const QModelIndex& index);
    void slotBlockInviteClicked(const QModelIndex& index);
    void slotIgnoreInviteClicked(const QModelIndex& index);
    void slotCustomContextMenuRequested(const QPoint & pos);
    void slotAccountChanged(int index);
    void slotShowCallView(const QString & accountId, const lrc::api::conversation::Info & convInfo);
    void slotShowIncomingCallView(const QString & accountId, const lrc::api::conversation::Info & convInfo);
    void slotShowChatView(const QString & accountId, const lrc::api::conversation::Info & convInfo);
    void slotNewTrustRequest(const QString& accountId, const QString& contactUri);
    void slotToggleFullScreenClicked();
    void update();
    void ShowContextMenu(const QPoint& pos);
    void Paste();
    void Copy();
    void CreateCopyPasteContextMenu();
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
    void smartListSelectionChanged(const QItemSelection  &selected, const QItemSelection  &deselected);
    void slotSetChatVisibility(bool visible);
    void slotVideoDeviceListChanged();

private:
    void callTerminating(const QString& callid);
    void onNewInteraction(const QString& accountId, const QString& convUid,
                           uint64_t interactionId, const lrc::api::interaction::Info& interaction);
    void conversationsButtonClicked();
    void invitationsButtonClicked();
    void setupSmartListContextMenu(const QPoint &pos);
    void backToWelcomePage();

    void selectConversation(const QModelIndex& index);
    bool selectConversation(const lrc::api::conversation::Info& item);
    void deselectConversation();
    bool connectConversationModel();
    void updateConversationView(const QString& convUid);
    void showConversationView();
    bool selectSmartlistItem(const QString& convUid);
    QImage imageForConv(const QString & convUid);
    void processContactLineEdit();
    void hideMiniSpinner();
    void updateConversationForNewContact(const QString& convUid);
    void updateSmartList();
    void setSelectedAccount(const QString & accountId);
    void setConversationFilter(const QString& filter);
    void setConversationFilter(lrc::api::profile::Type filter);
    void updateConversationsFilterWidget();
    void updateComboBox();
    void connectAccount(const QString& accId);
    void setCallPanelVisibility(bool visible);
    void updateChatviewFrame();
    void registerShortCuts();

    QMenu* menu_;
    QClipboard* clipboard_;

    Ui::CallWidget* ui;
    QMovie* miniSpinner_;

    constexpr static int qrSize_ = 200;

    QString lastConvUid_ {};
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
    QMetaObject::Connection addedToConferenceConnection_;

};
