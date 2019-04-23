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

// From: https://stackoverflow.com/questions/14780517/toggle-switch-in-qt

#include "toggleswitch.h"

ToggleSwitch::ToggleSwitch(QWidget *parent)
    : QAbstractButton(parent),
    height_(16),
    opacity_(0.000),
    switch_(false),
    margin_(3),
    thumb_("#d5d5d5"),
    anim_(new QPropertyAnimation(this, "offset", this))
{
    setOffset(height_ / 2);
    y_ = height_ / 2;
    setBrush(QColor("#009688"));
}

ToggleSwitch::ToggleSwitch(const QBrush &brush, QWidget *parent)
    : QAbstractButton(parent),
    height_(16),
    switch_(false),
    opacity_(0.000),
    margin_(3),
    thumb_("#d5d5d5"),
    anim_(new QPropertyAnimation(this, "offset", this))
{
    setOffset(height_ / 2);
    y_ = height_ / 2;
    setBrush(brush);
}

void ToggleSwitch::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    if (isEnabled()) {
        p.setBrush(switch_ ? brush() : Qt::black);
        p.setOpacity(switch_ ? 0.5 : 0.38);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(QRect(margin_, margin_, width() - 2 * margin_, height() - 2 * margin_), 8.0, 8.0);
        p.setBrush(thumb_);
        p.setOpacity(1.0);
        p.drawEllipse(QRectF(offset() - (height_ / 2), y_ - (height_ / 2), height(), height()));
    } else {
        p.setBrush(Qt::black);
        p.setOpacity(0.12);
        p.drawRoundedRect(QRect(margin_, margin_, width() - 2 * margin_, height() - 2 * margin_), 8.0, 8.0);
        p.setOpacity(1.0);
        p.setBrush(QColor("#BDBDBD"));
        p.drawEllipse(QRectF(offset() - (height_ / 2), y_ - (height_ / 2), height(), height()));
    }
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() & Qt::LeftButton) {
        switch_ = switch_ ? false : true;
        thumb_ = switch_ ? brush_ : QBrush("#d5d5d5");
        if (switch_) {
            anim_->setStartValue(height_ / 2);
            anim_->setEndValue(width() - height_);
            anim_->setDuration(120);
            anim_->start();
        } else {
            anim_->setStartValue(offset());
            anim_->setEndValue(height_ / 2);
            anim_->setDuration(120);
            anim_->start();
        }
    }
    QAbstractButton::mouseReleaseEvent(e);
}

void ToggleSwitch::enterEvent(QEvent *e)
{
    setCursor(Qt::PointingHandCursor);
    QAbstractButton::enterEvent(e);
}

QSize ToggleSwitch::sizeHint() const
{
    return QSize(2 * (height_ + margin_), height_ + 2 * margin_);
}