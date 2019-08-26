/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
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
#include "overlayButton.h"

#include "utils.h"

#include <QEvent>

OverlayButton::OverlayButton(QWidget *parent)
    : QPushButton(parent)
{
    btnCallbacks_.push_back(&OverlayButton::hoverEnter);
    btnCallbacks_.push_back(&OverlayButton::hoverLeave);
    btnCallbacks_.push_back(&OverlayButton::mousePress);
}

OverlayButton::~OverlayButton()
{
}

void OverlayButton::setOriginPix(QPixmap originPixPath)
{
    pathOriginal_ = originPixPath;
}

QPixmap OverlayButton::getOriginPix() const
{
    return pathOriginal_;
}

void OverlayButton::setCheckedPix(QPixmap checkedPixPath)
{
    pathChecked_ = checkedPixPath;
}

QPixmap OverlayButton::getCheckedPix() const
{
    return pathChecked_;
}

void OverlayButton::setTintColor(QColor tint_color)
{
    tintColor_ = tint_color;
}

QColor OverlayButton::getTintColor() const
{
    return tintColor_;
}

bool OverlayButton::hoverEnter(QEvent* event)
{
    if (event->type() != QEvent::HoverEnter)
        return false;
    isHovered_ = true;
    return true;
}

bool OverlayButton::hoverLeave(QEvent* event)
{
    if (event->type() != QEvent::HoverLeave)
        return false;
    isHovered_ = false;
    return true;
}

bool OverlayButton::mousePress(QEvent* event)
{
    if (event->type() != QEvent::MouseButtonPress)
        return false;
    isSelected_ = !isSelected_;
    return true;
}

void OverlayButton::iconUpdate(QEvent* event)
{
    switch (event->type()) {
    case QEvent::HoverEnter: break;
    case QEvent::HoverLeave: break;
    case QEvent::MouseButtonPress: break;
    default:
        return;
    }

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

bool OverlayButton::event(QEvent* event)
{
    QPushButton::event(event);
    if (isFirstTime_)
    {
        originIc_ = QPixmap(pathOriginal_);
        checkedIc_ = QPixmap(pathChecked_);
        tintOriginIc_ = Utils::generateTintedPixmap(originIc_, tintColor_);
        tintCheckedIc_ = Utils::generateTintedPixmap(checkedIc_, tintColor_);
        setIcon(originIc_);
        isFirstTime_ = false;
    }
    bool isHandle = false;
    // iterate the handlers of this class
    for (auto go : btnCallbacks_)
    {
        isHandle = (this->*go)(event);
    }
    // emit the signal so that other callback can be defined outside of this class
    iconUpdate(event);
    emit signalBtnEvent(event);

    return isHandle;
}
