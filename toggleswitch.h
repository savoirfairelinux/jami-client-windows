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

#pragma once

#include <QtWidgets>

class ToggleSwitch : public QAbstractButton {
    Q_OBJECT
        Q_PROPERTY(int offset READ offset WRITE setOffset)
        Q_PROPERTY(QBrush brush READ brush WRITE setBrush)

public:
    ToggleSwitch(QWidget* parent = nullptr);
    ToggleSwitch(const QBrush& brush, QWidget* parent = nullptr);

    QSize sizeHint() const override;

    QBrush brush() const {
        return brush_;
    }
    void setBrush(const QBrush &brush) {
        brush_ = brush;
    }

    int offset() const {
        return x_;
    }
    void setOffset(int o) {
        x_ = o;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void enterEvent(QEvent*) override;

private:
    bool switch_;
    qreal opacity_;
    int x_;
    int y_;
    int height_;
    int margin_;
    QBrush thumb_;
    QBrush track_;
    QBrush brush_;
    QPropertyAnimation *anim_ = nullptr;
};