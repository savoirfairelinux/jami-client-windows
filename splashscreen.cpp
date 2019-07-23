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
#include "splashscreen.h"

SplashScreen::SplashScreen(const QPixmap& pixmap)
{
    QSplashScreen::setPixmap(pixmap);
    QRect rect = this->geometry();
    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(16);
    setFont(font);
    setMessageRect(QRect(0, 0, rect.width(), rect.height()), Qt::AlignCenter);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::SplashScreen);
};

SplashScreen::~SplashScreen()
{
};

void SplashScreen::drawContents(QPainter *painter)
{
    painter->setPen(this->color);
    painter->drawText(this->rect, this->alignement, this->message);
};

void SplashScreen::showStatusMessage(const QString &message, const QColor &color)
{
    this->message = message;
    this->color = color;
    this->showMessage(this->message, this->alignement, this->color);
};

void SplashScreen::setMessageRect(QRect rect, int alignement)
{
    this->rect = rect;
    this->alignement = alignement;
};