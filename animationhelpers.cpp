/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
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

#include "animationhelpers.h"

#include "ui_animatedoverlay.h"

#include <QTimer>
#include <QtMath>

OpacityAnimation::OpacityAnimation(QWidget* target, QObject* parent)
    : QObject(parent),
    target_(target),
    timer_(nullptr),
    frameTime_((1.0 / 24.0) * 1000),
    startValue_(0.0),
    endValue_(1.0),
    t_(0),
    value_(0),
    duration_(1000)
{
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateAnimation()));

    effect_ = new QGraphicsOpacityEffect(this);
    effect_->setOpacity(startValue_);

    target_->setGraphicsEffect(effect_);
    target_->setAutoFillBackground(true);
}

OpacityAnimation::~OpacityAnimation()
{
}

void
OpacityAnimation::setFPS(const int& fps)
{
    frameTime_ = (1.0 / static_cast<double>(fps)) * 1000;
}

void
OpacityAnimation::setFrameTime(const int& milliseconds)
{
    frameTime_ = milliseconds;
}

void
OpacityAnimation::setDuration(const int& milliseconds)
{
    duration_ = milliseconds;
}

void
OpacityAnimation::setStartValue(const double& value)
{
    startValue_ = value;
    effect_->setOpacity(startValue_);
}

void
OpacityAnimation::setEndValue(const double& value)
{
    endValue_ = value;
}

void
OpacityAnimation::start()
{
    timer_->start(frameTime_);
}

void
OpacityAnimation::stop()
{
    timer_->stop();
}

void
OpacityAnimation::updateAnimation()
{
    double d = (startValue_ + endValue_) * 0.5;
    double a = abs(startValue_ - endValue_) * 0.5;

    t_ += frameTime_;
    value_ = a * sin(2 * M_PI * t_ * duration_ * .000001) + d;
    effect_->setOpacity(value_);
    target_->update();
}

AnimatedOverlay::AnimatedOverlay(QColor color, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AnimatedOverlay)
{
    ui->setupUi(this);
    ui->backgroundLabel->setAutoFillBackground(true);
    auto values = QString("%1,%2,%3,255")
        .arg(color.red())
        .arg(color.green())
        .arg(color.green());
    ui->backgroundLabel->setStyleSheet("background-color: rgba(" + values + ");");

    oa_ = new OpacityAnimation(this, this);
    oa_->setFPS(16);
    oa_->setDuration(1000);
    oa_->setStartValue(0.0);
    oa_->setEndValue(0.25);
    oa_->start();
}

AnimatedOverlay::~AnimatedOverlay()
{
    disconnect(this);
    delete ui;
}
