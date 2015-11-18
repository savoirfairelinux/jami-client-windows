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

    actionModel_ = CallModel::instance().userActionModel();
    setAttribute(Qt::WA_NoSystemBackground);

    auto muteAudio = new QAction(tr("Mute Audio"), this);
    muteAudio->setCheckable(true);
    connect(muteAudio, &QAction::triggered, [=](bool) {
       actionModel_->execute(UserActionModel::Action::MUTE_AUDIO);
    });

    auto muteVideo = new QAction(tr("Mute Video"), this);
    muteVideo->setCheckable(true);
    connect(muteVideo, &QAction::triggered, [=](bool) {
        actionModel_->execute(UserActionModel::Action::MUTE_VIDEO);
    });

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

    connect(ui->nomicButton, &QPushButton::toggled, [=](bool) {
       actionModel_->execute(UserActionModel::Action::MUTE_AUDIO);
    });


	connect(ui->novideoButton, &QPushButton::toggled, [=](bool) {
       actionModel_->execute(UserActionModel::Action::MUTE_VIDEO);
    });
    
    
    setStyleSheet("QPushButton#holdButton{ border-image: url(:/images/video-conf/btn-pause.svg);}"
				  "QPushButton#holdButton:checked{ border-image: url(:/images/video-conf/btn-pause-toggled.svg);}"
				  "QPushButton#hangupButton{ border-image: url(:/images/video-conf/btn-hangup.svg);}"
				  "QPushButton#chatButton{ border-image: url(:/images/video-conf/btn-chat.svg);}"
				  "QPushButton#nomicButton{ border-image: url(:/images/video-conf/btn-nomic.svg);}"
				  "QPushButton#nomicButton:checked{ border-image: url(:/images/video-conf/btn-nomic-toggled.svg);}"
				  "QPushButton#novideoButton{ border-image: url(:/images/video-conf/btn-novideo.svg);}"
				  "QPushButton#novideoButton:checked{ border-image: url(:/images/video-conf/btn-novideo-toggled.svg);}"
				  "QPushButton#transferButton{ border-image: url(:/images/video-conf/btn-missing.svg);}"
	);
	
	
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
VideoOverlay::on_holdButton_toggled(bool checked)
{
    Q_UNUSED(checked)
    actionModel_->execute(UserActionModel::Action::HOLD);
    
    ui->chatButton->setVisible(!checked);
    ui->nomicButton->setVisible(!checked);
    ui->novideoButton->setVisible(!checked);
    ui->transferButton->setVisible(!checked);
    
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
    auto pos = this->mapToGlobal(ui->transferButton->pos());
    transferDialog_->move(pos.x() + ui->transferButton->width(), pos.y() - (transferDialog_->height()/2));
    checked ? transferDialog_->show() : transferDialog_->hide();
}
