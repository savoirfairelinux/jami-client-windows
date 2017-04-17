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

#include "contactpicker.h"
#include "ui_contactpicker.h"

#include <QMouseEvent>

#include "categorizedcontactmodel.h"
#include "personmodel.h"

#include "utils.h"

ContactPicker::ContactPicker(ContactMethod *number, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactPicker),
    personSelected_(nullptr),
    number_(number)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);

    contactProxyModel_ = new OnlyPersonProxyModel(&PersonModel::instance());
    contactProxyModel_->setSortRole(static_cast<int>(Qt::DisplayRole));
    contactProxyModel_->sort(0,Qt::AscendingOrder);
    contactProxyModel_->setFilterRole(Qt::DisplayRole);

    ui->contactView->setModel(contactProxyModel_);
    ui->numberLineEdit->setText(number->uri());
}

ContactPicker::~ContactPicker()
{
    delete ui;
}

void
ContactPicker::on_contactView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    this->accept();
}

void
ContactPicker::accept()
{
    /* Force LRC to update contact model as adding a number
    to a contact without one didn't render him reachable */
    CategorizedContactModel::instance().setUnreachableHidden(false);

    auto idx = ui->contactView->currentIndex();

    //There is only one collection on Windows
    auto personCollection = PersonModel::instance().collections().at(0);

    if (not ui->nameLineEdit->text().isEmpty()) {
        auto *newPerson = new Person();
        newPerson->setFormattedName(ui->nameLineEdit->text());
        Person::ContactMethods cM;
        cM.append(number_);
        newPerson->setContactMethods(cM);
        newPerson->setUid(Utils::GenGUID().toLocal8Bit());
        PersonModel::instance().addNewPerson(newPerson, personCollection);
    } else if (idx.isValid()) {
        auto p = idx.data(static_cast<int>(Person::Role::Object)).value<Person*>();
        Person::ContactMethods cM (p->phoneNumbers());
        cM.append(number_);
        p->setContactMethods(cM);
        p->save();
    }
    CategorizedContactModel::instance().setUnreachableHidden(true);

    QDialog::accept();
}

void
ContactPicker::on_okButton_clicked()
{
    this->accept();
}

void
ContactPicker::on_createNewButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void
ContactPicker::on_searchBar_textChanged(const QString &arg1)
{
    contactProxyModel_->setFilterRegExp(QRegExp(arg1, Qt::CaseInsensitive, QRegExp::FixedString));
}

void
ContactPicker::mousePressEvent(QMouseEvent *event)
{
    mpos_ = event->pos();
    if (not rect().contains(mpos_))
        QDialog::reject();
}

void ContactPicker::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint diff = event->pos() - mpos_;
        QPoint newpos = this->pos() + diff;

        this->move(newpos);
    }
}
