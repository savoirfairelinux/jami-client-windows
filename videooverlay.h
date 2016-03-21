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
#include <QMenu>

#include "useractionmodel.h"

#include "callutilsdialog.h"
#include "qualitydialog.h"

namespace Ui {
class VideoOverlay;
}

class VideoOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit VideoOverlay(QWidget* parent = 0);
    ~VideoOverlay();

public:
    void setName(const QString& name);
    void setTime(const QString& time);
    inline bool isDialogVisible(){ return dialogVisible_; };

//UI SLOTS
private slots:
    void on_hangupButton_clicked();
    void on_chatButton_toggled(bool checked);
    void on_transferButton_clicked();
    void on_addPersonButton_clicked();
    void on_holdButton_clicked();
    void on_joinButton_clicked();
    void on_noMicButton_clicked();
    void on_noVideoButton_clicked();
    void on_qualityButton_clicked();
    void on_addToContactButton_clicked();
    void on_recButton_clicked();

private:
    Ui::VideoOverlay* ui;
    UserActionModel* actionModel_;
    CallUtilsDialog* transferDialog_;
    QualityDialog* qualityDialog_;
    bool dialogVisible_ = false;

signals:
    void setChatVisibility(bool visible);
};

