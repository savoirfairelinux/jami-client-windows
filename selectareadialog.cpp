/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                                *
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

#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QCursor>

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
        qDebug() << "MOUSE PRESS" << event->globalPos() << event->pos() << event->screenPos() << event->localPos();
        qDebug() << "QCURSOR " << QCursor::pos(QGuiApplication::primaryScreen());
        rubberBand_->setGeometry(QRect(origin_, QSize()));
        rubberBand_->show();
    }
}

void
SelectAreaDialog::mouseMoveEvent(QMouseEvent* event)
{
  if (rubberBand_)
      rubberBand_->setGeometry(QRect(origin_, event->globalPos()).normalized());
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
                qDebug() << "REALRECT : " << realRect;
                qDebug() << "SCREEN GEOMETRY : " << QGuiApplication::primaryScreen()->geometry();
                qDebug() << "SCREEN PHYSICAL : " << QGuiApplication::primaryScreen()->physicalSize();
                qDebug() << "SCREEN SIZE : " << QGuiApplication::primaryScreen()->size();
                qDebug() << "SCREEN VIRTUAL SIZE : " << QGuiApplication::primaryScreen()->virtualSize();
                realRect.setX(realRect.x());
                realRect.setY(realRect.y());
                realRect.setWidth(static_cast<int>(realRect.width() * QGuiApplication::primaryScreen()->devicePixelRatio()));
                realRect.setHeight(static_cast<int>(realRect.height() * QGuiApplication::primaryScreen()->devicePixelRatio()));
                qDebug() << realRect;
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
