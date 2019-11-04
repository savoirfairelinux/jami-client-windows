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

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QPainter>

class FadeOutable : public QWidget
{
    Q_OBJECT
public:
    explicit FadeOutable(QWidget* parent = nullptr);
    ~FadeOutable();

    void resetOpacity();

    void setFadeTime(const quint64 ms) { fadeTime_ = ms; }
    void setFadeDelay(const quint64 ms) { fadeDelay_ = ms; }

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
    FadeAnimation* fadeAnimation_;
    QTimer fadeTimer_;

    quint64 fadeTime_{ 1000 };
    quint64 fadeDelay_{ 2000 };

};

class VignetteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VignetteWidget(QWidget* parent = nullptr);
    ~VignetteWidget();

public slots:
    void slotWillFadeOut();
    void slotWillResetOpacity();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    FadeAnimation* fadeOutAnimation_;
    quint64 height_{ 128 };
    quint64 fadeTime_{ 1000 };

};

class PopupWidget : public QWidget
{
    Q_OBJECT
public:
    // insider content of PopupDialog
    explicit PopupWidget(QWidget* parent = nullptr) {
        widgetContainer_ = new PopupDialog(parent);
        setParent(widgetContainer_);
        widgetContainer_->insertWidget(this);
    }
    virtual ~PopupWidget() {}

    PopupDialog *getContainer() { return widgetContainer_; }

protected:
    PopupDialog *widgetContainer_;

};
