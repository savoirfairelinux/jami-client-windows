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

#include "animationhelpers.h"

#include <QGraphicsOpacityEffect>

OpacityAnimation::OpacityAnimation(QWidget* target)
    : QPropertyAnimation(target)
{
    auto effect = new QGraphicsOpacityEffect(target);
    target->setGraphicsEffect(effect);
    setTargetObject(effect);
    setPropertyName("opacity");
}

void
OpacityAnimation::reset()
{
    stop();
    setToStartValue();
}

void
OpacityAnimation::setToStartValue()
{
    targetObject()->setProperty(propertyName(), startValue());
}

void
OpacityAnimation::setToEndValue()
{
    targetObject()->setProperty(propertyName(), endValue());
}

OpacityAnimation::~OpacityAnimation()
{}

FadeAnimation::FadeAnimation(QWidget* target,
                             const quint64 duration,
                             const qreal minOpacity,
                             const qreal maxOpacity,
                             const QEasingCurve curve)
    : OpacityAnimation(target)
{
    if (auto effect = qobject_cast<QGraphicsOpacityEffect*>(targetObject())) {
        effect->setOpacity(maxOpacity);
    }
    setDuration(duration);
    setStartValue(maxOpacity);
    setEndValue(minOpacity);
    setEasingCurve(curve);
}

FadeAnimation::~FadeAnimation()
{}

SizeAnimation::SizeAnimation(QWidget* target,
                             const QRect& startRect,
                             const QRect& endRect,
                             const quint64 duration)
    :QPropertyAnimation(target, "geometry")
{
    setDuration(duration);
    setStartValue(startRect);
    setEndValue(endRect);
}

SizeAnimation::~SizeAnimation()
{}