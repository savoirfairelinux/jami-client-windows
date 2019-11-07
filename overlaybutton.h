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
#pragma once

#include <QColor>
#include <QImage>
#include <QPushButton>

class OverlayButton : public QPushButton {
    Q_OBJECT;

    Q_PROPERTY(QPixmap originPix READ getOriginPix WRITE setOriginPix DESIGNABLE true);
    Q_PROPERTY(QPixmap checkedPix READ getCheckedPix WRITE setCheckedPix DESIGNABLE true);
    Q_PROPERTY(QColor tintColor READ getTintColor WRITE setTintColor DESIGNABLE true);

public:
    explicit OverlayButton(QWidget* parent = nullptr);
    ~OverlayButton();

    void resetToOriginal();
    void setCheckedState(bool);

public:
    void setOriginPix(QPixmap);
    QPixmap getOriginPix() const;
    void setCheckedPix(QPixmap);
    QPixmap getCheckedPix() const;
    void setTintColor(QColor);
    QColor getTintColor() const;

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent* event) override;

private:
    void updateIcon();

private slots:
    void slotToggled(bool);

private:
    bool isHovered_ = false;
    bool isSelected_ = false;

    QPixmap normalPixmap_;
    QPixmap checkedPixmap_;
    QPixmap tintNormalPixmap_;
    QPixmap tintCheckedPixmap_;

    QColor tintColor_;
};
