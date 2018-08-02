/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                                *
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
#include <QKeyEvent>
#include <QSettings>

#include "call.h"
#include "media/media.h"

#include "imdelegate.h"

namespace Ui {
class InstantMessagingWidget;
}

class InstantMessagingWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit InstantMessagingWidget(QWidget *parent = 0);
    ~InstantMessagingWidget();
    void setMediaText(Call* call);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void showEvent(QShowEvent * event) override;

//UI SLOTS
private slots:
    void on_sendButton_clicked();

private slots:
    void mediaAdd(Media::Media *media);
    void onMsgReceived(const QMap<QString, QString>& message);

private:
    Ui::InstantMessagingWidget *ui;
    ImDelegate* imDelegate_;
    QSettings settings_;
    void copyToClipboard();
};

