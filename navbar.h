/***************************************************************************
 * Copyright (C) 2011-2015 by Savoir-Faire Linux                           *
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

#ifndef NAVBAR_H
#define NAVBAR_H

#include <QWidget>

#include "navwidget.h"

namespace Ui {
class NavBar;
}

class NavBar : public NavWidget
{
    Q_OBJECT

public:
    explicit NavBar(QWidget *parent = 0);
    ~NavBar();
    void atExit();

//UI SLOTS
private slots:
    void on_backButton_clicked();

private:
    Ui::NavBar *ui;
};

#endif // NAVBAR_H
