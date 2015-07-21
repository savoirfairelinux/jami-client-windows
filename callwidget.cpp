/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
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

#include "audio/settings.h"
#include "personmodel.h"
#include "person.h"
#include "fallbackpersoncollection.h"
#include "categorizedcontactmodel.h"
#include "localhistorycollection.h"
#include "media/text.h"
#include "media/recording.h"
#include "media/textrecording.h"

#include "wizarddialog.h"
#include "windowscontactbackend.h"
#include "contactdialog.h"
#include "contactpicker.h"
#include "contactmethodpicker.h"

CallWidget::CallWidget(QWidget *parent) :
    NavWidget(Main ,parent),
    ui(new Ui::CallWidget),
    menu_(new QMenu())
{
    ui->setupUi(this);

    ui->callInvite->setVisible(false);
    ui->videoWidget->setVisible(false);

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

        ui->callList->setModel(callModel_);
        ui->callList->setSelectionModel(callModel_->selectionModel());

        auto personCollection = PersonModel::instance()->
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

                auto copyAction = new QAction("Copy number", this);
                menu.addAction(copyAction);
                connect(copyAction, &QAction::triggered, [=]() {
                    QApplication::clipboard()->setText(contactMethod->uri());
                });
                if (not contactMethod->contact()) {
                    auto addNew = new QAction("Add to new contact", this);
                    menu.addAction(addNew);
                    connect(addNew, &QAction::triggered, [=]() {
                        ContactDialog dialog(contactMethod->uri());
                        auto ret = dialog.exec();
                        if (!ret || dialog.getName().isEmpty())
                            return;
                        auto *newPerson = new Person();
                        newPerson->setFormattedName(dialog.getName());
                        Person::ContactMethods cM;
                        cM.append(contactMethod);
                        newPerson->setContactMethods(cM);
                        PersonModel::instance()->addNewPerson(newPerson, personCollection);
                    });
                    auto addExisting = new QAction("Add to existing contact", this);
                    menu.addAction(addExisting);
                    connect(addExisting, &QAction::triggered, [=]() {
                        CategorizedContactModel::instance()->setUnreachableHidden(false);
                        ContactPicker contactPicker;
                        contactPicker.move(globalPos.x(), globalPos.y() - (contactPicker.height()/2));
                        auto ret = contactPicker.exec();
                        if (!ret)
                            return;
                        auto p = contactPicker.getPersonSelected();
                        Person::ContactMethods cM (p->phoneNumbers());
                        cM.append(contactMethod);
                        p->setContactMethods(cM);
                        p->save();
                        CategorizedContactModel::instance()->setUnreachableHidden(true);
                    });
                }
                menu.exec(globalPos);
            }
        });

        ui->sortComboBox->setModel(CategorizedHistoryModel::SortedProxy::instance()->categoryModel());

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
    delete historyDelegate_;
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
            ui->ringIdLabel->setText(
                         "Your Ring ID: " + username.toString());
            found = true;
            return;
        }
    }
    if (not found){
        ui->ringIdLabel->setText("NO RING ACCOUNT FOUND");
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
            ui->ringIdLabel->setText(
                        "Your Ring ID: " + username);
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
    if (!call->account()->isAutoAnswer()) {
        ui->callLabel->setText("Call from " + call->formattedName());
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
}

void
CallWidget::on_refuseButton_clicked()
{
    if (actualCall_ == nullptr)
        return;
    actualCall_->performAction(Call::Action::REFUSE);
    setActualCall(nullptr);
    ui->callInvite->setVisible(false);
}

void
CallWidget::addedCall(Call* call, Call* parent)
{
    Q_UNUSED(parent);
    if (call->direction() == Call::Direction::OUTGOING) {
        displaySpinner(true);
        setActualCall(call);
    }
}

void
CallWidget::callStateChanged(Call* call, Call::State previousState)
{
    Q_UNUSED(previousState)
    if (call == nullptr)
        return;
    ui->callList->setCurrentIndex(callModel_->getIndex(actualCall_));
    if (call->state() == Call::State::OVER
            || call->state() == Call::State::ERROR
            || call->state() == Call::State::FAILURE
            || call->state() == Call::State::ABORTED) {
        setActualCall(nullptr);
        ui->videoWidget->hide();
        displaySpinner(false);
        auto onHoldCall = callModel_->getActiveCalls().first();
        if (onHoldCall != nullptr && onHoldCall->state() == Call::State::HOLD) {
            setActualCall(onHoldCall);
            onHoldCall->performAction(Call::Action::HOLD);
        }
    } else if (call->state() == Call::State::CURRENT) {
        displaySpinner(false);
        ui->videoWidget->show();
    }
    ui->callStateLabel->setText("Call State : " + call->toHumanStateName());
}

void
CallWidget::on_callList_activated(const QModelIndex &index)
{
    Call *callSelected = callModel_->getCall(index);
    if (actualCall_ != nullptr) {
        if (callSelected == actualCall_)
            return;
        ui->videoWidget->hide();
        actualCall_->performAction(Call::Action::HOLD);
    }
    setActualCall(callSelected);
    actualCall_->performAction(Call::Action::HOLD);
    ui->videoWidget->show();
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

    auto number = index.model()->data(index, static_cast<int>(Call::Role::ContactMethod)).value<ContactMethod*>();
    if (number) {
        auto outCall = CallModel::instance()->dialingCall();
        outCall->setDialNumber(number);
        outCall->performAction(Call::Action::ACCEPT);
    }
}

void
CallWidget::setActualCall(Call* value)
{
    actualCall_ = value;
    ui->instantMessagingWidget->setMediaText(actualCall_);
}

void
CallWidget::on_sortComboBox_currentIndexChanged(int index)
{
    auto idx = CategorizedHistoryModel::SortedProxy::instance()->
            categoryModel()->index(index, 0);
    CategorizedHistoryModel::SortedProxy::instance()->categorySelectionModel()->
            setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
    ui->historyList->setModel(CategorizedHistoryModel::SortedProxy::instance()->model());
}

void
CallWidget::displaySpinner(bool display)
{
    display ? ui->spinnerWidget->show() : ui->spinnerWidget->hide();
    if (ui->spinnerLabel->movie())
        display ? ui->spinnerLabel->movie()->start() : ui->spinnerLabel->movie()->stop();
}

void CallWidget::on_cancelButton_clicked()
{
    if (actualCall_)
        actualCall_->performAction(Call::Action::REFUSE);
}
