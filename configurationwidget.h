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

#ifndef CONFIGURATIONWIDGET_H
#define CONFIGURATIONWIDGET_H

#include <QWidget>
#include <QItemSelection>

#include "navwidget.h"
#include "accountdetails.h"

#include "accountmodel.h"
#include "video/devicemodel.h"
#include "audio/codecmodel.h"

namespace Ui {
class ConfigurationWidget;
}

class ConfigurationWidget : public NavWidget
{
    Q_OBJECT

public:
    explicit ConfigurationWidget(QWidget *parent = 0);
    ~ConfigurationWidget();
    void atExit();

//UI SLOTS
private slots:
    void on_deviceBox_currentIndexChanged(int index);
    void on_sizeBox_currentIndexChanged(int index);
    void on_rateBox_currentIndexChanged(int index);
    void on_testVideoButton_toggled(bool checked);
    void on_deleteAccountButton_clicked();
    void on_addAccountButton_clicked();

private slots:
    void accountSelected(QItemSelection itemSel);

private:
    Ui::ConfigurationWidget *ui;
    AccountModel* accountModel_;
    Video::DeviceModel* deviceModel_;
    CodecModel* codecModel_;
    bool isLoading_;
    AccountDetails* accountDetails_;
};

#endif // CONFIGURATIONWIDGET_H
