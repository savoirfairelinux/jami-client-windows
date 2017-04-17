/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
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

#include <QDialog>

namespace Ui {
class PhotoBoothDialog;
}

class PhotoBoothDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PhotoBoothDialog(QWidget* parent = 0);
    ~PhotoBoothDialog();
public:
    QString fileName_;

protected:
    void closeEvent(QCloseEvent* event);

//UI SLOTS
private slots:
    void on_importButton_clicked();
    void on_takePhotoButton_clicked();

private:
    Ui::PhotoBoothDialog* ui;
};
