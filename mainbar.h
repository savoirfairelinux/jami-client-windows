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


#ifndef MAINBAR_H
#define MAINBAR_H

#include <QWidget>
#include <QMenu>

#include "navwidget.h"

namespace Ui {
class MainBar;
}

class MainBar : public NavWidget
{
    Q_OBJECT

public:
    explicit MainBar(QWidget *parent = 0);
    ~MainBar();
    void atExit();

//UI SLOTS
private slots:
    void on_confButton_clicked();
    void on_callLineEdit_returnPressed();
    void on_callButton_clicked();
    void on_exitButton_clicked();
    void on_minimizeButton_clicked();
    void showAboutDialog();

private:
    Ui::MainBar *ui;
    void callAction();
    QMenu *menu_;

signals:
    void minimize();
};

#endif // MAINBAR_H
