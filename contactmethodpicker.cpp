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

#include "contactmethodpicker.h"
#include "ui_contactmethodpicker.h"

#include "contactmethod.h"

ContactMethodPicker::ContactMethodPicker(const Person::ContactMethods& cM, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactMethodPicker),
    contactMethods_(cM)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);

    for (auto contactMethod : cM) {
        auto item = new QListWidgetItem();
        item->setText(contactMethod->uri());
        ui->contactMethodListWidget->addItem(item);
    }
}

ContactMethodPicker::~ContactMethodPicker()
{
    delete ui;
}

void
ContactMethodPicker::on_contactMethodListWidget_clicked(const QModelIndex &index)
{
    index_ = index.row();
    accept();
}

ContactMethod*
ContactMethodPicker::getSelected() const
{
    return contactMethods_.at(index_);
}
