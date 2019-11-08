/**************************************************************************
* Copyright (C) 2019 by Savoir-faire Linux                                *
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

#include "audiovolumemeter.h"
#include "ui_audiovolumemeter.h"

#include "lrcinstance.h"

#include <QtConcurrent/QtConcurrent>
#include <QPainter>

AudioVolumeMeter::AudioVolumeMeter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioVolumeMeter)
{
    ui->setupUi(this);

    connect(&LRCInstance::avModel(), &lrc::api::AVModel::audioMeter,
        [this](const std::string& id, float level) {
            if (id == "audiolayer_id") {
                audioLevel_ = level;
            }
        });
}

AudioVolumeMeter::~AudioVolumeMeter()
{
    delete ui;
}

void AudioVolumeMeter::startAudioMeter(bool async)
{
    if (LRCInstance::getActiveCalls().size()) {
        return;
    }
    //TODO: control what the UI does when audio meter starts

    auto f = [this] {
        LRCInstance::avModel().startAudioDevice();
        LRCInstance::avModel().setAudioMeterState(true);
    };
    if (async) {
        QtConcurrent::run(f);
    } else {
        f();
    }
}

void AudioVolumeMeter::stopAudioMeter(bool async)
{
    if (LRCInstance::getActiveCalls().size()) {
        return;
    }
    // TODO: control what the UI does when audio meter ends

    auto f = [this] {
        LRCInstance::avModel().stopAudioDevice();
        LRCInstance::avModel().setAudioMeterState(false);
    };
    if (async) {
        QtConcurrent::run(f);
    } else {
        f();
    }
}

QColor
AudioVolumeMeter::getBackgroundColor()
{
    return backgroundColor_;
}

QColor
AudioVolumeMeter::getvolumeColor()
{
    return volumeColor_;
}

void
AudioVolumeMeter::setBackgroundColor(QColor)
{

}

void
AudioVolumeMeter::setVolumeColor(QColor)
{
}

void
AudioVolumeMeter::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    //TODO: implement how to draw the volume meter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QBrush(Qt::red));

    painter.drawEllipse(rect().center(),50,50);
}
