/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                                *
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

#include <QWidget>
#include <QItemSelection>
#include <QSettings>

#include "navwidget.h"
#include "accountdetails.h"
#include "accountstatedelegate.h"

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

protected:
    void showEvent(QShowEvent *event);

//UI SLOTS
private slots:
    void on_deviceBox_currentIndexChanged(int index);
    void on_sizeBox_currentIndexChanged(int index);
    void on_deleteAccountButton_clicked();
    void on_addAccountButton_clicked();
    void on_startupBox_toggled(bool checked);
    void on_clearHistoryButton_clicked();
    void on_historyDaySettingsSpinBox_valueChanged(int limit);
    void on_closeOrMinCheckBox_toggled(bool checked);
    void on_checkUpdateButton_clicked();
    void on_autoUpdateCheckBox_toggled(bool checked);
    void on_intervalUpdateCheckSpinBox_valueChanged(int arg1);

private slots:
    void accountSelected(QItemSelection itemSel);
    void deviceIndexChanged(int index);

private:
    Ui::ConfigurationWidget *ui;
    AccountModel* accountModel_;
    Video::DeviceModel* deviceModel_;
    CodecModel* codecModel_;
    bool isLoading_;
    AccountDetails* accountDetails_;
    AccountStateDelegate *accountStateDelegate_;
    QSettings settings_;
};

