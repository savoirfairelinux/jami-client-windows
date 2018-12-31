/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 **************************************************************************/
#include <QPixmap>
#include <QTimer>
#include <QModelIndex>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSettings>

// account settings
#include "api/newdevicemodel.h"
#include "settingsitemwidget.h"

#include "settingswidget.h"
#include "ui_settingswidget.h"

#include "ui_advancedsettingswidget.h"

#include "passworddialog.h"

#include "regnamedialog.h"
#include "ui_regnamedialog.h"

#include "setavatardialog.h"
#include "ui_setavatardialog.h"

#include "deleteaccountdialog.h"
#include "ui_deleteaccountdialog.h"


// general Settings
#include "winsparkle.h"
#include "media/recordingmodel.h"

// av setttings
#include "audio/settings.h"
#include "audio/outputdevicemodel.h"
#include "audio/inputdevicemodel.h"

#include "video/devicemodel.h"
#include "video/channel.h"
#include "video/resolution.h"
#include "video/rate.h"
#include "video/previewmanager.h"

#include "callmodel.h"

SettingsWidget::SettingsWidget(QWidget* parent)
    : NavWidget(parent),
    ui(new Ui::SettingsWidget),
    scrollArea_(new QScrollArea(this)),
    deviceModel_(&Video::DeviceModel::instance()),
    gif(new QMovie(":/images/ajax-loader.gif"))
{
    ui->setupUi(this);

    ui->accountSettingsButton->setAutoFillBackground(true);
    ui->generalSettingsButton->setAutoFillBackground(true);
    ui->avSettingsButton->setAutoFillBackground(true);

    ui->accountSettingsButton->setChecked(true);

    ui->exitSettingsButton->setIconSize(QSize(24, 24));
    ui->exitSettingsButton->setIcon(QPixmap(":/images/icons/round-close-24px.svg"));


    // display name (aka alias)
    ui->displayNameLineEdit->setAlignment(Qt::AlignHCenter);
    ui->displayNameLineEdit->setPlaceholderText(tr("Enter the displayed name"));


    setSelected(Button::accountSettingsButton);

    ui->currentRegisteredID->setReadOnly(true);
    ui->currentRegisteredID->setStyleSheet("border : 0px;");

    ui->currentRingID->setReadOnly(true);

    avatarSize_ = ui->currentAccountAvatar->width();

    ui->currentAccountAvatar->setIconSize(QSize(avatarSize_, avatarSize_));

    // create ellipse-selectable avatar image
    QRegion avatarClickableRegion(-1, -1,
        ui->currentAccountAvatar->width() + 2, ui->currentAccountAvatar->height() + 2, QRegion::Ellipse);
    ui->currentAccountAvatar->setMask(avatarClickableRegion);

    QString styleS(
        "QPushButton{"
        "  background-color: rgb(245, 245, 245);"
        "  border: 0px;"
        "}"
        " QPushButton:hover{"
        "     background-color: rgb(250, 250, 250);"
        "     border: 0px;"
        " }"

        "QPushButton:checked{"
        "    background-color: white;"
        "    border: 0px;"
        "}"
    );

    ui->accountSettingsButton->setStyleSheet(styleS);
    ui->generalSettingsButton->setStyleSheet(styleS);
    ui->avSettingsButton->setStyleSheet(styleS);

    ui->advancedAccountSettingsPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_down-24px.svg"));
    ui->linkDevPushButton->setIcon(QPixmap(":/images/icons/round-add-24px.svg"));
    ui->blockedContactsBtn->setIcon(QPixmap(":/images/icons/round-arrow_drop_up-24px.svg"));

    ui->advancedSettingsOffsetLabel->show();

    auto accountList = LRCInstance::accountModel().getAccountList();
    if (!accountList.size()) {
        QMetaObject::Connection* toDisconnect = &accountAddedConnection_;
        accountAddedConnection_ = connect(&LRCInstance::accountModel(),
            &lrc::api::NewAccountModel::accountAdded,
            [this, toDisconnect](const std::string& accountId) {
                Q_UNUSED(accountId);
                setConnections();
                QObject::disconnect(*toDisconnect);
            });
    } else {
        setConnections();
    }

    ui->containerWidget->setVisible(false);
}

void
SettingsWidget::navigated(bool to)
{
    ui->containerWidget->setVisible(to);
}

void SettingsWidget::updateCustomUI()
{
}

void
SettingsWidget::leaveSettingsSlot()
{
    if (advancedSettingsDropped_) {
        toggleAdvancedSettings();
    }

    Video::PreviewManager::instance().stopPreview();
    saveSizeIndex();

    emit NavigationRequested(ScreenEnum::CallScreen);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

// called at every callwidget -> settingsWidget navigation
void
SettingsWidget::updateSettings(int size)
{
    setSelected(Button::accountSettingsButton);
    resize(size);
    updateAccountInfoDisplayed();
}

void
SettingsWidget::resize(int size)
{
    ui->rightSettingsWidget->setGeometry(size, 0, this->width() - size, this->height());
    ui->accountSettingsButton->setFixedWidth(size);
}

void
SettingsWidget::setSelected(Button sel)
{
    saveSizeIndex();

    switch (sel)
    {
    case Button::accountSettingsButton:
        Video::PreviewManager::instance().stopPreview();

        if (advancedSettingsDropped_) { toggleAdvancedSettings(); }
        ui->stackedWidget->setCurrentWidget(ui->currentAccountSettingsScrollWidget);
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
        Video::PreviewManager::instance().stopPreview();

        ui->stackedWidget->setCurrentWidget(ui->generalSettings);
        populateGeneralSettings();
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
        populateAVSettings();
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

// if no registered name is found for account
    if (LRCInstance::getCurrentAccountInfo().registeredName.empty()) {
        ui->currentRegisteredID->setReadOnly(false);
    }
    else {
        ui->currentRegisteredID->setReadOnly(true);
        setRegNameUi(RegName::BLANK);
    }

    ui->currentAccountAvatar->setIcon(LRCInstance::getCurrAccPixmap().
        scaledToHeight(avatarSize_, Qt::SmoothTransformation));

    ui->accountEnableCheckBox->setChecked(LRCInstance::getCurrentAccountInfo().enabled);

    ui->displayNameLineEdit->setText(QString::fromStdString(LRCInstance::getCurrentAccountInfo().profileInfo.alias));

    updateAndShowDevicesSlot();
    bannedContactsShown_ = false;
    if (!LRCInstance::getCurrentAccountInfo().contactModel->getBannedContacts().size()){
        ui->blockedContactsBtn->hide();
    } else {
        ui->blockedContactsBtn->show();
    }
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
        ui->currentAccountSettingsScrollLayout->removeWidget(advancedSettingsWidget_);
        ui->scrollBarLabel->show();
        ui->advancedSettingsOffsetLabel->hide();
        delete advancedSettingsWidget_;
    }
    else { // will show advanced settings next
        ui->advancedAccountSettingsPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_up-24px.svg"));
        advancedSettingsWidget_ = new AdvancedSettingsWidget(this);
        advancedSettingsWidget_->setMaximumWidth(ui->scrollAreaWidgetContents->width() - 10);
        ui->currentAccountSettingsScrollLayout->addWidget(advancedSettingsWidget_);
        ui->advancedSettingsOffsetLabel->show();
        ui->scrollBarLabel->hide();
    }
    advancedSettingsDropped_ = !advancedSettingsDropped_;
}

void
SettingsWidget::toggleBannedContacts()
{
    if (bannedContactsShown_) { // will show linked devices next
        bannedContactsShown_ = false;
        updateAndShowDevicesSlot();
    }
    else { // will show banned contacts next
        bannedContactsShown_ = true;
        updateAndShowBannedContactsSlot();
    }
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
                LRCInstance::setCurrAccAvatar(pixString);
                updateAccountInfoDisplayed();
            }
        }
    );
    avatarDialog.exec();
}

void
SettingsWidget::verifyRegisteredNameSlot()
{
    if (!LRCInstance::getCurrentAccountInfo().registeredName.empty()) {
        setRegNameUi(RegName::BLANK);
    }
    else {
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
bool
SettingsWidget::validateRegNameForm(const QString& regName)
{
    QRegularExpression regExp(" ");
    if (regName.size() > 2 && !regName.contains(regExp)) {
        return true;
    } else {
        return false;
    }
}

void
SettingsWidget::receiveRegNameSlot(const std::string& accountID,
    lrc::api::account::LookupStatus status, const std::string& address, const std::string& name)
{
    Q_UNUSED(accountID); Q_UNUSED(address);
    afterNameLookup(status, name);
}

void
SettingsWidget::beforeNameLookup()
{
    NameDirectory::instance().lookupName(nullptr, QString(), registeredName_);
}

void
SettingsWidget::afterNameLookup(lrc::api::account::LookupStatus status, const std::string& regName)
{
    if (registeredName_.toStdString() == regName && regName.length() > 2) {
        if (status == lrc::api::account::LookupStatus::NOT_FOUND) {
            setRegNameUi(RegName::FREE);
        }
        else {
            setRegNameUi(RegName::TAKEN);
        }
    }
    else {
        setRegNameUi(RegName::BLANK);
    }
}

void SettingsWidget::setRegNameUi(RegName stat)
{
    disconnect(gif, SIGNAL(frameChanged(int)), this, SLOT(setButtonIconSlot(int)));
    disconnect(ui->regNameButton, &QPushButton::clicked, this, &SettingsWidget::regNameRegisteredSlot);

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

        connect(ui->regNameButton, &QPushButton::clicked, this, &SettingsWidget::regNameRegisteredSlot);
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

void
SettingsWidget::setButtonIconSlot(int frame)
{
    Q_UNUSED(frame);
    ui->regNameButton->setIcon(QIcon(gif->currentPixmap()));
}

void
SettingsWidget::regNameRegisteredSlot()
{
    if (!regNameBtn_) { return; }

    RegNameDialog regNameDialog(registeredName_, this);
    if (regNameDialog.exec() == QDialog::Accepted) {
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

void
SettingsWidget::setAccEnableSlot(int state)
{
    LRCInstance::editableAccountModel()->enableAccount(LRCInstance::getCurrAccId(), (bool)state);

    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

void
SettingsWidget::delAccountSlot()
{
    DeleteAccountDialog delDialog(this);
    delDialog.exec();

    LRCInstance::setSelectedAccountId("");
    if (!LRCInstance::accountModel().getAccountList().size()) {
        emit NavigationRequested(ScreenEnum::WizardScreen);
    } else {
        emit NavigationRequested(ScreenEnum::CallScreen);
    }
}

void
SettingsWidget::removeDeviceSlot(int index)
{
    if (!index) { return; }

    auto deviceList = LRCInstance::getCurrentAccountInfo().deviceModel->getAllDevices();
    auto it = deviceList.begin();

    std::advance(it, index);
    QString psswd;

    bool ok = false;
    if (LRCInstance::getCurrAccConfig().archiveHasPassword) {
        psswd = QInputDialog::getText(this, tr("Remove Device"),
            tr("Enter this account's password to confirm the removal of this device"), QLineEdit::Password,
            QDir::home().dirName(), &ok);
    }
    else {
        psswd = "";
        QMessageBox devDel;
        devDel.setText(tr("Please confirm that you wish to remove this device"));
        devDel.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        devDel.setDefaultButton(QMessageBox::Cancel);
        if (devDel.exec() == QMessageBox::Ok) { goto delete_; }
    }

    if (ok) {
        delete_:
        LRCInstance::getCurrentAccountInfo().deviceModel->revokeDevice(it->id, psswd.toStdString());
        updateAndShowDevicesSlot();
    }
}

void
SettingsWidget::unban(int index)
{
    auto bannedContactList = LRCInstance::getCurrentAccountInfo().contactModel->getBannedContacts();
    auto it = bannedContactList.begin();
    std::advance(it, index);

    auto contactInfo = LRCInstance::getCurrentAccountInfo().contactModel->getContact(*it);

    LRCInstance::getCurrentAccountInfo().contactModel->addContact(contactInfo);
    updateAndShowBannedContactsSlot();
}

void
SettingsWidget::exportAccountSlot()
{
    QFileDialog dialog(this);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Export Account Here"),
        QDir::homePath() + "/Desktop", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        LRCInstance::accountModel().exportToFile(LRCInstance::getCurrAccId(), (dir + "/export.gz").toStdString());
    }
}

void
SettingsWidget::updateAndShowDevicesSlot()
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
                }
            );
        }
    }
}

void
SettingsWidget::updateAndShowBannedContactsSlot()
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
            }
            );
        }
        if (!bannedContactList.size()) { updateAndShowDevicesSlot(); ui->blockedContactsBtn->hide(); }
    }
}

void
SettingsWidget::showLinkDevSlot()
{
    if (!advancedSettingsWidget_) { delete advancedSettingsWidget_; }

    linkDevWidget = new LinkDevWidget(ui->scrollAreaWidgetContents);
    linkDevWidget->setMinimumWidth(600);

    ui->accountHorLayout->insertWidget(1, linkDevWidget);

    linkDevWidget->show();
    ui->centralWidget->hide();

    connect(linkDevWidget->cancelBtn(), &QPushButton::clicked, this, &SettingsWidget::showCurrentAccountSlot);
    connect(linkDevWidget->endCancelBtn(), &QPushButton::clicked, this, &SettingsWidget::showCurrentAccountSlot);
}

void
SettingsWidget::showCurrentAccountSlot()
{
    disconnect(linkDevWidget);
    delete linkDevWidget;

    ui->centralWidget->show();
    updateAndShowDevicesSlot();
}

void
SettingsWidget::setConnections()
{
    // exitSettingsButton
    connect(ui->exitSettingsButton, &QPushButton::clicked, this, &SettingsWidget::leaveSettingsSlot);

    connect(ui->accountSettingsButton, &QPushButton::clicked, [this]() {
        setSelected(Button::accountSettingsButton); }
    );

    connect(ui->generalSettingsButton, &QPushButton::clicked, [this]() {
        setSelected(Button::generalSettingsButton); }
    );

    connect(ui->avSettingsButton, &QPushButton::clicked, [this]() {
        setSelected(Button::avSettingsButton); }
    );

    connect(ui->passwdPushButton, &QPushButton::clicked, [this]() {
        passwordClicked(); }
    );

    connect(ui->currentAccountAvatar, &QPushButton::clicked, [this]() {
        avatarClicked(); }
    );

    connect(ui->advancedAccountSettingsPButton, &QPushButton::clicked, this, &SettingsWidget::toggleAdvancedSettings);

    connect(ui->currentRegisteredID, &QLineEdit::textChanged, this, &SettingsWidget::verifyRegisteredNameSlot);

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

    connect(ui->displayNameLineEdit, &QLineEdit::textChanged, [this](const QString& displayName) {
        LRCInstance::setCurrAccDisplayName(displayName.toStdString());
    }
    );

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

    ui->autoUpdateCheckBox->setChecked(win_sparkle_get_automatic_check_for_updates());
    ui->intervalUpdateCheckSpinBox->setValue(win_sparkle_get_update_check_interval() / 86400);
}

void
SettingsWidget::setNotificationsSlot(int state)
{
    QSettings settings;
    if (state == Qt::CheckState::Unchecked) {
        settings.setValue(SettingsKey::enableNotifications, false);
    } else {
        settings.setValue(SettingsKey::enableNotifications, true);
    }
}

void
SettingsWidget::setClosedOrMinSlot(int state)
{
    QSettings settings;
    if (state == Qt::CheckState::Unchecked) {
        settings.setValue(SettingsKey::closeOrMinimized, false);
    }
    else {
        settings.setValue(SettingsKey::closeOrMinimized, true);
    }
}

void
SettingsWidget::checkForUpdateSlot()
{
    win_sparkle_check_update_with_ui();
}

void
SettingsWidget::setUpdateIntervalSlot(int value)
{
    win_sparkle_set_update_check_interval(value * 86400);
}

void
SettingsWidget::setUpdateAutomaticSlot(int state)
{
    if (state == Qt::CheckState::Unchecked) {
        win_sparkle_set_automatic_check_for_updates(false);
        ui->intervalUpdateCheckSpinBox->setEnabled(false);
    } else {
        win_sparkle_set_automatic_check_for_updates(true);
        ui->intervalUpdateCheckSpinBox->setEnabled(true);
    }
}

void
SettingsWidget::openDownloadFolderSlot()
{
    QSettings settings;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select A Folder For Your Downloads"),
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), QFileDialog::ShowDirsOnly
        | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        ui->downloadButton->setText(dir);
        settings.setValue(SettingsKey::downloadPath, dir);
        LRCInstance::editableDataTransferModel()->downloadDirectory = dir.toStdString() + "/";
    }
}

void
SettingsWidget::setAlwaysRecordingSlot(int state)
{
    if (state == Qt::CheckState::Unchecked) {
        media::RecordingModel::instance().setAlwaysRecording(false);
    } else {
        media::RecordingModel::instance().setAlwaysRecording(true);
    }
}

void
SettingsWidget::openRecordFolderSlot()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select A Folder For Your Recordings"),
        media::RecordingModel::instance().recordPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        media::RecordingModel::instance().setRecordPath(dir);
        ui->recordPathButton->setText(media::RecordingModel::instance().recordPath());
    }
}


// *************************  Audio/Visual Settings  *************************

void
SettingsWidget::populateAVSettings()
{
    ui->deviceBox->setModel(deviceModel_);
    connect(deviceModel_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(deviceIndexChanged(int)));

    // Audio settings
    auto inputModel = Audio::Settings::instance().inputDeviceModel();
    auto outputModel = Audio::Settings::instance().outputDeviceModel();

    ui->inputComboBox->setModel(inputModel);
    ui->outputComboBox->setModel(outputModel);

    auto inputIndex = inputModel->selectionModel()->currentIndex();
    auto outputIndex = outputModel->selectionModel()->currentIndex();

    ui->inputComboBox->setCurrentIndex(inputIndex.row());
    ui->outputComboBox->setCurrentIndex(outputIndex.row());

    if (ui->deviceBox->count() > 0) {
        deviceBoxCurrentIndexChangedSlot(0);
    }

    if (currentResIndex >= 0) {
        ui->sizeBox->setCurrentIndex(currentResIndex);
    }

    connect(ui->outputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::outputDevIndexChangedSlot);
    connect(ui->inputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsWidget::inputdevIndexChangedSlot);

    connect(ui->deviceBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &SettingsWidget::deviceBoxCurrentIndexChangedSlot);
    connect(ui->sizeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &SettingsWidget::sizeBoxCurrentIndexChangedSlot);

    showPreview();
}


void
SettingsWidget::saveSizeIndex()
{
    currentResIndex = ui->sizeBox->currentIndex();
}

void
SettingsWidget::showPreview()
{
    if (!CallModel::instance().getActiveCalls().size()) {
        ui->previewUnavailableLabel->hide();
        ui->videoWidget->show();
        startVideo();
        ui->videoWidget->setIsFullPreview(true);
    } else {
        ui->previewUnavailableLabel->show();
        ui->videoWidget->hide();
    }
}

void
SettingsWidget::deviceBoxCurrentIndexChangedSlot(int index)
{
    if (index < 0) {
        return;
    }

    deviceModel_->setActive(index);

    auto device = deviceModel_->activeDevice();

    ui->sizeBox->clear();

    if (device->channelList().size() > 0) {
        for (auto resolution : device->channelList()[0]->validResolutions()) {
            ui->sizeBox->addItem(resolution->name());
        }
    }
    ui->sizeBox->setCurrentIndex(
        device->channelList()[0]->activeResolution()->relativeIndex());
}

void
SettingsWidget::sizeBoxCurrentIndexChangedSlot(int index)
{
    if (index < 0) {
        return;
    }

    auto device = deviceModel_->activeDevice();

    device->channelList()[0]->setActiveResolution(device->channelList()[0]->validResolutions()[index]);

    QTimer::singleShot(200, this, [this]() {
    deviceModel_->setActive(ui->deviceBox->currentIndex());
    });
}

void
SettingsWidget::deviceIndexChanged(int index)
{
    ui->deviceBox->setCurrentIndex(index);

    ui->videoLayout->update();
}

void
SettingsWidget::outputDevIndexChangedSlot(int index)
{
    auto outputModel = Audio::Settings::instance().outputDeviceModel();
    outputModel->selectionModel()->setCurrentIndex(outputModel->index(index), QItemSelectionModel::ClearAndSelect);
}

void
SettingsWidget::inputdevIndexChangedSlot(int index)
{
    auto inputModel = Audio::Settings::instance().inputDeviceModel();
    inputModel->selectionModel()->setCurrentIndex(inputModel->index(index), QItemSelectionModel::ClearAndSelect);
}

void
SettingsWidget::startVideo()
{
    Video::PreviewManager::instance().stopPreview();
    Video::PreviewManager::instance().startPreview();
}

void
SettingsWidget::stopVideo()
{
    Video::PreviewManager::instance().stopPreview();
}
