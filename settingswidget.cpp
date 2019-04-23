/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony L�onard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.  *
 **************************************************************************/

#include "settingswidget.h"
#include "ui_settingswidget.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QModelIndex>
#include <QPixmap>
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

#include "deleteaccountdialog.h"
#include "passworddialog.h"
#include "nameregistrationdialog.h"
#include "setavatardialog.h"
#include "settingsitemwidget.h"
#include "settingskey.h"
#include "utils.h"

#include "api/newdevicemodel.h"
#include "audio/inputdevicemodel.h"
#include "audio/outputdevicemodel.h"
#include "audio/settings.h"
#include "callmodel.h"
#include "media/recordingmodel.h"
#include "video/channel.h"
#include "video/devicemodel.h"
#include "video/previewmanager.h"
#include "video/rate.h"
#include "video/resolution.h"

#ifdef Q_OS_WIN
#include "winsparkle.h"
#endif

SettingsWidget::SettingsWidget(QWidget* parent)
    : NavWidget(parent)
    , ui(new Ui::SettingsWidget)
    , scrollArea_(new QScrollArea(this))
    , scrollSIPArea_(new QScrollArea(this))
    , deviceModel_(&Video::DeviceModel::instance())
    , gif(new QMovie(":/images/ajax-loader.gif"))
{
    ui->setupUi(this);

    ui->accountSettingsButton->setChecked(true);

    ui->btnExitSettings->setIconSize(QSize(24, 24));
    ui->btnExitSettings->setIcon(QPixmap(":/images/icons/round-close-24px.svg"));

    //Show Password
    ui->passSIPlineEdit->setEchoMode(QLineEdit::Password);

    //display name SIP
    ui->displaySIPNameLineEdit->setAlignment(Qt::AlignHCenter);

    //SIP User Name
    ui->usernameSIP->setStyleSheet("border : 0px;");

    //SIP Advanced Setting initilization
    ui->advancedAccountSettingsSIPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_down-24px.svg"));

    //SIP Avatar
    avatarSIPSize_ = ui->currentSIPAccountAvatar->width();
    ui->currentSIPAccountAvatar->setIconSize(QSize(avatarSIPSize_, avatarSIPSize_));
    QRegion avatarSIPClickableRegion(-1, -1,
        ui->currentSIPAccountAvatar->width() + 2, ui->currentSIPAccountAvatar->height() + 2, QRegion::Ellipse);
    ui->currentSIPAccountAvatar->setMask(avatarSIPClickableRegion);

    ui->scrollSIPArea->verticalScrollBar()->setEnabled(true);
    ui->scrollSIPArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 10px; }");
    ui->advancedSIPSettingsWidget->setVisible(false);

    // display name (aka alias)
    ui->displayNameLineEdit->setAlignment(Qt::AlignHCenter);
    ui->displayNameLineEdit->setPlaceholderText(tr("Enter the displayed name"));

    ui->currentRegisteredID->setReadOnly(true);
    ui->currentRegisteredID->setStyleSheet("border : 0px;");

    ui->currentRingID->setReadOnly(true);

    //Ring Avatar
    avatarSize_ = ui->currentAccountAvatar->width();

    ui->currentAccountAvatar->setIconSize(QSize(avatarSize_, avatarSize_));

    // create ellipse-selectable avatar image
    QRegion avatarClickableRegion(-1, -1,
        ui->currentAccountAvatar->width() + 2, ui->currentAccountAvatar->height() + 2, QRegion::Ellipse);
    ui->currentAccountAvatar->setMask(avatarClickableRegion);

    ui->advancedAccountSettingsPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_down-24px.svg"));
    ui->linkDevPushButton->setIcon(QPixmap(":/images/icons/round-add-24px.svg"));
    ui->blockedContactsBtn->setIcon(QPixmap(":/images/icons/round-arrow_drop_up-24px.svg"));

    ui->scrollArea->verticalScrollBar()->setEnabled(true);
    ui->scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 10px; }");

    ui->advancedSettingsWidget->setVisible(false);

    auto accountList = LRCInstance::accountModel().getAccountList();

    ui->containerWidget->setVisible(false);
}

void SettingsWidget::slotAccountOnBoarded()
{
    setSelected(Button::accountSettingsButton);
    setConnections();
}

void SettingsWidget::navigated(bool to)
{
    ui->containerWidget->setVisible(to);
    if (to) {
        setSelected(Button::accountSettingsButton);
        updateAccountInfoDisplayed();
    }
}

void SettingsWidget::updateCustomUI()
{
}

void SettingsWidget::leaveSettingsSlot()
{
    if (advancedSettingsDropped_) {
        toggleAdvancedSettings();
    }

    if (advancedSIPSettingsDropped_) {
        toggleAdvancedSIPSettings();
    }

    Video::PreviewManager::instance().stopPreview();

    emit NavigationRequested(ScreenEnum::CallScreen);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::setSelected(Button sel)
{
    switch (sel) {
    case Button::accountSettingsButton:
        Video::PreviewManager::instance().stopPreview();

        if (LRCInstance::getCurrentAccountInfo().profileInfo.type == lrc::api::profile::Type::SIP) {
            ui->stackedWidget->setCurrentWidget(ui->currentSIPAccountSettingsScrollWidget);

            if (advancedSIPSettingsDropped_) {
                toggleAdvancedSIPSettings();
            }

        } else {
            ui->stackedWidget->setCurrentWidget(ui->currentAccountSettingsScrollWidget);
        }

        if (pastButton_ == Button::generalSettingsButton) {
            ui->accountSettingsButton->setChecked(true);
            ui->generalSettingsButton->setChecked(false);
            break;

        } else {
            if (advancedSettingsDropped_) {
                toggleAdvancedSettings();
            }

            ui->accountSettingsButton->setChecked(true);
            ui->avSettingsButton->setChecked(false);
            break;
        }

    case Button::generalSettingsButton:
        Video::PreviewManager::instance().stopPreview();

        ui->stackedWidget->setCurrentWidget(ui->generalSettings);
        populateGeneralSettings();

        if (pastButton_ == Button::avSettingsButton) {
            ui->generalSettingsButton->setChecked(true);
            ui->avSettingsButton->setChecked(false);
            break;

        } else {
            ui->generalSettingsButton->setChecked(true);
            ui->accountSettingsButton->setChecked(false);
            break;
        }

    case Button::avSettingsButton:
        ui->stackedWidget->setCurrentWidget(ui->avSettings);
        populateAVSettings();

        if (pastButton_ == Button::accountSettingsButton) {
            ui->avSettingsButton->setChecked(true);
            ui->accountSettingsButton->setChecked(false);
            break;

        } else {
            ui->avSettingsButton->setChecked(true);
            ui->generalSettingsButton->setChecked(false);
            break;
        }
    }

    pastButton_ = sel;
}

// called to update current settings information when navigating to settingsWidget
void SettingsWidget::updateAccountInfoDisplayed()
{
    auto& accInfo = LRCInstance::getCurrentAccountInfo();
    auto accConfig = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());

    ui->usernameSIP->setText(QString::fromStdString(accConfig.username));
    ui->hostnameSIP->setText(QString::fromStdString(accConfig.hostname));
    ui->passSIPlineEdit->setText(QString::fromStdString(accConfig.password));
    ui->ProxySIP->setText(QString::fromStdString(accConfig.proxyServer));

    ui->currentRegisteredID->setText(QString::fromStdString(accInfo.registeredName));
    ui->currentRingID->setText(QString::fromStdString(accInfo.profileInfo.uri));

    // if no registered name is found for account
    if (accInfo.registeredName.empty()) {
        ui->currentRegisteredID->setReadOnly(false);

    } else {
        ui->currentRegisteredID->setReadOnly(true);
        setRegNameUi(RegName::BLANK);
    }

    //Sip Avator set
    ui->currentSIPAccountAvatar->setIcon(LRCInstance::getCurrAccPixmap().scaledToHeight(avatarSize_, Qt::SmoothTransformation));

    //Ring Avator set
    ui->currentAccountAvatar->setIcon(LRCInstance::getCurrAccPixmap().scaledToHeight(avatarSize_, Qt::SmoothTransformation));

    ui->accountEnableCheckBox->setChecked(accInfo.enabled);
    ui->accountSIPEnableCheckBox->setChecked(accInfo.enabled);

    ui->displayNameLineEdit->setText(QString::fromStdString(accInfo.profileInfo.alias));
    ui->displaySIPNameLineEdit->setText(QString::fromStdString(accInfo.profileInfo.alias));

    updateAndShowDevicesSlot();
    bannedContactsShown_ = false;

    ui->blockedContactsBtn->setVisible(accInfo.contactModel->getBannedContacts().size());
}

void SettingsWidget::passwordClicked()
{
    PasswordDialog passwdDialog(this);
    passwdDialog.exec();
}

void SettingsWidget::showHideButtonClicked()
{
    if (sipPasswordHidden_) {
        ui->passSIPlineEdit->setEchoMode(QLineEdit::Password);
        ui->showHideButton->setIcon(QPixmap(":/images/icons/ic_show_password.png"));
    } else {
        ui->passSIPlineEdit->setEchoMode(QLineEdit::Normal);
        ui->showHideButton->setIcon(QPixmap(":/images/icons/ic_hide_password.png"));
    }
    sipPasswordHidden_ = !sipPasswordHidden_;
}

void SettingsWidget::toggleAdvancedSIPSettings()
{

    if (advancedSIPSettingsDropped_) {
        ui->advancedAccountSettingsSIPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_down-24px.svg"));
        ui->scrollBarSIPLabel->show();
        ui->advancedSIPSettingsWidget->setVisible(false);

    } else {
        ui->advancedAccountSettingsSIPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_up-24px.svg"));
        ui->scrollBarSIPLabel->hide();
        ui->advancedSIPSettingsWidget->setVisible(true);
        ui->advancedSIPSettingsWidget->updateAdvancedSIPSettings();
        QTimer::singleShot(50, this,
            [this] {
                auto top = ui->advancedAccountSettingsSIPButton->frameGeometry().top();
                ui->scrollSIPArea->verticalScrollBar()->setSliderPosition(top);
            });
    }

    advancedSIPSettingsDropped_ = !advancedSIPSettingsDropped_;
}

void SettingsWidget::toggleAdvancedSettings()
{
    if (advancedSettingsDropped_) {
        ui->advancedAccountSettingsPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_down-24px.svg"));
        ui->scrollBarLabel->show();
        ui->advancedSettingsWidget->setVisible(false);
    } else {
        ui->advancedAccountSettingsPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_up-24px.svg"));
        ui->scrollBarLabel->hide();
        ui->advancedSettingsWidget->setVisible(true);
        ui->advancedSettingsWidget->updateAdvancedSettings();
        QTimer::singleShot(50, this,
            [this] {
                auto top = ui->advancedAccountSettingsPButton->frameGeometry().top();
                ui->scrollArea->verticalScrollBar()->setSliderPosition(top);
            });
    }

    advancedSettingsDropped_ = !advancedSettingsDropped_;
}

void SettingsWidget::toggleBannedContacts()
{
    if (bannedContactsShown_) { // will show linked devices next
        bannedContactsShown_ = false;
        updateAndShowDevicesSlot();

    } else { // will show banned contacts next
        bannedContactsShown_ = true;
        updateAndShowBannedContactsSlot();
    }
}

void SettingsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    scrollSIPArea_->resize(ui->currentSIPAccountSettingsScrollWidget->width(), this->height());
    scrollArea_->resize(ui->currentAccountSettingsScrollWidget->width(), this->height());
}

void SettingsWidget::avatarClicked()
{
    SetAvatarDialog avatarDialog(this);

    // return new avatar pixmap from setAvatarDialog
    connect(&avatarDialog, &SetAvatarDialog::pixmapSignal,
        [this](const std::string& pixString) {
            if (!pixString.empty()) {
                LRCInstance::setCurrAccAvatar(pixString);
                updateAccountInfoDisplayed();
            }
        });
    avatarDialog.exec();
}

void SettingsWidget::verifyRegisteredNameSlot()
{
    if (!LRCInstance::getCurrentAccountInfo().registeredName.empty()) {
        setRegNameUi(RegName::BLANK);

    } else {
        registeredName_ = ui->currentRegisteredID->text().simplified();

        if (!registeredName_.isEmpty()) {
            if (validateRegNameForm(registeredName_)) { // name has valid form
                setRegNameUi(RegName::SEARCHING);
                QTimer::singleShot(300, this, SLOT(beforeNameLookup()));

            } else { // name does not have valid form
                setRegNameUi(RegName::INVALIDFORM);
            }

        } else {
            setRegNameUi(RegName::BLANK);
        }
    }
}

// returns true if name is valid registered name
bool SettingsWidget::validateRegNameForm(const QString& regName)
{
    QRegularExpression regExp(" ");

    if (regName.size() > 2 && !regName.contains(regExp)) {
        return true;

    } else {
        return false;
    }
}

void SettingsWidget::receiveRegNameSlot(const std::string& accountID,
    lrc::api::account::LookupStatus status, const std::string& address, const std::string& name)
{
    Q_UNUSED(accountID);
    Q_UNUSED(address);
    afterNameLookup(status, name);
}

void SettingsWidget::beforeNameLookup()
{
    NameDirectory::instance().lookupName(nullptr, QString(), registeredName_);
}

void SettingsWidget::afterNameLookup(lrc::api::account::LookupStatus status, const std::string& regName)
{
    if (registeredName_.toStdString() == regName && regName.length() > 2) {
        if (status == lrc::api::account::LookupStatus::NOT_FOUND) {
            setRegNameUi(RegName::FREE);

        } else {
            setRegNameUi(RegName::TAKEN);
        }

    } else {
        setRegNameUi(RegName::BLANK);
    }
}

void SettingsWidget::setRegNameUi(RegName stat)
{
    disconnect(gif, SIGNAL(frameChanged(int)), this, SLOT(setButtonIconSlot(int)));
    disconnect(ui->regNameButton, &QPushButton::clicked, this, &SettingsWidget::slotRegisterName);

    switch (stat) {
    case RegName::BLANK:
        ui->currentRegisteredID->setStyleSheet("padding-left: 5px; border: 0px; border-radius: 3px; border: 1px solid rgb(245, 245, 245);");
        regNameBtn_ = false;
        ui->currentRegisteredID->setToolTip(tr(""));
        ui->regNameButton->setIcon(QPixmap());
        ui->regNameButton->setEnabled(false);
        break;

    case RegName::INVALIDFORM:
        ui->currentRegisteredID->setStyleSheet("padding-left: 5px; border: 1px solid red; border-radius: 3px;");
        regNameBtn_ = false;
        ui->currentRegisteredID->setToolTip(tr("A registered name should not have any spaces and must be at least three letters long"));
        ui->regNameButton->setIcon(QPixmap(":/images/icons/round-error-24px.svg"));
        ui->regNameButton->setToolTip(tr("A registered name should not have any spaces and must be at least three letters long"));
        ui->regNameButton->setEnabled(true);
        break;

    case RegName::TAKEN:
        ui->currentRegisteredID->setStyleSheet("padding-left: 5px; border: 1px solid orange; border-radius: 3px;");
        regNameBtn_ = false;
        ui->currentRegisteredID->setToolTip(tr("This name is already taken"));
        ui->regNameButton->setIcon(QPixmap(":/images/icons/round-error-24px.svg"));
        ui->regNameButton->setToolTip(tr("This registered name is already taken"));
        ui->regNameButton->setEnabled(true);
        break;

    case RegName::FREE:
        ui->currentRegisteredID->setStyleSheet("padding-left: 5px; border: 1px solid green; border-radius: 3px;");
        regNameBtn_ = true;
        ui->currentRegisteredID->setToolTip(tr("This name is available"));
        ui->regNameButton->setIcon(QPixmap(":/images/icons/round-check_circle-24px.svg"));
        ui->regNameButton->setToolTip(tr("Register this name"));
        ui->regNameButton->setEnabled(true);

        connect(ui->regNameButton, &QPushButton::clicked, this, &SettingsWidget::slotRegisterName);
        break;

    case RegName::SEARCHING:
        ui->currentRegisteredID->setStyleSheet("padding-left: 5px; border: 1px solid rgb(2, 187, 213); border-radius: 3px;");
        regNameBtn_ = false;
        ui->currentRegisteredID->setToolTip(tr(""));

        connect(gif, SIGNAL(frameChanged(int)), this, SLOT(setButtonIconSlot(int)));
        gif->start();
        ui->regNameButton->setEnabled(false);
        break;
    }
}

void SettingsWidget::setButtonIconSlot(int frame)
{
    Q_UNUSED(frame);
    ui->regNameButton->setIcon(QIcon(gif->currentPixmap()));
}

void
SettingsWidget::slotRegisterName()
{
    if (!regNameBtn_) {
        return;
    }

    NameRegistrationDialog nameRegistrationDialog(registeredName_, this);

    if (nameRegistrationDialog.exec() == QDialog::Accepted) {
        ui->currentRegisteredID->setReadOnly(true);
        ui->currentRegisteredID->setText(registeredName_);

        lrc::api::account::ConfProperties_t accountProperties = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), accountProperties);

    } else {
        ui->currentRegisteredID->setText("");
        registeredName_ = "";
    }

    setRegNameUi(RegName::BLANK);
}

void SettingsWidget::setAccEnableSlot(int state)
{
    LRCInstance::editableAccountModel()->enableAccount(LRCInstance::getCurrAccId(), (bool)state);

    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void SettingsWidget::delAccountSlot()
{
    DeleteAccountDialog delDialog(this);
    auto ret = delDialog.exec();

    if (ret == QDialog::Accepted) {
        LRCInstance::setSelectedAccountId("");

        if (!LRCInstance::accountModel().getAccountList().size()) {
            emit NavigationRequested(ScreenEnum::WizardScreen);

        } else {
            LRCInstance::setSelectedConvId("");
            emit NavigationRequested(ScreenEnum::CallScreen);
        }
    }
}

void SettingsWidget::removeDeviceSlot(int index)
{
    if (!index) {
        return;
    }

    auto deviceList = LRCInstance::getCurrentAccountInfo().deviceModel->getAllDevices();
    auto it = deviceList.begin();

    std::advance(it, index);
    QString psswd;

    bool ok = false;

    if (LRCInstance::getCurrAccConfig().archiveHasPassword) {
        psswd = QInputDialog::getText(this, tr("Remove Device"),
            tr("Enter this account's password to confirm the removal of this device"), QLineEdit::Password,
            QDir::home().dirName(), &ok);

    } else {
        psswd = "";
        QMessageBox devDel;
        devDel.setText(tr("Please confirm that you wish to remove this device"));
        devDel.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        devDel.setDefaultButton(QMessageBox::Cancel);

        if (devDel.exec() == QMessageBox::Ok) {
            goto delete_;
        }
    }

    if (ok) {
    delete_:
        LRCInstance::getCurrentAccountInfo().deviceModel->revokeDevice(it->id, psswd.toStdString());
        updateAndShowDevicesSlot();
    }
}

void SettingsWidget::unban(int index)
{
    auto bannedContactList = LRCInstance::getCurrentAccountInfo().contactModel->getBannedContacts();
    auto it = bannedContactList.begin();
    std::advance(it, index);

    auto contactInfo = LRCInstance::getCurrentAccountInfo().contactModel->getContact(*it);

    LRCInstance::getCurrentAccountInfo().contactModel->addContact(contactInfo);
    updateAndShowBannedContactsSlot();
}

void SettingsWidget::exportAccountSlot()
{
    QFileDialog dialog(this);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Export Account Here"),
        QDir::homePath() + "/Desktop", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        LRCInstance::accountModel().exportToFile(LRCInstance::getCurrAccId(), (dir + "/export.gz").toStdString());
    }
}

void SettingsWidget::updateAndShowDevicesSlot()
{
    ui->settingsListWidget->clear();

    ui->label->setText(tr("Linked Devices"));
    ui->blockedContactsBtn->setText(tr("Blocked Contacts"));
    ui->linkDevPushButton->show();

    auto deviceList = LRCInstance::getCurrentAccountInfo().deviceModel->getAllDevices();

    int i = 0;

    for (auto it = deviceList.begin(); it != deviceList.end(); ++it, ++i) {
        SettingsItemWidget* item = new SettingsItemWidget(itemHeight_, i, false, ui->settingsListWidget);
        item->setSizeHint(QSize(ui->settingsListWidget->width(), itemHeight_));
        ui->settingsListWidget->addItem(item);

        if (i) {
            connect(item->button_, &QPushButton::clicked, [this, i]() {
                removeDeviceSlot(i);
            });
        }
    }
}

void SettingsWidget::updateAndShowBannedContactsSlot()
{
    if (bannedContactsShown_) {
        ui->settingsListWidget->clear();

        ui->label->setText(tr("Blocked Contacts"));
        ui->blockedContactsBtn->setText(tr("Linked Devices"));
        ui->linkDevPushButton->hide();

        auto bannedContactList = LRCInstance::getCurrentAccountInfo().contactModel->getBannedContacts();

        int i = 0;

        for (auto it = bannedContactList.begin(); it != bannedContactList.end(); ++it, ++i) {
            SettingsItemWidget* item = new SettingsItemWidget(itemHeight_, i, true, ui->settingsListWidget);
            item->setSizeHint(QSize(ui->settingsListWidget->width(), itemHeight_));
            ui->settingsListWidget->addItem(item);

            connect(item->button_, &QPushButton::clicked, [this, i]() {
                unban(i);
            });
        }

        if (!bannedContactList.size()) {
            updateAndShowDevicesSlot();
            ui->blockedContactsBtn->hide();
        }
    }
}

void SettingsWidget::showLinkDevSlot()
{
    if (!advancedSettingsWidget_) {
        delete advancedSettingsWidget_;
    }

    linkDevWidget = new LinkDevWidget(ui->scrollAreaWidgetContents);
    linkDevWidget->setMinimumWidth(600);

    ui->accountHorLayout->insertWidget(1, linkDevWidget);

    linkDevWidget->show();
    ui->centralWidget->hide();

    connect(linkDevWidget->cancelBtn(), &QPushButton::clicked, this, &SettingsWidget::showCurrentAccountSlot);
    connect(linkDevWidget->endCancelBtn(), &QPushButton::clicked, this, &SettingsWidget::showCurrentAccountSlot);
}

void SettingsWidget::showCurrentAccountSlot()
{
    disconnect(linkDevWidget);
    delete linkDevWidget;

    ui->centralWidget->show();
    updateAndShowDevicesSlot();
}

void SettingsWidget::setConnections()
{
    // btnExitSettings
    connect(ui->btnExitSettings, &QPushButton::clicked, this, &SettingsWidget::leaveSettingsSlot);

    connect(ui->accountSettingsButton, &QPushButton::clicked, [this]() {
        setSelected(Button::accountSettingsButton);
    });

    connect(ui->generalSettingsButton, &QPushButton::clicked, [this]() {
        setSelected(Button::generalSettingsButton);
    });

    connect(ui->avSettingsButton, &QPushButton::clicked, [this]() {
        setSelected(Button::avSettingsButton);
    });

    connect(ui->currentSIPAccountAvatar, &QPushButton::clicked, [this]() {
        avatarClicked();
    });

    connect(ui->advancedAccountSettingsSIPButton, &QPushButton::clicked, this, &SettingsWidget::toggleAdvancedSIPSettings);

    // connect "delete SIP account" button
    connect(ui->btnSIPDeletAccount, &QPushButton::clicked, this, &SettingsWidget::delAccountSlot);

    connect(ui->accountSIPEnableCheckBox, &QCheckBox::clicked, this, &SettingsWidget::setAccEnableSlot);

    connect(ui->displaySIPNameLineEdit, &QLineEdit::editingFinished,
        [this] {
            LRCInstance::setCurrAccDisplayName(ui->displaySIPNameLineEdit->text().toStdString());
        });

    connect(ui->showHideButton, &QPushButton::clicked,
        [this]() {
            showHideButtonClicked();
        });

    connect(ui->passSIPlineEdit, &QLineEdit::editingFinished,
        [this] {
            confProps_ = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
            confProps_.password = ui->passSIPlineEdit->text().toStdString();
            LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps_);
        });

    connect(ui->ProxySIP, &QLineEdit::editingFinished,
        [this] {
            confProps_ = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
            confProps_.proxyServer = ui->ProxySIP->text().toStdString();
            LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps_);
        });

    connect(ui->passwdPushButton, &QPushButton::clicked,
        [this]() {
            passwordClicked();
        });

    connect(ui->currentAccountAvatar, &QPushButton::clicked,
        [this]() {
            avatarClicked();
        });

    connect(ui->advancedAccountSettingsPButton, &QPushButton::clicked, this, &SettingsWidget::toggleAdvancedSettings);

    connect(ui->currentRegisteredID, &QLineEdit::textEdited, this, &SettingsWidget::verifyRegisteredNameSlot);

    connect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::registeredNameFound,
        this, &SettingsWidget::receiveRegNameSlot);

    //connect "export account" button
    connect(ui->btnExportAccount, &QPushButton::clicked, this, &SettingsWidget::exportAccountSlot);

    // connect "delete account" button
    connect(ui->btnDeletAccount, &QPushButton::clicked, this, &SettingsWidget::delAccountSlot);

    // connect "banned contacts" button
    connect(ui->blockedContactsBtn, &QPushButton::clicked, this, &SettingsWidget::toggleBannedContacts);

    // connect "link device" button
    connect(ui->linkDevPushButton, &QPushButton::clicked, this, &SettingsWidget::showLinkDevSlot);

    // update banned accounts automatically
    connect(LRCInstance::getCurrentAccountInfo().contactModel.get(), &lrc::api::ContactModel::modelUpdated,
        this, &SettingsWidget::updateAndShowBannedContactsSlot);

    // update linked devices automatically
    QObject::connect(LRCInstance::getCurrentAccountInfo().deviceModel.get(), &lrc::api::NewDeviceModel::deviceUpdated,
        this, &SettingsWidget::updateAndShowDevicesSlot);

    // account settings setters {
    connect(ui->accountEnableCheckBox, &QCheckBox::clicked, this, &SettingsWidget::setAccEnableSlot);

    connect(ui->displayNameLineEdit, &QLineEdit::editingFinished,
        [this] {
            LRCInstance::setCurrAccDisplayName(ui->displayNameLineEdit->text().toStdString());
        });

    connect(ui->usernameSIP, &QLineEdit::editingFinished,
        [this] {
            confProps_ = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
            confProps_.username = ui->usernameSIP->text().toStdString();
            LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps_);
        });

    connect(ui->hostnameSIP, &QLineEdit::editingFinished,
        [this] {
            confProps_ = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
            confProps_.hostname = ui->hostnameSIP->text().toStdString();
            LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps_);
        });

    // general settings

    connect(ui->notificationCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::setNotificationsSlot);

    connect(ui->closeOrMinCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::setClosedOrMinSlot);

    connect(ui->downloadButton, &QAbstractButton::clicked, this, &SettingsWidget::openDownloadFolderSlot);

    connect(ui->alwaysRecordingCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::setAlwaysRecordingSlot);

    connect(ui->checkUpdateButton, &QAbstractButton::clicked, this, &SettingsWidget::checkForUpdateSlot);

    connect(ui->intervalUpdateCheckSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::setUpdateIntervalSlot);

    connect(ui->autoUpdateCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::setUpdateAutomaticSlot);

    // audio / visual settings

    connect(ui->recordPathButton, &QPushButton::clicked, this, &SettingsWidget::openRecordFolderSlot);
}

// *************************  General Settings  *************************

void SettingsWidget::populateGeneralSettings()
{
    QSettings settings;

    // settings
    ui->downloadButton->setText(QString::fromStdString(LRCInstance::dataTransferModel().downloadDirectory));
    ui->closeOrMinCheckBox->setChecked(settings.value(SettingsKey::closeOrMinimized).toBool());
    ui->notificationCheckBox->setChecked(settings.value(SettingsKey::enableNotifications).toBool());

    //recordings
    ui->alwaysRecordingCheckBox->setChecked(media::RecordingModel::instance().isAlwaysRecording());

    if (media::RecordingModel::instance().recordPath().isEmpty()) {
        QString recordPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        media::RecordingModel::instance().setRecordPath(recordPath);
    }

    ui->recordPathButton->setText(media::RecordingModel::instance().recordPath());

#ifdef Q_OS_WIN
    ui->autoUpdateCheckBox->setChecked(win_sparkle_get_automatic_check_for_updates());
    ui->intervalUpdateCheckSpinBox->setValue(win_sparkle_get_update_check_interval() / 86400);
#endif
}

void SettingsWidget::setNotificationsSlot(int state)
{
    QSettings settings;
    settings.setValue(SettingsKey::enableNotifications, state == Qt::CheckState::Checked);
}

void SettingsWidget::setClosedOrMinSlot(int state)
{
    QSettings settings;
    settings.setValue(SettingsKey::closeOrMinimized, state == Qt::CheckState::Checked);
}

void SettingsWidget::checkForUpdateSlot()
{
#ifdef Q_OS_WIN
    win_sparkle_check_update_with_ui();
#endif
}

void SettingsWidget::setUpdateIntervalSlot(int value)
{
#ifdef Q_OS_WIN
    win_sparkle_set_update_check_interval(value * 86400);
#endif
}

void SettingsWidget::setUpdateAutomaticSlot(int state)
{
#ifdef Q_OS_WIN
    win_sparkle_set_automatic_check_for_updates(state == Qt::CheckState::Checked);
    ui->intervalUpdateCheckSpinBox->setEnabled(state == Qt::CheckState::Checked);
#endif
}

void SettingsWidget::openDownloadFolderSlot()
{
    QSettings settings;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select A Folder For Your Downloads"),
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        ui->downloadButton->setText(dir);
        settings.setValue(SettingsKey::downloadPath, dir);
        LRCInstance::editableDataTransferModel()->downloadDirectory = dir.toStdString() + "/";
    }
}

void SettingsWidget::setAlwaysRecordingSlot(int state)
{
    media::RecordingModel::instance().setAlwaysRecording(state == Qt::CheckState::Checked);
}

void SettingsWidget::openRecordFolderSlot()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select A Folder For Your Recordings"),
        media::RecordingModel::instance().recordPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        media::RecordingModel::instance().setRecordPath(dir);
        ui->recordPathButton->setText(media::RecordingModel::instance().recordPath());
    }
}

// *************************  Audio/Visual Settings  *************************

void SettingsWidget::populateAVSettings()
{
    // audio
    auto inputModel = Audio::Settings::instance().inputDeviceModel();
    auto outputModel = Audio::Settings::instance().outputDeviceModel();

    disconnect(ui->outputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::outputDevIndexChangedSlot);
    disconnect(ui->inputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::inputdevIndexChangedSlot);
    connect(ui->outputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::outputDevIndexChangedSlot);
    connect(ui->inputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::inputdevIndexChangedSlot);

    ui->inputComboBox->setModel(inputModel);
    ui->outputComboBox->setModel(outputModel);

    auto inputIndex = inputModel->selectionModel()->currentIndex();
    auto outputIndex = outputModel->selectionModel()->currentIndex();

    ui->inputComboBox->setCurrentIndex(inputIndex.row());
    ui->outputComboBox->setCurrentIndex(outputIndex.row());

    // video
    disconnect(deviceModel_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(deviceModelIndexChanged(int)));
    disconnect(ui->deviceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::slotDeviceBoxCurrentIndexChanged);
    disconnect(ui->formatBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::slotFormatBoxCurrentIndexChanged);
    connect(deviceModel_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(deviceModelIndexChanged(int)));
    connect(ui->deviceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::slotDeviceBoxCurrentIndexChanged);
    connect(ui->formatBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::slotFormatBoxCurrentIndexChanged);

    ui->deviceBox->setModel(deviceModel_);
    currentDeviceName_ = deviceModel_->activeDevice()->name();
    setFormatListForDevice(deviceModel_->activeDevice());

    showPreview();
}

void SettingsWidget::outputDevIndexChangedSlot(int index)
{
    auto outputModel = Audio::Settings::instance().outputDeviceModel();
    outputModel->selectionModel()->setCurrentIndex(outputModel->index(index), QItemSelectionModel::ClearAndSelect);
}

void SettingsWidget::inputdevIndexChangedSlot(int index)
{
    auto inputModel = Audio::Settings::instance().inputDeviceModel();
    inputModel->selectionModel()->setCurrentIndex(inputModel->index(index), QItemSelectionModel::ClearAndSelect);
}

void SettingsWidget::deviceModelIndexChanged(int index)
{
    if (index < 0) {
        currentDeviceName_ = "";
        toggleVideoSettings(false);
        toggleVideoPreview(false);
        return;
    }

    toggleVideoSettings(true);
    toggleVideoPreview(true);
    ui->deviceBox->setCurrentIndex(index);
    setFormatListForDevice(deviceModel_->activeDevice());
    currentDeviceName_ = deviceModel_->activeDevice()->name();
}

void SettingsWidget::slotDeviceBoxCurrentIndexChanged(int index)
{
    if (index < 0) {
        return;
    }

    auto deviceList = deviceModel_->devices();

    if (!deviceList.isEmpty() && currentDeviceName_ != deviceList[index]->name()) {
        deviceModel_->setActive(index);
    }

    currentDeviceName_ = deviceModel_->activeDevice()->name();
}

void SettingsWidget::slotFormatBoxCurrentIndexChanged(int index)
{
    if (index < 0) {
        return;
    }

    if (auto activeChannel = deviceModel_->activeDevice()->activeChannel()) {
        auto resolutionIndex = formatIndexList_.at(index).first;
        auto rateIndex = formatIndexList_.at(index).second;
        activeChannel->setActiveMode(resolutionIndex, rateIndex);
    }
}

void SettingsWidget::startVideo()
{
    Video::PreviewManager::instance().stopPreview();
    Video::PreviewManager::instance().startPreview();
}

void SettingsWidget::stopVideo()
{
    Video::PreviewManager::instance().stopPreview();
}

void SettingsWidget::toggleVideoSettings(bool enabled)
{
    ui->formatBox->clear();
    ui->deviceBox->clear();
    ui->formatBox->setEnabled(enabled);
    ui->deviceBox->setEnabled(enabled);
}

void SettingsWidget::toggleVideoPreview(bool enabled)
{
    ui->previewUnavailableLabel->setVisible(!enabled);
    ui->videoLayoutWidget->setVisible(enabled);
}

void SettingsWidget::showPreview()
{
    if (!CallModel::instance().getActiveCalls().size()) {
        ui->previewUnavailableLabel->hide();
        ui->videoLayoutWidget->show();
        startVideo();
        ui->videoWidget->setIsFullPreview(true);

    } else {
        ui->previewUnavailableLabel->show();
        ui->videoLayoutWidget->hide();
    }
}

void SettingsWidget::setFormatListForDevice(Video::Device* device)
{
    auto activeChannel = device->activeChannel();

    if (!activeChannel) {
        return;
    }

    auto activeResolution = activeChannel->activeResolution();
    auto activeRate = activeResolution->activeRate();
    ui->formatBox->blockSignals(true);
    ui->formatBox->clear();
    formatIndexList_.clear();

    for (auto resolution : activeChannel->validResolutions()) {
        auto resolutionIndex = activeChannel->validResolutions().indexOf(resolution);

        for (auto rate : resolution->validRates()) {
            auto ratenIndex = resolution->validRates().indexOf(rate);
            formatIndexList_.append(QPair<int, int>(resolutionIndex, ratenIndex));
            auto fr = rate->name().toDouble();
            auto sizeRateString = QString("%1 [%2 fps]").arg(resolution->name()).arg(round(fr));
            ui->formatBox->addItem(sizeRateString);

            if (resolution == activeResolution && rate == activeRate) {
                ui->formatBox->setCurrentIndex(ui->formatBox->count() - 1);
            }

            ui->formatBox->count();
        }
    }

    ui->formatBox->blockSignals(false);
}
