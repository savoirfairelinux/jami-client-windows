/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#ifndef DELETECONTACTDIALOG_H
#define DELETECONTACTDIALOG_H

#include <QDialog>

// LRC
#include "contactmethod.h"

namespace Ui {
class DeleteContactDialog;
}

class DeleteContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteContactDialog(ContactMethod* cm, Account *ac, QWidget *parent = 0);
    ~DeleteContactDialog();

private slots:
    void on_deleteCancelBtn_clicked();
    void on_deleteAcceptBtn_clicked();
    void on_deleteBanBtn_clicked();

private:
    Ui::DeleteContactDialog *ui;
    ContactMethod* contactMethod_;
    Account* account_;
};

#endif // DELETECONTACTDIALOG_H
