/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

#include "currentaccountcombobox.h"

#include "accountitemdelegate.h"
#include "pixbufmanipulator.h"
#include "utils.h"
#include "ringthemeutils.h"
#include "lrcinstance.h"

#include <QPixmap>
#include <QMessageBox>
#include <QMouseEvent>

#undef REGISTERED

CurrentAccountComboBox::CurrentAccountComboBox(QWidget* parent)
{
    Q_UNUSED(parent);

    setMouseTracking(true);
    gearLabel_.setMouseTracking(true);
    voicemailButton_.setMouseTracking(true);
    voicemailButton_.setNeedToShowNotify(false);
    voicemailButton_.setEnabled(false);

    accountListUpdate();
    accountItemDelegate_ = new AccountItemDelegate(this);
    this->setItemDelegate(accountItemDelegate_);

    // combobox index changed and so must the avatar
    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](const int& index) {
                setCurrentIndex(index);
            });

    // account added to combobox
    connect(&LRCInstance::accountModel(),
            &lrc::api::NewAccountModel::accountAdded,
            [this](const std::string& accountId) {
                auto accountList = LRCInstance::accountModel().getAccountList();
                auto it = std::find(accountList.begin(), accountList.end(), accountId);
                if (it != accountList.end()) {
                    this->setCurrentIndex(std::distance(accountList.begin(), it));
                }
            });

    // voicemail received
    if (LRCInstance::accountModel().getAccountList().size()) {
        connectVoiceMail();
    }

    connect(&LRCInstance::instance(),
            &LRCInstance::accountListChanged,
            this, &CurrentAccountComboBox::connectVoiceMail,
            Qt::UniqueConnection);

    connect(&voicemailButton_, &QAbstractButton::clicked,
            [this] {
                auto currentAccountId = LRCInstance::accountModel().getAccountList()[currentIndex()];
                auto &accInfo = LRCInstance::accountModel().getAccountInfo(currentAccountId);
                auto conversationModel = accInfo.conversationModel.get();
                auto confProps = LRCInstance::accountModel().getAccountConfig(currentAccountId);

                auto possibleConv = LRCInstance::getConversationFromPeerUri(confProps.mailbox);
                if (possibleConv.uid.empty()) {
                    if (confProps.mailbox.empty()) {
                        QMessageBox::information(0, "Voicemail", "Voicemail dial code is empty");
                    }
                    // construct new contact
                    lrc::api::profile::Type type{ lrc::api::profile::Type::SIP };
                    lrc::api::contact::Info contactInfo{ {{confProps.mailbox}, {} , {"Voicemail"}, type }, {} };

                    Utils::oneShotConnect(this, &CurrentAccountComboBox::placeAudioOnlyCall,
                        [this, conversationModel] (const std::string& convUid) {
                            conversationModel->placeAudioOnlyCall(convUid);
                        });
                    accInfo.contactModel->addContact(contactInfo);
                } else {
                    // contact existed
                    conversationModel->placeAudioOnlyCall(possibleConv.uid);
                }
            });

    gearLabel_.setPixmap(QPixmap(":/images/icons/round-settings-24px.svg"));
    gearLabel_.setParent(this);
    gearLabel_.setStyleSheet("background: transparent;");
    setupSettingsButton();

    voicemailButton_.setIcon(QIcon(QPixmap(":/images/icons/ic_voicemail_black_24dp_2x_.png")));
    voicemailButton_.setParent(this);
    voicemailButton_.setStyleSheet("background: transparent;");
    setupVoicemailButton();
}

CurrentAccountComboBox::~CurrentAccountComboBox()
{
    delete accountItemDelegate_;
}

void
CurrentAccountComboBox::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPoint avatarTopLeft(16, 8);
    QPainter painter(this);
    painter.setRenderHints((QPainter::Antialiasing | QPainter::TextAntialiasing), true);

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter);

    // define and set the two fonts
    QFont fontPrimary = QFont(QStringLiteral("Segoe UI Emoji"));
    QFont fontSecondary = painter.font();
    fontPrimary.setWeight(QFont::ExtraLight);
    auto scalingRatio = Utils::getCurrentScalingRatio();
    fontPrimary.setPointSize(scalingRatio > 1.0 ? 10 : 11);
    fontSecondary.setPointSize(scalingRatio > 1.0 ? 9 : 10);

    QFontMetrics fontMetricPrimary(fontPrimary);
    QFontMetrics fontMetricSecondary(fontSecondary);

    painter.drawPixmap(avatarTopLeft, currentAccountAvatarImage_);

    auto& accInfo = LRCInstance::getCurrentAccountInfo();

    // fill in presence indicator if account is registered
    auto accountStatus = accInfo.status;
    if (accountStatus == lrc::api::account::Status::REGISTERED) {
        // paint the presence indicator circle
        QPainterPath outerCircle, innerCircle;
        QPointF presenceCenter(avatarSize_ + avatarTopLeft.x() - 9, avatarSize_ - 3);
        qreal outerCircleRadius = avatarSize_ / 6;
        qreal innerCircleRadius = outerCircleRadius * 0.75;
        outerCircle.addEllipse(presenceCenter, outerCircleRadius, outerCircleRadius);
        innerCircle.addEllipse(presenceCenter, innerCircleRadius, innerCircleRadius);
        painter.fillPath(outerCircle, Qt::white);
        painter.fillPath(innerCircle, RingTheme::presenceGreen_);
    }

    QRect comboBoxRect(
        avatarSize_ + avatarTopLeft.x() + 10 + 2,
        8,
        this->width() - avatarSize_,
        avatarSize_ - 10); // [screen awareness]

    // write primary and secondary account identifiers to combobox label
    QString primaryAccountID = QString::fromStdString(Utils::bestNameForAccount(accInfo));
    painter.setFont(fontPrimary);
    painter.setPen(RingTheme::lightBlack_);
    primaryAccountID = fontMetricPrimary.elidedText(primaryAccountID, Qt::ElideRight,
        comboBoxRect.width() - elidConst - (popupPresent ? 0 : 2 * gearSize_ + 2 * voicemailSize_));
    painter.drawText(comboBoxRect, Qt::AlignLeft, primaryAccountID);

    QString secondaryAccountID = QString::fromStdString(Utils::secondBestNameForAccount(accInfo));
    secondaryAccountID = fontMetricSecondary.elidedText(secondaryAccountID, Qt::ElideRight,
                                                        comboBoxRect.width() - elidConst - 2 - (popupPresent ? 0 : 2 * gearSize_ + 2 * voicemailSize_)); // [screen awareness]

    if (secondaryAccountID.length()) { // if secondary accound id exists
        painter.setFont(fontSecondary);
        painter.setPen(Qt::lightGray);
        painter.drawText(comboBoxRect, (Qt::AlignBottom | Qt::AlignLeft), secondaryAccountID);
    }

    if (QWidget* popupWidget = this->findChild<QFrame*>()) {
        popupWidget->setStyleSheet("QFrame{border: 0px;border-bottom: 1px solid #f0f0f0;border-right: 1px solid #f0f0f0; }");
    }
}

void CurrentAccountComboBox::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    setupSettingsButton();
    setupVoicemailButton();
}

void
CurrentAccountComboBox::setupSettingsButton()
{
    gearPoint_.setX(this->width() - gearSize_ - 5 * gearBorder_ - 1);
    gearPoint_.setY(this->height() / 2 - gearLabel_.height() / 2 - 2 * gearBorder_ + 8);
    gearLabel_.setGeometry(
        gearPoint_.x(), gearPoint_.y(),
        gearSize_ + 2 * gearBorder_,
        gearSize_ + 2 * gearBorder_);
    gearLabel_.setMargin(gearBorder_);
}

void
CurrentAccountComboBox::setupVoicemailButton()
{
    voicemailPoint_.setX(this->width() - gearSize_ - voicemailSize_ - 6 * voicemailBorder_ - 5 * gearBorder_ - 1);
    voicemailPoint_.setY(this->height() / 2 - voicemailButton_.height() / 2 - 2 * voicemailBorder_ + 8);
    voicemailButton_.setGeometry(
        voicemailPoint_.x(), voicemailPoint_.y(),
        voicemailSize_ + 2 * voicemailBorder_,
        voicemailSize_ + 2 * voicemailBorder_);
}

void
CurrentAccountComboBox::connectVoiceMail()
{
    connect(LRCInstance::getCurrentCallModel(), &lrc::api::NewCallModel::voiceMailNotify,
        [this](const std::string& accountId, int newCount, int oldCount, int urgentCount) {
            Q_UNUSED(urgentCount);
            voicemailMap_[accountId] = std::make_pair(newCount, oldCount);
            if (LRCInstance::accountModel().getAccountList()[currentIndex()] == accountId) {
                if (newCount == 0 && oldCount == 0) {
                    voicemailButton_.setNeedToShowNotify(false);
                    voicemailButton_.setEnabled(false);
                    return;
                }
                if (newCount == 0) {
                    voicemailButton_.setNotifyNumber(oldCount);
                    voicemailButton_.setGreyStyleNotification(true);
                } else {
                    voicemailButton_.setNotifyNumber(newCount);
                    voicemailButton_.setGreyStyleNotification(false);
                }
                voicemailButton_.setNeedToShowNotify(true);
                voicemailButton_.setEnabled(true);
            }
        });
}

// import account background account pixmap and scale pixmap to fit in label
void
CurrentAccountComboBox::importLabelPhoto(int index)
{
    currentAccountAvatarImage_ = accountListModel_->data(
            accountListModel_->index(index, 0),
            AccountListModel::Role::Picture)
        .value<QPixmap>()
        .scaledToHeight(avatarSize_ - 4, Qt::SmoothTransformation);
}

void
CurrentAccountComboBox::setCurrentIndex(int index)
{
    auto accountListSize = LRCInstance::accountModel().getAccountList().size();

    if (index == accountListSize) {
        emit newAccountClicked();
    } else if (index < accountListSize) {
        importLabelPhoto(index);
        QComboBox::setCurrentIndex(index);

        auto accountId = LRCInstance::accountModel().getAccountList()[index];
        auto& info = LRCInstance::accountModel().getAccountInfo(accountId);
        if (!(info.profileInfo.type == lrc::api::profile::Type::SIP)) {
            voicemailButton_.setNeedToShowNotify(false);
            voicemailButton_.setEnabled(false);
        } else {
            if (voicemailMap_.find(accountId) != voicemailMap_.end()) {
                int newVoiceMail = voicemailMap_[accountId].first;
                int oldVoiceMail = voicemailMap_[accountId].second;

                if (newVoiceMail == 0 && oldVoiceMail == 0) {
                    voicemailButton_.setNeedToShowNotify(false);
                    voicemailButton_.setEnabled(false);
                    return;
                }
                if (newVoiceMail == 0) {
                    voicemailButton_.setNotifyNumber(oldVoiceMail);
                    voicemailButton_.setGreyStyleNotification(true);
                } else {
                    voicemailButton_.setNotifyNumber(newVoiceMail);
                    voicemailButton_.setGreyStyleNotification(false);
                }
                voicemailButton_.setNeedToShowNotify(true);
                voicemailButton_.setEnabled(true);
                return;
            }
            voicemailButton_.setNeedToShowNotify(false);
            voicemailButton_.setEnabled(false);
        }
    }
}

void
CurrentAccountComboBox::accountListUpdate()
{
    accountListModel_.reset(new AccountListModel());
    this->setModel(accountListModel_.get());
}

// if gearLabel is clicked
void
CurrentAccountComboBox::mousePressEvent(QMouseEvent* mouseEvent)
{
    if (gearLabel_.frameGeometry().contains(mouseEvent->localPos().toPoint())) {
        emit settingsButtonClicked();
    } else {
        QComboBox::mousePressEvent(mouseEvent);
    }
}

// if gear label is hovered over
void
CurrentAccountComboBox::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    if (gearLabel_.frameGeometry().contains(mouseEvent->x(), mouseEvent->y())) {
        QComboBox::mouseMoveEvent(mouseEvent);
        gearLabel_.setStyleSheet("background: rgb(237, 237, 237); border-width: 0px; border-radius: 15px;");
        return;
    } else if (voicemailButton_.geometry().contains(mouseEvent->localPos().toPoint())) {
        QComboBox::mouseMoveEvent(mouseEvent);
        voicemailButton_.setStyleSheet("background: rgb(237, 237, 237); border-width: 0px; border-radius: 15px;");
        return;
    }

    voicemailButton_.setStyleSheet("background: transparent;");
    gearLabel_.setStyleSheet("background: transparent;");
}

void
CurrentAccountComboBox::showPopup()
{
    voicemailButton_.setNeedToShowNotify(false);
    voicemailButton_.setEnabled(false);
    gearLabel_.hide();
    popupPresent = true;
    QComboBox::showPopup();
}

void
CurrentAccountComboBox::hidePopup()
{
    auto accountId { LRCInstance::getCurrAccId() };
    auto& info = LRCInstance::accountModel().getAccountInfo(accountId);
    if (info.profileInfo.type == lrc::api::profile::Type::SIP
        && voicemailMap_.find(accountId) != voicemailMap_.end()) {
        voicemailButton_.setNeedToShowNotify(true);
        voicemailButton_.setEnabled(true);
    }
    gearLabel_.show();
    popupPresent = false;
    QComboBox::hidePopup();
}

void
CurrentAccountComboBox::leaveEvent(QEvent* event)
{
    voicemailButton_.setStyleSheet("background: transparent;");
    gearLabel_.setStyleSheet("background: transparent;");
    QComboBox::leaveEvent(event);
}

void
CurrentAccountComboBox::updateComboBoxDisplay()
{
    auto currAccIndex = LRCInstance::getCurrentAccountIndex();
    if (currAccIndex != currentIndex())
        setCurrentIndex(currAccIndex);
    importLabelPhoto(currAccIndex);
}
