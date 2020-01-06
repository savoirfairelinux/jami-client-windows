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

#include "animationhelpers.h"
#include "popupdialog.h"
#include "utils.h"

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#include <QtQuick/QtQuick>
#include <QQuickItem>
#include <QQuickWidget>

class FadeOutable : public QWidget
{
    Q_OBJECT
public:
    explicit FadeOutable(QWidget* parent = nullptr);
    ~FadeOutable();

    void resetToFadeIn(bool resetTimer);

    void setFadeDelay(const quint64 ms) { fadeDelay_ = ms; }
    void setFadeOutStartLocation(const QRect& rect) { Utils::whileBlocking(fadeAnimation_)->setStartValue(rect); }
    void setFadeOutEndLocation(const QRect& rect) { Utils::whileBlocking(fadeAnimation_)->setEndValue(rect); }

    virtual bool shouldFadeOut() { return false; };

signals:
    void willFadeOut();
    void willReset();

protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void slotTimeout();

private:
    SizeAnimation* fadeAnimation_;
    QTimer fadeTimer_;

    quint64 fadeDelay_{ 2000 };

};

class Blinkable : public QWidget
{
    Q_OBJECT
public:
    explicit Blinkable(QWidget* parent = nullptr);
    ~Blinkable();

    void start();
    void stop();

    void setBlinkTime(const quint64 ms) { blinkTime_ = ms; }
    void setPixmap(QPixmap* pixmap) { pixmap_ = pixmap; }
    void unsetPixmap() { pixmap_ = nullptr; }

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void slotAnimationFinished();

private:
    FadeAnimation* fadeAnimation_;

    quint64 blinkTime_{ 1000 };
    QPixmap* pixmap_{ nullptr };
    QMetaObject::Connection connection_;
};

class VignetteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VignetteWidget(QWidget* parent = nullptr);
    ~VignetteWidget();

    void setDrawRoundedCorner(bool state) { drawRoundedCorner_ = state; }

public slots:
    void slotWillFadeOut();
    void slotWillResetOpacity();
    void updateGeometry(QSize size);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void fillRoundRectPath(QPainter& painter, const QLinearGradient& gradient);

    SizeAnimation* geometryAnimation_;
    constexpr static qreal cornerRadius_ = 10.0f;
    quint64 height_{ 128 };
    quint64 fadeTime_{ 1000 };
    bool drawRoundedCorner_{ false };

};

class PopupWidget : public QWidget
{
    Q_OBJECT
public:
    // insider content of PopupDialog
    explicit PopupWidget(QWidget* parent = nullptr,
                         QColor spikeColor = Qt::white,
                         PopupDialog::SpikeLabelAlignment spikeAlignment = PopupDialog::SpikeLabelAlignment::AlignLeft)
            : QWidget(parent) {
        widgetContainer_ = QSharedPointer<PopupDialog>(new PopupDialog(parent, spikeColor, spikeAlignment));
        setParent(widgetContainer_.data());
        widgetContainer_->insertWidget(this);
    }
    virtual ~PopupWidget() { setParent(nullptr); }

    QWeakPointer<PopupDialog> getContainer() { return widgetContainer_.toWeakRef(); }

protected:
    QSharedPointer<PopupDialog> widgetContainer_;

};

class QmlPopupWidget : public QQuickWidget
{
    Q_OBJECT
public:
    // insider content of PopupDialog
    explicit QmlPopupWidget(const QUrl& source,
                            QWidget* parent = nullptr,
                            QColor spikeColor = Qt::white,
                            PopupDialog::SpikeLabelAlignment spikeAlignment = PopupDialog::SpikeLabelAlignment::None)
        : QQuickWidget(source, parent) {

        widgetContainer_ = QSharedPointer<PopupDialog>(new PopupDialog(parent, spikeColor, spikeAlignment));
        setMouseTracking(true);
        setParent(widgetContainer_.data());
        widgetContainer_->insertWidget(this);
    }
    virtual ~QmlPopupWidget() { setParent(nullptr); }

    QWeakPointer<PopupDialog> getContainer() { return widgetContainer_.toWeakRef(); }

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    QSharedPointer<PopupDialog> widgetContainer_;

};

class Darkenable : public QWidget
{
    Q_OBJECT
public:
    explicit Darkenable(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_NoSystemBackground);
        animation_ = new FadeAnimation(this, 400, 0, .33);
        this->hide();
    };
    ~Darkenable() {};

    void darken() {
        disconnect(connection_);
        this->show();
        animation_->setDirection(QAbstractAnimation::Backward);
        animation_->start();
    }
    void lighten() {
        animation_->setDirection(QAbstractAnimation::Forward);
        disconnect(connection_);
        connection_ = connect(animation_, &QPropertyAnimation::finished,
            [this] {
                this->hide();
            });
        animation_->start();
    }

    void setAnimationTime(const quint64 ms) { animation_->setDuration(ms); }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setBrush(Qt::black);
        auto rect = qobject_cast<QWidget*>(parent())->rect();
        setGeometry(rect);
        painter.drawRect(rect);
    };

private:
    FadeAnimation* animation_;
    QMetaObject::Connection connection_;

};

class FullScreenNotification : public QLabel
{
    Q_OBJECT
public:
    FullScreenNotification(const QString& text, QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setText(text);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        fadeAnimation_ = new FadeAnimation(this, 1000, 0, 0.6f);
        fadeTimer_.setSingleShot(true);
        connect(&fadeTimer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));
        connect(fadeAnimation_, &QAbstractAnimation::finished,
            [this]() { this->hide(); });
    }

protected:
    void showEvent(QShowEvent* event) override {
        fadeTimer_.start(2000);
        QLabel::showEvent(event);
    };

private slots:
    void slotTimeout() { fadeAnimation_->start(); };

private:
    FadeAnimation* fadeAnimation_;
    QTimer fadeTimer_;
};