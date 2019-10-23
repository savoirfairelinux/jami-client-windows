/**************************************************************************
* Copyright (C) 2019 by Savoir-faire Linux                                *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
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
#include "overlaybutton.h"

#include "../misc/utils.h"

#include <QEvent>

OverlayButton::OverlayButton(QWidget *parent)
    : QPushButton(parent)
{
    btnCallbacks_.push_back(
        [this](QEvent* event) {
            if (event->type() == QEvent::HoverEnter) isHovered_ = true;
            return true;
        });
    btnCallbacks_.push_back(
        [this](QEvent* event) {
            if (event->type() == QEvent::HoverLeave) isHovered_ = false;
            return true;
        });

    connect(this, SIGNAL(toggled(bool)), this, SLOT(slotOnToggle(bool)));
}

OverlayButton::~OverlayButton()
{
}

void
OverlayButton::setOriginPix(QPixmap originPixPath)
{
    pathOriginal_ = originPixPath;
}

QPixmap
OverlayButton::getOriginPix() const
{
    return pathOriginal_;
}

void
OverlayButton::setCheckedPix(QPixmap checkedPixPath)
{
    pathChecked_ = checkedPixPath;
}

QPixmap
OverlayButton::getCheckedPix() const
{
    return pathChecked_;
}

void
OverlayButton::setTintColor(QColor tint_color)
{
    tintColor_ = tint_color;
}

QColor
OverlayButton::getTintColor() const
{
    return tintColor_;
}

void
OverlayButton::updateIcon(QEvent* event)
{
    if (!event)
        return;
    if (event->type() != QEvent::HoverEnter &&
        event->type() != QEvent::HoverLeave) {
        return;
    }
    setButtonIcon();
}

void
OverlayButton::setButtonIcon()
{
    if (isSelected_) {
        if (isHovered_) {
            setIcon(tintCheckedIc_);
        } else {
            setIcon(checkedIc_);
        }
    } else {
        if (isHovered_) {
            setIcon(tintOriginIc_);
        } else {
            setIcon(originIc_);
        }
    }
}

bool
OverlayButton::event(QEvent* event)
{
    QPushButton::event(event);

    if (isFirstTime_) {
        originIc_ = QPixmap(pathOriginal_);
        checkedIc_ = QPixmap(pathChecked_);
        tintOriginIc_ = Utils::generateTintedPixmap(originIc_, tintColor_);
        tintCheckedIc_ = Utils::generateTintedPixmap(checkedIc_, tintColor_);
        setIcon(originIc_);
        isFirstTime_ = false;
    }

    bool isHandled = false;
    // iterate the handlers of this class
    for (auto cb : btnCallbacks_) {
        isHandled = cb(event);
    }
    // emit the signal so that other callback can be defined outside of this class
    updateIcon(event);
    emit signalBtnEvent(event);

    return isHandled;
}

void
OverlayButton::slotOnToggle(bool checked)
{
    isSelected_ = checked;
    setButtonIcon();
}

void
OverlayButton::resetToOriginal()
{
    setIcon(originIc_);
}

void
OverlayButton::setOverlayButtonChecked(bool checked)
{
    Utils::whileBlocking(this)->setChecked(checked);
    isSelected_ = checked;
    setButtonIcon();
}
