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

#include "videooverlay.h"
#include "ui_videooverlay.h"

#include "callmodel.h"

VideoOverlay::VideoOverlay(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VideoOverlay),
    transferDialog_(new CallUtilsDialog())
{
    ui->setupUi(this);

    ui->chatButton->setCheckable(true);

    actionModel_ = CallModel::instance().userActionModel();
    setAttribute(Qt::WA_NoSystemBackground);

    ui->noMicButton->setCheckable(true);

    connect(actionModel_,&UserActionModel::dataChanged, [=](const QModelIndex& tl, const QModelIndex& br) {
        const int first(tl.row()),last(br.row());
        for(int i = first; i <= last;i++) {
            const QModelIndex& idx = actionModel_->index(i,0);
            switch (idx.data(UserActionModel::Role::ACTION).value<UserActionModel::Action>()) {
            case UserActionModel::Action::MUTE_AUDIO:
                ui->noMicButton->setChecked(idx.data(Qt::CheckStateRole).value<bool>());
                ui->noMicButton->setEnabled(idx.flags() & Qt::ItemIsEnabled);
                break;
            case UserActionModel::Action::MUTE_VIDEO:
                ui->noVideoButton->setChecked(idx.data(Qt::CheckStateRole).value<bool>());
                ui->noVideoButton->setEnabled(idx.flags() & Qt::ItemIsEnabled);
                break;
            case UserActionModel::Action::HOLD:
                ui->holdButton->setChecked(idx.data(Qt::CheckStateRole).value<bool>());
                ui->holdButton->setEnabled(idx.flags() & Qt::ItemIsEnabled);
                break;
            default:
                break;
            }
        }
    });

    connect(CallModel::instance().selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &current, const QModelIndex &previous) {
        Q_UNUSED(previous)
        Call* c = current.data(static_cast<int>(Call::Role::Object)).value<Call*>();
        if (c) {
            if (c->hasParentCall()) {
                ui->holdButton->hide();
                ui->transferButton->hide();
                ui->addPersonButton->hide();
                ui->chatButton->hide();

                ui->joinButton->show();
            } else {
                ui->holdButton->show();
                ui->transferButton->show();
                ui->addPersonButton->show();
                ui->chatButton->show();

                ui->joinButton->hide();
            }
        }
    });
}

VideoOverlay::~VideoOverlay()
{
    delete ui;
    delete transferDialog_;
}

void
VideoOverlay::setName(const QString& name)
{
    ui->nameLabel->setText(name);
}

void
VideoOverlay::setTime(const QString& time)
{
    ui->timerLabel->setText(time);
}

void
VideoOverlay::on_hangupButton_clicked()
{
    actionModel_->execute(UserActionModel::Action::HANGUP);
    ui->chatButton->setChecked(false);
}

void
VideoOverlay::on_chatButton_toggled(bool checked)
{
    emit setChatVisibility(checked);
}

void
VideoOverlay::on_transferButton_clicked()
{
    transferDialog_->setConfMode(false);
    auto pos = this->mapToGlobal(ui->transferButton->pos());
    transferDialog_->move(pos.x()
                          - transferDialog_->size().width()/2
                          + ui->transferButton->size().width()/2,
                          pos.y() - (transferDialog_->height()));
    transferDialog_->show();
}

void
VideoOverlay::on_addPersonButton_clicked()
{
    transferDialog_->setConfMode(true);
    auto pos = this->mapToGlobal(ui->addPersonButton->pos());
    transferDialog_->move(pos.x()
                          - transferDialog_->size().width()/2
                          + ui->addPersonButton->size().width()/2,
                          pos.y() - (transferDialog_->height()));
    transferDialog_->show();
}

void
VideoOverlay::on_holdButton_clicked()
{
    actionModel_->execute(UserActionModel::Action::HOLD);
}

void
VideoOverlay::on_noMicButton_clicked()
{
    actionModel_->execute(UserActionModel::Action::MUTE_AUDIO);
}

void
VideoOverlay::on_noVideoButton_clicked()
{
    actionModel_->execute(UserActionModel::Action::MUTE_VIDEO);
}


void VideoOverlay::on_joinButton_clicked()
{
    CallModel::instance().selectedCall()->joinToParent();
}
