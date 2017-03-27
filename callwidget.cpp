/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "audio/settings.h"
#include "personmodel.h"
#include "person.h"
#include "fallbackpersoncollection.h"
#include "categorizedcontactmodel.h"
#include "localhistorycollection.h"
#include "media/text.h"
#include "media/recording.h"
#include "media/recordingmodel.h"
#include "recentmodel.h"
#include "contactmethod.h"
#include "globalinstances.h"
#include <availableaccountmodel.h>
#include "wizarddialog.h"
#include "windowscontactbackend.h"
#include "contactpicker.h"
#include "contactmethodpicker.h"
#include "globalsystemtray.h"
#include "historydelegate.h"
#include "contactdelegate.h"
#include "smartlistdelegate.h"
#include "imdelegate.h"
#include "pixbufmanipulator.h"
#include "settingskey.h"

#include "profilemodel.h"
#include "peerprofilecollection.h"
#include "localprofilecollection.h"

CallWidget::CallWidget(QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::CallWidget),
    menu_(new QMenu()),
    imDelegate_(new ImDelegate())
{
    ui->setupUi(this);

    pageAnim_ = new QPropertyAnimation(ui->welcomePage, "pos", this);

    setActualCall(nullptr);
    videoRenderer_ = nullptr;

    connect(ui->settingsButton, &QPushButton::clicked, this, &CallWidget::settingsButtonClicked);

    connect(ui->videoWidget, SIGNAL(setChatVisibility(bool)),
            ui->instantMessagingWidget, SLOT(setVisible(bool)));

    QPixmap logo(":/images/logo-ring-standard-coul.png");
    ui->ringLogo->setPixmap(logo.scaledToHeight(100, Qt::SmoothTransformation));
    ui->ringLogo->setAlignment(Qt::AlignHCenter);

    ui->qrLabel->hide();

    GlobalInstances::setPixmapManipulator(std::unique_ptr<Interfaces::PixbufManipulator>(new Interfaces::PixbufManipulator()));

    try {
        callModel_ = &CallModel::instance();

        connect(callModel_, SIGNAL(incomingCall(Call*)),
                this, SLOT(callIncoming(Call*)));
        connect(callModel_, SIGNAL(callStateChanged(Call*, Call::State)),
                this, SLOT(callStateChanged(Call*, Call::State)));

        connect(&AccountModel::instance()
                , SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>))
                , this
                , SLOT(findRingAccount(QModelIndex, QModelIndex, QVector<int>)));

        RecentModel::instance().peopleProxy()->setFilterRole(static_cast<int>(Ring::Role::Name));
        RecentModel::instance().peopleProxy()->setFilterCaseSensitivity(Qt::CaseInsensitive);
        ui->smartList->setModel(RecentModel::instance().peopleProxy());

        smartListDelegate_ = new SmartListDelegate();
        ui->smartList->setSmartListItemDelegate(smartListDelegate_);

        PersonModel::instance().addCollection<PeerProfileCollection>(LoadOptions::FORCE_ENABLED);
        ProfileModel::instance().addCollection<LocalProfileCollection>(LoadOptions::FORCE_ENABLED);

        PersonModel::instance().
                addCollection<WindowsContactBackend>(LoadOptions::FORCE_ENABLED);

        connect(ui->smartList, &QTreeView::entered, this, &CallWidget::on_entered);

        smartListDelegate_ = new SmartListDelegate();
        ui->smartList->setSmartListItemDelegate(smartListDelegate_);

        findRingAccount();
        setupOutOfCallIM();
        setupSmartListMenu();

        connect(ui->smartList, &SmartList::btnVideoClicked, this, &CallWidget::btnComBarVideoClicked);

        connect(RecentModel::instance().selectionModel(),
                SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this,
                SLOT(smartListCurrentChanged(QModelIndex,QModelIndex)));

        connect(RecentModel::instance().selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selected, const QItemSelection &deselected) {
                    // lambda used to focus on the correct smartList element when switching automatically between two calls
                    Q_UNUSED(deselected)
                    if (selected.size()) {
                        auto idx = selected.indexes().first();
                        auto realIdx = RecentModel::instance().peopleProxy()->mapFromSource(idx);
                        ui->smartList->selectionModel()->setCurrentIndex(realIdx, QItemSelectionModel::ClearAndSelect);
                    } else
                        ui->smartList->clearSelection();
                });

        connect(&NameDirectory::instance(), SIGNAL(registeredNameFound(const Account*,NameDirectory::LookupStatus,const QString&,const QString&)),
                this, SLOT(contactLineEdit_registeredNameFound(const Account*,NameDirectory::LookupStatus,const QString&,const QString&)));

    } catch (const std::exception& e) {
        qDebug() << "INIT ERROR" << e.what();
    }
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
    QSettings settings;
    auto displayDate = new QAction(tr("Display date"), this);
    displayDate->setCheckable(true);
    displayDate->setChecked(settings.value(SettingsKey::imShowDate).toBool());
    ui->listMessageView->addAction(displayDate);
    auto displayAuthor = new QAction(tr("Display author"), this);
    displayAuthor->setCheckable(true);
    displayAuthor->setChecked(settings.value(SettingsKey::imShowAuthor).toBool());
    ui->listMessageView->addAction(displayAuthor);
    auto lamdba = [=](){
        QSettings settings;
        settings.setValue(SettingsKey::imShowAuthor, displayAuthor->isChecked());
        settings.setValue(SettingsKey::imShowDate, displayDate->isChecked());
        emit imDelegate_->sizeHintChanged(QModelIndex());
    };
    connect(displayAuthor, &QAction::triggered, lamdba);
    connect(displayDate, &QAction::triggered, lamdba);

    connect(&::Media::RecordingModel::instance(),
            SIGNAL(newTextMessage(::Media::TextRecording*, ContactMethod*)),
            this,
            SLOT(onIncomingMessage(::Media::TextRecording*, ContactMethod*)));
}

void
CallWidget::onIncomingMessage(::Media::TextRecording* t, ContactMethod* cm) {
    Q_UNUSED(cm)

    if (!QApplication::focusWidget()) {
        auto idx = t->instantTextMessagingModel()->
                index(t->instantTextMessagingModel()->rowCount()-1, 0);
        GlobalSystemTray::instance()
                .showMessage("Ring",
                             QString(tr("Message incoming from %1")).arg(
                                 idx.data((int)Media::TextRecording::Role::AuthorDisplayname).toString()));
        QApplication::alert(this, 5000);
    }
}

void
CallWidget::setupSmartListMenu() {
    ui->smartList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->smartList, &QListView::customContextMenuRequested, [=](const QPoint& pos){
        auto idx = ui->smartList->currentIndex();
        if (not idx.isValid())
            return;
        QPoint globalPos = ui->smartList->mapToGlobal(pos);
        QMenu menu;

        QVector<ContactMethod*> contactMethods = RecentModel::instance()
                .getContactMethods(RecentModel::instance().peopleProxy()->mapToSource(idx));
        if (contactMethods.isEmpty())
            return;

        if (contactMethods.size() == 1) {
            auto contactMethod = contactMethods.at(0);
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
            if (not contactMethod->contact() || contactMethod->contact()->isPlaceHolder()) {
                auto addExisting = new QAction(tr("Add to contact"), this);
                menu.addAction(addExisting);
                connect(addExisting, &QAction::triggered, [globalPos, contactMethod]() {
                    ContactPicker contactPicker(contactMethod);
                    contactPicker.move(globalPos.x(), globalPos.y() - (contactPicker.height()/2));
                    contactPicker.exec();
                });
            }
        }
        else {
           auto callMenu = menu.addMenu(tr("Call Number"));
           auto copyMenu = menu.addMenu(tr("Copy Number"));
           for (auto cM : contactMethods) {
               auto uri = cM->uri();
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
        menu.exec(globalPos);
    });
}

void
CallWidget::findRingAccount(QModelIndex idx1, QModelIndex idx2, QVector<int> vec)
{
    Q_UNUSED(idx1)
    Q_UNUSED(idx2)
    Q_UNUSED(vec)

    auto a_count = AccountModel::instance().rowCount();
    for (int i = 0; i < a_count; ++i) {
        auto idx = AccountModel::instance().index(i, 0);
        auto protocol = idx.data(static_cast<int>(Account::Role::Proto));
        if (static_cast<Account::Protocol>(protocol.toUInt()) == Account::Protocol::RING) {
            auto account = AccountModel::instance().getAccountByModelIndex(idx);
            auto registeredName = account->registeredName();
            auto username = idx.data(static_cast<int>(Account::Role::Username));
            if (registeredName.isEmpty()) {
                ui->ringIdLabel->setText(username.toString());
            } else
                ui->ringIdLabel->setText(registeredName);
            setupQRCode(username.toString());
            return;
        }
    }
    ui->ringIdLabel->setText(tr("NO RING ACCOUNT FOUND"));
}

void CallWidget::setupQRCode(QString ringID)
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

void
CallWidget::findRingAccount()
{
    ui->ringIdLabel->clear();

    auto a_count = AccountModel::instance().rowCount();
    for (int i = 0; i < a_count; ++i) {
        auto idx = AccountModel::instance().index(i, 0);
        auto protocol = idx.data(static_cast<int>(Account::Role::Proto));
        if (static_cast<Account::Protocol>(protocol.toUInt()) == Account::Protocol::RING) {
            auto account = AccountModel::instance().getAccountByModelIndex(idx);
            if (account->displayName().isEmpty())
                account->displayName() = account->alias();
            if (account->needsMigration()) {
                WizardDialog dlg(WizardDialog::MIGRATION, account);
                dlg.exec();
            }
            if (ui->ringIdLabel->text().isEmpty()) {
                auto registeredName = account->registeredName();
                ui->ringIdLabel->setText((registeredName.isEmpty())?account->username():registeredName);
                setupQRCode(account->username());
            }
        }
    }
    if (ui->ringIdLabel->text().isEmpty()) {
        ui->ringIdLabel->setText(tr("NO RING ACCOUNT FOUND"));
        WizardDialog wizardDialog;
        wizardDialog.exec();
    }
}

void
CallWidget::callChangedSlot()
{
    if (not actualCall_)
        return;
    ui->callerIdLabel->setText(QString("%1")
                               .arg(actualCall_->formattedName()));
    ui->callerPhoto->setPixmap(
                QPixmap::fromImage(
                    GlobalInstances::pixmapManipulator()
                    .callPhoto(actualCall_, QSize(130,130)).value<QImage>()));

    if(actualCall_->state() == Call::State::OVER || actualCall_->state() == Call::State::FAILURE){
        ui->outboundCallLabel->setText(actualCall_->toHumanStateName(Call::State::INITIALIZATION));
    } else {
        ui->outboundCallLabel->setText(actualCall_->toHumanStateName());
    }
}

void
CallWidget::callIncoming(Call* call)
{
    if (!QApplication::focusWidget()) {
        GlobalSystemTray::instance()
                .showMessage("Ring",
                             QString(tr("Call incoming from %1")).arg(call->formattedName()));
        QApplication::alert(this, 5000);
    }

    setActualCall(call);
}

void
CallWidget::on_acceptButton_clicked()
{
    if (actualCall_ != nullptr)
        actualCall_->performAction(Call::Action::ACCEPT);
}

void
CallWidget::on_refuseButton_clicked()
{
    if (actualCall_ == nullptr)
        return;
    actualCall_->performAction(Call::Action::REFUSE);
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    setActualCall(nullptr);
}

void
CallWidget::callStateChanged(Call* call, Call::State previousState)
{
    Q_UNUSED(previousState)
    if (call == nullptr
            || call != actualCall_)
        return;

    callStateToView(call);

    if (call->state() == Call::State::OVER) {
        setActualCall(nullptr);
        ui->smartList->clearSelection();
        RecentModel::instance().selectionModel()->clear();
    }
}

void
CallWidget::atExit()
{
}

void CallWidget::callStateToView(Call* value)
{
    if (value) {
        switch (value->state()) {
        case Call::State::INCOMING:
            if (not value->account()->isAutoAnswer())
                ui->stackedWidget->setCurrentWidget(ui->callInvitePage);
            else
                ui->stackedWidget->setCurrentWidget(ui->videoPage);
            break;
        case Call::State::CURRENT:
        case Call::State::CONFERENCE:
        case Call::State::HOLD:
            ui->stackedWidget->setCurrentWidget(ui->videoPage);
            break;
        case Call::State::OVER:
            ui->stackedWidget->setCurrentWidget(ui->welcomePage);
            break;
        case Call::State::FAILURE:
        case Call::State::ERROR:
            on_cancelButton_clicked();
            break;
        case Call::State::INITIALIZATION:
        case Call::State::CONNECTED:
        case Call::State::RINGING:
            ui->stackedWidget->setCurrentWidget(ui->outboundCallPage);
            break;
        default:
            qWarning() << "Call state not handled doing nothing : "
                       << value->toHumanStateName();
            break;
        }
    }
}

void
CallWidget::setActualCall(Call* value)
{
    actualCall_ = value;
    CallModel::instance().selectCall(value);
    ui->videoWidget->pushRenderer(value);
    ui->instantMessagingWidget->setMediaText(actualCall_);
    callStateToView(value);
    if (actualCall_) {
        callChangedConnection_ = connect(actualCall_,
                                         SIGNAL(changed()),
                                         this,
                                         SLOT(callChangedSlot()));
    } else
        disconnect(callChangedConnection_);

}

void
CallWidget::on_cancelButton_clicked()
{
    if (actualCall_){
        actualCall_->performAction(Call::Action::REFUSE);
        ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    }
}

void
CallWidget::on_smartList_doubleClicked(const QModelIndex& index)
{
    auto realIndex = RecentModel::instance().peopleProxy()->mapToSource(index);
    if (RecentModel::instance().hasActiveCall(realIndex))
        return;

    ContactMethod* m = nullptr;
    if (auto cm = realIndex.data((int)Call::Role::ContactMethod).value<ContactMethod*>()) {
        m = cm;
    } else {
        if (auto person =  realIndex.data((int)Person::Role::Object).value<Person*>()) {
            m = person->phoneNumbers().first();
        }
    }

    if (m && !RecentModel::instance().index(0, 0, realIndex).isValid()) {

        QPixmap map = QPixmap::fromImage(
                        GlobalInstances::pixmapManipulator().callPhoto(m, QSize(130,130)).value<QImage>());
        ui->callingPhoto->setPixmap(map);

        Call* c = CallModel::instance().dialingCall(m);
        c->performAction(Call::Action::ACCEPT);
        setActualCall(c);
    }
}

void
CallWidget::smartListCurrentChanged(const QModelIndex &currentIdx, const QModelIndex &previousIdx)
{
    Q_UNUSED(previousIdx);
    if (not currentIdx.isValid())
        return;

    //catch call of current index
    auto currentIdxCall = RecentModel::instance().getActiveCall(currentIdx);

    if (currentIdxCall && currentIdxCall != actualCall_) { //if it is different from actual call, switch between the two
        setActualCall(currentIdxCall);
    } else if (currentIdxCall == nullptr){ // if there is no call attached to this smartlist index (contact tab)
        setActualCall(nullptr);
        showIMOutOfCall(currentIdx); // change page to contact request of messaging page with correct behaviour
    } else { // if non defined behaviour disconnect instant messaging and return to welcome page
        setActualCall(nullptr);
        if (imConnection_)
            disconnect(imConnection_);
        ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    }
}

void
CallWidget::configureSendCRPageButton(const QModelIndex& currentIdx)
{
    auto cmVector = RecentModel::instance().getContactMethods(currentIdx);
    URI cmUri = cmVector[0]->uri();
    if (cmUri.protocolHint() == URI::ProtocolHint::RING || uriNeedNameLookup(cmUri)) {
        ui->sendContactRequestPageButton->show();
    } else {
        ui->sendContactRequestPageButton->hide();
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
CallWidget::searchContactLineEditEntry(const URI &uri)
{
    auto cm = PhoneDirectoryModel::instance().getNumber(uri);
    // if its a new CM, bring it to the top
    if (cm->lastUsed() == 0)
        cm->setLastUsed(QDateTime::currentDateTime().toTime_t());

    // select cm
    RecentModel::instance().selectionModel()->setCurrentIndex(RecentModel::instance().getIndex(cm),
                                                              QItemSelectionModel::ClearAndSelect);
    ui->ringContactLineEdit->clear();
}

bool
CallWidget::uriNeedNameLookup(const URI uri_passed)
{
    if (uri_passed.protocolHint() == URI::ProtocolHint::RING_USERNAME ) {
        return TRUE;
    } else if (
        uri_passed.protocolHint() != URI::ProtocolHint::RING && // not a RingID
        uri_passed.schemeType() == URI::SchemeType::NONE // scheme type not specified
    ){
        // if no scheme type has been specified, determine ring vs sip by the first available account
        auto idx = AvailableAccountModel::instance().index(0, 0);
        if (idx.isValid()) {
            auto account = idx.data((int)Ring::Role::Object).value<Account *>();
            if (account && account->protocol() == Account::Protocol::RING)
                return TRUE;
        }
    }

    return FALSE;
}

void
CallWidget::on_ringContactLineEdit_returnPressed()
{
    auto contactLineText = ui->ringContactLineEdit->text();
    URI uri_passed = URI(contactLineText);

    if (!contactLineText.isNull() && !contactLineText.isEmpty()){
        if (uriNeedNameLookup(uri_passed)){
            NameDirectory::instance().lookupName(nullptr, QString(), uri_passed);
        } else {
            searchContactLineEditEntry(uri_passed);
        }
    }
}

void
CallWidget::on_btnCall_clicked()
{
    placeCall();
}

void
CallWidget::btnComBarVideoClicked()
{
    if (not highLightedIndex_.isValid())
        return;

    on_smartList_doubleClicked(highLightedIndex_);
}

void
CallWidget::showIMOutOfCall(const QModelIndex& nodeIdx)
{
    ui->contactMethodComboBox->clear();
    configureSendCRPageButton(nodeIdx);
    ui->imNameLabel->setText(QString(tr("Conversation with %1", "%1 is the contact name"))
                             .arg(nodeIdx.data(static_cast<int>(Ring::Role::Name)).toString()));
    auto cmVector = RecentModel::instance().getContactMethods(nodeIdx);
    ui->contactMethodComboBox->setEnabled(cmVector.size() > 1);
    foreach (const ContactMethod* cm, cmVector) {
        ui->contactMethodComboBox->addItem(cm->uri());
    }
    ui->stackedWidget->setCurrentWidget(ui->messagingPage);
    disconnect(imClickedConnection_);
    imClickedConnection_ = connect(ui->listMessageView, &QListView::clicked, [this](const QModelIndex& index) {
        auto urlList = index.data(static_cast<int>(Media::TextRecording::Role::LinkList)).value<QList<QUrl>>();
        if (urlList.size() == 1)
            QDesktopServices::openUrl(urlList.at(0));
        else if (urlList.size()) {
            //TODO Handle multiple url in one message
        }
    });
}

void
CallWidget::on_sendIMButton_clicked()
{
    if (ui->imMessageEdit->text().trimmed().isEmpty())
        return;
    auto number = ui->contactMethodComboBox->currentText();
    if (auto cm = PhoneDirectoryModel::instance().getNumber(number)) {
        QMap<QString, QString> msg;
        msg["text/plain"] = ui->imMessageEdit->text();
        cm->sendOfflineTextMessage(msg);
        ui->imMessageEdit->clear();
    } else {
        qWarning() << "Contact Method not found for " << number;
    }
}

void
CallWidget::on_imMessageEdit_returnPressed()
{
    on_sendIMButton_clicked();
}

void
CallWidget::on_contactMethodComboBox_currentIndexChanged(const QString& number)
{
    auto cm = PhoneDirectoryModel::instance().getNumber(number);
    if (auto txtRecording = cm->textRecording()) {
        ui->listMessageView->setModel(txtRecording->instantMessagingModel());
        if (imConnection_)
            disconnect(imConnection_);
        imConnection_ = connect(txtRecording,
                                SIGNAL(messageInserted(QMap<QString,QString>,ContactMethod*,Media::Media::Direction)),
                                this,
                                SLOT(slotAccountMessageReceived(QMap<QString,QString>,ContactMethod*,Media::Media::Direction)));
        auto messagesPresent = txtRecording->instantMessagingModel()->rowCount() > 0;
        if (messagesPresent) {
            ui->listMessageView->scrollToBottom();
            txtRecording->setAllRead();
        }
        ui->listMessageView->setVisible(messagesPresent);
        ui->noMessagesLabel->setVisible(!messagesPresent);
        if (not messagesPresent) {
            imVisibleConnection_ = connect(txtRecording->instantMessagingModel(),
                                           &QAbstractItemModel::rowsInserted,
                                           [this, txtRecording]() {
                if (imVisibleConnection_)
                    disconnect(imVisibleConnection_);
                auto messagesPresent = txtRecording->instantMessagingModel()->rowCount() > 0;
                ui->listMessageView->setVisible(messagesPresent);
                ui->noMessagesLabel->setVisible(!messagesPresent);
            });
        }
    }
}

void
CallWidget::slotAccountMessageReceived(const QMap<QString,QString> message,
                                       ContactMethod* cm,
                                       Media::Media::Direction dir)
{
    Q_UNUSED(message)
    Q_UNUSED(dir)

    ui->listMessageView->scrollToBottom();
    cm->textRecording()->setAllRead();
}

void
CallWidget::on_ringContactLineEdit_textChanged(const QString& text)
{
    RecentModel::instance().peopleProxy()->setFilterWildcard(text);
}

void
CallWidget::on_imBackButton_clicked()
{
    RecentModel::instance().selectionModel()->clear();
    slidePage(ui->welcomePage);
}

void
CallWidget::slidePage(QWidget* widget, bool toRight)
{
    short dir = (toRight ? -1 : 1);
    ui->stackedWidget->setCurrentWidget(widget);
    pageAnim_->setTargetObject(widget);
    pageAnim_->setDuration(animDuration_);
    pageAnim_->setStartValue(QPoint(widget->width() * dir, widget->y()));
    pageAnim_->setEndValue(QPoint(widget->x(), widget->y()));
    pageAnim_->setEasingCurve(QEasingCurve::OutQuad);
    pageAnim_->start();
}

void
CallWidget::on_smartList_clicked(const QModelIndex& index)
{
    RecentModel::instance().selectionModel()->setCurrentIndex(
                RecentModel::instance().peopleProxy()->mapToSource(index),
                QItemSelectionModel::ClearAndSelect);
}

void
CallWidget::on_copyCMButton_clicked()
{
    auto text = ui->contactMethodComboBox->currentText();
    QApplication::clipboard()->setText(text);
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
CallWidget::contactLineEdit_registeredNameFound(const Account* account,NameDirectory::LookupStatus status,
                                                const QString& address,const QString& name)
{
    URI uri = URI(ui->ringContactLineEdit->text());
    QString username_to_lookup = uri.userinfo();

    if (username_to_lookup.compare(name) != 0){
        return;
    }

    switch (status)
    {
        case NameDirectory::LookupStatus::SUCCESS:
        {

            uri = URI("ring:" + address);
            qDebug() << "contactLineEdit username to search: " << username_to_lookup;
            qDebug() << uri;
            searchContactLineEditEntry(uri);
            auto cm = PhoneDirectoryModel::instance().getNumber(uri);
            //cm->setAccount(account);
            break;
        }
        case NameDirectory::LookupStatus::INVALID_NAME:
        {
           qDebug() << "Invalid Ring username";
           break;
        }
        case NameDirectory::LookupStatus::ERROR:
        case NameDirectory::LookupStatus::NOT_FOUND:
        default:
        {
            qDebug() << "Could not resolve Ring username";
            break;
        }
    }
}

void
CallWidget::on_sendContactRequestPageButton_clicked()
{
    ui->sendContactRequestWidget->setup(RecentModel::instance().selectionModel()->currentIndex());
    slidePage(ui->sendContactRequestPage);
}

void CallWidget::on_sendCRBackButton_clicked()
{
    slidePage(ui->messagingPage);
}
