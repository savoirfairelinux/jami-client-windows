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

    actionModel_ = CallModel::instance()->userActionModel();
    setAttribute(Qt::WA_NoSystemBackground);
}

VideoOverlay::~VideoOverlay()
{
    delete ui;
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
}

void
VideoOverlay::on_chatButton_toggled(bool checked)
{
    //TODO : Link this to im class once it's merged
}

