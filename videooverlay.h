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

#ifndef VIDEOOVERLAY_H
#define VIDEOOVERLAY_H

#include <QWidget>

#include "useractionmodel.h"

namespace Ui {
class VideoOverlay;
}

class VideoOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit VideoOverlay(QWidget *parent = 0);
    ~VideoOverlay();

public:
    void setName(const QString &name);
    void setTime(const QString &time);

//UI SLOTS
private slots:
    void on_holdButton_toggled(bool checked);
    void on_hangupButton_clicked();
    void on_chatButton_toggled(bool checked);

private:
    Ui::VideoOverlay *ui;
    UserActionModel* actionModel_;

signals:
    void setChatVisibility(bool visible);
};

#endif // VIDEOOVERLAY_H
