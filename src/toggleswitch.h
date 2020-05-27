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
#include <QPainterPath>

class ToggleSwitch : public QAbstractButton {
    Q_OBJECT;

    Q_PROPERTY(QString text READ text WRITE setText);
    Q_PROPERTY(qreal offset READ offset WRITE setOffset);
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush);

public:
    enum class SwitchStyle { MATERIAL, UWP };

    ToggleSwitch(QWidget* parent = nullptr);

    QSize sizeHint() const override;

    QString text() const {
        return text_;
    }
    void setText(const QString& text) {
        text_ = text;
    }

    QBrush brush() const {
        return brush_;
    }
    void setBrush(const QBrush& brush) {
        brush_ = brush;
    }

    qreal offset() const {
        return offset_;
    }
    void setOffset(qreal o) {
        offset_ = o;
        update();
    }

    void setChecked(bool state);

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    QString text_;
    bool switch_;
    qreal opacity_;
    qreal offset_;
    SwitchStyle style_;

    int width_;
    int height_;
    int margin_;
    int textMargin_;
    int knobRadius_;
    int pinRadius_;
    int pinWidth_;

    QBrush thumb_;
    QBrush track_;
    QBrush brush_;
    QPropertyAnimation* anim_ = nullptr;
};