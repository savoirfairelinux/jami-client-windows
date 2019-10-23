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
    connect(this, SIGNAL(toggled(bool)),
            this, SLOT(slotToggled(bool)));
}

OverlayButton::~OverlayButton()
{}

void
OverlayButton::setOriginPix(QPixmap originPixPath)
{
    normalPixmap_ = originPixPath;
    setIcon(normalPixmap_);
}

QPixmap
OverlayButton::getOriginPix() const
{
    return normalPixmap_;
}

void
OverlayButton::setCheckedPix(QPixmap checkedPixPath)
{
    checkedPixmap_ = checkedPixPath;
}

QPixmap
OverlayButton::getCheckedPix() const
{
    return checkedPixmap_;
}

void
OverlayButton::setTintColor(QColor tint_color)
{
    tintColor_ = tint_color;
    tintNormalPixmap_ = Utils::generateTintedPixmap(normalPixmap_, tintColor_);
    tintCheckedPixmap_ = Utils::generateTintedPixmap(checkedPixmap_, tintColor_);
}

QColor
OverlayButton::getTintColor() const
{
    return tintColor_;
}

void
OverlayButton::updateIcon()
{
    if (isSelected_) {
        setIcon(isHovered_ ? tintCheckedPixmap_ : checkedPixmap_);
    } else {
        setIcon(isHovered_ ? tintNormalPixmap_ : normalPixmap_);
    }
}

void
OverlayButton::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    isHovered_ = true;
    updateIcon();
}

void
OverlayButton::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    isHovered_ = false;
    updateIcon();
}

void
OverlayButton::slotToggled(bool checked)
{
    isSelected_ = checked;
    updateIcon();
}

void
OverlayButton::resetToOriginal()
{
    setIcon(normalPixmap_);
}

void
OverlayButton::setOverlayButtonChecked(bool checked)
{
    Utils::whileBlocking(this)->setChecked(checked);
    isSelected_ = checked;
    updateIcon();
}
