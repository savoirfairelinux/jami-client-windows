/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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
#include "contactrequestitemdelegate.h"
#include "deletecontactdialog.h"


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

    try {
        callModel_ = &CallModel::instance();

        connect(callModel_, SIGNAL(incomingCall(Call*)),
                this, SLOT(callIncoming(Call*)));
        connect(callModel_, SIGNAL(callStateChanged(Call*, Call::State)),
                this, SLOT(callStateChanged(Call*, Call::State)));

        RecentModel::instance().peopleProxy()->setFilterRole(static_cast<int>(Ring::Role::Name));
        RecentModel::instance().peopleProxy()->setFilterCaseSensitivity(Qt::CaseInsensitive);
        ui->smartList->setModel(RecentModel::instance().peopleProxy());

        PersonModel::instance().addCollection<PeerProfileCollection>(LoadOptions::FORCE_ENABLED);
        ProfileModel::instance().addCollection<LocalProfileCollection>(LoadOptions::FORCE_ENABLED);

        PersonModel::instance().
                addCollection<WindowsContactBackend>(LoadOptions::FORCE_ENABLED);

        connect(ui->smartList, &QTreeView::entered, this, &CallWidget::on_entered);

        smartListDelegate_ = new SmartListDelegate();
        ui->smartList->setSmartListItemDelegate(smartListDelegate_);

        ui->contactRequestList->setItemDelegate(new ContactRequestItemDelegate());

        findRingAccount();
        setupOutOfCallIM();
        ui->smartList->setContextMenuPolicy(Qt::CustomContextMenu);

        connect(ui->smartList, &SmartList::btnVideoClicked, this, &CallWidget::btnComBarVideoClicked);

        connect(RecentModel::instance().selectionModel(),
                SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this,
                SLOT(smartListCurrentChanged(QModelIndex,QModelIndex)));

        //set most recent call to view
        connect(&RecentModel::instance(), &QAbstractItemModel::dataChanged, [=](const QModelIndex &topLeft, const QModelIndex &bottomRight,const QVector<int> &vec){
            Q_UNUSED(bottomRight)
            Q_UNUSED(vec)

            if (topLeft.isValid() && RecentModel::instance().hasActiveCall(topLeft)){
                ui->smartList->selectionModel()->setCurrentIndex(topLeft,QItemSelectionModel::ClearAndSelect);
            }
        });

        connect(RecentModel::instance().selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selected, const QItemSelection &deselected) {
                    // lambda used to focus on the correct smartList element when switching automatically between two calls
                    Q_UNUSED(deselected)
                    if (selected.size()) {
                        auto idx = selected.indexes().first();
                        auto realIdx = RecentModel::instance().peopleProxy()->mapFromSource(idx);
                        ui->smartList->selectionModel()->setCurrentIndex(realIdx, QItemSelectionModel::ClearAndSelect);
                    } else {
                        RecentModel::instance().selectionModel()->clearCurrentIndex();
                        ui->smartList->clearSelection();
                        ui->smartList->selectionModel()->clearCurrentIndex();
                    }
                });

        connect(&NameDirectory::instance(), SIGNAL(registeredNameFound(Account*,NameDirectory::LookupStatus,const QString&,const QString&)),
                this, SLOT(contactLineEdit_registeredNameFound(Account*,NameDirectory::LookupStatus,const QString&,const QString&)));

        connect(&AccountModel::instance(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                ui->currentAccountWidget, SLOT(update()));

        connect(ui->searchBtn, SIGNAL(clicked(bool)), this, SLOT(searchBtnClicked()));

        connect(ui->sendContactRequestWidget, &SendContactRequestWidget::sendCRclicked, [=]{slidePage(ui->messagingPage);});

        connect(ui->contactRequestWidget, &ContactRequestWidget::choiceMade, [this]() {
            if (getSelectedAccount()->pendingContactRequestModel()->rowCount() == 0)
                ui->mainTabMenu->setCurrentWidget(ui->mainTabMenuPage1);
            else
                ui->contactRequestList->selectionModel()->clear();
        });

        connect(AvailableAccountModel::instance().selectionModel(), &QItemSelectionModel::currentChanged,
                this, &CallWidget::selectedAccountChanged);

        // It needs to be called manually once to initialize the ui with the account selected at start.
        // The second argument (previous) is set to an invalid QModelIndex as it is the first selection.
        selectedAccountChanged(AvailableAccountModel::instance().selectionModel()->currentIndex(), QModelIndex());

        // This connect() is used to initialise and track changes of profile's picture
        connect(&ProfileModel::instance(), &ProfileModel::dataChanged,
                ui->currentAccountWidget, &CurrentAccountWidget::setPhoto);

        connect(ui->videoWidget, &VideoView::videoSettingsClicked, this, &CallWidget::settingsButtonClicked);

        connect(ui->smartList, &QListView::customContextMenuRequested, [=](const QPoint& pos){ setupSmartListMenu(pos);});

        // setup searchingfor mini spinner
        miniSpinner_ = new QMovie(":/images/waiting.gif");
        ui->spinnerLabel->setMovie(miniSpinner_);
        ui->spinnerLabel->hide();

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
CallWidget::onIncomingMessage(::Media::TextRecording* t, ContactMethod* cm)
{
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
CallWidget::triggerDeleteContactDialog(ContactMethod *cm, Account *ac)
{
    auto dlg = new DeleteContactDialog(cm, ac);
    dlg->exec();
}

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

    auto ac = getSelectedAccount();
    if (ac && !ac->hasContact(contactMethod)) {
        auto addExisting = new QAction(tr("Add to contacts"), this);
        menu.addAction(addExisting);
        connect(addExisting, &QAction::triggered, [this, contactMethod, ac]() {
            /* uncomment and capture globalPos in lambda to reactivate popup
            ContactPicker contactPicker(contactMethod);
            contactPicker.move(globalPos.x(), globalPos.y() - (contactPicker.height()/2));
            contactPicker.exec();
            */
            ac->addContact(contactMethod);
        });
    } else if (ac) {
        auto removeExisting = new QAction(tr("Remove from contacts"), this);
        menu.addAction(removeExisting);
        connect(removeExisting, &QAction::triggered, [this, contactMethod, ac]() {
            triggerDeleteContactDialog(contactMethod, ac);
        });
    }

    menu.exec(globalPos);
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
    bool ringAccountFound = false;

    auto a_count = AccountModel::instance().rowCount();
    for (int i = 0; i < a_count; ++i) {
        auto idx = AccountModel::instance().index(i, 0);
        auto protocol = idx.data(static_cast<int>(Account::Role::Proto));
        if (static_cast<Account::Protocol>(protocol.toUInt()) == Account::Protocol::RING) {
            ringAccountFound = true;
            auto account = AccountModel::instance().getAccountByModelIndex(idx);
            if (account->displayName().isEmpty())
                account->displayName() = account->alias();
            if (account->needsMigration()) {
                WizardDialog dlg(WizardDialog::MIGRATION, account);
                dlg.exec();
            }
        }
    }
    if (!ringAccountFound) {
        WizardDialog wizardDialog;
        wizardDialog.exec();
    }

    ui->currentAccountWidget->update();

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
    ui->callerBestIdLabel->setText(actualCall_->peerContactMethod()->bestId());

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

void
CallWidget::callStateToView(Call* value)
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
            hideMiniSpinner();
            break;
        case Call::State::OVER:
            ui->stackedWidget->setCurrentWidget(ui->welcomePage);
            break;
        case Call::State::FAILURE:
        case Call::State::ERROR:
            on_cancelButton_clicked();
            hideMiniSpinner();
            break;
        case Call::State::INITIALIZATION:
        case Call::State::CONNECTED:
        case Call::State::RINGING:
            miniSpinner_->start();
            ui->spinnerLabel->show();
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
CallWidget::on_smartList_clicked(const QModelIndex& index)
{
    RecentModel::instance().selectionModel()->setCurrentIndex(
                RecentModel::instance().peopleProxy()->mapToSource(index),
                QItemSelectionModel::ClearAndSelect);
}

void
CallWidget::on_smartList_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

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
    if (not currentIdx.isValid()) {
        auto widget = ui->stackedWidget->currentWidget();
        if (widget == ui->messagingPage || widget == ui->videoPage)
            slidePage(ui->welcomePage);
        if(actualCall_)
            setActualCall(nullptr);
        return;
    }

    //catch call of current index
    auto currentIdxCall = RecentModel::instance().getActiveCall(currentIdx);

    if (currentIdxCall) {
        if (currentIdxCall != actualCall_) //if it is different from actual call, switch between the two
            setActualCall(currentIdxCall);
    } else { // if there is no call attached to this smartlist index (contact tab)
        setActualCall(nullptr);
        showIMOutOfCall(currentIdx); // change page to contact request of messaging page with correct behaviour
    }
    /*
    else { // if non defined behaviour disconnect instant messaging and return to welcome page
        setActualCall(nullptr);
        if (imConnection_)
            disconnect(imConnection_);
        ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    }
    */
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
            slidePage(ui->welcomePage);
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
    auto cm = PhoneDirectoryModel::instance().getNumber(uri, getSelectedAccount());
    // if its a new CM, bring it to the top
    if (cm->lastUsed() == 0)
        cm->setLastUsed(QDateTime::currentDateTime().toTime_t());

    // select cm
    RecentModel::instance().selectionModel()->setCurrentIndex(RecentModel::instance().getIndex(cm),
                                                              QItemSelectionModel::ClearAndSelect);
    ui->ringContactLineEdit->clear();
    cm->setAccount(AvailableAccountModel::instance().currentDefaultAccount(cm));
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
        // if no scheme type has been specified, determine ring vs sip by the current selected account
        auto idx = AvailableAccountModel::instance().selectionModel()->currentIndex();
        if (idx.isValid()) {
            auto account = idx.data((int)Ring::Role::Object).value<Account *>();
            if (account && account->protocol() == Account::Protocol::RING)
                return TRUE;
        }
    }

    return FALSE;
}

void
CallWidget::processContactLineEdit()
{
    auto contactLineText = ui->ringContactLineEdit->text();
    URI uri_passed = URI(contactLineText);
    Account* ac = getSelectedAccount();

    if (!contactLineText.isNull() && !contactLineText.isEmpty()){
        if (uriNeedNameLookup(uri_passed)){
            NameDirectory::instance().lookupName(ac, QString(), uri_passed);
        } else {
            searchContactLineEditEntry(uri_passed);
        }
    }
}

void
CallWidget::on_ringContactLineEdit_returnPressed()
{
    processContactLineEdit();
}

void
CallWidget::searchBtnClicked()
{
    processContactLineEdit();
}

void
CallWidget::btnComBarVideoClicked()
{
    if (not highLightedIndex_.isValid())
        return;

    on_smartList_doubleClicked(highLightedIndex_);
}

void
CallWidget::selectedAccountChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)

    if (current.isValid()) {
        if (ui->selectBar->isHidden()){
            ui->selectBar->show();
        }
        auto ac = current.data(static_cast<int>(Account::Role::Object)).value<Account*>();

        // First, we get back to the welcome view (except if in call)

        if (ui->stackedWidget->currentWidget() != ui->videoPage &&
            ui->stackedWidget->currentWidget() != ui->welcomePage) {
            slidePage(ui->welcomePage);
        }

        // We setup the ringIdLabel and the QRCode
        auto protocol = ac->protocol();
        if (protocol == Account::Protocol::RING) {
            ui->ringIdLabel->setText((ac->registeredName().isEmpty())?ac->username():ac->registeredName());
            setupQRCode(ac->username());
        } else {
            ui->ringIdLabel->setText(tr("NO RING ACCOUNT FOUND"));
        }

        // Then, we update the pending CR list with those from the newly selected account
        if (disconnect(crListSelectionConnection_)) {
            ui->contactRequestList->selectionModel()->clear();
            // The selection model must be deleted by the application (see QT doc).
            QItemSelectionModel* sMod = ui->contactRequestList->selectionModel();
            delete sMod;
            RecentModel::instance().selectionModel()->clear();
        }

        ui->contactRequestList->setItemModel(ac->pendingContactRequestModel());

        // We modify the currentAccountWidget to reflect the new selected account
        // if the event wasn't triggered by this widget
        ui->currentAccountWidget->changeSelectedIndex(current.row());

        if (actualCall_){
            // keep call on foreground
            callStateToView(actualCall_);
        }
    } else {
        ui->selectBar->hide();
        ui->ringIdLabel->setText("");
    }
}

void
CallWidget::showIMOutOfCall(const QModelIndex& nodeIdx)
{
    ui->contactMethodComboBox->clear();
    QString name = nodeIdx.data(static_cast<int>(Ring::Role::Name)).toString();
    QString number = nodeIdx.data(static_cast<int>(Ring::Role::Number)).toString();

    ui->imNameLabel->setText(QString(tr("%1", "%1 is the contact username"))
                                     .arg(name));

    if (name != number){
        ui->imIdLabel->show();
        ui->imIdLabel->setText(QString(tr("%1", "%1 is the contact unique identifier"))
                                   .arg(number));
    } else {
        ui->imIdLabel->hide();
    }

    auto cmVector = RecentModel::instance().getContactMethods(nodeIdx);
    ui->contactMethodComboBox->setVisible(cmVector.size() > 1);
    foreach (const ContactMethod* cm, cmVector) {
        ui->contactMethodComboBox->addItem(cm->bestId());
    }

    ui->sendContactRequestPageButton->setVisible(shouldDisplayInviteButton(*cmVector[0]));

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

    auto idx = RecentModel::instance().selectionModel()->currentIndex();
    auto cmVec = RecentModel::instance().getContactMethods(idx);
    if (cmVec.size() > 0) {
            auto cm = cmVec[0];
            if(!cm) {
                qWarning() << "Contact Method not found";
                return;
            }
            QMap<QString, QString> msg;
            msg["text/plain"] = ui->imMessageEdit->text();
            cm->sendOfflineTextMessage(msg);
            ui->imMessageEdit->clear();
    } else {
        qWarning() << "No contact method found for messaging";
    }
}

void
CallWidget::on_imMessageEdit_returnPressed()
{
    on_sendIMButton_clicked();
}

void CallWidget::on_contactMethodComboBox_currentIndexChanged(int index)
{
    auto idx = RecentModel::instance().selectionModel()->currentIndex();
    auto cmVec = RecentModel::instance().getContactMethods(idx);
    ContactMethod* cm {};

    if (index < cmVec.size() && index >= 0 ){
        cm = cmVec[index];
    }

    if (cm){
        ui->sendContactRequestPageButton->setVisible(shouldDisplayInviteButton(*cm));
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
    RecentModel::instance().peopleProxy()->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
}

void
CallWidget::backToWelcomePage()
{
    RecentModel::instance().selectionModel()->clear();
    disconnect(imConnection_);
}

void CallWidget::hideMiniSpinner()
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
CallWidget::contactLineEdit_registeredNameFound(Account* account,NameDirectory::LookupStatus status,
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
            cm->setAccount(account);
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
    ui->sendContactRequestWidget->sendCR(RecentModel::instance().selectionModel()->currentIndex());
        // temporarly changed comportment waiting for message in payload implementation
        /*ui->sendContactRequestWidget->setup(RecentModel::instance().selectionModel()->currentIndex());
        slidePage(ui->sendContactRequestPage);*/
}

void
CallWidget::on_sendCRBackButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->messagingPage);
}

void
CallWidget::on_pendingCRBackButton_clicked()
{
    ui->contactRequestList->selectionModel()->clear();
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
}

Account*
CallWidget::getSelectedAccount()
{
    auto idx = AvailableAccountModel::instance().selectionModel()->currentIndex();
    if (idx.isValid()) {
        auto ac = idx.data(static_cast<int>(Ring::Role::Object)).value<Account*>();
        return ac;
    }
    return nullptr;
}

bool
CallWidget::shouldDisplayInviteButton(ContactMethod &cm)
{
    // Displaying the button for a SIP ContactMethod is a nonsense
    if (cm.protocolHint() == URI::ProtocolHint::RING) {
        // If we know that the other accepted us
        if (cm.isConfirmed())
            return false;

        // If not sure, we still check if the contact method is linked to a
        // Ring account or if the selected account is a RING one.
        if(auto linkedAccount = cm.account())
            return linkedAccount->protocol() == Account::Protocol::RING;
        else if (auto acc = getSelectedAccount())
            return acc->protocol() == Account::Protocol::RING;
    }
    return false;
}

void CallWidget::on_contactRequestList_clicked(const QModelIndex &index)
{
    RecentModel::instance().selectionModel()->clear();
    contactReqListCurrentChanged(index, QModelIndex());
}
