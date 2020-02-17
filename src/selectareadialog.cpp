/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "selectareadialog.h"

#ifdef Q_OS_WIN
#define NOMINMAX
#include <windows.h>
#endif

#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QDesktopWidget>

#include "lrcinstance.h"
#include "utils.h"

SelectAreaDialog::SelectAreaDialog(int& screenNumberRef,
                                   QRect& rectArea,
                                   QWidget* parent) :
    rubberBand_(nullptr),
    screenNumber_(screenNumberRef),
    rectArea_(rectArea),
    QDialog(parent)
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setWindowState(Qt::WindowFullScreen);
    setParent(0);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_PaintOnScreen);
    setWindowModality(Qt::ApplicationModal);
    grabMouse();
    rubberBand_ = new QRubberBand(QRubberBand::Rectangle, this);
    QApplication::setOverrideCursor(Qt::CrossCursor);

    screenNumber_ = qApp->desktop()->screenNumber(parent);
    if (auto screen = qApp->screens().at(screenNumber_)) {
        originalPixmap_ = screen->grabWindow(0);
        originalPixmap_.setDevicePixelRatio(screen->devicePixelRatio());
    }
}

void
SelectAreaDialog::mousePressEvent(QMouseEvent* event)
{
    if (rubberBand_) {
        origin_ = event->globalPos();
        rubberBand_->setGeometry(QRect(origin_, QSize()));
        rubberBand_->show();
    }
}

void
SelectAreaDialog::mouseMoveEvent(QMouseEvent* event)
{
    int top    = std::min(event->globalY(), origin_.y());
    int left   = std::min(event->globalX(), origin_.x());
    int bottom = std::max(event->globalY(), origin_.y());
    int right  = std::max(event->globalX(), origin_.x());

  if (rubberBand_)
      rubberBand_->setGeometry(QRect(QPoint(left,top), QPoint(right,bottom)));
}

void
SelectAreaDialog::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event)
    if (!rubberBand_) {
        return;
    }

    QScreen* screen = qApp->screens().at(screenNumber_);
    if (!screen) {
        screen = qApp->primaryScreen();
    }

    QApplication::restoreOverrideCursor();
    releaseMouse();
    QRect rect = rubberBand_->geometry();
#if defined(Q_OS_WIN)
    if (screen && screen->devicePixelRatio() > 1.0) {
        auto scaledSize = screen->geometry();
        auto sourceHdc = GetDC(nullptr);
        auto vertres = GetDeviceCaps(sourceHdc, VERTRES);
        auto horzres = GetDeviceCaps(sourceHdc, HORZRES);
        auto height = rect.height() * screen->devicePixelRatio();
        auto width = rect.width() * screen->devicePixelRatio();
        float xRatio = static_cast<float>(horzres) / static_cast<float>(scaledSize.width());
        float yRatio = static_cast<float>(vertres) / static_cast<float>(scaledSize.height());
        rect.setX(static_cast<int>(rect.x() * xRatio));
        rect.setY(static_cast<int>(rect.y() * yRatio));
        rect.setWidth(static_cast<int>(width));
        rect.setHeight(static_cast<int>(height));
    }
# endif

    // Minimun size
    rect.setHeight(rect.height() < 128 ? 128 : rect.height());
    rect.setWidth(rect.width() < 128 ? 128 : rect.width());
    rectArea_ = rect;

    rubberBand_->deleteLater();
    done(0);
}

void
SelectAreaDialog::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.drawPixmap(QPoint(0, 0), originalPixmap_);
}
