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

#include <QEvent>
#include <QMovie>
#include <QPainterPath>

constexpr static const int textSectionHeight = 48;

SplashScreen::SplashScreen()
{

    this->installEventFilter(this);
}

SplashScreen::~SplashScreen()
{
    if (spinner_) {
        disconnect(spinner_);
    }
}

void
SplashScreen::setupUI(const QPixmap& logo,
    const QString& headerText,
    const QString& footerText,
    const QColor& bgColor,
    const QColor& textColor)
{
    logo_ = logo;
    headerText_ = headerText;
    footerText_ = footerText;
    bgColor_ = bgColor;
    textColor_ = textColor;

    spinner_ = new QMovie(":/images/waiting.gif");
    connect(spinner_, &QMovie::frameChanged,
        [this](int frame) {
            Q_UNUSED(frame);
            update();
        });
    spinner_->start();

    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(16);
    setFont(font);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::SplashScreen);
    QFontMetrics fontMetrics(font);
    auto baseWidth = logo_.width();
    auto baseHeight = logo_.height()
        + textSectionHeight * 2
        + spinner_->frameRect().height() * 3;
    QPixmap bgPixmap(baseWidth, baseHeight);
    bgPixmap.fill(Qt::transparent);
    QSplashScreen::setPixmap(bgPixmap);
}

bool SplashScreen::eventFilter(QObject *target, QEvent *e)
{
    Q_UNUSED(target)
        if ((e->type() == QEvent::MouseButtonPress) ||
            (e->type() == QEvent::MouseButtonDblClick) ||
            (e->type() == QEvent::MouseButtonRelease) ||
            (e->type() == QEvent::KeyPress) ||
            (e->type() == QEvent::KeyRelease))
            return true;

    return false;
}

void SplashScreen::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    auto rect = QRect(0, 0, this->geometry().width(), this->geometry().height());

    // background
    QPainterPath path;
    path.addRoundedRect(rect, 10, 10);
    painter.fillPath(path, bgColor_);

    // footer text
    QFont font(this->font());
    font.setWeight(QFont::Weight::DemiBold);
    painter.setFont(font);
    painter.setPen(textColor_);
    auto textRect = QRect(rect.x(), 0,
                          rect.width(), textSectionHeight);
    painter.drawText(textRect, Qt::AlignCenter, headerText_);

    // logo
    painter.drawPixmap(0, textSectionHeight, logo_);

    // footer text
    font.setWeight(QFont::Weight::Normal);
    painter.setFont(font);
    painter.setPen(textColor_);
    textRect = QRect(rect.x(), rect.y() + textSectionHeight + logo_.height(),
                     rect.width(), textSectionHeight);
    painter.drawText(textRect, Qt::AlignCenter, footerText_);

    // spinner
    auto spinnerRect = QRect(rect.x() + rect.width() / 2 - textSectionHeight / 2,
                             textRect.y() + textSectionHeight,
                             spinner_->frameRect().width(), spinner_->frameRect().height());
    painter.drawPixmap(spinnerRect, spinner_->currentPixmap());

    QSplashScreen::paintEvent(e);
}
