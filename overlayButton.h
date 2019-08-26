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
#pragma once

#include <QColor>
#include <QImage>
#include <QPushButton>

class OverlayButton : public QPushButton {
    Q_OBJECT;

    Q_PROPERTY(QPixmap originPix READ getOriginPix WRITE setOriginPix DESIGNABLE true NOTIFY originPixChanged);
    Q_PROPERTY(QPixmap checkedPix READ getCheckedPix WRITE setCheckedPix DESIGNABLE true NOTIFY checkedPixChanged);
    Q_PROPERTY(QColor tintColor READ getTintColor WRITE setTintColor DESIGNABLE true NOTIFY tintColorChanged);

public:
    OverlayButton(QWidget*);
    virtual ~OverlayButton();
    typedef bool (OverlayButton::*EventCallback)(QEvent*);

public:
    void setOriginPix(QPixmap);
    QPixmap getOriginPix() const;
    void setCheckedPix(QPixmap);
    QPixmap getCheckedPix() const;
    void setTintColor(QColor);
    QColor getTintColor() const;

signals:
    void signalBtnEvent(QEvent* event);
    void originPixChanged(QString);
    void checkedPixChanged(QString);
    void tintColorChanged(QColor);

protected:
    virtual bool event(QEvent* event);

private:
    bool hoverEnter(QEvent* event);
    bool hoverLeave(QEvent* event);
    bool mousePress(QEvent* event);
    void iconUpdate(QEvent* event);

private:
    bool isHovered_ = false;
    bool isSelected_ = false;
    bool isFirstTime_ = true;
    QPixmap pathOriginal_;
    QPixmap pathChecked_;
    QPixmap originIc_;
    QPixmap tintOriginIc_;
    QPixmap checkedIc_;
    QPixmap tintCheckedIc_;
    QColor tintColor_;
    std::vector<EventCallback> btnCallbacks_;
};
