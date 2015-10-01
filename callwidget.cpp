/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
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

#include <memory>

//ERROR is defined in windows.h
#include "utils.h"
#undef ERROR

#include "audio/settings.h"
#include "personmodel.h"
#include "person.h"
#include "fallbackpersoncollection.h"
#include "categorizedcontactmodel.h"
#include "localhistorycollection.h"
#include "media/text.h"
#include "media/recording.h"
#include "media/textrecording.h"
#include "recentmodel.h"


#include "wizarddialog.h"
#include "windowscontactbackend.h"
#include "contactpicker.h"
#include "contactmethodpicker.h"
#include "globalsystemtray.h"

CallWidget::CallWidget(QWidget *parent) :
    NavWidget(END ,parent),
    ui(new Ui::CallWidget),
    menu_(new QMenu())
{
    ui->setupUi(this);

    ui->callInvite->setVisible(false);

    setActualCall(nullptr);
    videoRenderer_ = nullptr;

    connect(ui->videoWidget, SIGNAL(setChatVisibility(bool)),
            ui->instantMessagingWidget, SLOT(setVisible(bool)));

    ui->spinnerWidget->hide();
    spinner_ = new QMovie(":/images/spinner.gif");
    if (spinner_->isValid()) {
        ui->spinnerLabel->setMovie(spinner_);
    }

    try {
        callModel_ = CallModel::instance();

        connect(callModel_, SIGNAL(incomingCall(Call*)),
                this, SLOT(callIncoming(Call*)));
        connect(callModel_, SIGNAL(callAdded(Call*,Call*)),
                this, SLOT(addedCall(Call*, Call*)));
        connect(callModel_, SIGNAL(callStateChanged(Call*, Call::State)),
                this, SLOT(callStateChanged(Call*, Call::State)));

        connect(AccountModel::instance(),
                SIGNAL(accountStateChanged(Account*,Account::RegistrationState)),
                this,
                SLOT(checkRegistrationState(Account*,Account::RegistrationState)));

        connect(AccountModel::instance()
                , SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>))
                , this
                , SLOT(findRingAccount(QModelIndex, QModelIndex, QVector<int>)));

        ui->smartList->setModel(RecentModel::instance()->peopleProxy());
        connect(ui->smartList->selectionModel(),
                SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this,
                SLOT(smartListSelectionChanged(QItemSelection,QItemSelection)));
        smartListDelegate_ = new SmartListDelegate();
        ui->smartList->setItemDelegate(smartListDelegate_);
        ui->smartList->setHeaderHidden(true);

        PersonModel::instance()->
                addCollection<WindowsContactBackend>(LoadOptions::FORCE_ENABLED);

        CategorizedContactModel::instance()->setSortAlphabetical(false);
        CategorizedContactModel::instance()->setUnreachableHidden(true);
        ui->contactView->setModel(CategorizedContactModel::instance());
        contactDelegate_ = new ContactDelegate();
        ui->contactView->setItemDelegate(contactDelegate_);

        CategorizedHistoryModel::instance()->
                addCollection<LocalHistoryCollection>(LoadOptions::FORCE_ENABLED);

        ui->historyList->setModel(CategorizedHistoryModel::SortedProxy::instance()->model());
        CategorizedHistoryModel::SortedProxy::instance()->model()->sort(0, Qt::DescendingOrder);
        ui->historyList->setHeaderHidden(true);
        historyDelegate_ = new HistoryDelegate();
        ui->historyList->setItemDelegate(historyDelegate_);

        connect(CategorizedHistoryModel::SortedProxy::instance()->model(), &QSortFilterProxyModel::layoutChanged, [=]() {
            auto idx = CategorizedHistoryModel::SortedProxy::instance()->model()->index(0,0);
            if (idx.isValid())
                ui->historyList->setExpanded(idx, true);
        });

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

    } catch (...) {
        qDebug() << "INIT ERROR";
    }
}

CallWidget::~CallWidget()
{
    delete ui;
    delete spinner_;
    delete menu_;
    delete contactDelegate_;
}

void
CallWidget::findRingAccount(QModelIndex idx1, QModelIndex idx2, QVector<int> vec)
{
    Q_UNUSED(idx1)
    Q_UNUSED(idx2)
    Q_UNUSED(vec)

    auto a_count = AccountModel::instance()->rowCount();
    auto found = false;
    for (int i = 0; i < a_count; ++i) {
        auto idx = AccountModel::instance()->index(i, 0);
        auto protocol = idx.data(static_cast<int>(Account::Role::Proto));
        if ((Account::Protocol)protocol.toUInt() == Account::Protocol::RING) {
            auto username = idx.data(static_cast<int>(Account::Role::Username));
            ui->ringIdLabel->setText(username.toString());
            found = true;
            return;
        }
    }
    if (not found){
        ui->ringIdLabel->setText(tr("NO RING ACCOUNT FOUND"));
    }
}

void
CallWidget::checkRegistrationState(Account *account,
                                   Account::RegistrationState state)
{
    Q_UNUSED(account);

    QPixmap pm(20,20);
    pm.fill();
    QPainter p(&pm);
    QPen pen(Qt::black, 2);
    p.setPen(pen);
    p.setRenderHint(QPainter::Antialiasing, true);

    switch(state) {
    case Account::RegistrationState::ERROR:
        p.setBrush(Qt::red);
        break;
    case Account::RegistrationState::TRYING:
        p.setBrush(Qt::yellow);
        break;
    case Account::RegistrationState::READY:
    {
        p.setBrush(Qt::green);
        auto a_count = AccountModel::instance()->rowCount();
        for (int i = 0; i < a_count; ++i) {
            auto idx = AccountModel::instance()->index(i, 0);
            auto stateAcc = idx.data(
                        static_cast<int>(Account::Role::RegistrationState));
            if (stateAcc.value<Account::RegistrationState>()
                    != Account::RegistrationState::READY) {
                checkRegistrationState(nullptr,
                                       stateAcc.value<Account::RegistrationState>());
                return;
            }
        }
    }
        break;
     default:
        break;
    }
    p.drawEllipse(3, 3, 16, 16);
    ui->accountStateHintLabel->setPixmap(pm);
    ui->accountStateHintLabel->show();
}

void
CallWidget::findRingAccount()
{

    auto a_count = AccountModel::instance()->rowCount();
    auto found = false;
    for (int i = 0; i < a_count; ++i) {
        auto idx = AccountModel::instance()->index(i, 0);
        auto protocol = idx.data(static_cast<int>(Account::Role::Proto));
        if ((Account::Protocol)protocol.toUInt() == Account::Protocol::RING) {
            auto account = AccountModel::instance()->getAccountByModelIndex(idx);
            if (account->displayName().isEmpty())
                account->displayName() = account->alias();
            auto username = account->username();
            ui->ringIdLabel->setText(username);
            found = true;
            return;
        }
    }
    if (!found) {
        WizardDialog *wizardDialog = new WizardDialog();
        wizardDialog->exec();
        delete wizardDialog;
    }
}

void
CallWidget::callIncoming(Call *call)
{
    if (!QApplication::activeWindow()) {
        GlobalSystemTray::instance().showMessage("Ring", "Call incoming from " + call->formattedName());
        QApplication::alert(this, 5000);
    }

    if (!call->account()->isAutoAnswer()) {
        ui->callLabel->setText(QString(tr("Call from %1", "%1 is the name of the caller"))
                               .arg(call->formattedName()));
        ui->stackedWidget->setCurrentWidget(ui->callInvitePage);
        ui->callInvite->setVisible(true);
        ui->callInvite->raise();
    }
    setActualCall(call);
}

void
CallWidget::on_acceptButton_clicked()
{
    if (actualCall_ != nullptr)
        actualCall_->performAction(Call::Action::ACCEPT);
    ui->callInvite->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->videoPage);
}

void
CallWidget::on_refuseButton_clicked()
{
    if (actualCall_ == nullptr)
        return;
    actualCall_->performAction(Call::Action::REFUSE);
    setActualCall(nullptr);
    ui->callInvite->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->welcomePage);
}

void
CallWidget::addedCall(Call* call, Call* parent)
{
    Q_UNUSED(parent);
    if (call->direction() == Call::Direction::OUTGOING) {
        displaySpinner(true);
        setActualCall(call);
        ui->stackedWidget->setCurrentWidget(ui->callInvitePage);
    }
}

void
CallWidget::callStateChanged(Call* call, Call::State previousState)
{
    Q_UNUSED(previousState)
    if (call == nullptr)
        return;

    //Force update of smartList
    ui->smartList->setFocus();

    if (call->state() == Call::State::OVER
            || call->state() == Call::State::ERROR
            || call->state() == Call::State::FAILURE
            || call->state() == Call::State::ABORTED) {
        setActualCall(nullptr);
        ui->instantMessagingWidget->setMediaText(nullptr);
        ui->stackedWidget->setCurrentWidget(ui->welcomePage);
        displaySpinner(false);
//TODO : Link this so that recentModel get selected correctly
//        auto onHoldCall = callModel_->getActiveCalls().first();
//        if (onHoldCall != nullptr && onHoldCall->state() == Call::State::HOLD) {
//            setActualCall(onHoldCall);
//            onHoldCall->performAction(Call::Action::HOLD);
//        }
    } else if (call->state() == Call::State::CURRENT) {
        displaySpinner(false);
        ui->instantMessagingWidget->setMediaText(actualCall_);
        ui->stackedWidget->setCurrentWidget(ui->videoPage);
    }
    ui->callStateLabel->setText(QString(tr("Call State: %1", "%1 is the state of the call"))
                                .arg(call->toHumanStateName()));
}

void
CallWidget::atExit()
{

}

void
CallWidget::on_contactView_doubleClicked(const QModelIndex &index)
{
    if (not index.isValid())
        return;

    ContactMethod* uri = nullptr;

    auto var = index.child(0,0).data(
                static_cast<int>(Person::Role::Object));
    if (var.isValid()) {
        Person* person = var.value<Person*>();
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
            auto outCall = CallModel::instance()->dialingCall(person->formattedName());
            outCall->setDialNumber(uri);
            outCall->performAction(Call::Action::ACCEPT);
        }
    }
}

void
CallWidget::on_historyList_doubleClicked(const QModelIndex &index)
{
    if (not index.isValid())
        return;

    auto number = index.data(static_cast<int>(Call::Role::ContactMethod)).value<ContactMethod*>();
    if (number) {
        auto outCall = CallModel::instance()->dialingCall();
        outCall->setDialNumber(number);
        outCall->performAction(Call::Action::ACCEPT);
        ui->mainTabMenu->setCurrentIndex(0);
        ui->smartList->setFocus();
    }
}

void
CallWidget::setActualCall(Call* value)
{
    actualCall_ = value;
    if (value)
        CallModel::instance()->selectCall(value);
}

void
CallWidget::displaySpinner(bool display)
{
    display ? ui->spinnerWidget->show() : ui->spinnerWidget->hide();
    if (ui->spinnerLabel->movie())
        display ? ui->spinnerLabel->movie()->start() : ui->spinnerLabel->movie()->stop();
}

void
CallWidget::on_cancelButton_clicked()
{
    if (actualCall_)
        actualCall_->performAction(Call::Action::REFUSE);
}

void
CallWidget::on_smartList_doubleClicked(const QModelIndex &index)
{
    auto realIndex = RecentModel::instance()->peopleProxy()->mapToSource(index);
    if (RecentModel::instance()->hasActiveCall(realIndex))
        return;

    ContactMethod* m = nullptr;
    auto cm = realIndex.data((int)Call::Role::ContactMethod).value<ContactMethod*>();
    if (cm) {
        m = cm;
    } else {
        auto person =  realIndex.data((int)Person::Role::Object).value<Person*>();
        if (person) {
            m = person->phoneNumbers().first();
        }
    }
    if (m && !RecentModel::instance()->index(0, 0, realIndex).isValid()) {
        Call* c = CallModel::instance()->dialingCall();
        c->setPeerContactMethod(m);
        c->performAction(Call::Action::ACCEPT);
    }
}

void
CallWidget::smartListSelectionChanged(const QItemSelection &newSel, const QItemSelection &oldSel) {

    Q_UNUSED(oldSel)

    auto newIdx = newSel.indexes().first();
    if (newIdx.parent().isValid())
        return;

    auto newIdxCall = RecentModel::instance()->getActiveCall(RecentModel::instance()->peopleProxy()->mapToSource(newIdx));

    if (newIdxCall == actualCall_)
        return;
    if (actualCall_ != nullptr) {
        actualCall_->performAction(Call::Action::HOLD);
    }
    if (newIdxCall) {
        newIdxCall->performAction(Call::Action::HOLD);
        setActualCall(newIdxCall);
        ui->stackedWidget->setCurrentWidget(ui->videoPage);
    } else {
        setActualCall(nullptr);
        ui->stackedWidget->setCurrentWidget(ui->welcomePage);
    }
}

void
CallWidget::on_callButton_clicked()
{
    if (ui->searchEdit->text().isEmpty())
        return;
    Call* c = CallModel::instance()->dialingCall();
    c->setPeerContactMethod(PhoneDirectoryModel::instance()->getNumber(ui->searchEdit->text()));
    c->performAction(Call::Action::ACCEPT);
    ui->searchEdit->clear();
}

void
CallWidget::on_searchEdit_returnPressed()
{
    on_callButton_clicked();
}

void
CallWidget::on_settingsButton_clicked()
{
    emit NavigationRequested(ScreenEnum::ConfScreen);
}
