/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
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

#include "cliprecorder.h"
#include "ui_cliprecorder.h"

#include "utils.h"

ClipRecorder::ClipRecorder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClipRecorder)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

ClipRecorder::~ClipRecorder()
{
    delete ui;
}

void
ClipRecorder::setPage(RecordPage pageType)
{
    Utils::setStackWidget(ui->stackedWidget,
                          pageType == RecordPage::Audio ? qobject_cast<QWidget*>(ui->audioRecordPage) : qobject_cast<QWidget*>(ui->videoRecordPage));
    if(pageType == RecordPage::Audio)
    {
        ui->audioRecordPage->show();
    }
}
