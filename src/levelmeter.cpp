/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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
#include "levelmeter.h"

#include <QPainter>
#include <QTimer>
#include <QDebug>

#include <algorithm>
#include <cmath>
#include <limits>

constexpr static const int redrawIntervalMs = 16;
constexpr static const int rmsThreshold = 0.01;

LevelMeter::LevelMeter(QWidget *parent)
    : QProgressBar(parent)
    , rmsLevel_(0.0)
    , currentLevel_(0.0)
    , redrawTimer_(new QTimer(this))
{
}

LevelMeter::~LevelMeter()
{
}

void LevelMeter::start()
{
    rmsLevel_ = 0;
    currentLevel_ = 0;
    connect(redrawTimer_, SIGNAL(timeout()), this, SLOT(redrawTimerExpired()));
    redrawTimer_->start(redrawIntervalMs);
}

void LevelMeter::stop()
{
    disconnect(redrawTimer_, SIGNAL(timeout()), this, SLOT(redrawTimerExpired()));
    redrawTimer_->stop();
}

void LevelMeter::setLevel(float rmsLevel)
{
    rmsLevel_ = rmsLevel;
    update();
}

void LevelMeter::redrawTimerExpired()
{
    animateLevel();

    this->setValue(std::clamp(currentLevel_ * 100.0f, 0.0f, 100.0f));

    update();
}

void LevelMeter::animateLevel()
{
    auto distance = rmsLevel_ - currentLevel_;
    auto velocity = 0.5f * (distance);
    if (abs(distance) <= rmsThreshold) {
        currentLevel_ = rmsLevel_;
        return;
    }
    currentLevel_ += velocity;
}