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

        CategorizedContactModel::instance().setSortAlphabetical(false);
        CategorizedContactModel::instance().setUnreachableHidden(true);
        ui->contactView->setModel(&CategorizedContactModel::instance());
        contactDelegate_ = new ContactDelegate();
        ui->contactView->setItemDelegate(contactDelegate_);

        CategorizedHistoryModel::instance().
                addCollection<LocalHistoryCollection>(LoadOptions::FORCE_ENABLED);

        ui->historyList->setModel(CategorizedHistoryModel::SortedProxy::instance().model());
        CategorizedHistoryModel::SortedProxy::instance().model()->sort(0, Qt::DescendingOrder);
        ui->historyList->setHeaderHidden(true);
        historyDelegate_ = new HistoryDelegate();
        ui->historyList->setItemDelegate(historyDelegate_);

        connect(CategorizedHistoryModel::SortedProxy::instance().model(), &QSortFilterProxyModel::layoutChanged, [=]() {
            auto idx = CategorizedHistoryModel::SortedProxy::instance().model()->index(0,0);
            if (idx.isValid())
                ui->historyList->setExpanded(idx, true);
        });

        connect(ui->smartList, &QTreeView::entered, this, &CallWidget::on_entered);

        smartListDelegate_ = new SmartListDelegate();
        ui->smartList->setSmartListItemDelegate(smartListDelegate_);

        ui->historyList->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->historyList, &QListView::customContextMenuRequested, [=](const QPoint& pos){
            if (ui->historyList->currentIndex().parent().isValid()) {
                QPoint globalPos = ui->historyList->mapToGlobal(pos);
                QMenu menu;

                ContactMethod* contactMethod = ui->historyList->currentIndex()
                        .data(static_cast<int>(Call::Role::ContactMethod)).value<ContactMethod*>();

                auto copyAction = new QAction(tr("Copy number"), this);
                menu.addAction(copyAction);
                connect(copyAction, &QAction::triggered, [=]() {
                    QApplication::clipboard()->setText(contactMethod->uri());
                });
                if (not contactMethod->contact() || contactMethod->contact()->isPlaceHolder()) {
                    auto addExisting = new QAction(tr("Add to contact"), this);
                    menu.addAction(addExisting);
                    connect(addExisting, &QAction::triggered, [=]() {
                        ContactPicker contactPicker(contactMethod);
                        contactPicker.move(globalPos.x(), globalPos.y() - (contactPicker.height()/2));
                        contactPicker.exec();
                    });
                }
                menu.exec(globalPos);
            }
        });

        findRingAccount();
        setupOutOfCallIM();
        setupSmartListMenu();

        connect(ui->smartList, &SmartList::btnVideoClicked, this, &CallWidget::btnComBarVideoClicked);

        connect(RecentModel::instance().selectionModel(),
                SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this,
                SLOT(smartListSelectionChanged(QItemSelection,QItemSelection)));

        connect(RecentModel::instance().selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selected, const QItemSelection &deselected) {
            Q_UNUSED(deselected)
            if (selected.size()) {
                auto idx = selected.indexes().first();
                auto realIdx = RecentModel::instance().peopleProxy()->mapFromSource(idx);
                ui->smartList->selectionModel()->setCurrentIndex(realIdx, QItemSelectionModel::ClearAndSelect);
            } else
                ui->smartList->clearSelection();
        });

    } catch (const std::exception& e) {
        qDebug() << "INIT ERROR" << e.what();
    }
}

CallWidget::~CallWidget()
{
    delete ui;
    delete menu_;
    delete contactDelegate_;
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
            auto username = idx.data(static_cast<int>(Account::Role::Username));
            ui->ringIdLabel->setText(username.toString());
            setupQRCode();
            return;
        }
    }
    ui->ringIdLabel->setText(tr("NO RING ACCOUNT FOUND"));
}

void CallWidget::setupQRCode()
{
    auto rcode = QRcode_encodeString(ui->ringIdLabel->text().toStdString().c_str(),
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
    ui->ringIdLabel->text().clear();

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
                auto username = account->username();
                ui->ringIdLabel->setText(username);
                setupQRCode();
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
    ui->callerIdLabel->setText(QString(tr("%1", "%1 is the name of the caller"))
                               .arg(actualCall_->formattedName()));
    ui->callerPhoto->setPixmap(
                QPixmap::fromImage(
                    GlobalInstances::pixmapManipulator()
                    .callPhoto(actualCall_, QSize(130,130)).value<QImage>()));
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

    if (call->state() == Call::State::OVER) {
        setActualCall(nullptr);
        RecentModel::instance().selectionModel()->clear();
    }
    callStateToView(call);
}

void
CallWidget::atExit()
{
}

void
CallWidget::on_contactView_doubleClicked(const QModelIndex& index)
{
    if (not index.isValid())
        return;

    auto var = index.child(0,0).data(
                static_cast<int>(Person::Role::Object));
    if (var.isValid()) {
        Person* person = var.value<Person*>();
        ContactMethod* uri = nullptr;
        if (person->phoneNumbers().size() == 1) {
            uri = person->phoneNumbers().at(0);
        } else if (person->phoneNumbers().size() > 1) {
            ContactMethodPicker dlg(person->phoneNumbers());
            auto pos = QCursor::pos();
            dlg.move(pos.x(), pos.y());
            if (dlg.exec())
                uri = dlg.getSelected();
        }
        if (uri) {
            auto outCall = CallModel::instance().dialingCall(uri);
            outCall->performAction(Call::Action::ACCEPT);
            ui->mainTabMenu->setCurrentIndex(0);
            ui->smartList->setFocus();
        }
    }
}

void
CallWidget::on_historyList_doubleClicked(const QModelIndex& index)
{
    if (not index.isValid())
        return;

    auto number = index.data(static_cast<int>(Call::Role::ContactMethod)).value<ContactMethod*>();
    if (number) {
        auto outCall = CallModel::instance().dialingCall(number);
        outCall->performAction(Call::Action::ACCEPT);
        ui->mainTabMenu->setCurrentIndex(0);
        ui->smartList->setFocus();
    }
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
        case Call::State::INITIALIZATION:
        case Call::State::CONNECTED:
        case Call::State::RINGING:
        case Call::State::FAILURE:
        case Call::State::ERROR:
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
    if (actualCall_)
        actualCall_->performAction(Call::Action::REFUSE);
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
        Call* c = CallModel::instance().dialingCall(m);
        c->performAction(Call::Action::ACCEPT);
        setActualCall(c);
    }
}

void
CallWidget::smartListSelectionChanged(const QItemSelection& newSel, const QItemSelection& oldSel) {

    Q_UNUSED(oldSel)

    if (newSel.indexes().empty()) {
        setActualCall(nullptr);
        return ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    }

    auto newIdx = newSel.indexes().first();
    if (not newIdx.isValid())
        return;

    auto newIdxCall = RecentModel::instance().getActiveCall(newIdx);
    if (newIdxCall && newIdxCall != actualCall_) {
        setActualCall(newIdxCall);
    } else if (newIdxCall == nullptr){
        setActualCall(nullptr);
        ui->instantMessagingWidget->hide();
        showIMOutOfCall(newIdx);
    } else {
        setActualCall(nullptr);
        ui->instantMessagingWidget->hide();
        if (imConnection_)
            disconnect(imConnection_);
        ui->stackedWidget->setCurrentWidget(ui->welcomePage);
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
CallWidget::contactButtonClicked(bool checked)
{
    ui->mainTabMenu->setCurrentIndex(checked ? 1 : 0);
}

void
CallWidget::historicButtonClicked(bool checked)
{
    ui->mainTabMenu->setCurrentIndex(checked ? 2 : 0);
}

void
CallWidget::on_ringContactLineEdit_returnPressed()
{
    auto cm = PhoneDirectoryModel::instance().getNumber(ui->ringContactLineEdit->text());
    time_t currentTime;
    ::time(&currentTime);
    cm->setLastUsed(currentTime);
    ui->ringContactLineEdit->clear();

    QModelIndex index = RecentModel::instance().getIndex(cm);
    RecentModel::instance().selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
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

    ui->imNameLabel->setText(QString(tr("Conversation with %1", "%1 is the contact name"))
                             .arg(nodeIdx.data(static_cast<int>(Ring::Role::Name)).toString()));
    auto cmVector = RecentModel::instance().getContactMethods(nodeIdx);
    ui->contactMethodComboBox->setEnabled(cmVector.size() > 1);
    foreach (const ContactMethod* cm, cmVector) {
        ui->contactMethodComboBox->addItem(cm->uri());
    }
    slidePage(ui->messagingPage, true);
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
