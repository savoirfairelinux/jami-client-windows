/***************************************************************************
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

#ifndef CONTACTREQUESTWIDGET_H
#define CONTACTREQUESTWIDGET_H

#include <QWidget>
#include "contactrequest.h"

namespace Ui {
class ContactRequestWidget;
}

class ContactRequestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContactRequestWidget(QWidget *parent = 0);
    ~ContactRequestWidget();
    void setCurrentContactRequest(const QModelIndex &current);

signals:
    void choiceMade();

private slots:
    void on_acceptCRButton_clicked();
    void on_discardCRButton_clicked();
    void on_blockCRButton_clicked();

private:
    Ui::ContactRequestWidget *ui;
    ContactRequest* cr_ = nullptr;
};

#endif // CONTACTREQUESTWIDGET_H
