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

#pragma once

#include <QWidget>
#include <QPropertyAnimation>

// TODO: fix when changing Qt version
// Full(1.0) opacity bug affecting many Qt versions (macOS + win10)
// causing the render to take a buggy code path which can be avoided
// by using opacity values other than precisely 1.0.
// https://bugreports.qt.io/browse/QTBUG-65981
// https://bugreports.qt.io/browse/QTBUG-66803
constexpr static qreal MAX_OPACITY { 0.9999999999980000442 };

class OpacityAnimation : public QPropertyAnimation
{
    Q_OBJECT
public:
    explicit OpacityAnimation(QWidget* target);
    virtual ~OpacityAnimation();

    void reset();
    void setToStartValue();
    void setToEndValue();
};

class FadeAnimation : public OpacityAnimation
{
    Q_OBJECT
public:
    explicit FadeAnimation(QWidget* target,
                           const quint64 duration = 1000,
                           const qreal minOpacity = 0.0,
                           const qreal maxOpacity = MAX_OPACITY,
                           const QEasingCurve curve = QEasingCurve::OutQuad);
    ~FadeAnimation();
};

class SizeAnimation : public QPropertyAnimation
{
    Q_OBJECT
public:
    explicit SizeAnimation(QWidget* target,
                           const QRect& startRect = QRect(),
                           const QRect& endRect = QRect(),
                           const quint64 duration = 1000);
    ~SizeAnimation();
};