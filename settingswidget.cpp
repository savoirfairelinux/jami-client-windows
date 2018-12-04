/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 **************************************************************************/
#include <QPixmap>
#include <QTimer>
#include <QModelIndex>
#include <QFileDialog>
#include <QInputDialog>

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

SettingsWidget::SettingsWidget(QWidget* parent)
    : NavWidget(parent),
      ui(new Ui::SettingsWidget),
      scrollArea_(new QScrollArea)
{
    ui->setupUi(this);

    ui->accountSettingsButton->setCheckable(true);
    ui->generalSettingsButton->setCheckable(true);
    ui->avSettingsButton->setCheckable(true);
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

    //scrollArea_->setWidgetResizable(true);
    //scrollArea_->setParent(ui->currentAccountSettingsScrollWidget);
    //scrollArea_->setWidget(ui->currentAccountSettingsScrollWidget);
    //scrollArea_->setStyleSheet("border: 0px;");
    //scrollArea_->resize(ui->currentAccountSettingsScrollWidget->width(), this->height());

    avatarSize_ = ui->currentAccountAvatar->width();

    ui->currentAccountAvatar->setIconSize(QSize(avatarSize_, avatarSize_));

    // create ellipse-selectable avatar image
    QRegion avatarClickableRegion(-1, -1,
        ui->currentAccountAvatar->width() + 2, ui->currentAccountAvatar->height() + 2, QRegion::Ellipse);
    ui->currentAccountAvatar->setMask(avatarClickableRegion);

    // safe space below
    setConnections();
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
    switch (sel)
    {
    case Button::accountSettingsButton:
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

// if no registered name is found for account
    if (LRCInstance::getCurrentAccountInfo().registeredName.empty()) {
        ui->currentRegisteredID->setReadOnly(false);
    }
    else {
        ui->currentRegisteredID->setReadOnly(true);
        setRegNameUi(RegName::BLANK);
    }

    ui->currentAccountAvatar->setIcon(LRCInstance::getCurrAccData(AccountListModel::Role::Picture).value<QPixmap>().
        scaledToHeight(avatarSize_, Qt::SmoothTransformation));

    // accountEnableCheckBox getter
    ui->accountEnableCheckBox->setChecked(LRCInstance::getCurrentAccountInfo().enabled);

    ui->displayNameLineEdit->setText(QString::fromStdString(LRCInstance::getCurrentAccountInfo().profileInfo.alias));

    updateAndShowDevicesSlot();
    if (!LRCInstance::getCurrentAccountInfo().contactModel->getBannedContacts().size()){
        ui->blockedContactsBtn->hide();
    } else {
        ui->blockedContactsBtn->show();
    }
    bannedContactsShown_ = false;
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
        delete advancedSettingsWidget_;
    }
    else { // if advanced settings shown
        ui->advancedAccountSettingsPButton->setIcon(QPixmap(":/images/icons/round-arrow_drop_up-24px.svg"));
        advancedSettingsWidget_ = new AdvancedSettingsWidget(this);
        ui->currentAccountSettingsScrollLayout->addWidget(advancedSettingsWidget_);
    }
    advancedSettingsDropped_ = !advancedSettingsDropped_;
}

void
SettingsWidget::toggleBannedContacts()
{
    if (bannedContactsShown_) { // will show linked devices next
        updateAndShowDevicesSlot();
    }
    else { // will show banned contacts next
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
    }
    else {
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
    switch (stat) {
    case RegName::BLANK:
        ui->currentRegisteredID->setStyleSheet("border: 0px; border-radius: 3px;");
        regNameBtn_ = false;
        ui->currentRegisteredIDBtn->setText(tr("Registered Name: "));
        ui->currentRegisteredIDBtn->setToolTip(tr(""));
        ui->currentRegisteredID->setToolTip(tr(""));
        ui->currentRegisteredIDBtn->setStyleSheet("background: white;");
        break;

    case RegName::INVALIDFORM:
        ui->currentRegisteredID->setStyleSheet("border: 1px solid red; border-radius: 3px;");
        regNameBtn_ = false;
        ui->currentRegisteredIDBtn->setText(tr("Invalid Name"));
        ui->currentRegisteredIDBtn->setToolTip(tr(""));
        ui->currentRegisteredID->setToolTip(tr("A registered name should not have any spaces")); // [confirm]
        ui->currentRegisteredIDBtn->setStyleSheet("background: white;");
        break;

    case RegName::TAKEN:
        ui->currentRegisteredID->setStyleSheet("border: 1px solid orange; border-radius: 3px;");
        regNameBtn_ = false;
        ui->currentRegisteredIDBtn->setText(tr("Not Available"));
        ui->currentRegisteredIDBtn->setToolTip(tr(""));
        ui->currentRegisteredID->setToolTip(tr("This name is already taken")); // [confirm]
        ui->currentRegisteredIDBtn->setStyleSheet("background: white;");
        break;

    case RegName::FREE:
        ui->currentRegisteredID->setStyleSheet("border: 1px solid green; border-radius: 3px;");
        regNameBtn_ = true;
        ui->currentRegisteredIDBtn->setText(tr("Register Name"));
        ui->currentRegisteredIDBtn->setToolTip(tr(""));
        ui->currentRegisteredID->setToolTip(tr("This name is available")); // [confirm]
        ui->currentRegisteredIDBtn->setStyleSheet("background: #e8f5e9; border-radius: 3px;");
        break;

    case RegName::SEARCHING:
        ui->currentRegisteredID->setStyleSheet("border: 1px solid orange; border-radius: 3px;");
        regNameBtn_ = false;
        ui->currentRegisteredIDBtn->setText(tr("Searching..."));
        ui->currentRegisteredIDBtn->setToolTip(tr("Verifying that this name is available")); // [confirm]
        ui->currentRegisteredID->setToolTip(tr(""));
        ui->currentRegisteredIDBtn->setStyleSheet("background: white;");
        break;
    }
}

void
SettingsWidget::regNameRegisteredSlot()
{
    if (!regNameBtn_) { return; }

    RegNameDialog regNameDialog(registeredName_, this);
    if (regNameDialog.exec() == QDialog::Accepted) { // if user confirms regName choice
        ui->currentRegisteredID->setReadOnly(true);
    }
    else {
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
    if (!LRCInstance::accountModel().getAccountList().size()) {
        emit NavigationRequested(ScreenEnum::WizardScreen);
    }
}

void
SettingsWidget::removeDeviceSlot(int index)
{
    if (!index) { return; }

    auto deviceList = LRCInstance::getCurrentAccountInfo().deviceModel->getAllDevices();
    auto it = deviceList.begin();

    std::advance(it, index);

    bool ok;
    QString psswd = QInputDialog::getText(this, tr("Remove Device"),
        tr("Enter the password on this device: \nLeave empty if no password."), QLineEdit::Password,
        QDir::home().dirName(), &ok);

    if (ok) {
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
    bannedContactsShown_ = false;
}

void
SettingsWidget::updateAndShowBannedContactsSlot()
{
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
    bannedContactsShown_ = true;
    if (!bannedContactList.size()) { updateAndShowDevicesSlot(); ui->blockedContactsBtn->hide(); }
}

void
SettingsWidget::showLinkDevSlot()
{
    ui->currentAccountSettingsScrollWidget->hide();
    linkDevWidget = new LinkDevWidget(ui->scrollAreaWidgetContents);

    linkDevWidget->setGeometry(ui->currentAccountSettingsScrollWidget->rect());
    linkDevWidget->show();
    connect(linkDevWidget->cancelBtn(), &QPushButton::clicked, this, &SettingsWidget::showCurrentAccountSlot);
    connect(linkDevWidget->endCancelBtn(), &QPushButton::clicked, this, &SettingsWidget::showCurrentAccountSlot);
}

void
SettingsWidget::showCurrentAccountSlot()
{
    disconnect(linkDevWidget);
    ui->currentAccountSettingsScrollWidget->setGeometry(linkDevWidget->rect());
    ui->currentAccountSettingsScrollWidget->show();

    delete linkDevWidget;
}

void
SettingsWidget::setConnections()
{
    // exitSettingsButton
    connect(ui->exitSettingsButton, &QPushButton::clicked, [this]() {
        emit NavigationRequested(ScreenEnum::CallScreen);
        if (advancedSettingsDropped_) {
            toggleAdvancedSettings();
        }
    });

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

    connect(ui->currentRegisteredIDBtn, &QPushButton::clicked, this, &SettingsWidget::regNameRegisteredSlot);

    // update banned accounts automatically
    connect(LRCInstance::getCurrentAccountInfo().contactModel.get(), &lrc::api::ContactModel::modelUpdated,
        this, &SettingsWidget::updateAndShowBannedContactsSlot);

    // update linked devices automatically
    QObject::connect(LRCInstance::getCurrentAccountInfo().deviceModel.get(), &lrc::api::NewDeviceModel::deviceUpdated,
        this, &SettingsWidget::updateAndShowDevicesSlot);

    // account settings setters {
    connect(ui->accountEnableCheckBox, &QCheckBox::stateChanged, this, &SettingsWidget::setAccEnableSlot);

    connect(ui->displayNameLineEdit, &QLineEdit::textChanged, [this](const QString& displayName) {
        LRCInstance::setCurrAccDisplayName(displayName.toStdString());
    }
    );

}
//
//void
//SettingsWidget::setScrollWidgetSize()
//{
//    ui->currentAccountSettingsScrollWidget->setFixedWidth(ui->)
//}