/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

// client
#include "utils.h"
#ifdef Q_OS_WIN
//ERROR is defined in windows.h
#undef ERROR
#undef interface
#endif
#include "animationhelpers.h"
#include "conversationitemdelegate.h"
#include "globalsystemtray.h"
#include "lrcinstance.h"
#include "mainwindow.h"
#include "pixbufmanipulator.h"
#include "ringthemeutils.h"
#include "settingskey.h"
#include "aboutdialog.h"
#include "userprofile.h"

#include "globalinstances.h"

#include <qrencode.h>

#include <QComboBox>
#include <QtConcurrent/QtConcurrent>
#include <QDesktopServices>
#include <QScrollBar>
#include <QWebEngineScript>
#include <QMimeData>
#include <QShortcut>

#include <algorithm>
#include <memory>

CallWidget::CallWidget(QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::CallWidget),
    menu_(new QMenu())
{
    ui->setupUi(this);

    using namespace lrc::api;

    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);
    clipboard_ = QApplication::clipboard();

    QPixmap logo(":/images/logo-jami-standard-coul.png");
    ui->ringLogo->setPixmap(logo.scaledToHeight(100, Qt::SmoothTransformation));
    ui->ringLogo->setAlignment(Qt::AlignHCenter);

    ui->qrLabel->hide();

    QSettings settings("jami.net", "Jami");

    // select last used account if stored in registry
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (!accountList.empty()) {
        QString accountIdToStartWith;
        if (settings.contains(SettingsKey::selectedAccount)) {
            accountIdToStartWith = settings
                .value(SettingsKey::selectedAccount, true)
                .value<QString>();
            if (!accountList.contains(accountIdToStartWith)) {
                accountIdToStartWith = accountList.at(0);
            }
        }
        else {
            accountIdToStartWith = accountList.at(0);
        }
        setSelectedAccount(accountIdToStartWith);
        // get account index and set the currentAccountComboBox
        auto index = accountList.indexOf(accountIdToStartWith);
        if (index != -1) {
            ui->currentAccountComboBox->setCurrentIndex(index);
        }
    }

    if (settings.contains(SettingsKey::mainSplitterState)) {
        auto splitterStates = settings.value(SettingsKey::mainSplitterState).toByteArray();
        ui->mainActivitySplitter->restoreState(splitterStates);
        splitterStates = settings.value(SettingsKey::smartListToWebviewSplitterState).toByteArray();
        ui->splitter->restoreState(splitterStates);
    }

    ui->mainActivitySplitter->setCollapsible(0, false);
    ui->mainActivitySplitter->setCollapsible(1, false);

    //disable dropdown shadow on combobox
    ui->currentAccountComboBox->view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    // conversation list
    ui->smartList->setContextMenuPolicy(Qt::CustomContextMenu);

    // setup searchingfor mini spinner
    miniSpinner_ = new QMovie(":/images/waiting.gif");
    ui->spinnerLabel->setMovie(miniSpinner_);
    ui->spinnerLabel->hide();

    // shortcuts
    registerShortCuts();

    // connections
    connect(ui->currentAccountComboBox, &CurrentAccountComboBox::settingsButtonClicked,
            this, &CallWidget::settingsButtonClicked);

    connect(ui->currentAccountComboBox, &CurrentAccountComboBox::newAccountClicked,
        [this]() {
            emit NavigationRequested(ScreenEnum::WizardScreen);
        });

    connect(ui->mainActivitySplitter, &QSplitter::splitterMoved,
        [this](int pos, int index) {
            Q_UNUSED(index);
            Q_UNUSED(pos);
            QSettings settings("jami.net", "Jami");
            settings.setValue(SettingsKey::mainSplitterState, ui->mainActivitySplitter->saveState());
        });

    connect(ui->splitter, &QSplitter::splitterMoved,
        [this](int pos, int index) {
            Q_UNUSED(index);
            Q_UNUSED(pos);
            QSettings settings("jami.net", "Jami");
            settings.setValue(SettingsKey::smartListToWebviewSplitterState, ui->splitter->saveState());
        });

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

    connect(ui->messageView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(ShowContextMenu(const QPoint&)));

    connect(ui->messageView, &MessageWebView::pasteKeyDetected,
            this, &CallWidget::Paste);

    connect(ui->messageView, &MessageWebView::invitationAccepted,
            [this] {
                auto convUid = LRCInstance::getCurrentConvUid();
                LRCInstance::getCurrentConversationModel()->makePermanent(convUid);
                ui->sendContactRequestButton->hide();
            });

    // video view
    connect(ui->videoView, &VideoView::setChatVisibility,
            this, &CallWidget::slotSetChatVisibility, Qt::DirectConnection);

    connect(ui->videoView, &VideoView::toggleFullScreenClicked,
            this, &CallWidget::slotToggleFullScreenClicked);

    connect(&LRCInstance::behaviorController(), &BehaviorController::showCallView,
            this, &CallWidget::slotShowCallView);

    connect(&LRCInstance::behaviorController(), &BehaviorController::showChatView,
            this, &CallWidget::slotShowChatView);

    // these may trigger notifications
    connect(&LRCInstance::behaviorController(), &BehaviorController::showIncomingCallView,
            this, &CallWidget::slotShowIncomingCallView);

    connect(&LRCInstance::behaviorController(), &BehaviorController::newTrustRequest,
            this, &CallWidget::slotNewTrustRequest);

    connect(&LRCInstance::behaviorController(), &BehaviorController::newUnreadInteraction,
        [this](const QString& accountId, const QString& conversation,
               uint64_t interactionId, const interaction::Info& interaction) {
            if (LRCInstance::getCurrAccId() != accountId) {
                onNewInteraction(accountId, conversation, interactionId, interaction);
            }
        });

    connect(LRCInstance::renderer(), &RenderManager::videoDeviceListChanged,
            this, &CallWidget::slotVideoDeviceListChanged);

    connect(ui->changelogButton, &QAbstractButton::clicked,
        [this] {
            AboutDialog aboutDialog (&MainWindow::instance());
            aboutDialog.getContainer().toStrongRef()->exec();
        });

    // set first view to welcome view
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    ui->btnConversations->setChecked(true);

    // chat view
    ui->messageView->buildView();
    ui->messageView->setContextMenuPolicy(Qt::CustomContextMenu);

    // hide the call stack
    setCallPanelVisibility(false);

    setVisible(false);
    ui->sipCallerBestIdLabel->setVisible(false);

    // set collapsible
    ui->splitter->setCollapsible(ui->splitter->indexOf(ui->stackedWidget), false);
}

CallWidget::~CallWidget()
{
    delete ui;
    delete menu_;
}

void
CallWidget::navigated(bool to)
{
    setVisible(to);
    if (to) {
        updateSmartList();
        connectConversationModel();
        try {
            auto accountList = LRCInstance::accountModel().getAccountList();
            if (accountList.size() == 1) {
                auto index = accountList.indexOf(LRCInstance::getCurrAccId());
                if (index != -1) {
                    slotAccountChanged(index);
                }
            }
        } catch (...) {}
        ui->currentAccountComboBox->updateComboBoxDisplay();
        auto& conversation = LRCInstance::getCurrentConversation();
        if (!conversation.uid.isEmpty()) {
            selectSmartlistItem(conversation.uid);
            ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
        } else {
            backToWelcomePage();
        }
        /**
         * This will resize/position the preview when returning from the settings
         * in case of a resolution change.
         */
        ui->videoView->resetPreview();
        setFocus();
    } else {
        QObject::disconnect(smartlistSelectionConnection_);
        smartListModel_.reset(nullptr);
    }
}

void
CallWidget::updateCustomUI()
{
}

void
CallWidget::slotAccountListChanged()
{}

int
CallWidget::getLeftPanelWidth()
{
    return ui->currentAccountComboBox->width();
}

void
CallWidget::onNewInteraction(const QString& accountId, const QString& convUid,
                             uint64_t interactionId, const interaction::Info& interaction)
{
    Q_UNUSED(interactionId);
    try {
        auto& accountInfo = LRCInstance::getAccountInfo(accountId);
        auto& convModel = accountInfo.conversationModel;
        auto& conversation = LRCInstance::getConversationFromConvUid(convUid, accountId);
        if (conversation.uid.isEmpty()) {
            return;
        }
        if (!interaction.authorUri.isEmpty() &&
            (!QApplication::focusWidget() || LRCInstance::getCurrAccId() != accountId)) {
            auto bestName = Utils::bestNameForConversation(conversation, *convModel);
            Utils::showSystemNotification(this, bestName,interaction.body);
        }
        updateConversationsFilterWidget();
        if (convUid != LRCInstance::getCurrentConvUid()) {
            return;
        }
        convModel->clearUnreadInteractions(convUid);
        ui->messageView->printNewInteraction(*convModel, interactionId, interaction);
        if (interaction.type != interaction::Type::CALL) {
            ui->videoView->simulateShowChatview(true);
        }
    } catch (...) {}
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
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>();
    auto& conversation = LRCInstance::getConversationFromConvUid(convUid);
    if (conversation.uid.isEmpty()) return;
    auto contactUid = conversation.participants.at(0);
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
            if (convUid != LRCInstance::getCurrentConvUid()) {
                selectConversation(conversation);
            }
        });
    // audio call
    auto audioCallAction = new QAction(tr("Start audio call"), this);
    menu.addAction(audioCallAction);
    connect(audioCallAction, &QAction::triggered,
        [this, convUid, conversation, convModel]() {
            convModel->placeAudioOnlyCall(convUid);
            ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(convUid)));
            if (convUid != LRCInstance::getCurrentConvUid()) {
                selectConversation(conversation);
            }
        });
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

    if (contact.profileInfo.type == lrc::api::profile::Type::RING) {
        // separator
        menu.addSeparator();

        // block contact
        auto blockContactAction = new QAction(tr("Block contact"), this);
        menu.addAction(blockContactAction);
        connect(blockContactAction, &QAction::triggered,
            [convUid]() {
                LRCInstance::getCurrentConversationModel()->removeConversation(convUid, true);
            });

        // separator
        menu.addSeparator();

        // show user profile
        auto profileShowingAction = new QAction(tr("Profile"), this);
        menu.addAction(profileShowingAction);
        connect(profileShowingAction, &QAction::triggered,
            [conversation, this]() {
                UserProfile userProfile(conversation, qobject_cast<MainWindow*>(this->parent()->parent()->parent()));
                userProfile.getContainer().toStrongRef()->exec();
            });
    }
    smartListModel_->isContextMenuOpen = true;
    menu.exec(globalPos);
    smartListModel_->isContextMenuOpen = false;
}

void
CallWidget::on_smartList_clicked(const QModelIndex& index)
{
    Q_UNUSED(index);
}

void
CallWidget::on_acceptButton_clicked()
{
    auto convInfo = LRCInstance::getCurrentConversation();
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getCurrentCallModel()->accept(convInfo.callId);
    }
}

void
CallWidget::on_refuseButton_clicked()
{
    auto convInfo = LRCInstance::getCurrentConversation();
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getCurrentCallModel()->refuse(convInfo.callId);
        showConversationView();
    }
}

void
CallWidget::on_cancelButton_clicked()
{
    auto convInfo = LRCInstance::getCurrentConversation();
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getCurrentCallModel()->hangUp(convInfo.callId);
        showConversationView();
    }
}

void
CallWidget::showConversationView()
{
    if (LRCInstance::getCurrentConvUid().isEmpty()) {
        backToWelcomePage();
        return;
    }
    ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
    ui->messageView->setFocus();
    if (ui->messagesWidget->isHidden()) {
        ui->messagesWidget->show();
    }
    updateChatviewFrame();
}

bool
CallWidget::selectSmartlistItem(const QString& convUid)
{
    if (convUid.isEmpty() || !ui->smartList->selectionModel())
        return false;
    ui->smartList->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Deselect);
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto& conversation = LRCInstance::getConversationFromConvUid(convUid);
    if (conversation.uid.isEmpty()) {
        return false;
    }
    auto contactURI = conversation.participants[0];
    if (contactURI.isEmpty() ||
        convModel->owner.contactModel->getContact(contactURI).profileInfo.type == lrc::api::profile::Type::TEMPORARY) {
        return false;
    }
    for (int row = 0; row < smartListModel_->rowCount(); row++) {
        QModelIndex index = smartListModel_->index(row);
        auto indexContactURI = index.data(SmartListModel::Role::URI).value<QString>();
        if (indexContactURI == contactURI) {
            ui->smartList->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
            return true;
        }
    }
    return false;
}

void
CallWidget::on_smartList_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    selectConversation(index);

    LRCInstance::getCurrentConversationModel()->placeCall(LRCInstance::getCurrentConvUid());

    ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(LRCInstance::getCurrentConvUid())));
}

QImage
CallWidget::imageForConv(const QString& convUid)
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

void CallWidget::slotSetChatVisibility(bool visible)
{
    ui->messagesWidget->setVisible(visible);
}

void
CallWidget::slotVideoDeviceListChanged()
{
    if (LRCInstance::hasVideoCall()) {
        LRCInstance::renderer()->startPreviewing();
    }
}

void
CallWidget::conversationsButtonClicked()
{
    ui->btnConversations->setChecked(true);
    ui->btnInvites->setChecked(false);
    ui->ringContactLineEdit->setPlaceholderString(tr("Find a new or existing contact"));
    setConversationFilter(lrc::api::profile::Type::RING);
}

void
CallWidget::invitationsButtonClicked()
{
    ui->btnConversations->setChecked(false);
    ui->btnInvites->setChecked(true);
    ui->ringContactLineEdit->setPlaceholderString(tr("Search your received invitations"));
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

void
CallWidget::slotAcceptInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>();
    LRCInstance::getCurrentConversationModel()->makePermanent(convUid);
    ui->messageView->setInvitation(false);
    ui->sendContactRequestButton->hide();
}

void
CallWidget::slotBlockInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>();
    if (!convUid.isEmpty() && convUid == LRCInstance::getCurrentConvUid()) {
        backToWelcomePage();
    }
    LRCInstance::getCurrentConversationModel()->removeConversation(convUid, true);
}

void
CallWidget::slotIgnoreInviteClicked(const QModelIndex & index)
{
    auto convUid = index.data(static_cast<int>(SmartListModel::Role::UID)).value<QString>();
    if (!convUid.isEmpty() && convUid == LRCInstance::getCurrentConvUid()) {
        backToWelcomePage();
    }
    LRCInstance::getCurrentConversationModel()->removeConversation(convUid, false);
}

void
CallWidget::slotCustomContextMenuRequested(const QPoint& pos)
{
    setupSmartListContextMenu(pos);
}

void
CallWidget::slotAccountChanged(int index)
{
    auto accountList = LRCInstance::accountModel().getAccountList();
    if(accountList.size() > index)
        setSelectedAccount(accountList.at(index));
}

void
CallWidget::slotShowCallView(const QString& accountId,
                             const lrc::api::conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    qDebug() << "slotShowCallView";

    // control visible callwidget buttons
    setCallPanelVisibility(true);

    ui->callStackWidget->setCurrentWidget(ui->videoPage);
    hideMiniSpinner();

    ui->videoView->updateCall(convInfo.uid, accountId);
    ui->videoView->show();
    ui->videoView->setFocus();
}

void
CallWidget::slotShowIncomingCallView(const QString& accountId,
                                     const conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    qDebug() << "slotShowIncomingCallView";

    auto callModel = LRCInstance::getCurrentCallModel();

    if (!callModel->hasCall(convInfo.callId)) {
        return;
    }

    auto convModel = LRCInstance::getCurrentConversationModel();
    ui->callerPhoto->setPixmap(QPixmap::fromImage(imageForConv(convInfo.uid)));
    auto bestName = Utils::bestNameForConversation(convInfo, *convModel);
    auto bestId = Utils::bestIdForConversation(convInfo, *convModel);
    auto finalBestId = (bestName != bestId) ? bestId : "";

    auto call = callModel->getCall(convInfo.callId);
    auto isCallSelected = LRCInstance::getCurrentConvUid() == convInfo.uid;
    ui->callingStatusLabel->setText(lrc::api::call::to_string(call.status));

    auto itemInCurrentFilter = false;
    if (call.isOutgoing) {
        if (isCallSelected) {
            miniSpinner_->start();
            ui->spinnerLabel->show();
            ui->callStackWidget->setCurrentWidget(ui->outgoingCallPage);
            setCallPanelVisibility(true);
        }
    } else {
        if (!QApplication::focusWidget()) {
            auto formattedName = Utils::bestNameForConversation(convInfo, *convModel);
            Utils::showSystemNotification(this, QString(tr("Call incoming from %1")).arg(formattedName));
        }
        auto selectedAccountId = LRCInstance::getCurrentAccountInfo().id;
        auto accountProperties = LRCInstance::accountModel().getAccountConfig(selectedAccountId);
        if (!isCallSelected)
            itemInCurrentFilter = selectSmartlistItem(convInfo.uid);
        if (accountProperties.autoAnswer) {
            ui->callStackWidget->setCurrentWidget(ui->videoPage);
        } else if (isCallSelected || !itemInCurrentFilter) {
            ui->callStackWidget->setCurrentWidget(ui->incomingCallPage);
        }
        setCallPanelVisibility(true);
    }

    if (!itemInCurrentFilter && !isCallSelected) {
        if (ui->smartList->selectionModel())
            ui->smartList->selectionModel()->clear();
        LRCInstance::setSelectedConvId(convInfo.uid);
        showChatView(accountId, convInfo);
    } else if (ui->messagesWidget->isHidden()) {
        ui->messagesWidget->show();
    }

    QFontMetrics primaryCallLabelFontMetrics(ui->callingBestNameLabel->font());
    QFontMetrics sencondaryCallLabelFontMetrics(ui->callingBestIdLabel->font());

    QString elidedLabel = primaryCallLabelFontMetrics.elidedText(bestName, Qt::ElideRight, ui->callerBestNameLabel->width());
    ui->callerBestNameLabel->setText(elidedLabel);

    elidedLabel = primaryCallLabelFontMetrics.elidedText(bestName, Qt::ElideRight, ui->callingBestNameLabel->width());
    ui->callingBestNameLabel->setText(elidedLabel);

    elidedLabel = sencondaryCallLabelFontMetrics.elidedText(finalBestId, Qt::ElideRight, ui->callingBestIdLabel->width());
    ui->callingBestIdLabel->setText(elidedLabel);
    ui->sipCallerBestIdLabel->setText(elidedLabel);

    if(finalBestId.isEmpty())
        ui->sipCallerBestIdLabel->setVisible(false);
    else
        ui->sipCallerBestIdLabel->setVisible(true);

    ui->smartList->update();
}

void
CallWidget::slotShowChatView(const QString& accountId,
                      const lrc::api::conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    Q_UNUSED(convInfo);

    setCallPanelVisibility(false);
    showConversationView();
}

void
CallWidget::slotNewTrustRequest(const QString& accountId, const QString& contactUri)
{
    try {
        auto& accountInfo = LRCInstance::getAccountInfo(accountId);
        auto& contactModel = accountInfo.contactModel;
        if (!QApplication::focusWidget() || LRCInstance::getCurrAccId() != accountId) {
            try {
                auto contactInfo = contactModel->getContact(contactUri);
                auto bestName = Utils::bestNameForContact(contactInfo);
                Utils::showSystemNotification(this, bestName, QObject::tr("Contact request"));
            } catch (...) {
                qDebug() << "Can't get contact: ", contactUri;
                return;
            }
        }
    } catch (...) {
        qDebug() << "Can't get account:" << accountId;
    }
}

void
CallWidget::slotToggleFullScreenClicked()
{
    if (ui->mainActivityWidget->isFullScreen()) {
        ui->stackedWidget->addWidget(ui->mainActivityWidget);
        ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
        ui->mainActivityWidget->showNormal();
        MainWindow::instance().show();
    } else {
        auto geo = ui->stackedWidget->window()->geometry();
        ui->stackedWidget->removeWidget(ui->mainActivityWidget);
        ui->mainActivityWidget->setParent(0);
        ui->mainActivityWidget->window()->setGeometry(geo);
        MainWindow::instance().hide();
        ui->mainActivityWidget->showFullScreen();
    }
}

void
CallWidget::callTerminating(const QString& id)
{
    auto conversation = LRCInstance::getCurrentConversation();
    if ( conversation.uid.isEmpty() &&
         conversation.callId != id &&
         conversation.confId != id) {
        return;
    }
    if (ui->mainActivityWidget->isFullScreen()) {
        ui->stackedWidget->addWidget(ui->mainActivityWidget);
        ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
        ui->mainActivityWidget->showNormal();
        MainWindow::instance().showWindow();
    }
    showConversationView();
}

void
CallWidget::setSelectedAccount(const QString& accountId)
{
    LRCInstance::setSelectedAccountId(accountId);

    // First, we get back to the welcome view (except if in call)
    if (ui->stackedWidget->currentWidget() != ui->videoPage &&
        ui->stackedWidget->currentWidget() != ui->welcomePage) {
        backToWelcomePage();
    }

    // We setup the ringIdLabel and the QRCode
    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(accountId);
    auto id = accountInfo.registeredName.isEmpty() ? accountInfo.profileInfo.uri : accountInfo.registeredName;
    auto isRingAccount = accountInfo.profileInfo.type == lrc::api::profile::Type::RING;
    if (isRingAccount) {
        ui->ringIdLabel->setText(id);
        ui->qrLabel->setPixmap(QPixmap::fromImage(Utils::setupQRCode(accountInfo.profileInfo.uri, 5).scaled(QSize(qrSize_, qrSize_),
                               Qt::KeepAspectRatio)));
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
    if (!invites) {
        Utils::whileBlocking<QPushButton>(ui->btnConversations)->setChecked(true);
        Utils::whileBlocking<QPushButton>(ui->btnInvites)->setChecked(false);
    }
    ui->searchTopLeftWidget->setVisible(invites);
    ui->searchTopRightWidget->setVisible(invites);
    ui->conversationsFilterWidget->update();
    ui->conversationsFilterWidget->updateBadges();
}

void CallWidget::setConversationFilter(const QString & filter)
{
    LRCInstance::getCurrentConversationModel()->setFilter(filter);
}

void
CallWidget::showChatView(const QModelIndex& nodeIdx)
{
    auto convUid = nodeIdx.data(static_cast<int>(SmartListModel::Role::UID)).toString();
    auto& conversation = LRCInstance::getConversationFromConvUid(convUid);
    if (!conversation.uid.isEmpty()) {
        setupChatView(conversation);
    }
}

void
CallWidget::showChatView(const QString& accountId, const lrc::api::conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    setupChatView(convInfo);
}

void
CallWidget::setConversationProfileData(const lrc::api::conversation::Info& convInfo)
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto accInfo = &LRCInstance::getCurrentAccountInfo();
    auto contactUri = convInfo.participants.front();
    if (contactUri.isEmpty()) {
        return;
    }
    try {
        auto& contact = accInfo->contactModel->getContact(contactUri);
        auto bestName = Utils::bestNameForConversation(convInfo, *convModel);
        ui->messageView->setInvitation(
            (contact.profileInfo.type == lrc::api::profile::Type::PENDING),
            bestName,
            contactUri
        );
        if (!contact.profileInfo.avatar.isEmpty()) {
            ui->messageView->setSenderImage(contactUri, contact.profileInfo.avatar);
        } else {
            auto avatar = Utils::conversationPhoto(convInfo.uid, *accInfo);
            QByteArray ba;
            QBuffer bu(&ba);
            avatar.save(&bu, "PNG");
            ui->messageView->setSenderImage(contactUri, QString::fromLocal8Bit(ba.toBase64()));
        }
    } catch (...) {}
}

void
CallWidget::setupChatView(const lrc::api::conversation::Info& convInfo)
{
    auto& accInfo = LRCInstance::getCurrentAccountInfo();
    auto& contact = accInfo.contactModel->getContact(convInfo.participants.at(0));
    QString displayName = Utils::bestNameForContact(contact);
    QString displayId = Utils::bestIdForContact(contact);
    QString contactURI = convInfo.participants.at(0);

    bool isContact = false;
    auto selectedAccountId = LRCInstance::getCurrAccId();
    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(selectedAccountId);
    bool isRINGAccount = accountInfo.profileInfo.type == lrc::api::profile::Type::RING;

    lrc::api::profile::Type contactType;
    try {
        auto contactInfo = accountInfo.contactModel->getContact(contactURI);
        if (contactInfo.isTrusted) {
            isContact = true;
        }
        contactType = contactInfo.profileInfo.type;
    } catch (...) {}

    ui->imNameLabel->setText(displayName);
    ui->imIdLabel->setText(displayId);
    ui->imIdLabel->setVisible(isRINGAccount && displayName != displayId);

    bool shouldShowSendContactRequestBtn = !isContact && contactType != lrc::api::profile::Type::SIP;
    ui->sendContactRequestButton->setVisible(shouldShowSendContactRequestBtn);

    ui->messageView->setMessagesVisibility(false);
    Utils::oneShotConnect(ui->messageView, &MessageWebView::sendMessageContentSaved,
        [this, convInfo, accountId = accountInfo.id, lastConvUid = lastConvUid_](const QString& content) {
            if (!lastConvUid.isEmpty()) {
                LRCInstance::setContentDraft(lastConvUid, accountId, content);
            }
            Utils::oneShotConnect(ui->messageView, &MessageWebView::messagesCleared,
                [this, convInfo] {
                    auto convModel = LRCInstance::getCurrentConversationModel();
                    ui->messageView->printHistory(*convModel, convInfo.interactions);
                    Utils::oneShotConnect(ui->messageView, &MessageWebView::messagesLoaded,
                        [this] {
                            ui->messageView->setMessagesVisibility(true);
                        });
                    setConversationProfileData(convInfo);
                });
            ui->messageView->setInvitation(false);
            ui->messageView->clear();
            auto restoredContent = LRCInstance::getContentDraft(convInfo.uid, accountId);
            ui->messageView->setSendMessageContent(restoredContent);
            ui->smartList->update();
        });
    ui->messageView->requestSendMessageContent();
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
    ui->messageView->clear();
    Utils::oneShotConnect(ui->messageView, &MessageWebView::messagesCleared,
        [this] {
            QTimer::singleShot(33, this, [this] { backToWelcomePage(); });
        });
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
    auto convInfo = LRCInstance::getCurrentConversation();
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getCurrentConversationModel()->makePermanent(convInfo.uid);
        ui->sendContactRequestButton->hide();
    }
}

void
CallWidget::on_btnAudioCall_clicked()
{
    auto convInfo = LRCInstance::getCurrentConversation();
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getCurrentConversationModel()->placeAudioOnlyCall(convInfo.uid);
        ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(convInfo.uid)));
    }
}

void
CallWidget::on_btnVideoCall_clicked()
{
    auto convInfo = LRCInstance::getCurrentConversation();
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getCurrentConversationModel()->placeCall(convInfo.uid);
        ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForConv(convInfo.uid)));
    }
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
            selectSmartlistItem(LRCInstance::getCurrentConversation().uid);
            ui->smartList->update();
        }
    );
    modelUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationUpdated,
        [this](const QString& convUid) {
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
        [this](const QString& convUid) {
            updateSmartList();
            updateConversationForNewContact(convUid);
            ui->conversationsFilterWidget->update();
            ui->currentAccountComboBox->canPlaceAudioOnlyCall(convUid);
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
        [this](const QString& convUid) {
            ui->messageView->clear();
            // if currently selected,
            // switch to welcome screen (deselecting current smartlist item )
            if (convUid != LRCInstance::getCurrentConvUid()) {
                return;
            }
            backToWelcomePage();
        }
    );
    interactionStatusUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::interactionStatusUpdated,
        [this](const QString& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction) {
            if (convUid != LRCInstance::getCurrentConvUid()) {
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
        [this](const QString& convUid, uint64_t interactionId,
               const lrc::api::interaction::Info& interaction) {
            auto accountId = LRCInstance::getCurrAccId();
            onNewInteraction(accountId, convUid, interactionId, interaction);
        }
    );
    interactionRemovedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::interactionRemoved,
        [this](const QString& convUid, uint64_t interactionId) {
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
CallWidget::updateConversationView(const QString& convUid)
{
    if (convUid != LRCInstance::getCurrentConvUid()) {
        return;
    }
}

void
CallWidget::selectConversation(const QModelIndex& index)
{
    auto convModel = LRCInstance::getCurrentConversationModel();

    if (convModel == nullptr || !index.isValid()) {
        return;
    }

    const auto item = convModel->filteredConversation(index.row());

    if (selectConversation(item)) {
        showChatView(index);
        auto convUid = LRCInstance::getCurrentConvUid();
        if (!lastConvUid_.compare(convUid)) {
            return;
        }
        lastConvUid_ = convUid;

        auto callModel = LRCInstance::getCurrentCallModel();
        auto& conversation = LRCInstance::getConversationFromConvUid(convUid);
        if (!conversation.uid.isEmpty()) {
            if (callModel->hasCall(conversation.callId) && item.callId == conversation.callId) {
                setCallPanelVisibility(true);
                return;
            }
            setCallPanelVisibility(false);
        }
    }
}

bool
CallWidget::selectConversation(const lrc::api::conversation::Info& item)
{
    if (LRCInstance::getCurrentConvUid() == item.uid) {
        return false;
    } else if (item.participants.size() > 0) {
        auto& accInfo = LRCInstance::getAccountInfo(item.accountId);
        LRCInstance::setSelectedConvId(item.uid);
        accInfo.conversationModel->selectConversation(item.uid);
        accInfo.conversationModel->clearUnreadInteractions(item.uid);
        if (!item.callId.isEmpty()) {
            QtConcurrent::run(
                [convUid=item.uid, accId=item.accountId] {
                    auto item = LRCInstance::getConversationFromConvUid(convUid);
                    auto& accInfo = LRCInstance::getAccountInfo(item.accountId);
                    accInfo.callModel->setCurrentCall(item.callId);
                });
        }
        ui->conversationsFilterWidget->update();
        return true;
    }
}

void
CallWidget::deselectConversation()
{
    if (LRCInstance::getCurrentConvUid().isEmpty()) {
        return;
    }

    auto currentConversationModel = LRCInstance::getCurrentConversationModel();

    if (currentConversationModel == nullptr) {
        return;
    }

    currentConversationModel->selectConversation("");
    LRCInstance::setSelectedConvId();

    if (auto smartListSelectionModel = ui->smartList->selectionModel()) {
        smartListSelectionModel->clear();
    }

    disconnect(imConnection_);
}

void
CallWidget::updateConversationForNewContact(const QString& convUid)
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    if (convModel == nullptr) {
        return;
    }
    ui->ringContactLineEdit->setText("");
    auto selectedUid = LRCInstance::getCurrentConvUid();
    auto& conversation = LRCInstance::getConversationFromConvUid(convUid, {}, true);
    if (!conversation.uid.isEmpty()) {
        try {
            auto contact = convModel->owner.contactModel->getContact(conversation.participants[0]);
            if (!contact.profileInfo.uri.isEmpty() && contact.profileInfo.uri == selectedUid) {
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

    try {
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
    } catch (...) {
        qWarning() << "No account selected!";
        return;
    }
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
CallWidget::connectAccount(const QString& accountId)
{
    try {
        auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
        QObject::disconnect(callStatusChangedConnection_);
        callStatusChangedConnection_ = QObject::connect(
            accInfo.callModel.get(),
            &lrc::api::NewCallModel::callStatusChanged,
            [this, accountId](const QString& callId) {
                auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
                auto& callModel = accInfo.callModel;
                auto call = callModel->getCall(callId);

                // change status label text
                ui->callingStatusLabel->setText(lrc::api::call::to_string(call.status));

                switch (call.status) {
                case lrc::api::call::Status::INVALID:
                case lrc::api::call::Status::INACTIVE:
                case lrc::api::call::Status::ENDED:
                case lrc::api::call::Status::PEER_BUSY:
                case lrc::api::call::Status::TIMEOUT:
                case lrc::api::call::Status::TERMINATING: {
                    LRCInstance::renderer()->removeDistantRenderer(callId);
                    auto convInfo = LRCInstance::getConversationFromCallId(callId);
                    if (convInfo.uid.isEmpty()) {
                        break;
                    }
                    // If it's a conference, change the smartlist index
                    // to the next remaining participant.
                    bool forceCallOnly{ false };
                    if (!convInfo.confId.isEmpty()) {
                        auto callList = LRCInstance::getAPI().getConferenceSubcalls(convInfo.confId);
                        if (callList.empty()) {
                            auto lastConferencee = LRCInstance::instance().popLastConferencee(convInfo.confId);
                            callList.append(lastConferencee);
                            forceCallOnly = true;
                        }
                        for (const auto& callId : callList) {
                            if (!callModel->hasCall(callId)) {
                                continue;
                            }
                            auto otherConv = LRCInstance::getConversationFromCallId(callId);
                            if (!otherConv.uid.isEmpty() && otherConv.uid != convInfo.uid) {
                                LRCInstance::setSelectedConvId(otherConv.uid);
                                selectSmartlistItem(otherConv.uid);
                                ui->videoView->updateCall(otherConv.uid, otherConv.accountId, forceCallOnly);
                            }
                        }
                    } else {
                        ui->videoView->updateCall();
                        setCallPanelVisibility(false);
                        showConversationView();
                        callTerminating(callId);
                    }

                    break;
                }
                case lrc::api::call::Status::CONNECTED:
                case lrc::api::call::Status::IN_PROGRESS: {
                    auto convInfo = LRCInstance::getConversationFromCallId(callId, accountId);
                    if (!convInfo.uid.isEmpty() && convInfo.uid == LRCInstance::getCurrentConvUid()) {
                        accInfo.conversationModel->selectConversation(convInfo.uid);
                    }
                    LRCInstance::renderer()->addDistantRenderer(callId);
                    ui->videoView->updateCall();
                    break;
                }
                case lrc::api::call::Status::PAUSED:
                    ui->videoView->resetPreview();
                    ui->videoView->updateCall();
                default:
                    break;
                }

                ui->smartList->update();
            });
        QObject::disconnect(contactAddedConnection_);
        contactAddedConnection_ = QObject::connect(
            accInfo.contactModel.get(),
            &lrc::api::ContactModel::contactAdded,
            [this, accountId](const QString& contactUri) {
                auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
                auto convModel = LRCInstance::getCurrentConversationModel();
                auto conversation = LRCInstance::getCurrentConversation();
                if (conversation.uid.isEmpty()) {
                    return;
                }
                if (contactUri == accInfo.contactModel->getContact(conversation.participants.at(0)).profileInfo.uri) {
                    // update call screen
                    auto avatarImg = QPixmap::fromImage(imageForConv(conversation.uid));
                    ui->callingPhoto->setPixmap(avatarImg);
                    ui->callerPhoto->setPixmap(avatarImg);
                    // update conversation
                    ui->messageView->clear();
                    setConversationProfileData(conversation);
                    ui->messageView->printHistory(*convModel, conversation.interactions);
                }
            });
        QObject::disconnect(addedToConferenceConnection_);
        addedToConferenceConnection_ = QObject::connect(
            accInfo.callModel.get(),
            &NewCallModel::callAddedToConference,
            [this](const QString& callId, const QString& confId) {
                Q_UNUSED(callId);
                LRCInstance::renderer()->addDistantRenderer(confId);
                ui->videoView->updateCall();
            });
    } catch (...) {
        qWarning() << "Couldn't get account: " << accountId;
    }
}

void
CallWidget::setCallPanelVisibility(bool visible)
{
    ui->stackedWidget->setCurrentWidget(ui->mainActivityWidget);
    ui->imBackButton->setVisible(!visible);
    ui->btnAudioCall->setVisible(!visible);
    ui->btnVideoCall->setVisible(!visible);
    ui->callStackWidget->setVisible(visible);
    ui->messageView->setRecordButtonsVisibility(!visible);
}

void
CallWidget::updateChatviewFrame()
{
    // TODO: remove this to start using webview navbar
    // instead of native qt
    if (1) {
        ui->messageView->displayNavbar(false);
        return;
    }

    auto& accInfo = LRCInstance::getCurrentAccountInfo();
    auto& convInfo = LRCInstance::getCurrentConversation();
    if (convInfo.uid.isEmpty()) {
        return;
    }
    auto contactUri = convInfo.participants.front();

    lrc::api::contact::Info contactInfo;
    try {
        contactInfo = accInfo.contactModel->getContact(contactUri);
    } catch (const std::out_of_range&) {
        qDebug() << "updateChatviewFrame: failed to retrieve contactInfo";
        return;
    }

    bool temp = contactInfo.profileInfo.type == lrc::api::profile::Type::TEMPORARY ||
                contactInfo.profileInfo.type == lrc::api::profile::Type::PENDING;

    auto bestName = Utils::bestNameForContact(contactInfo);
    auto bestId = Utils::bestIdForContact(contactInfo);

    ui->messageView->updateChatviewFrame(accInfo.enabled, contactInfo.isBanned, temp, bestName, bestId);
}

void
CallWidget::registerShortCuts()
{
    //shortcuts
    auto startAudioCallSC = new QShortcut(QKeySequence(tr("Shift+Ctrl+C", "Start an audio call")), this);
    auto startVideoCallSC = new QShortcut(QKeySequence(tr("Shift+Ctrl+X", "Start an video call")), this);
    auto clearConvHistorySC = new QShortcut(QKeySequence(tr("Shift+Ctrl+L", "Clear history")), this);
    auto blockContactSC = new QShortcut(QKeySequence(tr("Shift+Ctrl+B", "Block contact")), this);
    auto copyContactNameSC = new QShortcut(QKeySequence(tr("Shift+Ctrl+J", "Copy contact name")), this);
    auto openAccountListSC = new QShortcut(QKeySequence(tr("Ctrl+J", "Open account list")), this);
    auto focusOnSmartListSC = new QShortcut(QKeySequence(tr("Ctrl+L", "Focus on smart list")), this);
    auto focusOnSmartListNextElementSC = new QShortcut(QKeySequence(tr("Ctrl+Down", "Focus on the next element on smart list")), this);
    auto focusOnSmartListPrevElementSC = new QShortcut(QKeySequence(tr("Ctrl+Up", "Focus on the previous element on smart list")), this);
    auto focusOnContactSearchBarSC = new QShortcut(QKeySequence(tr("Ctrl+F", "Focus on contact search bar")), this);
    auto answerCallSC = new QShortcut(QKeySequence(tr("Ctrl+Y", "Answer an incoming call")), this);
    auto declineCallSC = new QShortcut(QKeySequence(tr("Ctrl+D", "Decline the call")), this);
    auto shortcutsTableSC = new QShortcut(QKeySequence(tr("F10", "Show keyboard shortcuts table")), this);
    auto maximizeScreenSC = new QShortcut(QKeySequence(tr("F11", "Maximize Screen")), this);

    auto navigateToMediaSettingsSC = new QShortcut(QKeySequence(tr("Ctrl+M", "Navigate to media setting")), this);
    auto navigateToGeneralSettingsSC = new QShortcut(QKeySequence(tr("Ctrl+G", "Navigate to general setting")), this);
    auto navigateToAccountSettingsSC = new QShortcut(QKeySequence(tr("Ctrl+I", "Navigate to account setting")), this);

    // connections
    connect(startAudioCallSC, &QShortcut::activated,
        [this] {
            auto convModel = LRCInstance::getCurrentConversationModel();
            auto convUid = LRCInstance::getCurrentConvUid();

            if (convModel && !convUid.isEmpty())
                convModel->placeAudioOnlyCall(convUid);
        });

    connect(startVideoCallSC, &QShortcut::activated,
        [this] {
            auto convModel = LRCInstance::getCurrentConversationModel();
            auto convUid = LRCInstance::getCurrentConvUid();

            if (convModel && !convUid.isEmpty())
                convModel->placeCall(convUid);
        });

    connect(clearConvHistorySC, &QShortcut::activated,
        [this] {
            auto convModel = LRCInstance::getCurrentConversationModel();
            auto convUid = LRCInstance::getCurrentConvUid();

            if (convModel && !convUid.isEmpty()) {
                auto reply = Utils::getReplyMessageBox(this,
                    QString("Clear Conversation History"),
                    QString("Do you really want to clear the conversation history with this contact ?"));
                if (reply)
                    convModel->clearHistory(convUid);
            }
        });

    connect(blockContactSC, &QShortcut::activated,
        [this] {
            auto convModel = LRCInstance::getCurrentConversationModel();
            auto convUid = LRCInstance::getCurrentConvUid();

            if (convModel && !convUid.isEmpty()) {
                auto reply = Utils::getReplyMessageBox(this,
                    QString("Block Contact"),
                    QString("Do you really want to block this contact ?"));
                if (reply)
                    convModel->removeConversation(convUid, true);
            }
        });

    connect(copyContactNameSC, &QShortcut::activated,
        [this] {
            QPointer<QClipboard> clipboard = QApplication::clipboard();
            clipboard->setText(
                Utils::bestIdForContact(LRCInstance::getCurrentAccountInfo().contactModel->getContact(LRCInstance::getCurrentConversation().participants.front())
            ));
        });

    connect(openAccountListSC, &QShortcut::activated,
        [this] {
            ui->currentAccountComboBox->activateComboBox();
        });

    connect(focusOnSmartListSC, &QShortcut::activated,
        [this] {
            ui->smartList->setCurrentIndex(ui->smartList->model()->index(0, 0));
        });

    connect(focusOnSmartListNextElementSC, &QShortcut::activated,
        [this] {
            if (ui->smartList->currentIndex().row() == ui->smartList->model()->rowCount() - 1) {
                return;
            } else {
                ui->smartList->setCurrentIndex(ui->smartList->model()->index(ui->smartList->currentIndex().row() + 1, 0));
            }
        });

    connect(focusOnSmartListPrevElementSC, &QShortcut::activated,
        [this] {
            if (ui->smartList->currentIndex().row() == 0) {
                return;
            } else {
                ui->smartList->setCurrentIndex(ui->smartList->model()->index(ui->smartList->currentIndex().row() - 1, 0));
            }
        });

    connect(focusOnContactSearchBarSC, &QShortcut::activated,
        [this] {
            ui->ringContactLineEdit->setFocus();
        });

    connect(answerCallSC, &QShortcut::activated,
        [this] {
            auto& convInfo = LRCInstance::getCurrentConversation();
            if (!convInfo.callId.isEmpty())
                LRCInstance::getCurrentCallModel()->accept(convInfo.callId);
        });

    connect(declineCallSC, &QShortcut::activated,
        [this] {
            auto& convInfo = LRCInstance::getCurrentConversation();
            auto status = LRCInstance::getCallInfoForConversation(convInfo)->status;
            if (!convInfo.callId.isEmpty()) {
                if (status == lrc::api::call::Status::IN_PROGRESS || status == lrc::api::call::Status::OUTGOING_RINGING) {
                    LRCInstance::getCurrentCallModel()->hangUp(convInfo.callId);
                } else if (status == lrc::api::call::Status::INCOMING_RINGING) {
                    LRCInstance::getCurrentCallModel()->refuse(convInfo.callId);
                }
            }
        });

    connect(maximizeScreenSC, &QShortcut::activated,
        [this] {
            if (MainWindow::instance().windowState().testFlag(Qt::WindowFullScreen) == true) {
                MainWindow::instance().showNormal();
            }
            else {
                MainWindow::instance().showFullScreen();
            }
        });

    connect(shortcutsTableSC, &QShortcut::activated,
        [this] {
            QUrl qmlSource{ QStringLiteral("qrc:/src/KeyBoardShortcutTable.qml") };
            QmlPopupWidget qmlKeyboardShortcuts(qmlSource, qobject_cast<MainWindow*>(parent()->parent()->parent()));
            qmlKeyboardShortcuts.setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
            qmlKeyboardShortcuts.setAttribute(Qt::WA_AlwaysStackOnTop);
            qmlKeyboardShortcuts.setClearColor(Qt::transparent);
            qmlKeyboardShortcuts.setMinimumWidth(qmlKeyboardShortcuts.rootObject()->property("minWidth").toInt());
            qmlKeyboardShortcuts.setMinimumHeight(qmlKeyboardShortcuts.rootObject()->property("minHeight").toInt());
            qmlKeyboardShortcuts.getContainer().toStrongRef()->exec();
        });

    connect(navigateToMediaSettingsSC, &QShortcut::activated,
        [this] {
            emit NavigationSettingsPageRequested(SettingsMenu::Media);
        });

    connect(navigateToGeneralSettingsSC, &QShortcut::activated,
        [this] {
            emit NavigationSettingsPageRequested(SettingsMenu::General);
        });

    connect(navigateToAccountSettingsSC, &QShortcut::activated,
        [this] {
            emit NavigationSettingsPageRequested(SettingsMenu::Account);
        });
}

void
CallWidget::ShowContextMenu(const QPoint& pos)
{
    Q_UNUSED(pos);
    ui->messageView->runJsText();
    connect(ui->messageView, &MessageWebView::textSelectedReady, this, &CallWidget::CreateCopyPasteContextMenu);
}

void
CallWidget::CreateCopyPasteContextMenu()
{
    disconnect(ui->messageView, &MessageWebView::textSelectedReady, this, &CallWidget::CreateCopyPasteContextMenu);
    QPoint globalMousePos = QCursor::pos();
    const QMimeData* mimeData = clipboard_->mimeData();

    QAction action1("Copy", this);
    QAction action2("Paste", this);

    if (ui->messageView->textSelected()) {
        menu_->addAction(&action1);
        connect(&action1, SIGNAL(triggered()), this, SLOT(Copy()));
    }
    if (mimeData->hasImage() || mimeData->hasUrls() || mimeData->hasText()) {
        menu_->addAction(&action2);
        connect(&action2, SIGNAL(triggered()), this, SLOT(Paste()));
    }
    menu_->exec(globalMousePos);
}

void
CallWidget::Paste()
{
    const QMimeData* mimeData = clipboard_->mimeData();

    if (mimeData->hasImage()) {

        //save temp data into base64 format
        QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
        QByteArray ba;
        QBuffer bu(&ba);
        bu.open(QIODevice::WriteOnly);
        pixmap.save(&bu, "PNG");
        auto str = QString::fromLocal8Bit(ba.toBase64());

        ui->messageView->setMessagesImageContent(str, true);
    }
    else if (mimeData->hasUrls()) {

        QList<QUrl> urlList = mimeData->urls();
        // extract the local paths of the files
        for (int i = 0; i < urlList.size(); ++i) {
            // Trim file:/// from url
            QString filePath = urlList.at(i).toString().remove(0, 8);
            QByteArray imageFormat = QImageReader::imageFormat(filePath);

            //check if file is qt supported image file type
            if (!imageFormat.isEmpty()) {
                ui->messageView->setMessagesImageContent(filePath);
            } else {
                ui->messageView->setMessagesFileContent(filePath);
            }
        }
    } else {
        ui->messageView->setMessagesContent(mimeData->text());
    }
}

void
CallWidget::Copy()
{
    ui->messageView->copySelectedText(clipboard_);
}