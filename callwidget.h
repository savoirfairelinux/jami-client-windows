/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
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

class ContactDelegate;
class HistoryDelegate;
class SmartListDelegate;

namespace Ui {
class CallWidget;
}

class CallWidget : public NavWidget
{
    Q_OBJECT

public:
    explicit CallWidget(QWidget *parent = 0);
    ~CallWidget();
    void atExit();

//UI SLOTS
private slots:
    void on_acceptButton_clicked();
    void on_refuseButton_clicked();
    void on_contactView_doubleClicked(const QModelIndex &index);
    void on_cancelButton_clicked();
    void on_smartList_doubleClicked(const QModelIndex &index);
    void on_searchEdit_returnPressed();
    void on_historyList_doubleClicked(const QModelIndex &index);

private slots:
    void callIncoming(Call *call);
    void addedCall(Call *call, Call *parent);
    void callStateChanged(Call *call, Call::State previousState);
    void findRingAccount(QModelIndex idx1, QModelIndex idx2, QVector<int> vec);
    void smartListSelectionChanged(const QItemSelection &newSel, const QItemSelection &oldSel);

public slots:
    void on_contactButton_clicked(bool checked);
    void on_settingsButton_clicked();
    void on_historicButton_clicked(bool checked);
    void on_ringContactLineEdit_returnPressed();
    void on_btnCall_clicked();
    void on_btnvideo_clicked();
    inline void on_entered(const QModelIndex& i){highLightedIndex = i;};

private:
    Ui::CallWidget *ui;
    Call* actualCall_;
    Video::Renderer* videoRenderer_;
    CallModel* callModel_;
    int outputVolume_;
    int inputVolume_;
    QMenu *menu_;
    ContactDelegate *contactDelegate_;
    HistoryDelegate *historyDelegate_;
    SmartListDelegate* smartListDelegate_;
    QModelIndex highLightedIndex;

private:
    void findRingAccount();
    void setActualCall(Call *value);
    void placeCall();
};

