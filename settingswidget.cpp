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
#include <QtConcurrent/QtConcurrent>

#include "deleteaccountdialog.h"
#include "nameregistrationdialog.h"
#include "passworddialog.h"
#include "settingskey.h"
#include "utils.h"
#include "deviceitemwidget.h"
#include "banneditemwidget.h"
#include "version.h"

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

SettingsWidget::SettingsWidget(QWidget* parent)
    : NavWidget(parent)
    , ui(new Ui::SettingsWidget)
    , scrollArea_(new QScrollArea(this))
    , scrollSIPArea_(new QScrollArea(this))
    , deviceModel_(&Video::DeviceModel::instance())
{
    ui->setupUi(this);

    ui->accountSettingsButton->setPixmap(QPixmap(":/images/icons/baseline-people-24px.svg"));
    ui->generalSettingsButton->setPixmap(QPixmap(":/images/icons/round-settings-24px.svg"));
    ui->mediaSettingsButton->setPixmap(QPixmap(":/images/icons/baseline-desktop_windows-24px.svg"));

    ui->accountSettingsButton->setChecked(true);

    ui->btnExitSettings->setIconSize(QSize(24, 24));
    ui->btnExitSettings->setIcon(QIcon(":/images/icons/round-close-24px.svg"));

    // avatar
    avatarSize_ = 224;

    ///////////////////////////////////////////////////////////////////////////
    // SIP
    ///////////////////////////////////////////////////////////////////////////

    // Show Password
    ui->passSIPlineEdit->setEchoMode(QLineEdit::Password);

    // display name SIP
    ui->displaySIPNameLineEdit->setAlignment(Qt::AlignHCenter);
    ui->displaySIPNameLineEdit->setPlaceholderText(tr("Enter an alias"));

    // SIP User Name
    ui->usernameSIP->setStyleSheet("border : 0px;");

    // SIP Advanced Setting initilization
    ui->advancedAccountSettingsSIPButton->setIcon(QIcon(":/images/icons/round-arrow_drop_down-24px.svg"));

    ui->scrollSIPArea->verticalScrollBar()->setEnabled(true);
    ui->scrollSIPArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 10px; }");
    ui->advancedSIPSettingsWidget->setVisible(false);

    ///////////////////////////////////////////////////////////////////////////
    // Jami
    ///////////////////////////////////////////////////////////////////////////

    // display name (aka alias)
    ui->displayNameLineEdit->setAlignment(Qt::AlignHCenter);
    ui->displayNameLineEdit->setPlaceholderText(tr("Enter an alias"));

    ui->currentRegisteredID->setReadOnly(true);
    ui->currentRegisteredID->setStyleSheet("border : 0px;");

    ui->currentRingID->setReadOnly(true);

    ui->advancedAccountSettingsPButton->setIcon(QIcon(":/images/icons/round-arrow_drop_down-24px.svg"));
    ui->advancedAccountSettingsPButton->setIconSize(QSize(32, 32));
    ui->linkDevPushButton->setIcon(QIcon(":/images/icons/round-add-24px.svg"));
    ui->bannedContactsBtn->setIcon(QIcon(":/images/icons/round-arrow_drop_down-24px.svg"));

    ui->scrollArea->verticalScrollBar()->setEnabled(true);
    ui->scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 10px; }");

    ui->advancedSettingsWidget->setVisible(false);

    statusSuccessPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-done-24px.svg", RingTheme::presenceGreen_);
    statusInvalidPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-error_outline-24px.svg", RingTheme::urgentOrange_);
    statusErrorPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-close-24px.svg", RingTheme::red_);

    lookupSpinnerMovie_ = new QMovie(":/images/jami_rolling_spinner.gif");
    lookupSpinnerMovie_->setScaledSize(QSize(30, 30));

    ui->lookupStatusLabel->setMovie(lookupSpinnerMovie_);
    ui->lookupStatusLabel->hide();

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
        // hide banned list
        ui->bannedContactsListWidget->setVisible(false);
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

    QtConcurrent::run(
        [this] {
            ui->currentAccountAvatar->stopBooth();
            Video::PreviewManager::instance().stopPreview();
        });

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
        QtConcurrent::run(
            [this] {
                Video::PreviewManager::instance().stopPreview();
            });

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
            ui->mediaSettingsButton->setChecked(false);
            break;
        }

    case Button::generalSettingsButton:
        QtConcurrent::run(
            [this] {
                Video::PreviewManager::instance().stopPreview();
            });

        ui->stackedWidget->setCurrentWidget(ui->generalSettings);
        populateGeneralSettings();

        if (pastButton_ == Button::mediaSettingsButton) {
            ui->generalSettingsButton->setChecked(true);
            ui->mediaSettingsButton->setChecked(false);
            break;

        } else {
            ui->generalSettingsButton->setChecked(true);
            ui->accountSettingsButton->setChecked(false);
            break;
        }

    case Button::mediaSettingsButton:
        ui->stackedWidget->setCurrentWidget(ui->avSettings);
        populateAVSettings();

        if (pastButton_ == Button::accountSettingsButton) {
            ui->mediaSettingsButton->setChecked(true);
            ui->accountSettingsButton->setChecked(false);
            break;

        } else {
            ui->mediaSettingsButton->setChecked(true);
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
        ui->btnRegisterName->show();
        ui->btnRegisterName->setVisible(false);
        ui->currentRegisteredID->setStyleSheet("font-weight: normal; background-color: rgb(240, 240, 240);border: 2px;");
        ui->currentRegisteredID->setClearButtonEnabled(false);
        ui->currentRegisteredID->setReadOnly(false);
        setRegNameUi(RegName::BLANK);
    } else {
        ui->btnRegisterName->hide();
        ui->currentRegisteredID->setStyleSheet("font-weight: bold; background-color: transparent;border: 0px;");
        ui->currentRegisteredID->setClearButtonEnabled(false);
        ui->currentRegisteredID->setReadOnly(true);
    }

    // sip avatar set
    setAvatar(ui->currentSIPAccountAvatar);

    // jami avatar set
    setAvatar(ui->currentAccountAvatar);

    ui->accountEnableCheckBox->setChecked(accInfo.enabled);
    ui->accountSIPEnableCheckBox->setChecked(accInfo.enabled);

    ui->displayNameLineEdit->setText(QString::fromStdString(accInfo.profileInfo.alias));
    ui->displaySIPNameLineEdit->setText(QString::fromStdString(accInfo.profileInfo.alias));

    updateAndShowDevicesSlot();

    ui->bannedContactsListWidget->setVisible(false);
    ui->bannedContactsBtn->setIcon(QIcon(":/images/icons/round-arrow_drop_down-24px.svg"));
    ui->bannedContactsLayoutWidget->setVisible(accInfo.contactModel->getBannedContacts().size());
}

void SettingsWidget::setAvatar(PhotoboothWidget* avatarWidget)
{
    auto& accountInfo = LRCInstance::getCurrentAccountInfo();
    auto defaultAvatar = accountInfo.profileInfo.avatar.empty();
    auto avatar = Utils::accountPhoto(accountInfo, {avatarSize_, avatarSize_});
    avatarWidget->setAvatarPixmap(QPixmap::fromImage(avatar), defaultAvatar);
}

void SettingsWidget::passwordClicked()
{
    PasswordDialog passwdDialog(this);
    passwdDialog.exec();
}

void SettingsWidget::toggleAdvancedSIPSettings()
{

    if (advancedSIPSettingsDropped_) {
        ui->advancedAccountSettingsSIPButton->setIcon(QIcon(":/images/icons/round-arrow_drop_down-24px.svg"));
        ui->scrollBarSIPLabel->show();
        ui->advancedSIPSettingsWidget->setVisible(false);

    } else {
        ui->advancedAccountSettingsSIPButton->setIcon(QIcon(":/images/icons/round-arrow_drop_up-24px.svg"));
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
        ui->advancedAccountSettingsPButton->setIcon(QIcon(":/images/icons/round-arrow_drop_down-24px.svg"));
        ui->scrollBarLabel->show();
        ui->advancedSettingsWidget->setVisible(false);
    } else {
        ui->advancedAccountSettingsPButton->setIcon(QIcon(":/images/icons/round-arrow_drop_up-24px.svg"));
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
    auto bannedContactsVisible = ui->bannedContactsListWidget->isVisible();
    ui->bannedContactsListWidget->setVisible(!bannedContactsVisible);
    updateAndShowBannedContactsSlot();
}

void SettingsWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    scrollSIPArea_->resize(ui->currentSIPAccountSettingsScrollWidget->width(), this->height());
    scrollArea_->resize(ui->currentAccountSettingsScrollWidget->width(), this->height());
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
    disconnect(lookupSpinnerMovie_, SIGNAL(frameChanged(int)), this, SLOT(setButtonIconSlot(int)));
    disconnect(ui->btnRegisterName, &QPushButton::clicked, this, &SettingsWidget::slotRegisterName);

    switch (stat) {
    case RegName::BLANK:
        ui->btnRegisterName->setVisible(false);
        ui->lookupStatusLabel->hide();
        break;

    case RegName::INVALIDFORM:
        ui->btnRegisterName->setVisible(false);
        ui->lookupStatusLabel->setPixmap(statusInvalidPixmap_);
        ui->lookupStatusLabel->show();
        ui->lookupStatusLabel->setToolTip(tr("A registered name should not have any spaces and must be at least three letters long"));
        break;

    case RegName::TAKEN:
        ui->btnRegisterName->setVisible(false);
        ui->lookupStatusLabel->setPixmap(statusErrorPixmap_);
        ui->lookupStatusLabel->show();
        ui->lookupStatusLabel->setToolTip(tr("This name is already taken"));
        break;

    case RegName::FREE:
        ui->btnRegisterName->setVisible(true);
        ui->lookupStatusLabel->setPixmap(statusSuccessPixmap_);
        ui->lookupStatusLabel->show();
        ui->lookupStatusLabel->setToolTip(tr("Register this name"));

        connect(ui->btnRegisterName, &QPushButton::clicked, this, &SettingsWidget::slotRegisterName);
        break;

    case RegName::SEARCHING:
        ui->btnRegisterName->setVisible(false);
        ui->lookupStatusLabel->setMovie(lookupSpinnerMovie_);
        lookupSpinnerMovie_->stop();
        lookupSpinnerMovie_->start();
        ui->lookupStatusLabel->show();
        ui->lookupStatusLabel->setToolTip(tr(""));
        break;
    }
}

void
SettingsWidget::slotRegisterName()
{
    NameRegistrationDialog nameRegistrationDialog(registeredName_, this);

    if (nameRegistrationDialog.exec() == QDialog::Accepted) {
        lrc::api::account::ConfProperties_t accountProperties = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), accountProperties);
        ui->btnRegisterName->hide();
        ui->currentRegisteredID->setStyleSheet("font-weight: bold; background-color: transparent;border: 0px;");
        ui->currentRegisteredID->setClearButtonEnabled(false);
        ui->currentRegisteredID->setReadOnly(true);
        ui->lookupStatusLabel->hide();
    } else {
        ui->currentRegisteredID->setText("");
        registeredName_ = "";
        setRegNameUi(RegName::BLANK);
    }
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
    QString password;

    bool ok = false;

    if (LRCInstance::getCurrAccConfig().archiveHasPassword) {
        password = QInputDialog::getText(this, tr("Remove Device"),
            tr("Enter this account's password to confirm the removal of this device"),
            QLineEdit::Password,
            QDir::home().dirName(), &ok);

    } else {
        password = "";
        QMessageBox confirmDialog;
        confirmDialog.setText(tr("Are you sure you wish to remove this device?"));
        confirmDialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        confirmDialog.setDefaultButton(QMessageBox::Cancel);

        if (confirmDialog.exec() == QMessageBox::Ok) {
            goto delete_;
        }
    }

    if (ok) {
    delete_:
        LRCInstance::getCurrentAccountInfo().deviceModel->revokeDevice(it->id, password.toStdString());
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

    ui->linkDevPushButton->show();

    auto deviceList = LRCInstance::getCurrentAccountInfo().deviceModel->getAllDevices();

    int i = 0;
    for (auto it = deviceList.begin(); it != deviceList.end(); ++it, ++i) {
        auto item = new QListWidgetItem(ui->settingsListWidget);
        ui->settingsListWidget->addItem(item);
        auto row = new DeviceItemWidget(QString::fromStdString(it->name),
                                        QString::fromStdString(it->id),
                                        it->isCurrent,
                                        this);
        item->setSizeHint(QSize(ui->settingsListWidget->width(),
                                row->minimumSizeHint().height()));
        ui->settingsListWidget->setItemWidget(item, row);
        connect(row, &DeviceItemWidget::btnRemoveDeviceClicked,
            [this, i, isCurrent = it->isCurrent]() {
                removeDeviceSlot(i);
            });
    }
}

void SettingsWidget::updateAndShowBannedContactsSlot()
{
    if (!ui->bannedContactsListWidget->isVisible()) {
        ui->bannedContactsBtn->setIcon(QIcon(":/images/icons/round-arrow_drop_down-24px.svg"));
        return;
    }

    ui->bannedContactsBtn->setIcon(QIcon(":/images/icons/round-arrow_drop_up-24px.svg"));

    auto bannedContactList = LRCInstance::getCurrentAccountInfo().contactModel->getBannedContacts();

    if (!bannedContactList.size()) {
        toggleBannedContacts();
        ui->bannedContactsLayoutWidget->setVisible(false);
        return;
    }

    ui->bannedListWidget->clear();

    int i = 0;
    for (auto it = bannedContactList.begin(); it != bannedContactList.end(); ++it, ++i) {
        auto contactInfo = LRCInstance::getCurrentAccountInfo().contactModel->getContact(*it);
        auto contactName = QString::fromStdString(contactInfo.registeredName);
        auto contactId = QString::fromStdString(contactInfo.profileInfo.uri);
        auto item = new QListWidgetItem(ui->bannedListWidget);
        ui->bannedListWidget->addItem(item);
        auto row = new BannedItemWidget(contactName,
                                        contactId,
                                        this);
        item->setSizeHint(QSize(ui->bannedListWidget->width(),
                                row->minimumSizeHint().height()));
        ui->bannedListWidget->setItemWidget(item, row);
        connect(row, &BannedItemWidget::btnReAddContactClicked,
            [this, i]() {
                unban(i);
            });
    }
}

void SettingsWidget::showLinkDevSlot()
{
    if (!advancedSettingsWidget_) {
        delete advancedSettingsWidget_;
    }

    auto linkDeviceDialog = new LinkDeviceDialog(this);
    linkDeviceDialog->exec();
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

    connect(ui->mediaSettingsButton, &QPushButton::clicked, [this]() {
        setSelected(Button::mediaSettingsButton);
    });

    /*connect(ui->currentSIPAccountAvatar, &QPushButton::clicked, [this]() {
        avatarClicked();
    });*/

    connect(ui->advancedAccountSettingsSIPButton, &QPushButton::clicked, this, &SettingsWidget::toggleAdvancedSIPSettings);

    // connect "delete SIP account" button
    connect(ui->btnSIPDeletAccount, &QPushButton::clicked, this, &SettingsWidget::delAccountSlot);

    connect(ui->accountSIPEnableCheckBox, &QCheckBox::clicked, this, &SettingsWidget::setAccEnableSlot);

    connect(ui->displaySIPNameLineEdit, &QLineEdit::editingFinished,
        [this] {
            LRCInstance::setCurrAccDisplayName(ui->displaySIPNameLineEdit->text().toStdString());
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

    connect(ui->currentAccountAvatar, &PhotoboothWidget::clearedPhoto,
        [this] {
            LRCInstance::setCurrAccAvatar(QPixmap());
            setAvatar(ui->currentAccountAvatar);
        });

    connect(ui->currentAccountAvatar, &PhotoboothWidget::photoTaken,
        [this] {
            LRCInstance::setCurrAccAvatar(ui->currentAccountAvatar->getAvatarPixmap());
        });

    connect(ui->currentSIPAccountAvatar, &PhotoboothWidget::clearedPhoto,
        [this] {
            LRCInstance::setCurrAccAvatar(QPixmap());
            setAvatar(ui->currentSIPAccountAvatar);
        });

    connect(ui->currentSIPAccountAvatar, &PhotoboothWidget::photoTaken,
        [this] {
            LRCInstance::setCurrAccAvatar(ui->currentSIPAccountAvatar->getAvatarPixmap());
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
    connect(ui->bannedContactsBtn, &QPushButton::clicked, this, &SettingsWidget::toggleBannedContacts);

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

    connect(ui->notificationCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::slotSetNotifications);

    connect(ui->closeOrMinCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::slotSetClosedOrMin);

    connect(ui->downloadButton, &QAbstractButton::clicked, this, &SettingsWidget::openDownloadFolderSlot);

    connect(ui->alwaysRecordingCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::slotSetAlwaysRecording);

    connect(ui->checkUpdateButton, &QAbstractButton::clicked, this, &SettingsWidget::checkForUpdateSlot);

    connect(ui->autoUpdateCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::slotSetUpdateAutomatic);

    // audio / visual settings

    connect(ui->recordPathButton, &QPushButton::clicked, this, &SettingsWidget::openRecordFolderSlot);
}

// *************************  General Settings  *************************

void SettingsWidget::populateGeneralSettings()
{
    QSettings settings("jami.net", "Jami");

    // settings
    QString downloadPath = QDir::toNativeSeparators(QString::fromStdString(LRCInstance::dataTransferModel().downloadDirectory));
    int pos = downloadPath.lastIndexOf(QChar('\\'));
    if (pos == downloadPath.length() - 1) {
        downloadPath.truncate(pos);
    }
    Utils::setElidedText(ui->downloadButton, downloadPath);

    ui->closeOrMinCheckBox->setChecked(settings.value(SettingsKey::closeOrMinimized).toBool());
    auto notifs = settings.value(SettingsKey::enableNotifications).toBool();
    ui->notificationCheckBox->setChecked(notifs);

    //recordings
    ui->alwaysRecordingCheckBox->setChecked(media::RecordingModel::instance().isAlwaysRecording());

    if (media::RecordingModel::instance().recordPath().isEmpty()) {
        QString recordPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        media::RecordingModel::instance().setRecordPath(recordPath);
    }

    Utils::setElidedText(ui->recordPathButton, media::RecordingModel::instance().recordPath());

#ifdef Q_OS_WIN
    ui->autoUpdateCheckBox->setChecked(settings.value(SettingsKey::autoUpdate).toBool());
#endif
}

void SettingsWidget::slotSetNotifications(bool state)
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::enableNotifications, state);
}

void SettingsWidget::slotSetClosedOrMin(bool state)
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::closeOrMinimized, state);
}

void SettingsWidget::checkForUpdateSlot()
{
#ifdef Q_OS_WIN
    Utils::checkForUpdates(true, this);
#endif
}

void SettingsWidget::slotSetUpdateAutomatic(bool state)
{
#ifdef Q_OS_WIN
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::autoUpdate, state);
#endif
}

void SettingsWidget::openDownloadFolderSlot()
{
    QSettings settings("jami.net", "Jami");
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select A Folder For Your Downloads"),
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        Utils::setElidedText(ui->downloadButton, dir);
        settings.setValue(SettingsKey::downloadPath, dir);
        LRCInstance::editableDataTransferModel()->downloadDirectory = dir.toStdString() + "/";
    }
}

void SettingsWidget::slotSetAlwaysRecording(bool state)
{
    media::RecordingModel::instance().setAlwaysRecording(state);
}

void SettingsWidget::openRecordFolderSlot()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select A Folder For Your Recordings"),
        media::RecordingModel::instance().recordPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        media::RecordingModel::instance().setRecordPath(dir);
        Utils::setElidedText(ui->recordPathButton, media::RecordingModel::instance().recordPath());
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
