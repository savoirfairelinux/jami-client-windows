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

#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QMovie>

class OpacityAnimation : public QObject
{
    Q_OBJECT
public:
    explicit OpacityAnimation(QWidget* target, QObject* parent = nullptr);
    ~OpacityAnimation();

    void setFPS(const int& fps);
    void setFrameTime(const int& milliseconds);
    void setDuration(const int& milliseconds);
    void setStartValue(const double& value);
    void setEndValue(const double& value);

    void start();
    void stop();

private slots:
    void updateAnimation();

private:
    QGraphicsOpacityEffect* effect_;
    double value_;

    QWidget* target_;
    QTimer* timer_;
    int frameTime_;
    double t_;
    int duration_;

    double startValue_;
    double endValue_;
};

namespace Ui {
class AnimatedOverlay;
}

class AnimatedOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit AnimatedOverlay(QColor color, QWidget* parent = 0);
    ~AnimatedOverlay();

private:
    Ui::AnimatedOverlay* ui;

    OpacityAnimation* oa_;
};