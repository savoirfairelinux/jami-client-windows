/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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

#include "accountlistmodel.h"
#include "accountitemdelegate.h"
#include "notifypushbutton.h"

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QPainterPath>

namespace Ui {
    class CurrentAccountComboBox;
}

class CurrentAccountComboBox : public QComboBox
{
    Q_OBJECT
        CurrentAccountComboBox(const CurrentAccountComboBox& cpy);

public:
    explicit CurrentAccountComboBox(QWidget* parent = nullptr);
    ~CurrentAccountComboBox();
    void accountListUpdate();
    void setCurrentIndex(int index);
    void updateComboBoxDisplay();
    void canPlaceAudioOnlyCall(const QString& convUid) { emit placeAudioOnlyCall(convUid); }
    void activateComboBox() { showPopup(); }
    void deactivateComboBox() { hidePopup(); }

signals:
    void settingsButtonClicked();
    void newAccountClicked();
    void placeAudioOnlyCall(const QString& convUid);

protected:
    void paintEvent(QPaintEvent* e);
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* mouseEvent);
    void mouseMoveEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);
    void showPopup();
    void hidePopup();

private:
    void connectVoiceMail();
    void importLabelPhoto(int index);
    void setupSettingsButton();
    void setupVoicemailButton();

    AccountItemDelegate* accountItemDelegate_;
    std::unique_ptr<AccountListModel> accountListModel_;

    QPixmap currentAccountAvatarImage_;
    int cellHeight_ = 60; // [screen awareness]
    int avatarSize_ = 48; // [screen awareness]
    const int elidConst = 35; // [screen awareness]
    const int gearBorder_ = 4;
    const int gearSize_ = 24;
    bool popupPresent = false;

    QPoint gearPoint_;
    QLabel gearLabel_;

    QPoint voicemailPoint_;
    NotifyPushButton voicemailButton_;
    const int voicemailBorder_ = 4;
    const int voicemailSize_ = 24;

    std::map<QString, std::pair<int, int>> voicemailMap_;
};
