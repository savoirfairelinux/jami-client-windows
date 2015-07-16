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

#ifndef CONTACTPICKER_H
#define CONTACTPICKER_H

#include <QDialog>

#include "personmodel.h"

namespace Ui {
class ContactPicker;
}

class ContactPicker : public QDialog
{
    Q_OBJECT

public:
    explicit ContactPicker(QWidget *parent = 0);
    ~ContactPicker();

     Person *getPersonSelected();

//UI SLOTS
private slots:
    void on_contactView_doubleClicked(const QModelIndex &index);
    void on_cancelButton_clicked();

private:
    Ui::ContactPicker *ui;
    Person *personSelected_;
};

#endif // CONTACTPICKER_H
