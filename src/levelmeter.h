/***************************************************************************
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

#include <QTime>
#include <QProgressBar>

class LevelMeter : public QProgressBar {
    Q_OBJECT;

public:
    explicit LevelMeter(QWidget *parent = 0);
    ~LevelMeter();

public:
    void start();
    void stop();
    void setLevel(float rmsLevel);

private slots:
    void redrawTimerExpired();

private:
    void animateLevel();

    float rmsLevel_;
    float currentLevel_;

    QTimer* redrawTimer_;

};
