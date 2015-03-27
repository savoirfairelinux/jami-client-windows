/***************************************************************************
 * Copyright (C) 2011-2015 by Savoir-Faire Linux                           *
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

#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include <QWidget>

#include <QTableWidgetItem>

#include "accountmodel.h"
#include "audio/codecmodel.h"

namespace Ui {
class AccountDetails;
}

class AccountDetails : public QWidget
{
    Q_OBJECT

public:
    explicit AccountDetails(QWidget *parent = 0);
    ~AccountDetails();

    void setAccount(Account *currentAccount);
    void reloadCodec();
    void save();

//UI SLOTS
private slots:
    void on_upAudioButton_clicked();
    void on_downAudioButton_clicked();
    void on_upVideoButton_clicked();
    void on_downVideoButton_clicked();
    void on_audioCodecView_itemSelectionChanged();
    void on_videoCodecView_itemSelectionChanged();

private slots:
    void audio_codec_checked(int row, int column);
    void setVideoEnabled(bool enabled);
    void video_codec_checked(int row, int column);
    void setAutoAnswer(bool enabled);
    void aliasChanged();
    void bootstrapChanged();

private:
    Ui::AccountDetails *ui;
    CodecModel* codecModel_;
    Account*    currentAccount_;

};

#endif // ACCOUNTDETAILS_H
