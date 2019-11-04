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

#include "widgethelpers.h"

FadeOutable::FadeOutable(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    fadeAnimation_ = new FadeAnimation(this, fadeTime_);
    fadeTimer_.setSingleShot(true);
    connect(&fadeTimer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

FadeOutable::~FadeOutable()
{}

void
FadeOutable::resetOpacity()
{
    fadeAnimation_->reset();
    Q_EMIT willReset();
    fadeTimer_.start(fadeDelay_);
}

void
FadeOutable::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    fadeAnimation_->reset();
    Q_EMIT willReset();
    event->ignore();
}

void
FadeOutable::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    fadeTimer_.start(fadeDelay_);
    event->ignore();
}

void
FadeOutable::mouseMoveEvent(QMouseEvent* event)
{
    // start/restart the timer after which the widget will fade
    fadeAnimation_->reset();
    fadeTimer_.start(fadeDelay_);
    Q_EMIT willReset();
    event->ignore();
}

void
FadeOutable::slotTimeout()
{
    if (!shouldFadeOut()) {
        return;
    }
    fadeAnimation_->start();
    Q_EMIT willFadeOut();
}

VignetteWidget::VignetteWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    fadeOutAnimation_ = new FadeAnimation(this, fadeTime_);
}

VignetteWidget::~VignetteWidget()
{}

void
VignetteWidget::slotWillFadeOut()
{
    fadeOutAnimation_->setDirection(QAbstractAnimation::Direction::Forward);
    fadeOutAnimation_->start();
}

void
VignetteWidget::slotWillResetOpacity()
{
    if (fadeOutAnimation_->direction() != QAbstractAnimation::Direction::Forward) {
        return;
    }
    fadeOutAnimation_->setDirection(QAbstractAnimation::Direction::Backward);
    fadeOutAnimation_->start();
}

void
VignetteWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);

    QLinearGradient gradient;

    // top
    gradient.setStart(0, rect().top());
    gradient.setFinalStop(0, rect().top() + height_);
    gradient.setColorAt(0, QColor(0, 0, 0, 96));
    gradient.setColorAt(0.5, QColor(0, 0, 0, 32));
    gradient.setColorAt(1, Qt::transparent);
    painter.fillRect(rect(), gradient);

    // bottom
    gradient.setStart(0, rect().bottom() - height_);
    gradient.setFinalStop(0, rect().bottom());
    gradient.setColorAt(0, Qt::transparent);
    gradient.setColorAt(0.5, QColor(0, 0, 0, 32));
    gradient.setColorAt(1, QColor(0, 0, 0, 96));
    painter.fillRect(rect(), gradient);
}