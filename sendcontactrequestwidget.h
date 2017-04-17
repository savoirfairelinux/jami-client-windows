/**************************************************************************
* Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#ifndef SENDCONTACTREQUESTWIDGET_H
#define SENDCONTACTREQUESTWIDGET_H

#include <QWidget>

namespace Ui {
class SendContactRequestWidget;
}

class SendContactRequestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SendContactRequestWidget(QWidget *parent = 0);
    ~SendContactRequestWidget();
    void setup(const QModelIndex& nodeIdx);
    void sendCR(const QModelIndex& nodeIdx);

private:
    QMetaObject::Connection sendCRClickedConnection_;
    Ui::SendContactRequestWidget *ui;

signals:
    void sendCRclicked();
};

#endif // SENDCONTACTREQUESTWIDGET_H
