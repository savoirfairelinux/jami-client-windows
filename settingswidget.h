/***************************************************************************
 * Copyright (C) 2015-2018 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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
#include <QItemSelection>
#include <QSettings>
#include <QErrorMessage>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>

#include "navwidget.h"
#include "accountdetails.h"
#include "accountstatedelegate.h"

#include "advancedsettingswidget.h"

#include "accountmodel.h"
#include "video/devicemodel.h"
#include "codecmodel.h"



namespace Ui {
    class SettingsWidget;
}

class SettingsWidget : public NavWidget
{
    Q_OBJECT
    SettingsWidget(const SettingsWidget& cpy);

public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    ~SettingsWidget();
    void resize();

public slots:
    void updateSettings(int size);

private:
    enum Button {accountSettingsButton, generalSettingsButton, avSettingsButton};

    void setSelected(Button sel);
    void updateAccountInfoDisplayed();

    void resizeEvent(QResizeEvent* event);

    Ui::SettingsWidget* ui;

// currentAccountSettings {
    void verifyRegisteredName();
    void toggleAdvancedSettings();
    void passwordClicked();
    void avatarClicked();
    void replaceWidgets(QWidget* widgetGone, QWidget* widgetAppeared);

    AdvancedSettingsWidget* advancedSettingsWidget_;
    QScrollArea* scrollArea_;
    int leftWidgetSize_ = 324;
    Button pastButton_ = Button::generalSettingsButton;
    bool advancedSettingsDropped_ = false;

    int avatarSize_;
    QTimer* lookupNameTimer_;
    QPushButton* registerNameBtn_;

    QString pixLocation_;

//    QPushButton registeredNameBtn_;

// } currentAccountSettings


// generalSettings {


// } generalSettings

};