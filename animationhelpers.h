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

class FadeOutAnimation final : public OpacityAnimation
{
    Q_OBJECT
public:
    explicit FadeOutAnimation(QWidget* target,
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
    ~FadeOutAnimation() {};
};

class FadeOutable : public QWidget
{
    Q_OBJECT
public:
    explicit FadeOutable(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_NoSystemBackground);
        fadeAnimation_ = new FadeOutAnimation(this, fadeTime_);
        fadeTimer_.setSingleShot(true);
        connect(&fadeTimer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    };
    ~FadeOutable() {};

    void resetOpacity() {
        fadeAnimation_->reset();
        fadeTimer_.start(fadeDelay_);
    };

    void setFadeTime(const quint64 ms) { fadeTime_ = ms; }
    void setFadeDelay(const quint64 ms) { fadeDelay_ = ms; }

protected:
    void enterEvent(QEvent* event) override {
        Q_UNUSED(event);
        fadeAnimation_->reset();
    };
    void leaveEvent(QEvent* event) override {
        Q_UNUSED(event);
        fadeTimer_.start(fadeDelay_);
    };
    void mouseMoveEvent(QMouseEvent* event) override {
        Q_UNUSED(event);
        // start/restart the timer after which the widget will fade
        if (fadeTimer_.isActive()) {
            fadeAnimation_->reset();
        } else {
            fadeTimer_.start(fadeDelay_);
        }
        event->ignore();
    };

    virtual bool shouldFadeOut() { return false; };

private slots:
    void slotTimeout() {
        if (!shouldFadeOut()) {
            return;
        }
        fadeAnimation_->start(QAbstractAnimation::KeepWhenStopped);
    };

private:
    FadeOutAnimation* fadeAnimation_;
    QTimer fadeTimer_;

    quint64 fadeTime_{ 1000 };
    quint64 fadeDelay_{ 2000 };

};

class FadeOutableOverlay : public FadeOutable
{
    Q_OBJECT
public:
    explicit FadeOutableOverlay(QWidget* parent = nullptr)
        : FadeOutable(parent)
    {};
    ~FadeOutableOverlay() {};

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing, true);

        // vignette
        QLinearGradient gradient;

        // top
        gradient.setStart(0, rect().top());
        gradient.setFinalStop(0, rect().top() + height_);
        gradient.setColorAt(0, QColor(0, 0, 0, 128));
        gradient.setColorAt(0.5, QColor(0, 0, 0, 64));
        gradient.setColorAt(1, Qt::transparent);
        painter.fillRect(rect(), gradient);

        // bottom
        gradient.setStart(0, rect().bottom() - height_);
        gradient.setFinalStop(0, rect().bottom());
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(0.5, QColor(0, 0, 0, 64));
        gradient.setColorAt(1, QColor(0, 0, 0, 128));
        painter.fillRect(rect(), gradient);
    };

private:
    quint64 height_{ 96 };

};