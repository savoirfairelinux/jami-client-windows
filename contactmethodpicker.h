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

#ifndef CONTACTMETHODPICKER_H
#define CONTACTMETHODPICKER_H

#include <QDialog>

#include "person.h"
#include "personmodel.h"

namespace Ui {
class ContactMethodPicker;
}

class ContactMethodPicker final : public QDialog
{
    Q_OBJECT

public:
    explicit ContactMethodPicker(const Person::ContactMethods &cM, QWidget *parent = 0);
    ~ContactMethodPicker();

    ContactMethod* getSelected() const;

//UI SLOTS
private slots:
    void on_contactMethodListWidget_clicked(const QModelIndex &index);

private:
    Ui::ContactMethodPicker *ui;
    const Person::ContactMethods& contactMethods_;
    int index_;
};

#endif // CONTACTMETHODPICKER_H
