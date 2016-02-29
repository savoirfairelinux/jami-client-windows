/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                           *
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

class ContactDelegate;
class HistoryDelegate;
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

public slots:
    void contactButtonClicked(bool checked);
    void settingsButtonClicked();
    void historicButtonClicked(bool checked);
    void showIMOutOfCall(const QModelIndex& nodeIdx);
    void btnComBarVideoClicked();

//UI SLOTS
public slots:
    void on_ringContactLineEdit_returnPressed();
    void on_btnCall_clicked();
    inline void on_entered(const QModelIndex& i){highLightedIndex_ = i;};

//UI SLOTS
private slots:
    void on_acceptButton_clicked();
    void on_refuseButton_clicked();
    void on_contactView_doubleClicked(const QModelIndex& index);
    void on_cancelButton_clicked();
    void on_smartList_doubleClicked(const QModelIndex& index);
    void on_historyList_doubleClicked(const QModelIndex& index);
    void on_sendButton_clicked();
    void on_messageEdit_returnPressed();
    void on_contactMethodComboBox_currentIndexChanged(const QString& number);
    void on_ringContactLineEdit_textChanged(const QString& text);
    void on_imBackButton_clicked();
    void on_copyCMButton_clicked();
    void on_smartList_clicked(const QModelIndex &index);
    void on_shareButton_clicked();

private slots:
    void callIncoming(Call* call);
    void callStateChanged(Call* call, Call::State previousState);
    void findRingAccount(QModelIndex idx1, QModelIndex idx2, QVector<int> vec);
    void smartListSelectionChanged(const QItemSelection& newSel, const QItemSelection& oldSel);
    void slotAccountMessageReceived(const QMap<QString,QString> message,ContactMethod* cm,Media::Media::Direction dir);
    void onIncomingMessage(::Media::TextRecording* t, ContactMethod* cm);

private:
    Ui::CallWidget* ui;
    Call* actualCall_;
    Video::Renderer* videoRenderer_;
    CallModel* callModel_;
    int outputVolume_;
    int inputVolume_;
    QMenu* menu_;
    ContactDelegate* contactDelegate_;
    HistoryDelegate* historyDelegate_;
    SmartListDelegate* smartListDelegate_;
    QPersistentModelIndex highLightedIndex_;
    ImDelegate* imDelegate_;
    QMetaObject::Connection imConnection_;
    QMetaObject::Connection imVisibleConnection_;
    QPropertyAnimation* pageAnim_;

    constexpr static int animDuration_ = 200; //msecs

private:
    void findRingAccount();
    void setActualCall(Call* value);
    void placeCall();
    void setupOutOfCallIM();
    void setupSmartListMenu();
    void slidePage(QWidget* widget, bool toRight = false);
    void callStateToView(Call* value);
};

