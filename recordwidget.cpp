/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
* Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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
#include "recordwidget.h"
#include "ui_recordwidget.h"
#include "lrcinstance.h"

RecordWidget::RecordWidget(QWidget *parent) :
    PopupWidget(parent, Qt::black, PopupDialog::SpikeLabelAlignment::AlignHCenter),
    ui(new Ui::RecordWidget)
{
    ui->setupUi(this);
    recordOverlay_ = new RecordOverlay(this);
    previewWidget_ = new VideoRecordPreviewWidget(this);
    recordOverlay_->setParent(previewWidget_);
}

RecordWidget::~RecordWidget()
{
}

bool
RecordWidget::actionToStartRecord()
{
    return true;
}

bool
RecordWidget::actionToFinishRecord()
{
    return true;
}

bool
RecordWidget::actionToReRecord()
{
    return true;
}

bool
RecordWidget::actionToDeleteRecord()
{
    return true;
}

bool
RecordWidget::actionToSend()
{
    return true;
}

bool
RecordWidget::isAudio()
{
    return isAudio_;
}

void
RecordWidget::openRecorder(bool isaudio)
{
    isAudio_ = isaudio;
    widgetContainer_->show();
    if(!isAudio_) {
        LRCInstance::avModel().startPreview();
    }
}

void RecordWidget::resizeEvent(QResizeEvent* event)
{
    recordOverlay_->resize(this->size());
    previewWidget_->resize(this->size());

    previewWidget_->stackUnder(recordOverlay_);
}
