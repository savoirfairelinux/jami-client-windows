/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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

#include <QDialog>

namespace Ui {
class DeleteAccountDialog;
}

class DeleteAccountDialog : public QDialog
{
    Q_OBJECT

public:
    DeleteAccountDialog(QWidget *parent = nullptr);
    ~DeleteAccountDialog();

private slots:
    void on_deleteCancelBtn_clicked();
    void on_deleteAcceptBtn_clicked();
signals:
    void deleteAcceptClicked();

private:
    Ui::DeleteAccountDialog *ui;
};