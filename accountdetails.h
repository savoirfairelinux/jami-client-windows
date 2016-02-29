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
#include <QMap>
#include <QPushButton>

#include <QTableWidgetItem>

#include "accountmodel.h"
#include "audio/codecmodel.h"
#include "account.h"

namespace Ui {
class AccountDetails;
}

class AccountDetails : public QWidget
{
    Q_OBJECT

public:
    explicit AccountDetails(QWidget* parent = 0);
    ~AccountDetails();
    
    QPushButton* deleteButton();

    void setAccount(Account* currentAccount);
    void save();

//UI SLOTS
private slots:
    void on_upAudioButton_clicked();
    void on_downAudioButton_clicked();
    void on_upVideoButton_clicked();
    void on_downVideoButton_clicked();
    void on_playButton_clicked();

private slots:
    void onCertButtonClicked();
    void ringtonesBoxCurrentIndexChanged(int index);
    void audioCodecSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void videoCodecSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    Ui::AccountDetails* ui;
    CodecModel* codecModel_;
    Account*    currentAccount_;
    typedef void (Account::*ACC_PTR)(const QString&);
    QMap<QString, ACC_PTR > certMap_;

    void stopRingtone();
};

