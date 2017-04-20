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

#ifndef QUICKACTCONTACTREQUESTWIDGET_H
#define QUICKACTCONTACTREQUESTWIDGET_H

#include <QWidget>


namespace Ui {
class QuickActContactRequestWidget;
}

class QuickActContactRequestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuickActContactRequestWidget(QWidget *parent = 0);
    ~QuickActContactRequestWidget();

signals:
    void quickValidCRBtnClicked();
    void quickMuteCRBtnClicked();
    void quickBanCRBtnClicked();

private:
    Ui::QuickActContactRequestWidget *ui;
};

#endif // QUICKACTCONTACTREQUESTWIDGET_H
