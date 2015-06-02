/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
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

#ifndef CALLWIDGET_H
#define CALLWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QMenu>

#include "navwidget.h"
#include "instantmessagingwidget.h"

#include "callmodel.h"
#include "video/renderer.h"
#include "video/previewmanager.h"
#include "accountmodel.h"
#include "categorizedhistorymodel.h"

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
    void on_holdButton_toggled(bool checked);
    void on_hangupButton_clicked();
    void on_callList_activated(const QModelIndex &index);
    void on_muteSpeakerButton_toggled(bool checked);
    void on_muteMicButton_toggled(bool checked);
    void on_speakerSlider_sliderMoved(int position);
    void on_speakerSlider_sliderReleased();
    void on_micSlider_sliderMoved(int position);
    void on_micSlider_sliderReleased();
    void on_contactView_doubleClicked(const QModelIndex &index);
    void on_historyList_doubleClicked(const QModelIndex &index);
    void on_sortComboBox_currentIndexChanged(int index);

private slots:
    void callIncoming(Call *call);
    void addedCall(Call *call, Call *parent);
    void callStateChanged(Call *call, Call::State previousState);
    void findRingAccount(QModelIndex idx1, QModelIndex idx2, QVector<int> vec);
    void mediaAdd(Media::Media* media);
    void checkRegistrationState(Account* account,Account::RegistrationState state);

private:
    Ui::CallWidget *ui;
    Call* actualCall_;
    Video::Renderer* videoRenderer_;
    CallModel* callModel_;
    int outputVolume_;
    int inputVolume_;
    QMenu *menu_;
private:
    void findRingAccount();
    void setActualCall(Call *value);
};

#endif // CALLWIDGET_H
