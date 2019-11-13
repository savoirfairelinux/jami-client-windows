/**************************************************************************
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

// Based on: https://stackoverflow.com/questions/14780517/toggle-switch-in-qt

#include "toggleswitch.h"
#include "ringthemeutils.h"

ToggleSwitch::ToggleSwitch(QWidget *parent)
    : QAbstractButton(parent),
    width_(50),
    opacity_(0.000),
    switch_(false),
    margin_(3),
    thumb_("#d5d5d5"),
    textMargin_(18),
    anim_(new QPropertyAnimation(this, "offset", this)),
    style_(SwitchStyle::UWP)
{
    setMouseTracking(true);
    setBrush(QColor(43, 75, 126));
    setOffset(qreal(switch_));

    if (style_ == SwitchStyle::UWP) {
        pinRadius_ = height() * 0.5 - margin_;
        knobRadius_ = pinRadius_ * 0.8;
        height_ = 2 * pinRadius_;
        pinWidth_ = height_ * 2;
        width_ = pinWidth_;
    } else {
        knobRadius_ = height() * 0.5 - margin_;
        pinRadius_ = knobRadius_ * 0.6;
        height_ = 2 * knobRadius_;
        pinWidth_ = height_;
        width_ = pinWidth_ + 2 * knobRadius_;
    }
}

void ToggleSwitch::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setOpacity(isEnabled() ? (switch_ ? 1.0 : 0.5) : 0.12);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (style_ == SwitchStyle::UWP) {
        // pin
        QPainterPath path;
        p.setPen(switch_ ? Qt::NoPen : QPen(Qt::black, 2));
        path.addRoundedRect(QRect(1,
                                  margin_,
                                  pinWidth_,
                                  2 * pinRadius_),
                            pinRadius_, pinRadius_);
        p.fillPath(path, (switch_ ? brush() : Qt::white));
        p.drawPath(path);

        // knob
        p.setPen(Qt::NoPen);
        p.setBrush(switch_ ? Qt::white : Qt::black);
        p.drawEllipse(QPoint(1 + pinRadius_ + offset() * pinWidth_ * 0.5,
                             margin_ + pinRadius_),
                      knobRadius_, knobRadius_);
    } else {
        auto pinch = pinWidth_ / 8;
        p.setPen(Qt::NoPen);
        // pin
        p.setBrush(isEnabled() ? (switch_ ? brush() : Qt::black) : Qt::black);
        p.drawRoundedRect(QRect(1 + knobRadius_,
                                margin_ + knobRadius_ - pinRadius_,
                                pinWidth_,
                                2 * pinRadius_),
                          pinRadius_, pinRadius_);

        // knob
        p.setOpacity(1.0);
        p.setBrush(isEnabled() ? thumb_ : QColor("#bdbdbd"));
        p.drawEllipse(QPoint(1 + knobRadius_ + pinch + offset() * (pinWidth_ - pinch * 2),
                             margin_ + knobRadius_),
                      knobRadius_, knobRadius_);
    }
    p.setOpacity(1.0);
    p.setPen(isEnabled() ? Qt::black : Qt::gray);
    p.drawText(width_ + textMargin_, height_ - margin_ - 1, text_);
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent *e)
{
    QRect activeArea(rect());
    auto textWidth = QFontMetrics(font()).size(Qt::TextSingleLine, text_).width();
    activeArea.setWidth(width_ + textMargin_ + textWidth);
    if (e->button() & Qt::LeftButton
        && activeArea.contains(e->pos())) {
        switch_ = switch_ ? false : true;
        thumb_ = switch_ ? brush_ : QBrush("#d5d5d5");
        if (switch_) {
            anim_->setStartValue(0.0);
            anim_->setEndValue(1.0);
            anim_->setDuration(100);
            anim_->start();
        } else {
            anim_->setStartValue(offset());
            anim_->setEndValue(0.0);
            anim_->setDuration(100);
            anim_->start();
        }
        emit clicked(switch_);
    }
}

void ToggleSwitch::mouseMoveEvent(QMouseEvent *e)
{
    QRect activeArea(rect());
    auto textWidth = QFontMetrics(font()).size(Qt::TextSingleLine, text_).width();
    activeArea.setWidth(width_ + textMargin_ + textWidth);
    if (activeArea.contains(e->pos())) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    QAbstractButton::mouseMoveEvent(e);
}

QSize ToggleSwitch::sizeHint() const
{
    auto textWidth = QFontMetrics(font()).size(Qt::TextSingleLine, text_).width();
    return QSize(width_ + textMargin_ + textWidth, height_ + 2 * margin_);
}

void ToggleSwitch::setChecked(bool state)
{
    switch_ = state;
    setOffset(qreal(switch_));
    thumb_ = switch_ ? brush_ : QBrush("#d5d5d5");
}