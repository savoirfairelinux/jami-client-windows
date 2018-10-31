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

#include "settingswidget.h"
#include "ui_settingswidget.h"

#include "lrcinstance.h"

SettingsWidget::SettingsWidget(QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::SettingsWidget),
    palette(new QPalette)
{
    ui->setupUi(this);

    // exitSettingsButton
    connect(ui->exitSettingsButton, &QPushButton::clicked, this, [=]() {
        emit NavigationRequested(ScreenEnum::CallScreen);
    });

    connect(ui->accountSettingsButton, &QPushButton::clicked, [=]() {
        this->setSelected(Button::accountSettingsButton); }
    );

    connect(ui->generalSettingsButton, &QPushButton::clicked, [=]() {
        this->setSelected(Button::generalSettingsButton); }
    );

    connect(ui->avSettingsButton, &QPushButton::clicked, [=]() {
        this->setSelected(Button::avSettingsButton); }
    );
    setSelected(Button::accountSettingsButton);
    ui->leftSettingsWidget->setGeometry(0, 0, leftWidgetSize_, this->height());
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
    delete palette;
}

void
SettingsWidget::getLeftWidgetSize(int size)
{
    qDebug() << "######################" << size;

    leftWidgetSize_ = size;
    setSelected(Button::accountSettingsButton);
    resize();

}

void
SettingsWidget::resize()
{
    int height = this->height();
    int horizontalSpacerWidth = (ui->rightSettingsWidget->width() - ui->stackedWidget->width()) / 2;

    ui->leftSettingsWidget->setGeometry(0, 0, leftWidgetSize_, height);
    ui->rightSettingsWidget->setGeometry(leftWidgetSize_, 0, this->width() - leftWidgetSize_, height);

    ui->horizontalSpacer_6->changeSize(horizontalSpacerWidth, height);
    ui->horizontalSpacer_3->changeSize(horizontalSpacerWidth, height);

    qDebug() << "&&&&&&&&&&&&&&&&&&&&&&&&&&&" << leftWidgetSize_;
}

void
SettingsWidget::setSelected(Button sel)
{
    switch (sel)
    {
    case Button::accountSettingsButton:
        ui->stackedWidget->setCurrentWidget(ui->currentAccountSettings);
        if (pastButton_ == Button::generalSettingsButton) {

            //ui->accountSettingsButton->setStyleSheet("background: white; border: 0px;");
            //ui->generalSettingsButton->setStyleSheet("background: rgb(237, 237, 237); border: 0px;");
            break;
        }
        else {
            ui->accountSettingsButton->setStyleSheet("background: white; border: 0px;");
            ui->avSettingsButton->setStyleSheet("background: rgb(237, 237, 237); border: 0px;");
            break;
        }
    case Button::generalSettingsButton:
        ui->stackedWidget->setCurrentWidget(ui->generalSettings);
        if (pastButton_ == Button::avSettingsButton) {
            ui->generalSettingsButton->setStyleSheet("background: white; border: 0px;");
            ui->avSettingsButton->setStyleSheet("background: rgb(237, 237, 237); border: 0px;");
            break;
        }
        else {
            ui->generalSettingsButton->setStyleSheet("background: white; border: 0px;");
            ui->accountSettingsButton->setStyleSheet("background: rgb(237, 237, 237); border: 0px;");
            break;
        }
    case Button::avSettingsButton:
        ui->stackedWidget->setCurrentWidget(ui->avSettings);
        if (pastButton_ == Button::accountSettingsButton) {
            ui->avSettingsButton->setStyleSheet("background: white; border: 0px;");
            ui->accountSettingsButton->setStyleSheet("background: rgb(237, 237, 237); border: 0px;");
            break;
        }
        else {
            ui->avSettingsButton->setStyleSheet("background: white; border: 0px;");
            ui->generalSettingsButton->setStyleSheet("background: rgb(237, 237, 237); border: 0px;");
            break;
        }
    }
    pastButton_ = sel;
}