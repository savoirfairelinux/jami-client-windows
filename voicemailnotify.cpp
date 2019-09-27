/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include "voicemailnotify.h"
#include "ui_voicemailnotify.h"

VoicemailNotifyWidget::VoicemailNotifyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoicemailNotifyWidget)
{
    ui->setupUi(this);
}

VoicemailNotifyWidget::~VoicemailNotifyWidget()
{
    delete ui;
}

void
VoicemailNotifyWidget::setCurrentData(int newVM, int oldVM, int newUrgentVM)
{
    ui->newVoicemailLabel->setText(QString::number(newVM) + " new voicemail");
    ui->urgentVoicemailLabel->setText(QString::number(newUrgentVM) + " urgent voicemail");
    ui->oldVoicemailLabel->setText(QString::number(oldVM) + " old voicemail");
}
