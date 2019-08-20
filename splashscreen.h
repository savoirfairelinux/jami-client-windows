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

#include <QSplashScreen>
#include <QPainter>

class SplashScreen : public QSplashScreen
{
    Q_OBJECT;

public:
    SplashScreen();
    ~SplashScreen();

    void setupUI(const QPixmap& logo,
                 const QString& headerText = "",
                 const QString& footerText = "",
                 const QColor& bgColor = Qt::white,
                 const QColor& textColor = Qt::black);

protected:
    bool eventFilter(QObject *target, QEvent *e);
    void paintEvent(QPaintEvent* e) override;

private:
    QString headerText_;
    QPixmap logo_;
    QString footerText_;
    QColor bgColor_;
    QColor textColor_;
    QMovie* spinner_;
};
