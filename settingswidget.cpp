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

#include "deleteaccountdialog.h"
#include "nameregistrationdialog.h"
#include "passworddialog.h"
#include "settingskey.h"
#include "utils.h"
#include "deviceitemwidget.h"
#include "banneditemwidget.h"
#include "version.h"

#include "namedirectory.h"
#include "api/newdevicemodel.h"

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

SettingsWidget::SettingsWidget(QWidget* parent)
    : NavWidget(parent)
    , ui(new Ui::SettingsWidget)
    , scrollArea_(new QScrollArea(this))
    , scrollSIPArea_(new QScrollArea(this))
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

    // btnExitSettings
    connect(ui->btnExitSettings, &QPushButton::clicked, this, &SettingsWidget::leaveSettingsSlot);

    connect(ui->accountSettingsButton, &QPushButton::clicked, [this]() { setSelected(Button::accountSettingsButton); });

    connect(ui->generalSettingsButton, &QPushButton::clicked, [this]() { setSelected(Button::generalSettingsButton); });

    connect(ui->mediaSettingsButton, &QPushButton::clicked, [this]() { setSelected(Button::mediaSettingsButton); });

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
            setAvatar(ui->currentAccountAvatar, true);
        });

    connect(ui->currentAccountAvatar, &PhotoboothWidget::photoTaken,
        [this] {
            LRCInstance::setCurrAccAvatar(ui->currentAccountAvatar->getAvatarPixmap());
        });

    connect(ui->currentSIPAccountAvatar, &PhotoboothWidget::clearedPhoto,
        [this] {
            LRCInstance::setCurrAccAvatar(QPixmap());
            setAvatar(ui->currentSIPAccountAvatar, true);
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

    connect(ui->checkUpdateButton, &QAbstractButton::clicked, this, &SettingsWidget::checkForUpdateSlot);

    connect(ui->autoUpdateCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::slotSetUpdateAutomatic);

    // audio / visual settings

    connect(ui->alwaysRecordingCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::slotAlwaysRecordingClicked);

    connect(ui->recordPreviewCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::slotRecordPreviewClicked);

    connect(ui->recordPathButton, &QAbstractButton::clicked, this, &SettingsWidget::openRecordFolderSlot);

    connect(ui->recordQualitySlider, &QAbstractSlider::valueChanged, this, &SettingsWidget::slotRecordQualitySliderValueChanged);

    connect(ui->recordQualitySlider, &QAbstractSlider::sliderReleased, this, &SettingsWidget::slotRecordQualitySliderSliderReleased);

    connect(ui->hardwareAccelCheckBox, &QAbstractButton::clicked, this, &SettingsWidget::slotSetHardwareAccel);

    connect(&LRCInstance::avModel(), &lrc::api::AVModel::deviceEvent, [this] { videoDeviceEventHandlerAndMediaSettingSetUp(); });

    ui->containerWidget->setVisible(false);

    previewRenderer_ = PreviewRenderWidget::attachPreview();
}

void SettingsWidget::slotAccountListChanged()
{
    if (!LRCInstance::accountModel().getAccountList().size()) {
        setSelected(Button::accountSettingsButton);
    } else {
        disconnectAccountConnections();
    }
    auto device = LRCInstance::avModel().getDefaultDeviceName();
    if (LRCInstance::avModel().getCurrentVideoCaptureDevice().empty()) {
        LRCInstance::avModel().setCurrentVideoCaptureDevice(device);
    }
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

    stopAudioMeter();

    bool isSIP = LRCInstance::getCurrentAccountInfo().profileInfo.type == lrc::api::profile::Type::SIP;
    auto photoBooth = isSIP ? ui->currentSIPAccountAvatar : ui->currentAccountAvatar;
    QtConcurrent::run([this, &photoBooth] {
        photoBooth->stopBooth();
        if (isPreviewed_)
            stopPreviewing();
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

        connectCurrentAccount();

        ui->accountSettingsButton->setChecked(true);
        ui->generalSettingsButton->setChecked(false);
        ui->mediaSettingsButton->setChecked(false);
        if (pastButton_ == sel && pastAccount_ == LRCInstance::getCurrentAccountInfo().profileInfo.type) {
            return;
        }

        stopAudioMeter();
        stopPreviewing();

        if (LRCInstance::getCurrentAccountInfo().profileInfo.type == lrc::api::profile::Type::SIP) {
            ui->stackedWidget->setCurrentWidget(ui->currentSIPAccountSettingsScrollWidget);
            if (advancedSIPSettingsDropped_) {
                toggleAdvancedSIPSettings();
            }
            pastAccount_ = lrc::api::profile::Type::SIP;
        } else {
            ui->stackedWidget->setCurrentWidget(ui->currentAccountSettingsScrollWidget);
            if (advancedSettingsDropped_) {
                toggleAdvancedSettings();
            }
            pastAccount_ = lrc::api::profile::Type::RING;
        }

        break;

    case Button::generalSettingsButton:

        ui->generalSettingsButton->setChecked(true);
        ui->accountSettingsButton->setChecked(false);
        ui->mediaSettingsButton->setChecked(false);
        if (pastButton_ == sel) { return; }

        stopAudioMeter();
        stopPreviewing();

        ui->stackedWidget->setCurrentWidget(ui->generalSettings);
        populateGeneralSettings();
        // close photo booth carmera if necessary
        resetPhotoBoothStateWhenSettingChanged(Button::generalSettingsButton);

        break;

    case Button::mediaSettingsButton:
        ui->mediaSettingsButton->setChecked(true);
        ui->generalSettingsButton->setChecked(false);
        ui->accountSettingsButton->setChecked(false);
        if (pastButton_ == sel) { return; }

        ui->stackedWidget->setCurrentWidget(ui->avSettings);

        startPreviewing();
        populateAVSettings();
        startAudioMeter();
        // close photo booth carmera if necessary
        resetPhotoBoothStateWhenSettingChanged(Button::mediaSettingsButton);

        break;
    }

    pastButton_ = sel;
}
// called to update current settings information when navigating to settingsWidget
void SettingsWidget::updateAccountInfoDisplayed()
{
    auto& accInfo = LRCInstance::getCurrentAccountInfo();
    auto accConfig = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());

    bool showLocalAccountConfig = accConfig.managerUri.empty();
    ui->passwdPushButton->setVisible(showLocalAccountConfig);
    ui->btnExportAccount->setVisible(showLocalAccountConfig);
    ui->linkDevPushButton->setVisible(showLocalAccountConfig);

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
    setAvatar(ui->currentSIPAccountAvatar, false);

    // jami avatar set
    setAvatar(ui->currentAccountAvatar, false);

    ui->accountEnableCheckBox->setChecked(accInfo.enabled);
    ui->accountSIPEnableCheckBox->setChecked(accInfo.enabled);

    ui->displayNameLineEdit->setText(QString::fromStdString(accInfo.profileInfo.alias));
    ui->displaySIPNameLineEdit->setText(QString::fromStdString(accInfo.profileInfo.alias));

    updateAndShowDevicesSlot();

    ui->bannedContactsListWidget->setVisible(false);
    ui->bannedContactsBtn->setIcon(QIcon(":/images/icons/round-arrow_drop_down-24px.svg"));
    ui->bannedContactsLayoutWidget->setVisible(accInfo.contactModel->getBannedContacts().size());
}

void SettingsWidget::setAvatar(PhotoboothWidget* avatarWidget, bool stopPhotoboothPreview)
{
    auto& accountInfo = LRCInstance::getCurrentAccountInfo();
    auto defaultAvatar = accountInfo.profileInfo.avatar.empty();
    auto avatar = Utils::accountPhoto(accountInfo, {avatarSize_, avatarSize_});
    avatarWidget->setAvatarPixmap(QPixmap::fromImage(avatar), defaultAvatar, stopPhotoboothPreview);
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
    NameDirectory::instance().lookupName(QString(), registeredName_);
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
    LRCInstance::accountModel().setAccountEnabled(LRCInstance::getCurrAccId(), static_cast<bool>(state));
}

void SettingsWidget::delAccountSlot()
{
    DeleteAccountDialog delDialog(this);
    auto ret = delDialog.exec();

    if (ret == QDialog::Accepted) {
        LRCInstance::setSelectedAccountId();
        LRCInstance::setSelectedConvId();
        if (LRCInstance::accountModel().getAccountList().size()) {
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

    if (LRCInstance::getCurrAccConfig().managerUri.empty()) {
        ui->linkDevPushButton->show();
    }

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

void SettingsWidget::connectCurrentAccount()
{
    disconnectAccountConnections();

    auto& currentAccountInfo = LRCInstance::getCurrentAccountInfo();
    // update banned accounts automatically
    accountConnections_.append(
        QObject::connect(currentAccountInfo.contactModel.get(), &lrc::api::ContactModel::modelUpdated,
                         this, &SettingsWidget::updateAndShowBannedContactsSlot, Qt::UniqueConnection));

    // update linked devices automatically
    accountConnections_.append(
        QObject::connect(currentAccountInfo.deviceModel.get(), &lrc::api::NewDeviceModel::deviceUpdated,
                         this, &SettingsWidget::updateAndShowDevicesSlot, Qt::UniqueConnection));

    accountConnections_.append(
        QObject::connect(currentAccountInfo.deviceModel.get(), &lrc::api::NewDeviceModel::deviceRevoked,
                         this, &SettingsWidget::updateAndShowDevicesSlot, Qt::UniqueConnection));

    accountConnections_.append(
        QObject::connect(currentAccountInfo.deviceModel.get(), &lrc::api::NewDeviceModel::deviceAdded,
                         this, &SettingsWidget::updateAndShowDevicesSlot, Qt::UniqueConnection));
}

void SettingsWidget::disconnectAccountConnections()
{
    for (const auto& c : accountConnections_) { disconnect(c); }
}

// *************************  General Settings  *************************

QString getRecordQualityString(int value)
{
    return value ? QString::number(static_cast<float>(value) / 100, 'f', 1) + " Mbps" : "Default";
}

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

    // recording
    auto alwaysRecord = LRCInstance::avModel().getAlwaysRecord();
    ui->alwaysRecordingCheckBox->setChecked(alwaysRecord);

    auto recordPreview = LRCInstance::avModel().getRecordPreview();
    ui->recordPreviewCheckBox->setChecked(recordPreview);

    auto recordQuality = LRCInstance::avModel().getRecordQuality() / 100;
    auto recordQualityStr = getRecordQualityString(recordQuality);
    ui->recordQualityValueLabel->setText(recordQualityStr);
    ui->recordQualitySlider->setValue(recordQuality);

    if (LRCInstance::avModel().getRecordPath().empty()) {
        QString recordPath = QDir::toNativeSeparators(
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        LRCInstance::avModel().setRecordPath(recordPath.toStdString());
    }

    Utils::setElidedText(ui->recordPathButton,
        QString::fromStdString(LRCInstance::avModel().getRecordPath()));

    // updates
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
        LRCInstance::dataTransferModel().downloadDirectory = dir.toStdString() + "/";
    }
}

void SettingsWidget::openRecordFolderSlot()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Select A Folder For Your Recordings"),
        QString::fromStdString(LRCInstance::avModel().getRecordPath()),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!dir.isEmpty()) {
        LRCInstance::avModel().setRecordPath(dir.toStdString());
        Utils::setElidedText(ui->recordPathButton,
            QString::fromStdString(LRCInstance::avModel().getRecordPath())
        );
    }
}

void SettingsWidget::slotAlwaysRecordingClicked(bool state)
{
    LRCInstance::avModel().setAlwaysRecord(state);
}

void SettingsWidget::slotRecordPreviewClicked(bool state)
{
    LRCInstance::avModel().setRecordPreview(state);
}

void SettingsWidget::slotRecordQualitySliderValueChanged(int value)
{
    ui->recordQualityValueLabel->setText(getRecordQualityString(value));
}

void SettingsWidget::slotRecordQualitySliderSliderReleased()
{
    auto value = ui->recordQualitySlider->value();
    // 0 - 500 -> 0 - 50000
    LRCInstance::avModel().setRecordQuality(value * 100);
}

// *************************  Audio/Visual Settings  ************recordQuality*************

void SettingsWidget::populateAVSettings()
{
    // audio input devices
    disconnect(ui->inputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::inputdevIndexChangedSlot);
    ui->inputComboBox->clear();
    auto inputDevices = LRCInstance::avModel().getAudioInputDevices();
    auto inputDevice = LRCInstance::avModel().getInputDevice();
    auto inputIndex = Utils::indexInVector(inputDevices, inputDevice);
    for (auto id : inputDevices) {
        ui->inputComboBox->addItem(QString::fromStdString(id).toUtf8());
    }
    ui->inputComboBox->setCurrentIndex(inputIndex != -1 ? inputIndex : 0);
    connect(ui->inputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::inputdevIndexChangedSlot);

    connect(&LRCInstance::avModel(), &lrc::api::AVModel::audioMeter,
        [this](const std::string& id, float level) {
            if (id == "audiolayer_id") {
                ui->audioInputMeter->setLevel(level);
            }
        });

    // audio output devices
    disconnect(ui->outputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::outputDevIndexChangedSlot);
    ui->outputComboBox->clear();
    auto outputDevices = LRCInstance::avModel().getAudioOutputDevices();
    auto outputDevice = LRCInstance::avModel().getOutputDevice();
    auto outputIndex = Utils::indexInVector(outputDevices, outputDevice);
    for (auto od : outputDevices) {
        ui->outputComboBox->addItem(QString::fromStdString(od).toUtf8());
    }
    ui->outputComboBox->setCurrentIndex(outputIndex != -1 ? outputIndex : 0);
    connect(ui->outputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::outputDevIndexChangedSlot);

    previewRenderer_->setParent(ui->previewWidgetContainer);
    previewRenderer_->setGeometry(ui->previewWidgetContainer->rect());
    previewRenderer_->setCurrentConainerGeo(ui->previewWidgetContainer->width(), ui->previewWidgetContainer->height());
    previewRenderer_->resetBoarder();
    previewRenderer_->setPhotoMode(false);
    previewRenderer_->setNeedToCentre(true);
    previewRenderer_->show();

    // video
    videoDeviceEventHandlerAndMediaSettingSetUp();

    auto encodeAccel = LRCInstance::avModel().getHardwareAcceleration();
    ui->hardwareAccelCheckBox->setChecked(encodeAccel);
}

void SettingsWidget::outputDevIndexChangedSlot(int index)
{
    stopAudioMeter(true);
    auto selectedOutputDeviceName = ui->outputComboBox->itemData(index, Qt::DisplayRole)
        .toString().toStdString();
    LRCInstance::avModel().setOutputDevice(selectedOutputDeviceName);
    startAudioMeter(true);
}

void SettingsWidget::inputdevIndexChangedSlot(int index)
{
    stopAudioMeter(true);
    auto selectedInputDeviceName = ui->inputComboBox->itemData(index, Qt::DisplayRole)
        .toString().toStdString();
    LRCInstance::avModel().setInputDevice(selectedInputDeviceName);
    startAudioMeter(true);
}

void SettingsWidget::slotDeviceBoxCurrentIndexChanged(int index)
{
    std::string device = ui->deviceBox->itemData(index, Qt::DisplayRole).toString().toStdString();
    LRCInstance::avModel().setCurrentVideoCaptureDevice(device);
    LRCInstance::avModel().setDefaultDevice(device);
    setFormatListForDevice(device);
    emit videoDeviceChanged(device, true);
    startPreviewing(true);
}

void SettingsWidget::slotFormatBoxCurrentIndexChanged(int index)
{
    auto resolution = formatIndexList_.at(index).first;
    auto rate = formatIndexList_.at(index).second;
    auto decive = LRCInstance::avModel().getCurrentVideoCaptureDevice();
    auto currentSettings = LRCInstance::avModel().getDeviceSettings(decive);
    lrc::api::video::Settings settings{ {}, decive, rate, resolution };
    previewRenderer_->connectRendering();
    LRCInstance::avModel().setDeviceSettings(settings);
}

void SettingsWidget::startPreviewing(bool isDeviceChanged)
{
    if (!LRCInstance::getActiveCalls().size() || isDeviceChanged || LRCInstance::getIfCurrentSelectedCallIsAudioOnly()) {
        // if no active calls, or device is changed -> reactive preview
        previewRenderer_->connectRendering();
        QtConcurrent::run(
            [this] {
                LRCInstance::avModel().stopPreview();
                LRCInstance::avModel().startPreview();
            });
        isPreviewed_ = true;
    }
    ui->previewUnavailableLabel->hide();
    ui->videoLayoutWidget->show();
}

void SettingsWidget::stopPreviewing()
{
    if (!LRCInstance::getActiveCalls().size() || LRCInstance::getIfCurrentSelectedCallIsAudioOnly()) {
        QtConcurrent::run( [this] { LRCInstance::avModel().stopPreview(); });
    }
    isPreviewed_ = false;
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

void SettingsWidget::setFormatListForDevice(const std::string& device)
{
    auto deviceCapabilities = LRCInstance::avModel().getDeviceCapabilities(device);
    if (deviceCapabilities.size() == 0) {
        return;
    }
    auto currentSettings = LRCInstance::avModel().getDeviceSettings(device);
    auto currentChannel = currentSettings.channel;
    currentChannel = currentChannel.empty() ? "default" : currentChannel;
    auto channelCaps = deviceCapabilities.at(currentChannel);

    ui->formatBox->blockSignals(true);
    ui->formatBox->clear();
    formatIndexList_.clear();

    for (auto [resolution, frameRateList] : channelCaps) {
        for (auto rate : frameRateList) {
            formatIndexList_.append(QPair<std::string , float> (resolution, rate));
            auto sizeRateString = QString("%1 [%2 fps]")
                .arg(QString::fromStdString(resolution))
                .arg(round(rate));
            ui->formatBox->addItem(sizeRateString.toUtf8());
            if (resolution == currentSettings.size && rate == currentSettings.rate) {
                ui->formatBox->setCurrentIndex(ui->formatBox->count() - 1);
            }
        }
    }

    ui->formatBox->blockSignals(false);
}

void SettingsWidget::slotSetHardwareAccel(bool state)
{
    LRCInstance::avModel().setHardwareAcceleration(state);
}

void SettingsWidget::startAudioMeter(bool blocking)
{
    if (LRCInstance::getActiveCalls().size()) {
        return;
    }
    ui->audioInputMeter->start();
    auto f = [this] {
        LRCInstance::avModel().startAudioDevice();
        LRCInstance::avModel().setAudioMeterState(true);
    };
    if (blocking) {
        f();
    } else {
        QtConcurrent::run(f);
    }
}

void SettingsWidget::stopAudioMeter(bool blocking)
{
    if (LRCInstance::getActiveCalls().size()) {
        return;
    }
    LRCInstance::avModel().setAudioMeterState(false);
    ui->audioInputMeter->stop();
    auto f = [this] { LRCInstance::avModel().stopAudioDevice(); };
    blocking ? f() : QtConcurrent::run(f);
}

void SettingsWidget::resetPhotoBoothStateWhenSettingChanged(Button type)
{
    bool stopPhotoboothPreview = false;
    // if setting change from account (photo booth open) to general, have to stop video endering
    // if setting change from account (photo booth open) to media, does not have to stop video rendering

    // if we are in a call, it will only reset the avator label,buttons

    if (type == Button::generalSettingsButton) {
        stopPhotoboothPreview = true;
    }
    if (LRCInstance::getCurrentAccountInfo().profileInfo.type == lrc::api::profile::Type::SIP) {
        if (ui->currentSIPAccountAvatar->isPhotoBoothOpened()) {
            setAvatar(ui->currentSIPAccountAvatar, stopPhotoboothPreview);
            ui->currentSIPAccountAvatar->resetTakePhotoState(false);
        }
    } else {
        if (ui->currentAccountAvatar->isPhotoBoothOpened()) {
            setAvatar(ui->currentAccountAvatar, stopPhotoboothPreview);
            ui->currentAccountAvatar->resetTakePhotoState(false);
        }
    }
}

void SettingsWidget::videoDeviceEventHandlerAndMediaSettingSetUp()
{
    disconnect(ui->deviceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::slotDeviceBoxCurrentIndexChanged);
    disconnect(ui->formatBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::slotFormatBoxCurrentIndexChanged);
    auto devices = LRCInstance::avModel().getDevices();
    ui->deviceBox->clear();
    ui->formatBox->clear();

    // first is isSIP, second is shouldReinitial
    // if decive event happen when we are in opening photo booth
    bool isSIP = LRCInstance::getCurrentAccountInfo().profileInfo.type == lrc::api::profile::Type::SIP;
    auto photoBooth = isSIP ? ui->currentSIPAccountAvatar : ui->currentAccountAvatar;
    auto scrollWidget = isSIP ? ui->currentSIPAccountSettingsScrollWidget : ui->currentAccountSettingsScrollWidget;
    bool shouldRestart = scrollWidget->isVisible() && photoBooth->isPhotoBoothOpened();

    bool shouldReinitializePreview { true };
    int deviceSize { static_cast<int>(devices.size()) };
    bool hasVideoDevices = deviceSize;

    // decide whether the decive is plugged or unplugged or nothing changed
    Utils::DevicePlugStatus plugStatus = previousDeviceSize_ < deviceSize
                                           ? Utils::DevicePlugStatus::Plugged
                                           : (previousDeviceSize_ == deviceSize ? Utils::DevicePlugStatus::Unchanged : Utils::DevicePlugStatus::Unplugged);
    previousDeviceSize_ = deviceSize;
    ui->deviceBox->setEnabled(hasVideoDevices);
    ui->formatBox->setEnabled(hasVideoDevices);
    ui->labelVideoDevice->setEnabled(hasVideoDevices);
    ui->labelVideoFormat->setEnabled(hasVideoDevices);

    // if has devices, change parameters accordingly
    if (hasVideoDevices) {
        auto device = LRCInstance::avModel().getDefaultDeviceName();

        if (LRCInstance::avModel().getCurrentVideoCaptureDevice().empty()) {
            LRCInstance::avModel().setCurrentVideoCaptureDevice(device);
        }
        if (plugStatus == Utils::DevicePlugStatus::Plugged && LRCInstance::avModel().getCurrentVideoCaptureDevice() == device && !deviceWasEmpty_) {
            // if a device is plugged and current using device does not change and device was not empty
            // we do not need to re-startPreview
            shouldReinitializePreview = false;
            shouldRestart = false;
        } else if (plugStatus == Utils::DevicePlugStatus::Unplugged) {
            if(LRCInstance::avModel().getCurrentVideoCaptureDevice() != device){
                // unplugged decive equals to currentCapture device, reset it
                LRCInstance::avModel().setCurrentVideoCaptureDevice(device);
            } else {
                shouldReinitializePreview = false;
                shouldRestart = false;
            }
        } else if (!deviceWasEmpty_) {
            // if nothing changed and device was empty
            if(LRCInstance::avModel().getCurrentVideoCaptureDevice() != device){
                if (LRCInstance::getActiveCalls().size()) {
                    // if unchanged, but context menu change the video device
                    shouldReinitializePreview = false;
                } else {
                    LRCInstance::avModel().setCurrentVideoCaptureDevice(device);
                }
            } else {
                shouldReinitializePreview = false;
            }
            shouldRestart = false;
        }

        auto deviceIndex = Utils::indexInVector(devices, LRCInstance::avModel().getCurrentVideoCaptureDevice());
        for (auto d : devices) {
            ui->deviceBox->addItem(QString::fromStdString(d).toUtf8());
        }
        ui->deviceBox->setCurrentIndex(deviceIndex);
        setFormatListForDevice(LRCInstance::avModel().getCurrentVideoCaptureDevice());
        deviceWasEmpty_ = false;
    } else {
        LRCInstance::avModel().clearCurrentVideoCaptureDevice();
        ui->deviceBox->addItem(QObject::tr("No Device"));
        ui->formatBox->addItem(QObject::tr("No Device"));
        ui->deviceBox->setCurrentIndex(0);
        ui->deviceBox->setCurrentIndex(0);
        shouldRestart = false;
        shouldReinitializePreview = false;
        deviceWasEmpty_ = true;
        ui->previewUnavailableLabel->show();
        ui->videoLayoutWidget->hide();
        LRCInstance::avModel().switchInputTo("");
    }
    connect(ui->deviceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::slotDeviceBoxCurrentIndexChanged);
    connect(ui->formatBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::slotFormatBoxCurrentIndexChanged);

    if (shouldRestart) {
        // if we are in photo booth open state and in account setting widget,
        // reset the photo booth video
        emit videoDeviceChanged(LRCInstance::avModel().getDefaultDeviceName(), true);
        photoBooth->startBooth(true);
    } else if (shouldReinitializePreview && (ui->avSettings->isVisible() || LRCInstance::getActiveCalls().size())) {
        // if shouldReinitializePreview && has active calls -> videoInputDeviceLoseConnection to refersh video input
        emit videoDeviceChanged(LRCInstance::avModel().getDefaultDeviceName(), ui->avSettings->isVisible());
        if(ui->avSettings->isVisible())
            startPreviewing(true);
    }
}
