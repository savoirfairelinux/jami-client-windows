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

#include "videooverlay.h"
#include "ui_videooverlay.h"

#include "callmodel.h"

VideoOverlay::VideoOverlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoOverlay)
{
    ui->setupUi(this);

    ui->chatButton->setCheckable(true);

    actionModel_ = CallModel::instance()->userActionModel();
    setAttribute(Qt::WA_NoSystemBackground);

    menu_ = new QMenu(this);
    auto muteAudio = new QAction("Mute Audio", this);
    muteAudio->setCheckable(true);
    connect(muteAudio, &QAction::triggered, [=](bool) {
       actionModel_->execute(UserActionModel::Action::MUTE_AUDIO);
    });
    menu_->addAction(muteAudio);

    auto muteVideo = new QAction("Mute Video", this);
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
            default:
                break;
            }
        }
    });

    ui->moreButton->setMenu(menu_);
}

VideoOverlay::~VideoOverlay()
{
    delete ui;
    delete menu_;
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
VideoOverlay::on_holdButton_toggled(bool checked)
{
    Q_UNUSED(checked)
    actionModel_->execute(UserActionModel::Action::HOLD);
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

