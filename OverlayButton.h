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

    Q_PROPERTY(QPixmap originPixPath READ getoriginPixPath WRITE setoriginPixPath DESIGNABLE true NOTIFY originPixPathChanged);
    Q_PROPERTY(QPixmap checkedPixPath READ getcheckedPixPath WRITE setcheckedPixPath DESIGNABLE true NOTIFY checkedPixPathChanged);
    Q_PROPERTY(QColor tint_color READ gettint_color WRITE settint_color DESIGNABLE true NOTIFY tint_ColorChanged);

public:
    OverlayButton(QWidget* parent);
    virtual ~OverlayButton();
    typedef bool (OverlayButton::*EventCallback)(QEvent*);

public:
    void setoriginPixPath(QPixmap originPixPath);
    QPixmap getoriginPixPath() const;
    void setcheckedPixPath(QPixmap checkedPixPath);
    QPixmap getcheckedPixPath() const;
    void settint_color(QColor tint_color);
    QColor gettint_color() const;

signals:
    void signalBtnEvent(QEvent* event);
    void originPixPathChanged(QString);
    void checkedPixPathChanged(QString);
    void tint_ColorChanged(QColor);

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
    QPixmap path_Original_;
    QPixmap path_Checked_;
    QPixmap originIc_;
    QPixmap tintOriginIc_;
    QPixmap checkedIc_;
    QPixmap tintCheckedIc_;
    QColor tintColor_;
    std::vector<EventCallback> btnCallbacks_;
};
