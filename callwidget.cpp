/***************************************************************************
 * Copyright (C) 2015-2018 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include "callwidget.h"
#include "ui_callwidget.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QComboBox>

#include <memory>

#include "qrencode.h"

//ERROR is defined in windows.h
#include "utils.h"
#undef ERROR
#undef interface

// lrc
#include "globalinstances.h"
#include "profilemodel.h"
#include "localprofilecollection.h"

// client
#include "wizarddialog.h"
#include "windowscontactbackend.h"
#include "contactpicker.h"
#include "globalsystemtray.h"
#include "conversationitemdelegate.h"
#include "imdelegate.h"
#include "pixbufmanipulator.h"
#include "settingskey.h"
#include "lrcinstance.h"
#include "messagemodel.h"

CallWidget::CallWidget(QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::CallWidget),
    menu_(new QMenu()),
    imDelegate_(new ImDelegate())
{
    ui->setupUi(this);

    using namespace lrc::api;

    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);

    QPixmap logo(":/images/logo-ring-standard-coul.png");
    ui->ringLogo->setPixmap(logo.scaledToHeight(100, Qt::SmoothTransformation));
    ui->ringLogo->setAlignment(Qt::AlignHCenter);

    ui->qrLabel->hide();

    videoRenderer_ = nullptr;

    // this line is not welcome here, and must be removed
    ProfileModel::instance().addCollection<LocalProfileCollection>(LoadOptions::FORCE_ENABLED);

    // select last used account if stored in registry
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (!accountList.empty()) {
        std::string accountIdToStartWith;
        QSettings settings;
        if (settings.contains(SettingsKey::selectedAccount)) {
            accountIdToStartWith = settings
                .value(SettingsKey::selectedAccount, true)
                .value<QString>()
                .toStdString();
            if (Utils::indexInVector(accountList, accountIdToStartWith) == -1) {
                accountIdToStartWith = accountList.at(0);
            }
        }
        else {
            accountIdToStartWith = accountList.at(0);
        }
        setSelectedAccount(accountIdToStartWith);
        // get account index and set the currentAccountComboBox
        auto index = Utils::indexInVector(accountList, accountIdToStartWith);
        if (index != -1) {
            ui->currentAccountComboBox->setCurrentIndex(index);
            ui->currentAccountComboBox->importLabelPhoto(index);
        }
    }

    //disable dropdown shadow on combobox
    ui->currentAccountComboBox->view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    // conversation list
    conversationItemDelegate_ = new ConversationItemDelegate();
    ui->smartList->setContextMenuPolicy(Qt::CustomContextMenu);

    // setup searchingfor mini spinner
    miniSpinner_ = new QMovie(":/images/waiting.gif");
    ui->spinnerLabel->setMovie(miniSpinner_);
    ui->spinnerLabel->hide();

    setupOutOfCallIM();

    // connections
    connect(ui->settingsButton, &QPushButton::clicked,
            this, &CallWidget::settingsButtonClicked);

    connect(ui->videoWidget, SIGNAL(setChatVisibility(bool)),
            ui->instantMessagingWidget, SLOT(setVisible(bool)));

    connect(ui->videoWidget, &VideoView::videoSettingsClicked,
            this, &CallWidget::settingsButtonClicked);

    connect(ui->buttonConversations, &QPushButton::clicked,
            this, &CallWidget::conversationsButtonClicked);

    connect(ui->buttonInvites, &QPushButton::clicked,
            this, &CallWidget::invitationsButtonClicked);

    connect(ui->smartList, &QListView::customContextMenuRequested,
            this, &CallWidget::slotCustomContextMenuRequested);

    connect(ui->smartList, &SmartListView::btnAcceptInviteClicked,
            this, &CallWidget::slotAcceptInviteClicked);

    connect(ui->smartList, &SmartListView::btnBlockInviteClicked,
            this, &CallWidget::slotBlockInviteClicked);

    connect(ui->smartList, &SmartListView::btnIgnoreInviteClicked,
            this, &CallWidget::slotIgnoreInviteClicked);

    connect(&LRCInstance::behaviorController(), &BehaviorController::showCallView,
            this, &CallWidget::slotShowCallView);

    connect(&LRCInstance::behaviorController(), &BehaviorController::showIncomingCallView,
            this, &CallWidget::slotShowIncomingCallView);

    connect(&LRCInstance::behaviorController(), &BehaviorController::showChatView,
            this, &CallWidget::slotShowChatView);

    connect(ui->currentAccountComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CallWidget::slotAccountChanged);

    connect(ui->sendContactRequestButton, &QPushButton::clicked,
            this, &CallWidget::on_sendContactRequestButton_clicked);

    connect(ui->btnAudioCall, &QPushButton::clicked,
            this, &CallWidget::on_sendContactRequestButton_clicked);

    connect(ui->btnVideoCall, &QPushButton::clicked,
            this, &CallWidget::on_sendContactRequestButton_clicked);

    // set first view to welcome view
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
}

CallWidget::~CallWidget()
{
    delete ui;
    delete menu_;
    delete imDelegate_;
    delete conversationItemDelegate_;
}

void
CallWidget::setupOutOfCallIM()
{
    ui->listMessageView->setItemDelegate(imDelegate_);
    ui->listMessageView->setContextMenuPolicy(Qt::ActionsContextMenu);

    auto copyAction = new QAction(tr("Copy"), this);
    ui->listMessageView->addAction(copyAction);

    connect(copyAction, &QAction::triggered, [=]() {
        auto idx = ui->listMessageView->currentIndex();
        if (idx.isValid()) {
            auto text = ui->listMessageView->model()->data(idx);
            QApplication::clipboard()->setText(text.value<QString>());
        }
    });
}

void
CallWidget::onIncomingMessage(const std::string& convUid,
                              uint64_t interactionId,
                              const lrc::api::interaction::Info& interaction)
{
    Q_UNUSED(interactionId);
    if (!QApplication::focusWidget()) {
        Utils::showSystemNotification(this,
            QString(tr("Message incoming from %1"))
            .arg(QString::fromStdString(interaction.body)));
    }
    updateConversationView(convUid);
    ui->conversationsFilterWidget->update();
}

void
CallWidget::setupSmartListContextMenu(const QPoint& pos)
{
    QPoint globalPos = ui->smartList->mapToGlobal(pos);
    auto index = ui->smartList->indexAt(pos);
    if (not index.isValid()) {
        return;
    }

    auto convModel = LRCInstance::getCurrentConversationModel();
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID))
        .value<QString>()
        .toStdString();
    auto conversation = Utils::getConversationFromUid(convUid, *convModel);
    auto contactUid = (*conversation).participants.at(0);
    auto contact = LRCInstance::getCurrentAccountInfo().contactModel.get()->getContact(contactUid);

    if (!Utils::isContactValid(contactUid, *convModel)) {
        return;
    }

    QMenu menu;

    // video call
    auto videoCallAction = new QAction(tr("Start video call"), this);
    menu.addAction(videoCallAction);
    connect(videoCallAction, &QAction::triggered,
        [this, convUid, conversation, convModel]() {
            convModel->placeCall(convUid);
            ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(convUid)));
            if (convUid != selectedConvUid()) {
                selectConversation(*conversation, *convModel);
            }
        });
    // audio call
    auto audioCallAction = new QAction(tr("Start audio call"), this);
    menu.addAction(audioCallAction);
    connect(audioCallAction, &QAction::triggered,
        [this, convUid, conversation, convModel]() {
            convModel->placeAudioOnlyCall(convUid);
            ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(convUid)));
            if (convUid != selectedConvUid()) {
                selectConversation(*conversation, *convModel);
            }
        });

    if (contact.profileInfo.type == lrc::api::profile::Type::RING) {
        // separator
        menu.addSeparator();

        // clear conversation
        auto clearConversationAction = new QAction(tr("Clear conversation"), this);
        menu.addAction(clearConversationAction);
        connect(clearConversationAction, &QAction::triggered,
            [convUid]() {
                LRCInstance::getCurrentConversationModel()->clearHistory(convUid);
            });
        // block contact
        auto blockContactAction = new QAction(tr("Block contact"), this);
        menu.addAction(blockContactAction);
        connect(blockContactAction, &QAction::triggered,
            [convUid]() {
                LRCInstance::getCurrentConversationModel()->removeConversation(convUid, true);
            });

        // separator
        menu.addSeparator();

        // copy number(infohash)
        auto copyNumberAction = new QAction(tr("Copy number"), this);
        menu.addAction(copyNumberAction);
        connect(copyNumberAction, &QAction::triggered,
            [contact]() {
                QApplication::clipboard()->setText(
                    QString::fromStdString(contact.profileInfo.uri)
                );
            });
    }
    smartListModel_->isContextMenuOpen_ = true;
    menu.exec(globalPos);
    smartListModel_->isContextMenuOpen_ = false;
}

void
CallWidget::setupQRCode(QString ringID)
{
    auto rcode = QRcode_encodeString(ringID.toStdString().c_str(),
                                     0, //Let the version be decided by libqrencode
                                     QR_ECLEVEL_L, // Lowest level of error correction
                                     QR_MODE_8, // 8-bit data mode
                                     1);
    if (not rcode) {
        qWarning() << "Failed to generate QR code: " << strerror(errno);
        return;
    }

    auto margin = 5;
    int qrwidth = rcode->width + margin * 2;
    QImage result(QSize(qrwidth, qrwidth), QImage::Format_Mono);
    QPainter painter;
    painter.begin(&result);
    painter.setClipRect(QRect(0, 0, qrwidth, qrwidth));
    painter.setPen(QPen(Qt::black, 0.1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    painter.setBrush(Qt::black);
    painter.fillRect(QRect(0, 0, qrwidth, qrwidth), Qt::white);
    unsigned char* p;
    p = rcode->data;
    for(int y = 0; y < rcode->width; y++) {
        unsigned char* row = (p + (y * rcode->width));
        for(int x = 0; x < rcode->width; x++) {
            if(*(row + x) & 0x1) {
                painter.drawRect(margin + x, margin + y, 1, 1);
            }
        }

    }
    painter.end();
    QRcode_free(rcode);
    ui->qrLabel->setPixmap(QPixmap::fromImage(result.scaled(QSize(qrSize_, qrSize_),
                           Qt::KeepAspectRatio)));
}

void
CallWidget::on_smartList_clicked(const QModelIndex& index)
{
    Q_UNUSED(index);
}

void
CallWidget::on_acceptButton_clicked()
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto callModel = LRCInstance::getCurrentCallModel();
    auto conversation = Utils::getConversationFromUid(selectedConvUid(), *convModel);
    callModel->accept(conversation->callId);
}

void
CallWidget::on_refuseButton_clicked()
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(selectedConvUid(), *convModel);
    LRCInstance::getCurrentCallModel()->hangUp(conversation->callId);
    showConversationView();
}

void
CallWidget::on_cancelButton_clicked()
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(selectedConvUid(), *convModel);
    LRCInstance::getCurrentCallModel()->hangUp(conversation->callId);
    showConversationView();
}

void
CallWidget::showConversationView()
{
    ui->stackedWidget->setCurrentWidget(ui->messagingPage);
    ui->imMessageEdit->clear();
    ui->imMessageEdit->setFocus();
    disconnect(imClickedConnection_);
    imClickedConnection_ = connect(ui->listMessageView, &QListView::clicked, [this](const QModelIndex& index) {
        auto urlList = index.data(static_cast<int>(media::TextRecording::Role::LinkList)).value<QList<QUrl>>();
        if (urlList.size() == 1) {
            QDesktopServices::openUrl(urlList.at(0));
        } else if (urlList.size()) {
            //TODO Handle multiple url in one message
        }
    });
}

void
CallWidget::selectSmartlistItem(const std::string & convUid)
{
    if (convUid.empty()) {
        return;
    }
    ui->smartList->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Deselect);
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(convUid, *convModel);
    if (conversation == convModel->allFilteredConversations().end()) {
        return;
    }
    auto contactURI = QString::fromStdString((*conversation).participants[0]);
    if (contactURI.isEmpty() ||
        convModel->owner.contactModel->getContact(contactURI.toStdString()).profileInfo.type == lrc::api::profile::Type::TEMPORARY) {
        return;
    }
    for (int row = 0; row < smartListModel_->rowCount(); row++) {
        QModelIndex index = smartListModel_->index(row);
        auto indexContactURI = index.data(SmartListModel::Role::URI).value<QString>();
        if (indexContactURI == contactURI) {
            ui->smartList->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
            return;
        }
    }
}

void
CallWidget::on_smartList_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    LRCInstance::getCurrentConversationModel()->placeCall(selectedConvUid());

    ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(selectedConvUid())));
}

QImage
CallWidget::imageForConv(const std::string& convUid)
{
    return Utils::conversationPhoto(convUid, LRCInstance::getCurrentAccountInfo());
}

const std::string&
CallWidget::selectedAccountId()
{
    return LRCInstance::getSelectedAccountId();
}

const std::string&
CallWidget::selectedConvUid()
{
    return LRCInstance::getSelectedConvUid();
}

void
CallWidget::smartListSelectionChanged(const QItemSelection  &selected, const QItemSelection  &deselected)
{
    Q_UNUSED(deselected);
    QModelIndexList indices = selected.indexes();

    if (indices.isEmpty()) {
        return;
    }

    auto selectedIndex = indices.at(0);

    if (not selectedIndex.isValid()) {
        return;
    }

    selectConversation(selectedIndex);
}

void
CallWidget::contactReqListCurrentChanged(const QModelIndex &currentIdx, const QModelIndex &previousIdx)
{
    Q_UNUSED(previousIdx)

    if (currentIdx.isValid()) {
        ui->contactRequestWidget->setCurrentContactRequest(currentIdx);
        ui->stackedWidget->setCurrentWidget(ui->contactRequestPage);
    } else {
        ui->contactRequestWidget->setCurrentContactRequest(QModelIndex());
        if (ui->stackedWidget->currentWidget() == ui->contactRequestPage)
            Utils::setStackWidget(ui->stackedWidget, ui->welcomePage);
    }
}

void
CallWidget::placeCall()
{
    if (ui->ringContactLineEdit->text().isEmpty())
        return;
    Call* c = CallModel::instance().dialingCall(PhoneDirectoryModel::instance().getNumber(ui->ringContactLineEdit->text()));
    c->performAction(Call::Action::ACCEPT);
    ui->ringContactLineEdit->clear();
    ui->callingPhoto->setPixmap(
                QPixmap::fromImage(
                    GlobalInstances::pixmapManipulator()
                    .callPhoto(c, QSize(130,130)).value<QImage>()));
}

void
CallWidget::conversationsButtonClicked()
{
    setConversationFilter(lrc::api::profile::Type::RING);
}

void
CallWidget::invitationsButtonClicked()
{
    setConversationFilter(lrc::api::profile::Type::PENDING);
}

void
CallWidget::settingsButtonClicked()
{
    emit NavigationRequested(ScreenEnum::ConfScreen);
}

void
CallWidget::processContactLineEdit()
{
    auto contactLineText = ui->ringContactLineEdit->text();
    setConversationFilter(contactLineText);
}

void
CallWidget::on_ringContactLineEdit_returnPressed()
{
    // select current temporary item and show conversation
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversations = convModel->allFilteredConversations();
    auto contactIsValid = Utils::isContactValid(conversations.at(0).participants.at(0), *convModel);
    if (!conversations.empty() && contactIsValid) {
        selectConversation(smartListModel_->index(0));
    }
}

void CallWidget::slotAcceptInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>();
    LRCInstance::getCurrentConversationModel()->makePermanent(convUid.toStdString());
}

void CallWidget::slotBlockInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>();
    LRCInstance::getCurrentConversationModel()->removeConversation(convUid.toStdString(), true);
}

void CallWidget::slotIgnoreInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>();
    LRCInstance::getCurrentConversationModel()->removeConversation(convUid.toStdString());
}

void CallWidget::slotCustomContextMenuRequested(const QPoint& pos)
{
    setupSmartListContextMenu(pos);
}

void CallWidget::slotAccountChanged(int index)
{
    auto accountList = LRCInstance::accountModel().getAccountList();
    setSelectedAccount(accountList.at(index));
}

void CallWidget::slotShowCallView(const std::string& accountId,
                                  const lrc::api::conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    Q_UNUSED(convInfo);
    qDebug() << "BehaviorController::showCallView";
    ui->stackedWidget->setCurrentWidget(ui->videoPage);
    hideMiniSpinner();
}

void CallWidget::slotShowIncomingCallView(const std::string& accountId,
                                          const lrc::api::conversation::Info& convInfo) {
    Q_UNUSED(accountId);
    qDebug() << "BehaviorController::showIncomingCallView";
    auto callModel = LRCInstance::getCurrentCallModel();
    auto call = callModel->getCall(convInfo.callId);
    if (call.isOutgoing) {
        miniSpinner_->start();
        ui->spinnerLabel->show();
        ui->stackedWidget->setCurrentWidget(ui->outboundCallPage);
    }
    else {
        selectSmartlistItem(convInfo.uid);
        auto selectedAccountId = LRCInstance::getCurrentAccountInfo().id;
        auto accountProperties = LRCInstance::accountModel().getAccountConfig(selectedAccountId);
        if (!accountProperties.autoAnswer) {
            ui->stackedWidget->setCurrentWidget(ui->callInvitePage);
        }
        else {
            ui->stackedWidget->setCurrentWidget(ui->videoPage);
        }
        if (!QApplication::focusWidget()) {
            auto formattedName = Utils::bestNameForConversation(convInfo, *LRCInstance::getCurrentConversationModel());
            Utils::showSystemNotification(this,
                QString(tr("Call incoming from %1")).arg(QString::fromStdString(formattedName)));
        }
    }

    if (!callModel->hasCall(convInfo.callId)) {
        return;
    }

    ui->videoWidget->pushRenderer(convInfo.callId);

    ui->instantMessagingWidget->setupCallMessaging(convInfo.callId, messageModel_.get());

    disconnect(selectedCallChanged_);
    selectedCallChanged_ = connect(
        callModel,
        &lrc::api::NewCallModel::callStatusChanged,
        [this, callModel](const std::string& callUid) {
            auto call = callModel->getCall(callUid);
            qDebug() << "NewCallModel::callStatusChanged: " << static_cast<int>(call.status);
        }
    );

    ui->callerPhoto->setPixmap(QPixmap::fromImage(imageForConv(selectedConvUid())));
}

void CallWidget::slotShowChatView(const std::string& accountId,
                      const lrc::api::conversation::Info& convInfo) {
    Q_UNUSED(accountId);
    Q_UNUSED(convInfo);
    qDebug() << "BehaviorController::showChatView";
    showConversationView();
}

void
CallWidget::currentAccountChanged(const QModelIndex &current)
{
    if (!current.isValid()) {
        ui->selectBar->hide();
        ui->ringIdLabel->setText("");
        return;
    }
    if (ui->selectBar->isHidden()){
        ui->selectBar->show();
    }

    auto accountId = current.data(static_cast<int>(AccountListModel::Role::ID)).value<QString>().toStdString();

    setSelectedAccount(accountId);
}

void
CallWidget::setSelectedAccount(const std::string& accountId)
{
    LRCInstance::setSelectedAccountId(accountId);

    // First, we get back to the welcome view (except if in call)
    if (ui->stackedWidget->currentWidget() != ui->videoPage &&
        ui->stackedWidget->currentWidget() != ui->welcomePage) {
        Utils::setStackWidget(ui->stackedWidget, ui->welcomePage);
    }

    // We setup the ringIdLabel and the QRCode
    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(accountId);
    auto id = accountInfo.registeredName.empty() ? accountInfo.profileInfo.uri : accountInfo.registeredName;
    auto isRingAccount = accountInfo.profileInfo.type == lrc::api::profile::Type::RING;
    if (isRingAccount) {
        ui->ringIdLabel->setText(QString::fromStdString(id));
        setupQRCode(QString::fromStdString(accountInfo.profileInfo.uri));
    }

    updateSmartList();
    currentTypeFilter_ = accountInfo.profileInfo.type;
    LRCInstance::getCurrentConversationModel()->setFilter(accountInfo.profileInfo.type);
    updateConversationsFilterWidget();
    connectConversationModel();
}

void CallWidget::setConversationFilter(lrc::api::profile::Type filter)
{
    if (currentTypeFilter_ == filter) {
        return;
    }
    currentTypeFilter_ = filter;
    LRCInstance::getCurrentConversationModel()->setFilter(currentTypeFilter_);
}

void CallWidget::updateConversationsFilterWidget()
{
    auto invites = LRCInstance::getCurrentAccountInfo().contactModel->pendingRequestCount();
    if (invites == 0 && currentTypeFilter_ == lrc::api::profile::Type::PENDING) {
        currentTypeFilter_ = lrc::api::profile::Type::RING;
        LRCInstance::getCurrentConversationModel()->setFilter(currentTypeFilter_);
    }
    ui->conversationsFilterWidget->setVisible(invites);
    ui->missingButtonsDummyWidget->setVisible(!invites);
}

void CallWidget::setConversationFilter(const QString & filter)
{
    LRCInstance::getCurrentConversationModel()->setFilter(filter.toStdString());
}

void
CallWidget::showIMOutOfCall(const QModelIndex& nodeIdx)
{
    QString displayName = nodeIdx.data(static_cast<int>(SmartListModel::Role::DisplayName)).toString();
    QString displayId = nodeIdx.data(static_cast<int>(SmartListModel::Role::DisplayID)).toString();
    QString contactURI = nodeIdx.data(static_cast<int>(SmartListModel::Role::URI)).toString();

    bool isContact = false;
    auto selectedAccountId = LRCInstance::getSelectedAccountId();
    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(selectedAccountId);
    bool isRINGAccount = accountInfo.profileInfo.type == lrc::api::profile::Type::RING;
    try {
        accountInfo.contactModel->getContact(contactURI.toStdString());
        isContact = true;
    }
    catch (...) {}

    if (!isRINGAccount){
        ui->imMessageEdit->setPlaceholderText("No messaging possible out of call (SIP) ");
        ui->imMessageEdit->setEnabled(false);
        ui->sendIMButton->hide();
    } else {
        ui->imMessageEdit->setPlaceholderText("Type your message here");
        ui->imMessageEdit->setEnabled(true);
        ui->sendIMButton->show();
    }

    ui->imNameLabel->setText(QString(tr("%1", "%1 is the contact username"))
                                    .arg(displayName));

    if (isRINGAccount && displayName != displayId){
        ui->imIdLabel->show();
        ui->imIdLabel->setText(QString(tr("%1", "%1 is the contact unique identifier"))
                                    .arg(displayId));
    } else {
        ui->imIdLabel->hide();
    }

    bool shouldShowSendContactRequestBtn = !isContact && isRINGAccount;
    ui->sendContactRequestButton->setVisible(shouldShowSendContactRequestBtn);

    showConversationView();

    auto currentConversation = Utils::getConversationFromUid(selectedConvUid(),
                                                             *LRCInstance::getCurrentConversationModel());
    messageModel_.reset(new MessageModel(*currentConversation, accountInfo, this->parent()));
    ui->listMessageView->setModel(messageModel_.get());
    ui->listMessageView->scrollToBottom();
}

void
CallWidget::on_sendIMButton_clicked()
{
    auto msg = ui->imMessageEdit->text();
    if (msg.trimmed().isEmpty()) {
        return;
    }
    ui->imMessageEdit->clear();
    try {
        LRCInstance::getCurrentConversationModel()->sendMessage(selectedConvUid(), msg.toStdString());
    } catch (...) {
        qDebug() << "exception when sending message";
    }
}

void
CallWidget::on_imMessageEdit_returnPressed()
{
    on_sendIMButton_clicked();
}

void
CallWidget::slotAccountMessageReceived(const QMap<QString,QString> message,
                                       ContactMethod* cm,
                                       media::Media::Direction dir)
{
    Q_UNUSED(message)
    Q_UNUSED(dir)

    ui->listMessageView->scrollToBottom();
    cm->textRecording()->setAllRead();
}

void
CallWidget::on_ringContactLineEdit_textChanged(const QString& text)
{
    Q_UNUSED(text);
    processContactLineEdit();
}

void
CallWidget::backToWelcomePage()
{
    deselectConversation();
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
}

void
CallWidget::hideMiniSpinner()
{
    if(ui->spinnerLabel->isVisible()){
        miniSpinner_->stop();
        ui->spinnerLabel->hide();
    }
}

void
CallWidget::on_imBackButton_clicked()
{
    backToWelcomePage();
}

void
CallWidget::on_qrButton_toggled(bool checked)
{
    ui->qrLabel->setVisible(checked);
}

void
CallWidget::on_shareButton_clicked()
{
    Utils::InvokeMailto(tr("Contact me on Ring"), tr("My RingId is : ") + ui->ringIdLabel->text());
}

void
CallWidget::on_sendContactRequestButton_clicked()
{
    LRCInstance::getCurrentConversationModel()->makePermanent(selectedConvUid());
}

void
CallWidget::on_pendingCRBackButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
}

void
CallWidget::on_btnAudioCall_clicked()
{
    auto convUid = LRCInstance::getSelectedConvUid();
    LRCInstance::getCurrentConversationModel()->placeAudioOnlyCall(convUid);
    ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(convUid)));
}

void
CallWidget::on_btnVideoCall_clicked()
{
    auto convUid = LRCInstance::getSelectedConvUid();
    LRCInstance::getCurrentConversationModel()->placeCall(convUid);
    ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(convUid)));
}

bool
CallWidget::connectConversationModel()
{
    auto currentConversationModel = LRCInstance::getCurrentAccountInfo().conversationModel.get();

    if (ui->smartList->selectionModel()) {
        ui->smartList->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Deselect);
    }

    QObject::disconnect(modelSortedConnection_);
    QObject::disconnect(modelUpdatedConnection_);
    QObject::disconnect(filterChangedConnection_);
    QObject::disconnect(newConversationConnection_);
    QObject::disconnect(conversationRemovedConnection_);
    QObject::disconnect(conversationClearedConnection);
    QObject::disconnect(interactionStatusUpdatedConnection_);
    QObject::disconnect(newInteractionConnection_);

    modelSortedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::modelSorted,
        [this]() {
            updateConversationsFilterWidget();
            selectSmartlistItem(selectedConvUid());
            ui->smartList->update();
        });
    modelUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationUpdated,
        [this](const std::string& convUid) {
            Q_UNUSED(convUid);
            ui->smartList->update();
        });
    filterChangedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::filterChanged,
        [this]() {
            updateSmartList();
            updateConversationsFilterWidget();
            ui->smartList->update();
        });
    newConversationConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::newConversation,
        [this](const std::string& convUid) {
            updateSmartList();
            updateConversationForNewContact(convUid);
            ui->conversationsFilterWidget->update();
        });
    conversationRemovedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationRemoved,
        [this]() {
            backToWelcomePage();
        });
    conversationClearedConnection = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationCleared,
        [this](const std::string& convUid) {
            updateConversationView(convUid);
            // if currently selected,
            // switch to welcome screen (deselecting current smartlist item )
            if (convUid != selectedConvUid()) {
                return;
            }
            backToWelcomePage();
        });
    interactionStatusUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::interactionStatusUpdated,
        [this](const std::string& convUid) {
            if (convUid != selectedConvUid()) {
                return;
            }
            updateConversationView(convUid);
        });
    newInteractionConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::newInteraction,
        [this](const std::string& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction) {
            onIncomingMessage(convUid, interactionId, interaction);
        });
    currentConversationModel->setFilter("");
    // clear search field
    ui->ringContactLineEdit->setText("");
    return true;
}

void
CallWidget::updateConversationView(const std::string& convUid)
{
    if (convUid != selectedConvUid()) {
        return;
    }

    auto& currentAccountInfo = LRCInstance::getCurrentAccountInfo();
    auto currentConversationModel = currentAccountInfo.conversationModel.get();
    currentConversationModel->clearUnreadInteractions(convUid);
    ui->conversationsFilterWidget->update();
    auto currentConversation = Utils::getConversationFromUid(convUid,
                                                             *currentConversationModel);
    if (currentConversation == currentConversationModel->allFilteredConversations().end()) {
        return;
    }
    messageModel_.reset(new MessageModel(*currentConversation, currentAccountInfo, this->parent()));
    ui->listMessageView->setModel(messageModel_.get());
    ui->listMessageView->scrollToBottom();
}

void
CallWidget::selectConversation(const QModelIndex& index)
{
    auto currentConversationModel = LRCInstance::getCurrentConversationModel();

    if (currentConversationModel == nullptr || !index.isValid()) {
        return;
    }

    const auto item = currentConversationModel->filteredConversation(index.row());

    if (selectConversation(item, *currentConversationModel)) {
        auto convUid = selectedConvUid();
        if (!lastConvUid_.compare(convUid)) {
            return;
        }
        lastConvUid_.assign(convUid);
        auto currentConversationModel = LRCInstance::getCurrentConversationModel();
        auto callModel = LRCInstance::getCurrentCallModel();
        auto conversation = Utils::getConversationFromUid(convUid, *currentConversationModel);
        const auto item = currentConversationModel->filteredConversation(index.row());
        if (callModel->hasCall(conversation->callId)) {
            ui->stackedWidget->setCurrentWidget(ui->videoPage);
        }
        else {
            showIMOutOfCall(index);
        }
    }
}

bool
CallWidget::selectConversation( const lrc::api::conversation::Info& item,
                                lrc::api::ConversationModel& convModel)
{
    if (selectedConvUid() == item.uid) {
        return false;
    } else if (item.participants.size() > 0) {
        convModel.selectConversation(item.uid);
        LRCInstance::setSelectedConvId(item.uid);
        convModel.clearUnreadInteractions(item.uid);
        ui->conversationsFilterWidget->update();
        return true;
    }
}

void
CallWidget::deselectConversation()
{
    auto currentConversationModel = LRCInstance::getCurrentConversationModel();

    if (currentConversationModel == nullptr) {
        return;
    }

    currentConversationModel->selectConversation("");
    LRCInstance::setSelectedConvId("");

    ui->smartList->selectionModel()->clear();
    disconnect(imConnection_);
}

void
CallWidget::updateConversationForNewContact(const std::string& convUid)
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    if (convModel == nullptr) {
        return;
    }
    ui->ringContactLineEdit->setText("");
    auto selectedUid = selectedConvUid();
    auto it = Utils::getConversationFromUid(convUid, *convModel);
    if (it != convModel->allFilteredConversations().end()) {
        try {
            auto contact = convModel->owner.contactModel->getContact(it->participants[0]);
            if (!contact.profileInfo.uri.empty() && contact.profileInfo.uri.compare(selectedUid) == 0) {
                LRCInstance::setSelectedConvId(convUid);
                convModel->selectConversation(convUid);
            }
        } catch (...) {
            return;
        }
    }
}

void
CallWidget::updateSmartList()
{
    auto& currentAccountInfo = LRCInstance::getCurrentAccountInfo();
    smartListModel_.reset(new SmartListModel(currentAccountInfo, this->parent()));
    ui->smartList->setModel(smartListModel_.get());
    ui->smartList->setItemDelegate(new ConversationItemDelegate());

    // smartlist selection
    QObject::disconnect(smartlistSelectionConnection_);
    smartlistSelectionConnection_ = connect(ui->smartList->selectionModel(),
        SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this,
        SLOT(smartListSelectionChanged(QItemSelection, QItemSelection)));
}

void
CallWidget::update()
{
    updateSmartList();
    updateConversationsFilterWidget();
    connectConversationModel();
}
