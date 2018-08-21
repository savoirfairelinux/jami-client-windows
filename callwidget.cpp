/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include <memory>

#include "qrencode.h"

//ERROR is defined in windows.h
#include "utils.h"
#undef ERROR
#undef interface

//LRC
#include "audio/settings.h"
#include "accountmodel.h"
#include "personmodel.h"
#include "person.h"
#include "media/text.h"
#include "media/recording.h"
#include "media/recordingmodel.h"
#include "recentmodel.h"
#include "contactmethod.h"
#include "globalinstances.h"
#include <availableaccountmodel.h>
#include "pendingcontactrequestmodel.h"
#include "profilemodel.h"
#include "profile.h"
#include "peerprofilecollection.h"
#include "localprofilecollection.h"
#include "callmodel.h"

//Client
#include "wizarddialog.h"
#include "windowscontactbackend.h"
#include "contactpicker.h"
#include "contactmethodpicker.h"
#include "globalsystemtray.h"
#include "smartlistdelegate.h"
#include "imdelegate.h"
#include "pixbufmanipulator.h"
#include "settingskey.h"
#include "deletecontactdialog.h"

#include "lrcinstance.h"
#include "messagemodel.h"

CallWidget::CallWidget(QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::CallWidget),
    menu_(new QMenu()),
    imDelegate_(new ImDelegate())
{
    ui->setupUi(this);

    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);

    pageAnim_ = new QPropertyAnimation(ui->welcomePage, "pos", this);

    videoRenderer_ = nullptr;

    connect(ui->settingsButton, &QPushButton::clicked, this, &CallWidget::settingsButtonClicked);

    connect(ui->videoWidget, SIGNAL(setChatVisibility(bool)),
            ui->instantMessagingWidget, SLOT(setVisible(bool)));

    QPixmap logo(":/images/logo-ring-standard-coul.png");
    ui->ringLogo->setPixmap(logo.scaledToHeight(100, Qt::SmoothTransformation));
    ui->ringLogo->setAlignment(Qt::AlignHCenter);

    ui->qrLabel->hide();

    ProfileModel::instance().addCollection<LocalProfileCollection>(LoadOptions::FORCE_ENABLED);

    // conversation list
    smartListDelegate_ = new SmartListDelegate();
    ui->smartList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->sendContactRequestWidget, &SendContactRequestWidget::sendCRclicked,
        [=] {
            Utils::setStackWidget(ui->stackedWidget, ui->messagingPage);
        });

    connect(ui->currentAccountWidget, &CurrentAccountWidget::currentAccountChanged,
            this, &CallWidget::currentAccountChanged);

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
        } else {
            accountIdToStartWith = accountList.at(0);
        }
        setSelectedAccount(accountIdToStartWith);
        // get account index and set the currentAccountWidget selector
        auto index = Utils::indexInVector(accountList, accountIdToStartWith);
        if (index != -1) {
            ui->currentAccountWidget->changeSelectedIndex(index);
        }
    }

    // This connect() is used to initialise and track changes of profile's picture
    connect(&ProfileModel::instance(), &ProfileModel::dataChanged,
        ui->currentAccountWidget, &CurrentAccountWidget::setPhoto);

    connect(ui->videoWidget, &VideoView::videoSettingsClicked, this, &CallWidget::settingsButtonClicked);

    // TODO:(newlrc) contextMenu
    //connect(ui->smartList, &QListView::customContextMenuRequested, [=](const QPoint& pos){ setupSmartListMenu(pos);});

    auto& behaviorController = LRCInstance::behaviorController();
    connect(&behaviorController,
        &lrc::api::BehaviorController::showCallView,
        [this](const std::string& accountId,
               const lrc::api::conversation::Info& convInfo) {
                qDebug() << "BehaviorController::showCallView";
                ui->stackedWidget->setCurrentWidget(ui->videoPage);
                hideMiniSpinner();
        });

    connect(&behaviorController,
        &lrc::api::BehaviorController::showIncomingCallView,
        [this](const std::string& accountId,
               const lrc::api::conversation::Info& convInfo) {
                qDebug() << "BehaviorController::showIncomingCallView";
                auto callModel = LRCInstance::getCurrentCallModel();
                auto call = callModel->getCall(convInfo.callId);
                if (call.isOutgoing) {
                    miniSpinner_->start();
                    ui->spinnerLabel->show();
                    ui->stackedWidget->setCurrentWidget(ui->outboundCallPage);
                } else {
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
                        GlobalSystemTray::instance()
                            .showMessage("Ring",
                                QString(tr("Call incoming from %1")).arg(QString::fromStdString(formattedName)));
                        QApplication::alert(this, 5000);
                    }
                }

                if (!callModel->hasCall(convInfo.callId)) {
                    return;
                }

                ui->videoWidget->pushRenderer(convInfo.callId);
                // TODO:(new lrc)
                //ui->instantMessagingWidget->setMediaText(actualCall_);

                disconnect(selectedCallChanged_);
                selectedCallChanged_ = connect(
                    callModel,
                    &lrc::api::NewCallModel::callStatusChanged,
                    [this, callModel](const std::string& callUid) {
                        auto call = callModel->getCall(callUid);
                        qDebug() << "NewCallModel::callStatusChanged: " << static_cast<int>(call.status);
                    }
                );

                ui->callerPhoto->setPixmap(QPixmap::fromImage(imageForSelectedConv()));
        });

    connect(&behaviorController,
        &lrc::api::BehaviorController::showChatView,
        [this](const std::string& accountId,
               const lrc::api::conversation::Info& convInfo) {
                qDebug() << "BehaviorController::showChatView";
                showConversationView();
        });

    // setup searchingfor mini spinner
    miniSpinner_ = new QMovie(":/images/waiting.gif");
    ui->spinnerLabel->setMovie(miniSpinner_);
    ui->spinnerLabel->hide();

    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    setupOutOfCallIM();
}

CallWidget::~CallWidget()
{
    delete ui;
    delete menu_;
    delete imDelegate_;
    delete pageAnim_;
    delete smartListDelegate_;
    delete shareMenu_;
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

    // Todo new lrc
    connect(&::media::RecordingModel::instance(),
            SIGNAL(newTextMessage(::media::TextRecording*, ContactMethod*)),
            this,
            SLOT(onIncomingMessage(::media::TextRecording*, ContactMethod*)));
}

void
CallWidget::onIncomingMessage(::media::TextRecording* t, ContactMethod* cm)
{
    Q_UNUSED(cm)

    if (!QApplication::focusWidget()) {
        auto idx = t->instantTextMessagingModel()->
                index(t->instantTextMessagingModel()->rowCount()-1, 0);
        GlobalSystemTray::instance()
                .showMessage("Ring",
                             QString(tr("Message incoming from %1")).arg(
                                 idx.data((int)media::TextRecording::Role::AuthorDisplayname).toString()));
        QApplication::alert(this, 5000);
    }
}

void
CallWidget::onIncomingMessage(const lrc::api::interaction::Info& interaction)
{
}

void
CallWidget::triggerDeleteContactDialog(ContactMethod *cm, Account *ac)
{
    auto dlg = new DeleteContactDialog(cm, ac);
    dlg->exec();
}

// TODO:(newlrc) Context Menu
void
CallWidget::setupSmartListMenu(const QPoint& pos)
{
    auto idx = ui->smartList->currentIndex();

    if (not idx.isValid())
        return;

    QPoint globalPos = ui->smartList->mapToGlobal(pos);
    QMenu menu;
    QVector<ContactMethod*> contactMethods = RecentModel::instance()
            .getContactMethods(RecentModel::instance().peopleProxy()->mapToSource(idx));

    if (contactMethods.isEmpty())
        return;

    auto contactMethod = contactMethods.first();

    if (contactMethods.size() == 1){
        auto copyAction = new QAction(tr("Copy number"), this);
        menu.addAction(copyAction);
        connect(copyAction, &QAction::triggered, [contactMethod]() {
            QApplication::clipboard()->setText(contactMethod->uri());
        });

        auto copyNameAction = new QAction(tr("Copy name"), this);
        menu.addAction(copyNameAction);
        connect(copyNameAction, &QAction::triggered, [contactMethod]() {
            QApplication::clipboard()->setText(contactMethod->primaryName());
        });
    } else {
        auto callMenu = menu.addMenu(tr("Call Number"));
        auto copyMenu = menu.addMenu(tr("Copy Number"));
        for (auto cM : contactMethods) {
            auto uri = cM->bestId();
            auto copyAction = new QAction(tr("Copy %1").arg(uri), this);
            copyMenu->addAction(copyAction);
            connect(copyAction, &QAction::triggered, [uri]() {
                QApplication::clipboard()->setText(uri);
            });
            auto callAction = new QAction(tr("Call %1").arg(uri), this);
            callMenu->addAction(callAction);
            connect(callAction, &QAction::triggered, [cM]() {
                Call* c = CallModel::instance().dialingCall(cM);
                c->performAction(Call::Action::ACCEPT);
            });
        }
    }

    //auto ac = LRCInstance::getSelectedAccount();
    //if (ac && !ac->hasContact(contactMethod)) {
    //    auto addExisting = new QAction(tr("Add to contacts"), this);
    //    menu.addAction(addExisting);
    //    connect(addExisting, &QAction::triggered, [this, contactMethod, ac]() {
    //        /* uncomment and capture globalPos in lambda to reactivate popup
    //        ContactPicker contactPicker(contactMethod);
    //        contactPicker.move(globalPos.x(), globalPos.y() - (contactPicker.height()/2));
    //        contactPicker.exec();
    //        */
    //        ac->addContact(contactMethod);
    //    });
    //} else if (ac) {
    //    auto removeExisting = new QAction(tr("Remove from contacts"), this);
    //    menu.addAction(removeExisting);
    //    connect(removeExisting, &QAction::triggered, [this, contactMethod, ac]() {
    //        triggerDeleteContactDialog(contactMethod, ac);
    //    });
    //}

    menu.exec(globalPos);
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
    ui->qrLabel->setPixmap(QPixmap::fromImage(result.scaled(QSize(qrSize_, qrSize_), Qt::KeepAspectRatio)));
}

bool
CallWidget::findRingAccount()
{
    bool ringAccountFound = false;
    auto accountList = LRCInstance::accountModel().getAccountList();
    for (int i = 0; i < accountList.size(); ++i) {
        auto accountId = accountList.at(i);
        auto& accountInfo = LRCInstance::accountModel().getAccountInfo(accountId);
        if (accountInfo.profileInfo.type == lrc::api::profile::Type::RING) {
            ringAccountFound = true;
            if (accountInfo.status == lrc::api::account::Status::ERROR_NEED_MIGRATION) {
                WizardDialog dlg(WizardDialog::MIGRATION);
                dlg.exec();
            }
        }
    }

    if (!ringAccountFound) {
        WizardDialog wizardDialog;
        wizardDialog.exec();
        if (wizardDialog.result() != QDialog::Accepted) {
            return false;
        }
    }

    ui->currentAccountWidget->update();

    return true;
}

void
CallWidget::on_smartList_clicked(const QModelIndex& index)
{
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

    ui->callingPhoto->setPixmap(QPixmap::fromImage(imageForSelectedConv()));
}



QImage
CallWidget::imageForSelectedConv()
{
    auto conversation = Utils::getConversationFromUid(selectedConvUid(),
        *LRCInstance::getCurrentConversationModel());
    if (conversation == (*LRCInstance::getCurrentConversationModel()).allFilteredConversations().end()) {
        return QImage();
    }

    QVariant var = GlobalInstances::pixmapManipulator().decorationRole(*conversation, LRCInstance::getCurrentAccountInfo());
    return var.value<QImage>();
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
CallWidget::settingsButtonClicked()
{
    emit NavigationRequested(ScreenEnum::ConfScreen);
}

void
CallWidget::processContactLineEdit()
{
    auto contactLineText = ui->ringContactLineEdit->text();
    LRCInstance::getCurrentConversationModel()->setFilter(contactLineText.toStdString());
}

void
CallWidget::on_ringContactLineEdit_returnPressed()
{
    // TODO
    // select current temporary item and show IM
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversations = convModel->allFilteredConversations();
    auto firstContactUid = conversations.at(0).participants.at(0);
    if (Utils::isContactValid(conversations.at(0).participants.at(0), *convModel)) {
        //selectConversation(conversations.at(0), *convModel);
        selectConversation(smartListModel_->index(0));
    }
}

void
CallWidget::currentAccountChanged(const QModelIndex &current)
{
    qDebug() << "CallWidget::currentAccountChanged";

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
    } else {
        ui->ringIdLabel->setText(tr("NO RING ACCOUNT FOUND"));
    }

    updateSmartList();
    connectConversationModel();
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
    ui->sendContactRequestPageButton->setVisible(shouldShowSendContactRequestBtn);

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
    if (msg.trimmed().isEmpty()) return;
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
CallWidget::on_sendContactRequestPageButton_clicked()
{
    LRCInstance::getCurrentConversationModel()->makePermanent(selectedConvUid());
}

void
CallWidget::on_sendCRBackButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->messagingPage);
}

void
CallWidget::on_pendingCRBackButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
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

    // reload smartlist data;
    modelSortedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::modelSorted,
        [this]() {
            qDebug() << "modelSorted";
            //updateSmartList();
            // reselect correct item
            selectSmartlistItem(selectedConvUid());
            ui->smartList->update();
        }
    );
    modelUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationUpdated,
        [this](const std::string& convUid) {
            qDebug() << "conversationUpdated";
            ui->smartList->update();
            // reloadConversationWithUid : convUid
        }
    );
    filterChangedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::filterChanged,
        [this]() {
            qDebug() << "filterChanged";
            updateSmartList();
            ui->smartList->update();
        }
    );
    newConversationConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::newConversation,
        [this](const std::string& convUid) {
            qDebug() << "newConversation";
            // reload smartlist data;
            // updateConversationForNewContact : convUid
            updateConversationForNewContact(convUid);
        }
    );
    conversationRemovedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationRemoved,
        [this]() {
            // switch to smartlist
            // reload smartlist data;
        }
    );
    conversationClearedConnection = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationCleared,
        [this](const std::string& convUid) {
            qDebug() << "conversationCleared";
            // deselect smartlist item
            // switch to smartlist
        }
    );
    interactionStatusUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::interactionStatusUpdated,
        [this](const std::string& convUid) {
            qDebug() << "interactionStatusUpdated";
            if (convUid != selectedConvUid()) {
                return;
            }
            updateConversationView(convUid);
        }
    );
    newInteractionConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::newInteraction,
        [this](const std::string& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction) {
            qDebug() << "newInteraction";
            updateConversationView(convUid);
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
    if (convUid != selectedConvUid()) {
        return;
    }

    auto& currentAccountInfo = LRCInstance::getCurrentAccountInfo();
    auto currentConversationModel = currentAccountInfo.conversationModel.get();
    currentConversationModel->clearUnreadInteractions(convUid);
    auto currentConversation = Utils::getConversationFromUid(selectedConvUid(),
                                                             *currentConversationModel);
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
        if (!previousConvUid_.compare(convUid)) {
            return;
        }
        previousConvUid_.assign(convUid);
        auto currentConversationModel = LRCInstance::getCurrentConversationModel();
        auto callModel = LRCInstance::getCurrentCallModel();
        auto conversation = Utils::getConversationFromUid(convUid, *currentConversationModel);
        Call* thisCall = nullptr;
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
    ui->smartList->setItemDelegate(smartListDelegate_);

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
    qDebug() << "CallWidget::update";
    ui->currentAccountWidget->update();
    updateSmartList();
    connectConversationModel();
}