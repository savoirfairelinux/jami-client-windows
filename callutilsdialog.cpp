/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include "callutilsdialog.h"
#include "ui_callutilsdialog.h"

#include <QBitmap>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QScrollBar>

#include "callmodel.h"
#include "phonedirectorymodel.h"
#include "recentmodel.h"
#include "contactmethod.h"
#include "person.h"

CallUtilsDialog::CallUtilsDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::CallUtilsDialog),
    confMode_(false),
    smartListDelegate_(nullptr),
    notCurrentProxyModel_(nullptr),
    spikeMask_(new QPixmap(":/images/spikeMask.png"))
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint);

    ui->spike->setMask(spikeMask_->mask());

    effect_ = new QGraphicsOpacityEffect(this);
    effect_->setOpacity(1.0);
    setGraphicsEffect(effect_);
    fadeAnim_ = new QPropertyAnimation(effect_, "opacity");
    fadeAnim_->setDuration(fadeOverlayTime_);
    fadeAnim_->setStartValue(0.0);
    fadeAnim_->setEndValue(1.0);
    fadeAnim_->setEasingCurve(QEasingCurve::InExpo);

    ui->contactView->verticalScrollBar()->hide();
}

CallUtilsDialog::~CallUtilsDialog()
{
    delete effect_;
    delete spikeMask_;
    delete fadeAnim_;
    delete smartListDelegate_;
    delete notCurrentProxyModel_;
    delete ui;
}

void
CallUtilsDialog::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)

    ui->numberBar->clear();
    if (not notCurrentProxyModel_) {
        notCurrentProxyModel_ = new NotCurrentProxyModel(&RecentModel::instance());
    }
    ui->contactView->setModel(notCurrentProxyModel_);
    if (not smartListDelegate_) {
        smartListDelegate_ = new SmartListDelegate();
    }
    ui->contactView->setItemDelegate(smartListDelegate_);

    emit(isVisible(true));

    fadeAnim_->setDirection(QAbstractAnimation::Forward);
    fadeAnim_->start();
}

void CallUtilsDialog::removeProxyModel()
{
    ui->contactView->setModel(nullptr);
}

void CallUtilsDialog::closeEvent(QCloseEvent* event)
{
    //This prevent a crash happening in Qt5.5 in QSortFilterProxyModel
    Q_UNUSED(event)
    removeProxyModel();
    emit(isVisible(false));
}

void
CallUtilsDialog::on_doTransferButton_clicked()
{
    auto callList = CallModel::instance().getActiveCalls();
    for (auto c : callList) {
        if (c->state() == Call::State::CURRENT) {
            if (not ui->numberBar->text().isEmpty()) {
                auto number = PhoneDirectoryModel::instance().getNumber(ui->numberBar->text());
                CallModel::instance().transfer(c, number);
            }
            removeProxyModel();
            this->close();
            return;
        }
    }
}

void
CallUtilsDialog::setConfMode(bool active)
{
    confMode_ = active;
    ui->doTransferButton->setVisible(not active);
    ui->numberBar->setVisible(not active);
}

void
CallUtilsDialog::on_contactView_doubleClicked(const QModelIndex& index)
{
    removeProxyModel();
    if (not index.isValid())
        return;
    auto realIdx = notCurrentProxyModel_->mapToSource(index);
    if (not RecentModel::instance().hasActiveCall(realIdx)) {
        ContactMethod* m = nullptr;
        if (auto cm = realIdx.data(static_cast<int>(Call::Role::ContactMethod)).value<ContactMethod*>()) {
            m = cm;
        } else {
            if (auto person = realIdx.data(static_cast<int>(Person::Role::Object)).value<Person*>()) {
                m = person->phoneNumbers().first();
            }
        }
        if (confMode_) {
            if (m && !RecentModel::instance().index(0, 0, realIdx).isValid()) {
                Call* c = CallModel::instance().dialingCall(m, CallModel::instance().selectedCall());
                c->performAction(Call::Action::ACCEPT);
            }
        } else {
            if (m) {
                auto activeCall = CallModel::instance().selectedCall();
                CallModel::instance().transfer(activeCall, m);
            }
        }
    } else {
        auto activeCall = CallModel::instance().selectedCall();
        auto call = RecentModel::instance().getActiveCall(realIdx);
        if (not confMode_)
            CallModel::instance().attendedTransfer(activeCall, call);
        else
            CallModel::instance().createJoinOrMergeConferenceFromCall(activeCall, call);
    }
    this->close();
}

void
CallUtilsDialog::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    ui->contactView->verticalScrollBar()->show();
}

void
CallUtilsDialog::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    ui->contactView->verticalScrollBar()->hide();
}
