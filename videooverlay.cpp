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

VideoOverlay::VideoOverlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoOverlay),
    transferDialog_(new TransferDialog())
{
    ui->setupUi(this);

    ui->chatButton->setCheckable(true);
    ui->transferButton->setCheckable(true);
    ui->addPersonButton->setCheckable(true);

    actionModel_ = CallModel::instance()->userActionModel();
    setAttribute(Qt::WA_NoSystemBackground);

    menu_ = new QMenu(this);
    auto muteAudio = new QAction(tr("Mute Audio"), this);
    muteAudio->setCheckable(true);
    connect(muteAudio, &QAction::triggered, [=](bool) {
       actionModel_->execute(UserActionModel::Action::MUTE_AUDIO);
    });
    menu_->addAction(muteAudio);

    auto muteVideo = new QAction(tr("Mute Video"), this);
    muteVideo->setCheckable(true);
    connect(muteVideo, &QAction::triggered, [=](bool) {
        actionModel_->execute(UserActionModel::Action::MUTE_VIDEO);
    });
    menu_->addAction(muteVideo);

    connect(actionModel_,&UserActionModel::dataChanged, [=](const QModelIndex& tl, const QModelIndex& br) {
        const int first(tl.row()),last(br.row());
        for(int i = first; i <= last;i++) {
            const QModelIndex& idx = actionModel_->index(i,0);
            switch (idx.data(UserActionModel::Role::ACTION).value<UserActionModel::Action>()) {
               case UserActionModel::Action::MUTE_AUDIO:
                muteAudio->setChecked(idx.data(Qt::CheckStateRole).value<bool>());
                muteAudio->setEnabled(idx.flags() & Qt::ItemIsEnabled);
                break;
            case UserActionModel::Action::MUTE_VIDEO:
                muteVideo->setChecked(idx.data(Qt::CheckStateRole).value<bool>());
                muteVideo->setEnabled(idx.flags() & Qt::ItemIsEnabled);
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

    ui->moreButton->setMenu(menu_);

    connect(CallModel::instance()->selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &current, const QModelIndex &previous) {
        Q_UNUSED(previous)
        Call* c = current.data(static_cast<int>(Call::Role::Object)).value<Call*>();
        if (c) {
            if (c->hasParentCall()) {
                ui->holdButton->hide();
                ui->moreButton->hide();
                ui->transferButton->hide();
                ui->addPersonButton->hide();

                ui->joinButton->show();
            } else {
                ui->holdButton->show();
                ui->moreButton->show();
                ui->transferButton->show();
                ui->addPersonButton->show();

                ui->joinButton->hide();
            }
        }
    });
}

VideoOverlay::~VideoOverlay()
{
    delete ui;
    delete menu_;
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
VideoOverlay::on_transferButton_toggled(bool checked)
{
    transferDialog_->setConfMode(false);
    auto pos = this->mapToGlobal(ui->transferButton->pos());
    transferDialog_->move(pos.x() + ui->transferButton->width(), pos.y() - (transferDialog_->height()/2));
    checked ? transferDialog_->show() : transferDialog_->hide();
}

void
VideoOverlay::on_addPersonButton_clicked(bool checked)
{
    transferDialog_->setConfMode(true);
    auto pos = this->mapToGlobal(ui->addPersonButton->pos());
    transferDialog_->move(pos.x() + ui->addPersonButton->width(), pos.y() - (transferDialog_->height()/2));
    checked ? transferDialog_->show() : transferDialog_->hide();
}

void
VideoOverlay::on_holdButton_clicked()
{
    actionModel_->execute(UserActionModel::Action::HOLD);
}

void VideoOverlay::on_joinButton_clicked()
{
    CallModel::instance()->createConferenceFromParent();
}
