/***************************************************************************
 * Copyright (C) 2015-2018 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "callwidget.h"
#include "ui_callwidget.h"

#include <QScrollBar>
#include <QClipboard>
#include <QDesktopServices>
#include <QComboBox>
#include <QWebEngineScript>

#include <algorithm>
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
#include "globalsystemtray.h"
#include "conversationitemdelegate.h"
#include "pixbufmanipulator.h"
#include "settingskey.h"
#include "lrcinstance.h"
#include "animationhelpers.h"
#include "ringthemeutils.h"

CallWidget::CallWidget(QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::CallWidget),
    menu_(new QMenu())
{
    ui->setupUi(this);

    using namespace lrc::api;

    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);

    QPixmap logo(":/images/logo-jami-standard-coul.png");
    ui->ringLogo->setPixmap(logo.scaledToHeight(100, Qt::SmoothTransformation));
    ui->ringLogo->setAlignment(Qt::AlignHCenter);

    ui->qrLabel->hide();

    videoRenderer_ = nullptr;

    // this line is not welcome here, and must be removed
    ProfileModel::instance().addCollection<LocalProfileCollection>(LoadOptions::FORCE_ENABLED);

    QSettings settings;

    // select last used account if stored in registry
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (!accountList.empty()) {
        std::string accountIdToStartWith;
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
        }
    }

    if (settings.contains(SettingsKey::mainSplitterState)) {
        auto splitterStates = settings.value(SettingsKey::mainSplitterState).toByteArray();
        ui->mainActivitySplitter->restoreState(splitterStates);
    }

    ui->mainActivitySplitter->setCollapsible(0, false);
    ui->mainActivitySplitter->setCollapsible(1, false);
    ui->splitter->setCollapsible(0, false);
    ui->splitter->setCollapsible(1, false);

    //disable dropdown shadow on combobox
    ui->currentAccountComboBox->view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    // conversation list
    ui->smartList->setContextMenuPolicy(Qt::CustomContextMenu);

    // setup searchingfor mini spinner
    miniSpinner_ = new QMovie(":/images/waiting.gif");
    ui->spinnerLabel->setMovie(miniSpinner_);
    ui->spinnerLabel->hide();

    // connections
    connect(ui->currentAccountComboBox, &CurrentAccountComboBox::settingsButtonClicked,
            this, &CallWidget::settingsButtonClicked);

    connect(ui->currentAccountComboBox, &CurrentAccountComboBox::newAccountClicked,
        [this]() {
            emit NavigationRequested(ScreenEnum::WizardScreen);
        });

    connect(ui->videoWidget, &VideoView::setChatVisibility,
        [this](bool visible) {
            if (visible) {
                ui->messagesWidget->show();
            } else {
                ui->messagesWidget->hide();
            }
        });

    connect(ui->mainActivitySplitter, &QSplitter::splitterMoved,
        [this](int pos, int index) {
            Q_UNUSED(index);
            Q_UNUSED(pos);
            QSettings settings;
            settings.setValue(SettingsKey::mainSplitterState, ui->mainActivitySplitter->saveState());
        });

    connect(ui->videoWidget, &VideoView::videoSettingsClicked,
            this, &CallWidget::settingsButtonClicked);

    connect(ui->videoWidget, &VideoView::toggleFullScreenClicked,
        this, &CallWidget::slotToggleFullScreenClicked);

    connect(ui->videoWidget, &VideoView::closing,
        this, &CallWidget::slotVideoViewDestroyed);

    connect(ui->btnConversations, &QPushButton::clicked,
            this, &CallWidget::conversationsButtonClicked);

    connect(ui->btnInvites, &QPushButton::clicked,
            this, &CallWidget::invitationsButtonClicked);

    connect(ui->smartList, &QTreeView::customContextMenuRequested,
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

    connect(ui->currentAccountComboBox, QOverload<int>::of(&CurrentAccountComboBox::currentIndexChanged),
        [this] {
            ui->btnConversations->setChecked(true);
            ui->btnInvites->setChecked(false);
        });

    connect(ui->messageView, &MessageWebView::conversationRemoved,
        [this] {
            backToWelcomePage();
        });

    // set first view to welcome view
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    ui->btnConversations->setChecked(true);

    // chat view
    ui->messageView->buildView();

    // hide the call stack
    ui->callStackWidget->hide();

    ui->containerWidget->setVisible(false);
}

CallWidget::~CallWidget()
{
    delete ui;
    delete menu_;
}

void
CallWidget::navigated(bool to)
{
    ui->containerWidget->setVisible(to);
    if (to) {
        updateSmartList();
        connectConversationModel();
        try {
            auto accountList = LRCInstance::accountModel().getAccountList();
            if (accountList.size() == 1) {
                auto index = Utils::indexInVector(accountList, LRCInstance::getCurrAccId());
                if (index != -1) {
                    slotAccountChanged(index);
                }
            }
        } catch (...) {}
        ui->currentAccountComboBox->updateComboBoxDisplay();
    } else {
        QObject::disconnect(smartlistSelectionConnection_);
        smartListModel_.reset(nullptr);
    }
}

void CallWidget::updateCustomUI()
{
}

int
CallWidget::getLeftPanelWidth()
{
    return ui->currentAccountComboBox->width();
}

void
CallWidget::onIncomingMessage(const std::string& convUid,
                              uint64_t interactionId,
                              const lrc::api::interaction::Info& interaction)
{
    Q_UNUSED(interactionId);
    if (!QApplication::focusWidget()) {
        auto convModel = LRCInstance::getCurrentConversationModel();
        auto conversation = Utils::getConversationFromUid(convUid, *convModel);
        auto bestName = Utils::bestNameForConversation(*conversation, *convModel);
        Utils::showSystemNotification(this,
            QString(tr("Message incoming from %1"))
            .arg(QString::fromStdString(bestName)));
    }
    updateConversationsFilterWidget();
    if (convUid != LRCInstance::getSelectedConvUid()) {
        return;
    }

    auto convModel = LRCInstance::getCurrentConversationModel();
    convModel->clearUnreadInteractions(convUid);
    auto currentConversation = Utils::getConversationFromUid(convUid, *convModel);
    if (currentConversation == convModel->allFilteredConversations().end()) {
        return;
    }
    ui->messageView->printNewInteraction(*convModel, interactionId, interaction);
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
            if (convUid != LRCInstance::getSelectedConvUid()) {
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
            if (convUid != LRCInstance::getSelectedConvUid()) {
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
        // remove contact
        auto removeContactAction = new QAction(tr("Remove contact"), this);
        menu.addAction(removeContactAction);
        connect(removeContactAction, &QAction::triggered,
            [convUid]() {
                LRCInstance::getCurrentConversationModel()->removeConversation(convUid, false);
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
    smartListModel_->isContextMenuOpen = true;
    menu.exec(globalPos);
    smartListModel_->isContextMenuOpen = false;
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
    auto conversation = Utils::getConversationFromUid(LRCInstance::getSelectedConvUid(), *convModel);
    callModel->accept(conversation->callId);
}

void
CallWidget::on_refuseButton_clicked()
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(LRCInstance::getSelectedConvUid(), *convModel);
    LRCInstance::getCurrentCallModel()->hangUp(conversation->callId);
    showConversationView();
}

void
CallWidget::on_cancelButton_clicked()
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(LRCInstance::getSelectedConvUid(), *convModel);
    LRCInstance::getCurrentCallModel()->hangUp(conversation->callId);
    showConversationView();
}

void
CallWidget::showConversationView()
{
    ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
    ui->messageView->setFocus();
    if (ui->messagesWidget->isHidden()) {
        ui->messagesWidget->show();
    }
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

    LRCInstance::getCurrentConversationModel()->placeCall(LRCInstance::getSelectedConvUid());

    ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(LRCInstance::getSelectedConvUid())));
}

QImage
CallWidget::imageForConv(const std::string& convUid)
{
    return Utils::conversationPhoto(convUid, LRCInstance::getCurrentAccountInfo());
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
CallWidget::placeCall()
{
    if (ui->ringContactLineEdit->text().isEmpty())
        return;
    Call* c = CallModel::instance().dialingCall(PhoneDirectoryModel::instance().getNumber(ui->ringContactLineEdit->text()));
    c->performAction(Call::Action::ACCEPT);
    ui->ringContactLineEdit->clear();
    auto photoRect = ui->callingPhoto->frameGeometry();
    ui->callingPhoto->setPixmap(
                QPixmap::fromImage(
                    GlobalInstances::pixmapManipulator()
                    .callPhoto(c, QSize(photoRect.width(), photoRect.height()))
                    .value<QImage>()));
}

void
CallWidget::conversationsButtonClicked()
{
    ui->btnConversations->setChecked(true);
    ui->btnInvites->setChecked(false);
    setConversationFilter(lrc::api::profile::Type::RING);
}

void
CallWidget::invitationsButtonClicked()
{
    ui->btnConversations->setChecked(false);
    ui->btnInvites->setChecked(true);
    setConversationFilter(lrc::api::profile::Type::PENDING);
}

void
CallWidget::settingsButtonClicked()
{
    emit NavigationRequested(ScreenEnum::SetttingsScreen);
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
    if (!conversations.empty() &&
        Utils::isContactValid(conversations.at(0).participants.at(0), *convModel)) {
        selectConversation(smartListModel_->index(0));
    }
}

void CallWidget::slotAcceptInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>().toStdString();
    LRCInstance::getCurrentConversationModel()->makePermanent(convUid);
    ui->messageView->setInvitation(false);
}

void CallWidget::slotBlockInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>().toStdString();
    if (!convUid.empty() && convUid == LRCInstance::getSelectedConvUid()) {
        backToWelcomePage();
    }
    LRCInstance::getCurrentConversationModel()->removeConversation(convUid, true);
}

void CallWidget::slotIgnoreInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>().toStdString();
    if (!convUid.empty() && convUid == LRCInstance::getSelectedConvUid()) {
        backToWelcomePage();
    }
    LRCInstance::getCurrentConversationModel()->removeConversation(convUid, false);
}

void CallWidget::slotCustomContextMenuRequested(const QPoint& pos)
{
    setupSmartListContextMenu(pos);
}

void CallWidget::slotAccountChanged(int index)
{
    try {
        auto accountList = LRCInstance::accountModel().getAccountList();
        setSelectedAccount(accountList.at(index));
    } catch (...) {
        qWarning() << "CallWidget::slotAccountChanged exception";
    }
}

void CallWidget::slotShowCallView(const std::string& accountId,
                                  const lrc::api::conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    Q_UNUSED(convInfo);
    qDebug() << "slotShowCallView";
    ui->callStackWidget->show();
    ui->callStackWidget->setCurrentWidget(ui->videoPage);
    ui->videoWidget->showChatviewIfToggled();
    hideMiniSpinner();
}

void CallWidget::slotShowIncomingCallView(const std::string& accountId,
                                          const lrc::api::conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    qDebug() << "slotShowIncomingCallView";

    auto callModel = LRCInstance::getCurrentCallModel();

    if (!callModel->hasCall(convInfo.callId)) {
        return;
    }

    auto convModel = LRCInstance::getCurrentConversationModel();
    ui->callerPhoto->setPixmap(QPixmap::fromImage(imageForConv(convInfo.uid)));
    auto bestName = QString::fromStdString(Utils::bestNameForConversation(convInfo, *convModel));
    auto bestId = QString::fromStdString(Utils::bestIdForConversation(convInfo, *convModel));
    auto finalBestId = (bestName != bestId) ? bestId : "";

    auto call = callModel->getCall(convInfo.callId);
    auto isCallSelected = LRCInstance::getSelectedConvUid() == convInfo.uid;

    if (call.isOutgoing) {
        if (isCallSelected) {
            miniSpinner_->start();
            ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
            ui->spinnerLabel->show();
            ui->callStackWidget->setCurrentWidget(ui->outgoingCallPage);
            ui->callStackWidget->show();
        }
    } else {
        if (!QApplication::focusWidget()) {
            auto formattedName = Utils::bestNameForConversation(convInfo, *convModel);
            Utils::showSystemNotification(this,
                QString(tr("Call incoming from %1")).arg(QString::fromStdString(formattedName)));
        }
        selectSmartlistItem(convInfo.uid);
        auto selectedAccountId = LRCInstance::getCurrentAccountInfo().id;
        auto accountProperties = LRCInstance::accountModel().getAccountConfig(selectedAccountId);
        if (accountProperties.autoAnswer) {
            ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
            ui->callStackWidget->setCurrentWidget(ui->videoPage);
        } else if (isCallSelected) {
            ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
            ui->callStackWidget->setCurrentWidget(ui->incomingCallPage);
            ui->callStackWidget->show();
        }
    }

    if (ui->messagesWidget->isHidden()) {
        ui->messagesWidget->show();
    }

    ui->videoWidget->pushRenderer(convInfo.callId);

    QFontMetrics primaryCallLabelFontMetrics(ui->callingBestNameLabel->font());
    QFontMetrics sencondaryCallLabelFontMetrics(ui->callingBestIdLabel->font());

    QString elidedLabel = primaryCallLabelFontMetrics.elidedText(bestName, Qt::ElideRight, ui->callerBestNameLabel->width());
    ui->callerBestNameLabel->setText(elidedLabel);

    elidedLabel = primaryCallLabelFontMetrics.elidedText(bestName, Qt::ElideRight, ui->callingBestNameLabel->width());
    ui->callingBestNameLabel->setText(elidedLabel);

    elidedLabel = sencondaryCallLabelFontMetrics.elidedText(finalBestId, Qt::ElideRight, ui->callingBestIdLabel->width());
    ui->callingBestIdLabel->setText(elidedLabel);

    ui->smartList->update();
}

void CallWidget::slotShowChatView(const std::string& accountId,
                      const lrc::api::conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    Q_UNUSED(convInfo);

    ui->callStackWidget->hide();
    showConversationView();
}

void
CallWidget::slotToggleFullScreenClicked()
{
    if (ui->mainActivityWidget->isFullScreen()) {
        ui->stackedWidget->addWidget(ui->mainActivityWidget);
        ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
        ui->mainActivityWidget->showNormal();
    } else {
        ui->stackedWidget->removeWidget(ui->mainActivityWidget);
        ui->mainActivityWidget->setParent(0);
        ui->mainActivityWidget->showFullScreen();
    }
}

void
CallWidget::slotVideoViewDestroyed(const std::string& callid)
{
    auto convUid = LRCInstance::getSelectedConvUid();
    auto currentConversationModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(convUid, *currentConversationModel);
    if (conversation != currentConversationModel->allFilteredConversations().end() &&
        callid != conversation->callId) {
        return;
    }
    if (ui->mainActivityWidget->isFullScreen()) {
        ui->stackedWidget->addWidget(ui->mainActivityWidget);
        ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
        ui->mainActivityWidget->showNormal();
    }
    showConversationView();
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
    connectAccount(accountId);
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
    ui->conversationsFilterWidget->updateBadges();
    ui->conversationsFilterWidget->update();
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
    auto selectedAccountId = LRCInstance::getCurrAccId();
    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(selectedAccountId);
    bool isRINGAccount = accountInfo.profileInfo.type == lrc::api::profile::Type::RING;
    try {
        accountInfo.contactModel->getContact(contactURI.toStdString());
        isContact = true;
    }
    catch (...) {}

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

    auto convModel = LRCInstance::getCurrentConversationModel();
    auto currentConversation = Utils::getConversationFromUid(LRCInstance::getSelectedConvUid(),
                                                             *convModel);
    ui->messageView->clear();
    ui->messageView->printHistory(*convModel, currentConversation->interactions, true);

    // Contact Avatars
    auto accInfo = &LRCInstance::getCurrentAccountInfo();
    auto contactUri = currentConversation->participants.front();
    try {
        auto& contact = accInfo->contactModel->getContact(contactUri);
        auto bestName = Utils::bestNameForConversation(*currentConversation, *convModel);
        ui->messageView->setInvitation(
            (contact.profileInfo.type == lrc::api::profile::Type::PENDING),
            bestName,
            accInfo->contactModel->getContactProfileId(contact.profileInfo.uri)
        );
        if (!contact.profileInfo.avatar.empty()) {
            ui->messageView->setSenderImage(
                accInfo->contactModel->getContactProfileId(contactUri),
                contact.profileInfo.avatar);
        } else {
            auto avatar = Utils::conversationPhoto(LRCInstance::getSelectedConvUid(), *accInfo);
            QByteArray ba;
            QBuffer bu(&ba);
            avatar.save(&bu, "PNG");
            std::string avatarString = ba.toBase64().toStdString();
            ui->messageView->setSenderImage(
                accInfo->contactModel->getContactProfileId(contactUri),
                avatarString);
        }
    } catch (...) { }
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
    ui->messageView->hideMessages();
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
    Utils::InvokeMailto(tr("Contact me on Jami"), tr("My Id is : ") + ui->ringIdLabel->text());
}

void
CallWidget::on_sendContactRequestButton_clicked()
{
    LRCInstance::getCurrentConversationModel()->makePermanent(LRCInstance::getSelectedConvUid());
    ui->sendContactRequestButton->hide();
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
    QObject::disconnect(interactionRemovedConnection_);

    modelSortedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::modelSorted,
        [this]() {
            updateConversationsFilterWidget();
            selectSmartlistItem(LRCInstance::getSelectedConvUid());
            ui->smartList->update();
        }
    );
    modelUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationUpdated,
        [this](const std::string& convUid) {
            Q_UNUSED(convUid);
            ui->smartList->update();
        }
    );
    filterChangedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::filterChanged,
        [this]() {
            updateSmartList();
            updateConversationsFilterWidget();
            ui->smartList->update();
        }
    );
    newConversationConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::newConversation,
        [this](const std::string& convUid) {
            updateSmartList();
            updateConversationForNewContact(convUid);
            ui->conversationsFilterWidget->update();
        }
    );
    conversationRemovedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationRemoved,
        [this]() {
            backToWelcomePage();
        }
    );
    conversationClearedConnection = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationCleared,
        [this](const std::string& convUid) {
            ui->messageView->clear();
            // if currently selected,
            // switch to welcome screen (deselecting current smartlist item )
            if (convUid != LRCInstance::getSelectedConvUid()) {
                return;
            }
            backToWelcomePage();
        }
    );
    interactionStatusUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::interactionStatusUpdated,
        [this](const std::string& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction) {
            if (convUid != LRCInstance::getSelectedConvUid()) {
                return;
            }
            auto& currentAccountInfo = LRCInstance::getCurrentAccountInfo();
            auto currentConversationModel = currentAccountInfo.conversationModel.get();
            currentConversationModel->clearUnreadInteractions(convUid);
            ui->conversationsFilterWidget->update();
            ui->messageView->updateInteraction(*currentConversationModel, interactionId, interaction);
        }
    );
    newInteractionConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::newInteraction,
        [this](const std::string& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction) {
            onIncomingMessage(convUid, interactionId, interaction);
            if (interaction.type == lrc::api::interaction::Type::CALL) {
                return;
            }
            if (convUid == LRCInstance::getSelectedConvUid()) {
                ui->videoWidget->simulateShowChatview(true);
            }
        }
    );
    interactionRemovedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::interactionRemoved,
        [this](const std::string& convUid, uint64_t interactionId) {
            Q_UNUSED(convUid);
            ui->messageView->removeInteraction(interactionId);
        }
    );
    currentConversationModel->setFilter("");
    // clear search field
    ui->ringContactLineEdit->setText("");
    return true;
}

void
CallWidget::updateConversationView(const std::string& convUid)
{
    if (convUid != LRCInstance::getSelectedConvUid()) {
        return;
    }
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
        showIMOutOfCall(index);
        auto convUid = LRCInstance::getSelectedConvUid();
        if (!lastConvUid_.compare(convUid)) {
            return;
        }
        lastConvUid_.assign(convUid);
        auto currentConversationModel = LRCInstance::getCurrentConversationModel();
        auto callModel = LRCInstance::getCurrentCallModel();
        auto conversation = Utils::getConversationFromUid(convUid, *currentConversationModel);
        const auto item = currentConversationModel->filteredConversation(index.row());
        if (callModel->hasCall(conversation->callId) && item.callId == conversation->callId) {
            ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
            ui->callStackWidget->show();
            return;
        }
        ui->callStackWidget->hide();
    }
}

bool
CallWidget::selectConversation( const lrc::api::conversation::Info& item,
                                lrc::api::ConversationModel& convModel)
{
    if (LRCInstance::getSelectedConvUid() == item.uid) {
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
    auto selectedUid = LRCInstance::getSelectedConvUid();
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
    if (!ui->smartList->model()) {
        smartListModel_.reset(new SmartListModel(LRCInstance::getCurrAccId(), this));
        ui->smartList->setModel(smartListModel_.get());
        ui->smartList->setItemDelegate(new ConversationItemDelegate());
    } else {
        smartListModel_->setAccount(LRCInstance::getCurrAccId());
    }

    // smartlist selection
    QObject::disconnect(smartlistSelectionConnection_);
    smartlistSelectionConnection_ = connect(ui->smartList->selectionModel(),
        SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this,
        SLOT(smartListSelectionChanged(QItemSelection, QItemSelection)));
}

void
CallWidget::updateComboBox()
{
    ui->currentAccountComboBox->updateComboBoxDisplay();
}

void
CallWidget::update()
{
    updateSmartList();
    updateConversationsFilterWidget();
    updateComboBox();
    connectConversationModel();
}

void
CallWidget::connectAccount(const std::string& accId)
{
    auto callModel = LRCInstance::accountModel().getAccountInfo(accId).callModel.get();
    disconnect(callStatusChangedConnection_);
    callStatusChangedConnection_ = QObject::connect(callModel, &lrc::api::NewCallModel::callStatusChanged,
        [this, accId](const std::string& callId) {
            auto callModel = LRCInstance::accountModel().getAccountInfo(accId).callModel.get();
            auto call = callModel->getCall(callId);
            switch (call.status) {
            case lrc::api::call::Status::INVALID:
            case lrc::api::call::Status::INACTIVE:
            case lrc::api::call::Status::ENDED:
            case lrc::api::call::Status::PEER_BUSY:
            case lrc::api::call::Status::TIMEOUT:
            case lrc::api::call::Status::TERMINATING:
            {
                ui->callStackWidget->hide();
                showConversationView();
                break;
            }
            default:
                break;
            }
        });
    auto& contactModel = LRCInstance::getCurrentAccountInfo().contactModel;
    disconnect(contactAddedConnection_);
    contactAddedConnection_ = connect(contactModel.get(), &lrc::api::ContactModel::contactAdded,
        [this, &contactModel](const std::string & contactId) {
            auto convModel = LRCInstance::getCurrentConversationModel();
            auto currentConversation = Utils::getConversationFromUid(LRCInstance::getSelectedConvUid(),
                *convModel);
            if (currentConversation == convModel->allFilteredConversations().end()) {
                return;
            }
            if (contactId == contactModel.get()->getContact((*currentConversation).participants.at(0)).profileInfo.uri) {
                ui->messageView->clear();
                ui->messageView->printHistory(*convModel, currentConversation->interactions);
            }
        });
}