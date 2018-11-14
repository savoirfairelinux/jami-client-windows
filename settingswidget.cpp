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
#include <QPixmap>
#include <QDebug>

#include "settingswidget.h"
#include "ui_settingswidget.h"

#include "ui_advancedsettingswidget.h"

#include "passworddialog.h"

#include "setavatardialog.h"
#include "ui_setavatardialog.h"

#include "lrcinstance.h"

SettingsWidget::SettingsWidget(QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::SettingsWidget),
    scrollArea_(new QScrollArea) // scroll area provides the size that will be visible
{
    ui->setupUi(this);

    ui->accountSettingsButton->setCheckable(true);
    ui->generalSettingsButton->setCheckable(true);
    ui->avSettingsButton->setCheckable(true);
    ui->accountSettingsButton->setChecked(true);


    // exitSettingsButton
    connect(ui->exitSettingsButton, &QPushButton::clicked, this, [=]() {
        emit NavigationRequested(ScreenEnum::CallScreen);
    });

    connect(ui->accountSettingsButton, &QPushButton::clicked, [=]() {
        setSelected(Button::accountSettingsButton); }
    );

    connect(ui->generalSettingsButton, &QPushButton::clicked, [=]() {
        setSelected(Button::generalSettingsButton); }
    );

    connect(ui->avSettingsButton, &QPushButton::clicked, [=]() {
        setSelected(Button::avSettingsButton); }
    );

    connect(ui->passwdPushButton, &QPushButton::clicked, [=]() {
        passwordClicked(); }
    );

    connect(ui->currentAccountAvatar, &QPushButton::clicked, [=]() {
        avatarClicked(); }
    );

    connect(ui->advancedAccountSettingsPButton, &QPushButton::clicked, [=]() {
        toggleAdvancedSettings();
        }
    );

    setSelected(Button::accountSettingsButton);

    ui->displayNameLineEdit->setAlignment(Qt::AlignHCenter);

    ui->currentRegisteredID->setReadOnly(true);
    ui->currentRegisteredID->setStyleSheet("border : 0px;");

    ui->currentRingID->setReadOnly(true);
    ui->displayNameLineEdit->setPlaceholderText(tr("Enter the displayed name"));

    scrollArea_->setWidgetResizable(true);
    scrollArea_->setParent(ui->currentAccountSettings);
    scrollArea_->setWidget(ui->currentAccountSettingsScrollWidget);
    scrollArea_->setStyleSheet("border: 0px;");
    scrollArea_->resize(ui->currentAccountSettingsScrollWidget->width(), this->height());

    avatarSize_ = ui->currentAccountAvatar->width();

    ui->currentAccountAvatar->setIconSize(QSize(avatarSize_, avatarSize_));

    // create ellipse-selectable avatar image
    QRegion avatarClickableRegion(-1, -1,
        ui->currentAccountAvatar->width() + 2, ui->currentAccountAvatar->height() + 2, QRegion::Ellipse);
    ui->currentAccountAvatar->setMask(avatarClickableRegion);

    // accountEnableCheckBox setter
    connect(ui->accountEnableCheckBox, &QCheckBox::stateChanged, [=](int state) {
        LRCInstance::setCurrentAccountEnabledState((bool)state);
        }
    );
    updateAccountInfoDisplayed();
}

SettingsWidget::~SettingsWidget()
{
    delete advancedSettingsWidget_;
    delete scrollArea_;
    delete ui;
}

// called at every callwidget -> settingsWidget navigation
void
SettingsWidget::updateSettings(int size)
{
    leftWidgetSize_ = size;
    setSelected(Button::accountSettingsButton);
    resize();
    updateAccountInfoDisplayed();
}

void
SettingsWidget::resize()
{
    int height = this->height();

    ui->rightSettingsWidget->setGeometry(leftWidgetSize_, 0, this->width() - leftWidgetSize_, height);
    ui->accountSettingsButton->setMinimumSize(leftWidgetSize_, 60);
}

void
SettingsWidget::setSelected(Button sel)
{
    switch (sel)
    {
    case Button::accountSettingsButton:
        ui->stackedWidget->setCurrentWidget(ui->currentAccountSettings);
        if (pastButton_ == Button::generalSettingsButton) {

            ui->accountSettingsButton->setChecked(true);
            ui->generalSettingsButton->setChecked(false);
            break;
        }
        else {
            ui->accountSettingsButton->setChecked(true);
            ui->avSettingsButton->setChecked(false);
            break;
        }
    case Button::generalSettingsButton:
        ui->stackedWidget->setCurrentWidget(ui->generalSettings);
        if (pastButton_ == Button::avSettingsButton) {
            ui->generalSettingsButton->setChecked(true);
            ui->avSettingsButton->setChecked(false);
            break;
        }
        else {
            ui->generalSettingsButton->setChecked(true);
            ui->accountSettingsButton->setChecked(false);
            break;
        }
    case Button::avSettingsButton:
        ui->stackedWidget->setCurrentWidget(ui->avSettings);
        if (pastButton_ == Button::accountSettingsButton) {
            ui->avSettingsButton->setChecked(true);
            ui->accountSettingsButton->setChecked(false);
            break;
        }
        else {
            ui->avSettingsButton->setChecked(true);
            ui->generalSettingsButton->setChecked(false);
            break;
        }
    }
    pastButton_ = sel;
}

// called to update current settings information when navigating to settingsWidget
void
SettingsWidget::updateAccountInfoDisplayed()
{
    ui->currentRegisteredID->setText(QString::fromStdString(LRCInstance::getCurrentAccountInfo().registeredName));
    ui->currentRingID->setText(QString::fromStdString(LRCInstance::getCurrentAccountInfo().profileInfo.uri));

    ui->currentRegisteredID->setReadOnly(true);
    disconnect(ui->currentRegisteredID);

// if no registered name is found for account
    if (LRCInstance::getCurrentAccountInfo().registeredName.empty()) {
        ui->currentRegisteredID->setReadOnly(false);
        connect(ui->currentRegisteredID, &QLineEdit::textChanged, [=]() {
            verifyRegisteredName();
            }
        );
    }

    ui->currentAccountAvatar->setIcon(LRCInstance::getCurrentAccountData(AccountListModel::Role::Picture).value<QPixmap>().
        scaledToHeight(avatarSize_, Qt::SmoothTransformation));


    // accountEnableCheckBox getter
    LRCInstance::getCurrentAccountInfo().enabled ? ui->accountEnableCheckBox->setChecked(true) :
        ui->accountEnableCheckBox->setChecked(false);

    qDebug() << "updateAccountInfoDisplayed() is called too much\n";
}

void
SettingsWidget::passwordClicked()
{
    PasswordDialog passwdDialog(this);
    passwdDialog.exec();
}

void
SettingsWidget::toggleAdvancedSettings()
{
    if (advancedSettingsDropped_) {
        ui->advancedAccountSettingsPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_down-24px.svg"));
        ui->advancedAccountSettingsPButton->setStyleSheet("background: rgb(245, 245, 245); padding-right: 25px;");
        ui->currentAccountSettingsScrollLayout->removeWidget(advancedSettingsWidget_);
        delete advancedSettingsWidget_;
    }
    else { // if advanced settings shown
        ui->advancedAccountSettingsPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_up-24px.svg"));
        ui->advancedAccountSettingsPButton->setStyleSheet("background: rgb(250, 250, 250); padding-right:25px;");
        advancedSettingsWidget_ = new AdvancedSettingsWidget;
        ui->currentAccountSettingsScrollLayout->addWidget(advancedSettingsWidget_);
    }
    advancedSettingsDropped_ = !advancedSettingsDropped_;
}

void
SettingsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    scrollArea_->resize(ui->currentAccountSettingsScrollWidget->width(), this->height());
}

void
SettingsWidget::avatarClicked()
{
    SetAvatarDialog avatarDialog(this);

    // return new avatar pixmap from setAvatarDialog
    connect(&avatarDialog, &SetAvatarDialog::pixmapSignal, [&](const std::string& pixString) {
            if (!pixString.empty()) {
                LRCInstance::setCurrentAccountAvatar(pixString);
                updateAccountInfoDisplayed();
            }
        }
    );
    avatarDialog.exec();
}

void
SettingsWidget::verifyRegisteredName()
{
    QString registeredName = ui->currentRegisteredID->text();
    if (!registeredName.isEmpty()) {
        //lookupNameTimer_ = new QTimer(this); // implement timer [todo]
        //lookupNameTimer_->setSingleShot(true);

        switch (LRCInstance::currentAccountRegisteredNameValidity(registeredName)) {
        case 0:
            ui->currentRegisteredID->setStyleSheet("border : 1px solid green;");
            registerNameBtn_ = new QPushButton(ui->currentAccountSettingsScrollWidget);
            registerNameBtn_->setText(tr("Confirm"));
            registerNameBtn_->setToolTip(tr("Click here to confirm your registry")); // set button colors in stylesheet [todo]
            replaceWidgets(ui->currentRegisteredIDMessage, registerNameBtn_);
            break;
        case 1:
            ui->currentRegisteredID->setStyleSheet("border : 1px solid red;");
            break;
        case 2:
            ui->currentRegisteredID->setStyleSheet("border : 1px solid orange;");
            break;
        }
    }
    else {
        ui->currentRegisteredID->setStyleSheet("border : 0px;");
    }
}

// ensure parents are the same for both widgets before calling function
void
SettingsWidget::replaceWidgets(QWidget* widgetGone, QWidget* widgetAppeared)
{
    widgetAppeared->setGeometry(widgetGone->x(), widgetGone->y(), widgetGone->width(), widgetGone->height());
    widgetGone->hide();
    widgetAppeared->show();
}