/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                                *
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

#include <QTimer>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

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
    explicit OpacityAnimation(QWidget* target)
        : QPropertyAnimation(target)
    {
        auto effect = new QGraphicsOpacityEffect(target);
        target->setGraphicsEffect(effect);
        setTargetObject(effect);
        setPropertyName("opacity");
    }
    virtual ~OpacityAnimation() {};

    void reset() {
        stop();
        targetObject()->setProperty(propertyName(), startValue());
    };
};

class FadeAnimation final : public OpacityAnimation
{
    Q_OBJECT
public:
    explicit FadeAnimation(QWidget* target,
                           const quint64 duration = 1000,
                           const qreal minOpacity = 0.0,
                           const qreal maxOpacity = MAX_OPACITY)
        : OpacityAnimation(target)
    {
        if (auto effect = qobject_cast<QGraphicsOpacityEffect*>(targetObject())) {
            effect->setOpacity(maxOpacity);
        }
        setDuration(duration);
        setStartValue(maxOpacity);
        setEndValue(minOpacity);
        setEasingCurve(QEasingCurve::OutQuad);
    }
    ~FadeAnimation() {};
};

class FadeOutable : public QWidget
{
    Q_OBJECT
public:
    explicit FadeOutable(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_NoSystemBackground);
        fadeAnimation_ = new FadeAnimation(this, fadeTime_);
        fadeTimer_.setSingleShot(true);
        connect(&fadeTimer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    };
    ~FadeOutable() {};

    void resetOpacity() {
        fadeAnimation_->reset();
        Q_EMIT willReset();
        fadeTimer_.start(fadeDelay_);
    };

    void setFadeTime(const quint64 ms) { fadeTime_ = ms; }
    void setFadeDelay(const quint64 ms) { fadeDelay_ = ms; }

    virtual bool shouldFadeOut() { return false; };

signals:
    void willFadeOut();
    void willReset();

protected:
    void enterEvent(QEvent* event) override {
        Q_UNUSED(event);
        fadeAnimation_->reset();
        Q_EMIT willReset();
        event->ignore();
    };
    void leaveEvent(QEvent* event) override {
        Q_UNUSED(event);
        fadeTimer_.start(fadeDelay_);
        event->ignore();
    };
    void mouseMoveEvent(QMouseEvent* event) override {
        // start/restart the timer after which the widget will fade
        fadeAnimation_->reset();
        fadeTimer_.start(fadeDelay_);
        Q_EMIT willReset();
        event->ignore();
    };

private slots:
    void slotTimeout() {
        if (!shouldFadeOut()) {
            return;
        }
        fadeAnimation_->start();
        Q_EMIT willFadeOut();
    };

private:
    FadeAnimation* fadeAnimation_;
    QTimer fadeTimer_;

    quint64 fadeTime_{ 1000 };
    quint64 fadeDelay_{ 2000 };

};

class VignetteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VignetteWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_NoSystemBackground);
        fadeAnimation_ = new FadeAnimation(this, fadeTime_);
    };
    ~VignetteWidget() {};

public slots:
    void slotWillFadeOut() {
        fadeAnimation_->setDirection(QAbstractAnimation::Direction::Forward);
        fadeAnimation_->start();
    };
    void slotWillResetOpacity() {
        if (fadeAnimation_->direction() == QAbstractAnimation::Direction::Forward) {
            fadeAnimation_->setDirection(QAbstractAnimation::Direction::Backward);
            fadeAnimation_->start();
        }
    };

protected:
    void paintEvent(QPaintEvent *event) override {
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
    };

private:
    FadeAnimation* fadeAnimation_;
    quint64 height_{ 128 };
    quint64 fadeTime_{ 1000 };

};