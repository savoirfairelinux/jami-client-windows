/**************************************************************************
* Copyright (C) 2015-2017 by Savoir-faire Linux                           *
* Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
* Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include "callmodel.h"
#include "video/renderer.h"
#include "video/previewmanager.h"
#include "accountmodel.h"
#include "categorizedhistorymodel.h"
#include "media/textrecording.h"

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
    void atExit();
    void findRingAccount();

public slots:
    void settingsButtonClicked();
    void showIMOutOfCall(const QModelIndex& nodeIdx);
    void btnComBarVideoClicked();

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
    void callIncoming(Call* call);
    void callStateChanged(Call* call, Call::State previousState);
    void smartListCurrentChanged(const QModelIndex &currentIdx, const QModelIndex &previousIdx);
    void contactReqListCurrentChanged(const QModelIndex &currentIdx, const QModelIndex &previousIdx);
    void slotAccountMessageReceived(const QMap<QString,QString> message,ContactMethod* cm,Media::Media::Direction dir);
    void onIncomingMessage(::Media::TextRecording* t, ContactMethod* cm);
    void callChangedSlot();
    void contactLineEdit_registeredNameFound(Account *account, NameDirectory::LookupStatus status, const QString& address, const QString& name);
    void searchBtnClicked();
    void selectedAccountChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_contactMethodComboBox_currentIndexChanged(int index);
    void on_contactRequestList_clicked(const QModelIndex &index);

private:
    Ui::CallWidget* ui;
    Call* actualCall_;
    Video::Renderer* videoRenderer_;
    CallModel* callModel_;
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
    QMovie* miniSpinner_;

    constexpr static int animDuration_ = 200; //msecs
    constexpr static int qrSize_ = 200;

private:
    void setActualCall(Call* value);
    void placeCall();
    void setupOutOfCallIM();
    void setupSmartListMenu(const QPoint &pos);
    void slidePage(QWidget* widget, bool toRight = false);
    void callStateToView(Call* value);
    void setupQRCode(QString ringID);
    void searchContactLineEditEntry(const URI &uri);
    bool uriNeedNameLookup(const URI uri_passed);
    void processContactLineEdit();
    static Account* getSelectedAccount();
    static bool shouldDisplayInviteButton(ContactMethod& cm);
    void backToWelcomePage();
    void hideMiniSpinner();
    void triggerDeleteContactDialog(ContactMethod *cm, Account *ac);
};
