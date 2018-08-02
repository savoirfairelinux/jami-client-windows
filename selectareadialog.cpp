/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                                *
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
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <winuser.h>

#undef OUT
#undef IN
#undef ERROR
#endif

#include <QApplication>
#include <QScreen>
#include <QPainter>

#include "video/sourcemodel.h"
#include "media/video.h"
#include "callmodel.h"

SelectAreaDialog::SelectAreaDialog() :
    rubberBand_(nullptr)
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setWindowState(Qt::WindowFullScreen);
    setParent(0);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_PaintOnScreen);
    grabMouse();
    rubberBand_ = new QRubberBand(QRubberBand::Rectangle, this);
    QApplication::setOverrideCursor(Qt::CrossCursor);
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
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

    if(rubberBand_) {
        QApplication::restoreOverrideCursor();
        releaseMouse();
        if (auto call = CallModel::instance().selectedCall()) {
            if (auto outVideo = call->firstMedia<Media::Video>(Media::Media::Direction::OUT)) {
                QRect realRect = rubberBand_->geometry();
#ifdef Q_OS_WIN
                if (QGuiApplication::primaryScreen()->devicePixelRatio() > 1.0) {
                    auto scaledSize = QGuiApplication::primaryScreen()->geometry();
                    auto sourceHdc = GetDC(nullptr);
                    auto vertres = GetDeviceCaps(sourceHdc, VERTRES);
                    auto horzres = GetDeviceCaps(sourceHdc, HORZRES);
                    auto height = realRect.height() * QGuiApplication::primaryScreen()->devicePixelRatio();
                    auto width = realRect.width() * QGuiApplication::primaryScreen()->devicePixelRatio();
                    float xRatio = static_cast<float>(horzres) / static_cast<float>(scaledSize.width());
                    float yRatio = static_cast<float>(vertres) / static_cast<float>(scaledSize.height());
                    realRect.setX(static_cast<int>(realRect.x() * xRatio));
                    realRect.setY(static_cast<int>(realRect.y() * yRatio));
                    realRect.setWidth(static_cast<int>(width));
                    realRect.setHeight(static_cast<int>(height));
                }
#endif
                outVideo->sourceModel()->setDisplay(0, realRect);
            }
        }
        delete rubberBand_;
        rubberBand_ = nullptr;
        reject();
    }
}

void
SelectAreaDialog::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    painter.drawPixmap(QPoint(0, 0), originalPixmap_);
}
