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

#include "mainwindow.h"

FadeOutable::FadeOutable(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    fadeAnimation_ = new SizeAnimation(this, rect(), rect());
    fadeTimer_.setSingleShot(true);
    connect(&fadeTimer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

FadeOutable::~FadeOutable()
{}

void
FadeOutable::resetToFadeIn(bool resetTimer)
{
    fadeAnimation_->stop();
    fadeAnimation_->targetObject()->setProperty(fadeAnimation_->propertyName(), fadeAnimation_->startValue());
    Q_EMIT willReset();
    if(resetTimer)
        fadeTimer_.start(fadeDelay_);
}

void
FadeOutable::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    resetToFadeIn(false);
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
    resetToFadeIn(true);
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

Blinkable::Blinkable(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    fadeAnimation_ = new FadeAnimation(this, blinkTime_);
}

Blinkable::~Blinkable()
{}

void
Blinkable::slotAnimationFinished()
{
    fadeAnimation_->setDirection(fadeAnimation_->direction() == QAbstractAnimation::Forward
                                 ? QAbstractAnimation::Backward
                                 : QAbstractAnimation::Forward);
    fadeAnimation_->start();
}

void
Blinkable::start()
{
    connection_ = connect(fadeAnimation_, &QPropertyAnimation::finished, this, &Blinkable::slotAnimationFinished);
    fadeAnimation_->setDirection(QAbstractAnimation::Forward);
    fadeAnimation_->start();
}

void
Blinkable::stop()
{
    disconnect(connection_);
    fadeAnimation_->stop();
}

void
Blinkable::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    if(!pixmap_)
        return;
    QPainter painter(this);
    painter.drawPixmap(rect(), *pixmap_);
}

VignetteWidget::VignetteWidget(QWidget* parent)
    : QWidget(parent),
      geometryAnimation_(new SizeAnimation(this, rect(), rect()))
{
    setAttribute(Qt::WA_NoSystemBackground);
}

VignetteWidget::~VignetteWidget()
{}

void
VignetteWidget::slotWillFadeOut()
{
    geometryAnimation_->setDirection(QAbstractAnimation::Direction::Forward);
    geometryAnimation_->start();
}

void
VignetteWidget::slotWillResetOpacity()
{
    if (geometryAnimation_->direction() != QAbstractAnimation::Direction::Forward) {
        return;
    }
    geometryAnimation_->setDirection(QAbstractAnimation::Direction::Backward);
    geometryAnimation_->start();
}

void
VignetteWidget::updateGeometry(QSize size)
{
    resize(size);
    QRect endRect = rect();
    Utils::whileBlocking(geometryAnimation_)->setStartValue(endRect);
    endRect.setHeight(endRect.height() + height_);
    endRect.setY(endRect.y() - height_);
    Utils::whileBlocking(geometryAnimation_)->setEndValue(endRect);
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

    // draw upper rounded corner or just gradient itself
    fillRoundRectPath(painter, gradient);

    // bottom
    gradient.setStart(0, rect().bottom() - height_);
    gradient.setFinalStop(0, rect().bottom());
    gradient.setColorAt(0, Qt::transparent);
    gradient.setColorAt(0.5, QColor(0, 0, 0, 32));
    gradient.setColorAt(1, QColor(0, 0, 0, 96));

    // draw bottom rounded corner or just gradient itself
    fillRoundRectPath(painter, gradient);

}

void
VignetteWidget::fillRoundRectPath(QPainter& painter, const QLinearGradient & gradient)
{
    if (drawRoundedCorner_) {
        Utils::fillRoundRectPath(painter, gradient, rect(), cornerRadius_);
    } else {
        painter.fillRect(rect(), gradient);
    }
}

void
QmlPopupWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (auto cp = getContainer().toStrongRef()) {
        if (auto mainWindow = qobject_cast<MainWindow*>(cp->parentWidget())) {
            mainWindow->darken();
        }
    }
}

void
QmlPopupWidget::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    if (auto cp = getContainer().toStrongRef()) {
        if (auto mainWindow = qobject_cast<MainWindow*>(cp->parentWidget())) {
            mainWindow->lighten();
        }
    }
}